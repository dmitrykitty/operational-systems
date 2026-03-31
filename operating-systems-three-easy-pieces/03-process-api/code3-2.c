#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> //for working with processes
#include <sys/wait.h>

int main(){
    printf("hello world from process(%d)\n", getpid());
    //system call - OS creates new process
    //almost exact copy of calling process
    int rc = fork(); 

    if(rc < 0){
        perror("creating nw prcess failed\n");
        _exit(1);
    } else if(rc == 0){
        printf("hello from child process(%d)\n", getpid());
    } else {
        int wc = wait(NULL);
        printf("hello from parent process(%d). I'm parent of process(%d), wc:%d\n", getpid(), rc, wc);
    }
    return 0;
}