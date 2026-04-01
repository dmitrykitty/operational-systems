#define _GNU_SOURCE
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>


int main(int argc, char** argv){
    if(argc != 2){
        fprintf(stderr, "usage: kill <PID>"); 
    }

    pid_t pid = atoi(argv[1]);

    printf("process(%d) wants to terminate process(%d)\n", getpid(), pid);
    kill(pid, SIGUSR1);
    sleep(10);
    kill(pid, SIGUSR2);
}