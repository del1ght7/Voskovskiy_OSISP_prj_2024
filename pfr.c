#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h> 
#include <stdbool.h>
#include <pthread.h>
#include "rescue.h"

void signalhandler( int signum) 
{
    
    fprintf(stderr,"\n\nExiting ....\n");
    fflush(stderr);
    fflush(stdout);

    if(args->dst != NULL)
    {
        fflush(args->dst);
        fclose(args->dst);
    } 

    if(args->src != NULL)
    {
        fclose(args->src);
    }

    exit(signum);  
}

int main(int argc, char *argv[])
{
   
    args = analyze_cl(argc,argv);
    signal(SIGINT, signalhandler);
    rescue(args);
    fclose(args->src);
    fclose(args->dst);

    return E_OK;
}

