#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

int main(int argc, char* argv[]){
    printf("hello from process(%d)\n", getpid());

    int rc = fork(); 
    if(rc < 0){
        perror("fork");
    } else if(rc > 0){
        printf("it's process(%d): parent of process(%d)\n", getpid(), rc);
    } else {
        printf("hello, i'm child process(%d)\n", getpid()); 
        char* myargs[] = {"wc", "code3-3.c", NULL}; 
        execvp(myargs[0], myargs); 

        perror("execvp");
        _exit(1);
    }
    int wc = wait(NULL);
    return 0;
}