#include <stdio.h>
#include <stdlib.h>
#include<unistd.h> //for working with processes

int main(){
    printf("hello world from process(%d)\n", getpid());
    //system call - OS creates new process
    //almost exact copy of calling process
    int rc = fork(); 

    if(rc < 0){
        perror("creating nw prcess failed\n");
    } else if(rc == 0){
        printf("hello from child process(%d)\n", getpid());
    } else {
        printf("hello from parent process(%d). I'm parent of process(%d)\n", getpid(), rc);
    }
    return 0;
}

/*
Output is not deterministic
hello world from process(4011)
hello from parent process(4011). I'm parent of process(4012)
hello from child process(4012)
*/