#ifndef COURSEWORK_ANALYZE_COM_LINE_H
#define COURSEWORK_ANALYZE_COM_LINE_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define E_OK 0
#define E_FAIL 1

typedef long int _long;

typedef struct //структура ком строки
{
    FILE * src;
    FILE * dst;
    long int retries;
    long int sector_size;
    long int seek;
    long int limit;
    long int src_size;
    long int max_bad;

}args_t;

void help();
void default_args(args_t *args); 
void get_options(args_t *args, char *command_line);
void get_files( char * argv[], args_t *args);

char * while_not_space(int pos, const char* str);

args_t*  analyze_cl(int argc, char * argv[])
{
    if(argc < 3)
    {
        help();
        exit(0);
    }

    args_t *args = (args_t*) malloc(sizeof(args_t)); //выделение памяти на структуру
    default_args(args);                              //заполнение аргументами по умолчанию
    
    char *command_line = (char*) malloc(sizeof(&argv)); //ком строка

    for(int i = 3; i < argc; i++)   //заполнение ком строки
    {
        strcat(command_line, argv[i]);
        strcat(command_line, " ");
    }
    
    get_options(args, command_line);
    get_files(argv, args);
   
    return args;

}

void default_args(args_t *args)
{
    args->src=NULL;
    args->dst=NULL;
    args->retries=3;
    args->sector_size=512;
    args->seek=0;
    args->limit=0;
    args->src_size=0;
    args->max_bad=0;
}

void get_files( char * argv[], args_t *args)
{
    args->src = fopen (argv[1],"r");
    if (args->src == NULL)
    {
        fprintf(stderr,"Unable to open source file for reading: %s\n\n",argv[1]);
        exit(0);
    }    
    else
    {
        if(fseek(args->src,0,SEEK_END) != 0)
        {
            fprintf(stderr,"File is unrecoverable, seek failed: %s\n\n",argv[1]);
            fclose (args->src);
            exit(0);
        }
        args->src_size = ftell(args->src); 
        
        if(args->src_size <= 0)       
        {
            fprintf(stderr,"File is unrecoverable, seek failed: %s\n\n",argv[1]);
            fclose (args->src);
            exit(0);
        }
        if(args->src_size<args->sector_size) args->sector_size = args->src_size;
    }

    args->dst = fopen (argv[2],"r+");
    if (args->dst == NULL)
    {
        args->dst = fopen (argv[2],"w");
        if (args->dst == NULL)
        {
            fprintf(stderr,"Unable to open destination file for writing: %s\n\n",argv[2]);
            fclose (args->src);
            exit(0);
        }  

    }

    if(args->limit > 0) 
    {
        if(args->limit>args->src_size)
        {
            fprintf(stderr,"Error: Source is smaller than the ending position!\n\n");
            fclose (args->src);
            fclose (args->dst);
            exit(0);
        } else
        {
        args->src_size = args->limit;
        }
    }

    if(args->seek>0)
    {
        if(args->seek>args->src_size)
        {
            fprintf(stderr,"Error: Source is smaller than the starting position!\n\n");
            fclose (args->src);
            fclose (args->dst);
            exit(0);
        }
    }
}

void get_options(args_t *args, char *command_line)
{
    char temp_symbol;
    int i=0;
    long long buffer; 

    while((temp_symbol = command_line[i]) != '\0')  //пока не конец ком строки
    {
        i++;                                        //зачем вначале увеличивать строку???
        if(temp_symbol == ' ')
            continue;

        if(temp_symbol == '-')
        {

            temp_symbol = command_line[i];
            i++;
            switch(temp_symbol) 
            {
                case 'i' :
                {
                    i++;
                    buffer = atoll(while_not_space(i, command_line));
                    if (buffer < 0) 
                    {
                        printf("НЕПРАВИЛЬНЫЕ АРГУМЕНТЫ\n");
                        exit(-1);
                    }
                    args->seek = buffer;
                    break;
                }
                case 's': 
                {
                    i++;
                    buffer = atoll(while_not_space(i, command_line));
                    if (buffer <= 0) 
                    {
                        printf("НЕПРАВИЛЬНЫЕ АРГУМЕНТЫ\n");
                        exit(-1);
                    }
                    args->limit = (buffer-args->seek);
                    break;
                }
                case 'r': 
                {
                    i++;
                    buffer = atoll(while_not_space(i, command_line));
                    if (buffer <= 0) 
                    {
                        printf("НЕПРАВИЛЬНЫЕ АРГУМЕНТЫ\n");
                        exit(-1);
                    }
                    args->retries = buffer;
                    break;
                }
                case 'b': 
                {
                    i++;
                    buffer = atoll(while_not_space(i, command_line));
                    if (buffer <= 0) 
                    {
                        printf("НЕПРАВИЛЬНЫЕ АРГУМЕНТЫ\n");
                        exit(-1);
                    }
                    args->sector_size = buffer;
                    break;
                }
                case 'm': 
                {
                    i++;
                    buffer = atoll(while_not_space(i, command_line));
                    if (buffer <= 0) 
                    {
                        printf("НЕПРАВИЛЬНЫЕ АРГУМЕНТЫ\n");
                        exit(-1);
                    }
                    args->max_bad = buffer;
                    break;
                }
                default:
                {
                    printf("НЕПРАВИЛЛЬНЫЕ АРГУМЕНТЫ\n");
                    exit(-1);
                }
            }
        }
    }
}

void help()
{
        printf("Вы вызвали помощь\n"
           "Для выполнения программы используйте следующий шаблон:\n"
           "sudo ./myrescue [путь к диску] [выходной файл] [опции: -?]\n"
           "!!!Все параметры с размерами должны быть в байтах!!!\n"
           "Возможные опции:\n"
           "-b=<size> -- размер блока. Может быть 512, 1024, 2048 байт. По-умолчанию 512.\n"
           "-i=<pos> -- стартовая позиция сохранения. Если нужно сохранить весь диск, не используйте этот флаг.\n"
           "-s=<size> -- размер сохраненных данных. Если нужно сохранить весь диск, не используйте этот флаг.\n"
           "-r=<times> -- количество попыток чтения. По-умолчанию 3.\n"
           "-m=<amount> -- максимальное количество плохих секторов. По-умолчанию нет ограничения.\n");
}

char * while_not_space(int pos, const char* str)
{
    char *tmp_str = (char *) malloc(sizeof(str));
    int i = 0;
    while(str[pos] != ' ')
    {
        if(str[pos] == '\0')
            return tmp_str;
        if(str[pos] < '0' || str[pos] > '9')
        {
            printf("НЕПРАВИЛЬНЫЕ АРГУМЕНТЫ\n");
            exit(-1);
        }
        tmp_str[i] = str[pos];
        pos++;
        i++;
    }
    tmp_str[i] = '\0';
    return tmp_str;
}


#endif //COURSEWORK_ANALYZE_COM_LINE_H
