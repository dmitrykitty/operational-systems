#define _GNU_SOURCE

#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char** argv){
    if(argv != 2){
        fprintf(stderr, "usage: ping <PID>");
    }
    printf("ping process(%d) started", getpid()); 

    while(1){
        printf("pong"); 
        kill(argv[1], SIGUSR1);
        pause();
    }

}