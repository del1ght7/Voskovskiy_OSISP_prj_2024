#ifndef COURSEWORK_LOGGER_H
#define COURSEWORK_LOGGER_H

#include <time.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>

struct logger_data
{
    long int *current_pointer;
    long int *amount_of_bad;
    long int *acquired;
    float *progress;
    bool *status;       
};

void* print_log(void* arg)
{
    struct logger_data* data = (struct logger_data*)arg;  
    char symbol[] = "-\\|/";                            
    int i = 0;
    long time_differ = 1;                             
    time_t time_of_start = time(NULL), current_time;   
    while(1) 
    {

        system("clear");
        printf("РАБОТАЕТ ЛОГГЕР %c\n", symbol[i++]);
        if(*(data->status)==1)
        {
            printf("ВЫПОЛНЕНИЕ\n");
        }
        else  
        {
            printf("КОНЕЦ.\n");
        }
        printf("ТЕКУЩИЙ УКАЗАТЕЛЬ:   %ld \n", *(data->current_pointer));
        printf("ПЛОХИХ СЕКТОРОВ: %ld \n", *(data->amount_of_bad));
        printf("ХОРОШИХ СЕКТОРОВ:   %ld \n", *(data->acquired));
        printf("ПРОГРЕСС:   %f \n", *(data->progress));
        printf("ВРЕМЯ РАБОТЫ: %ld:%02ld\n", time_differ/60, time_differ%60);
        sleep(1);
        current_time = time(NULL);
        time_differ = current_time - time_of_start;
        fflush(stdin);
        if(i % 4 == 0)
            i = 0;
    }
    return 0;
}

#endif //COURSEWORK_LOGGER_H