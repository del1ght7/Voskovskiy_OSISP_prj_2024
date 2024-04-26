#ifndef COURSEWORK_RESCUE_H
#define COURSEWORK_RESCUE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h> 
#include <stdbool.h>
#include <pthread.h>
#include "analyze_com_line.h"
#include "logger.h"

#define BUFFSZ 1048576

typedef struct
{
    char buffer[BUFFSZ];
    size_t slen;
} storage;

args_t * args;

bool case_of_bad_block(args_t *args, long *cur_pointer);
void rescue(args_t *args);
void fill_bad_block(args_t *args, long *cur_pointer);

void fill_bad_block(args_t *args, long *cur_pointer)
{

storage buff;
buff.slen = 0;
buff.buffer[0] = 0x00;
for(int i=0;i<args->sector_size;i++,*cur_pointer++)
{
buff.buffer[buff.slen] = 0x00;
buff.slen++;

if(buff.slen ==(size_t)args->sector_size)
{
if(fwrite(buff.buffer,1,buff.slen,args->dst)!=buff.slen)
{
fprintf(stderr,"Failed to Write Destination!");
exit(0); 
}
buff.slen = 0;
buff.buffer[0] = 0x00;
}

if(*cur_pointer >= args->src_size)
{
return;;
}
}
return;
}

bool case_of_bad_block(args_t *args, long *cur_pointer)
{
    char tmp[BUFFSZ*2];
    long retry = args->retries;
    size_t slen;
    while(retry-- > 0)
    {
        fseek(args->src,*cur_pointer,SEEK_SET);

        tmp[0] = 0x00;
        slen = fread (tmp, 1, args->sector_size, args->src);
            
        if(slen == (size_t)args->sector_size)
        {

                if(fwrite(tmp,1,slen,args->dst) != slen)
                {
                    fprintf(stderr,"Failed to Write Destination!");
                    exit(0);                        
                }
                
                return true;
        }
        else
        {
            *cur_pointer-=slen;
        }
    }
    return false;
}

void rescue(args_t *args)
{
    bool STAT=1;
    char tmp[BUFFSZ*2];
    long int badBlocks = 0;
    size_t slen;
    bool success;
    float progress=0;
    unsigned int i;
    long int rlen = 0;
    long int cur_pointer = 0;
    long acquired=0;
    long bb =0;
    long rb=0;

    struct logger_data loggerinfo;

    pthread_t logger_thread = pthread_create(&logger_thread, NULL, print_log, (void*)&loggerinfo);

    while(true)
    {
        if(!feof(args->src))
        {
                progress = (float)((double)(double)100/(double)args->src_size)*(double)cur_pointer;
                loggerinfo.current_pointer = &cur_pointer;
                bb=badBlocks*args->sector_size;
                acquired = (cur_pointer - bb);
                loggerinfo.acquired = &acquired;
                loggerinfo.amount_of_bad = &bb;
                loggerinfo.progress = &progress;
                loggerinfo.status = &STAT;

           tmp[0] = 0x00;

            rlen = args->src_size - cur_pointer;
            if(rlen <= 0)
            {
                STAT = 0;
                break;
            }
            else if(rlen > BUFFSZ)
            {
                rlen = BUFFSZ;
            }

            slen = fread (tmp, 1, rlen, args->src);
            if(slen == BUFFSZ)
            {
                rlen = fwrite(tmp,1,BUFFSZ,args->dst);
                if(rlen != BUFFSZ)
                {
                    fprintf(stderr,"Failed to Write Destination [1] %lu!",rlen);
                    exit(0);                        
                }
                cur_pointer = ftell(args->src); 
                continue;
            }            
            else
            {
                if(slen > 0)
                {
                    if(fwrite(tmp,1,slen,args->dst) != slen)
                    {
                        fprintf(stderr,"Failed to Write Destination!");
                        exit(0);                        
                    }

                    cur_pointer += slen;
                    fseek(args->src,cur_pointer,SEEK_SET);
                    if (cur_pointer== args->src_size)
                    {
                        STAT=0;
                        break;
                    }
                }
               }
        }

        success = case_of_bad_block(args,&cur_pointer);
        if(success == false)
        {
            badBlocks++;
            fill_bad_block(args, &cur_pointer);
            if(args->max_bad!=0)
            {
                if(badBlocks==args->max_bad)
                {
                    STAT = 0;
                    break;
                }
            }
        }
        else if(success == true)
        {
            cur_pointer=ftell(args->src);
        }
        if(cur_pointer >= args->src_size)
        {
            STAT = 0;
            break;
        }        
    }
    acquired = (cur_pointer - badBlocks);
    loggerinfo.acquired = &acquired;
    bb=badBlocks*args->sector_size;
    loggerinfo.amount_of_bad = &bb;
    progress = (float)((double)(double)100/(double)args->src_size)*(double)cur_pointer;
    loggerinfo.progress = &progress;
    sleep(1);
    pthread_join(logger_thread, NULL);

}
#endif //COURSEWORK_RESCUE_H