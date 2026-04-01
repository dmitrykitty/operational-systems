#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

void handle_sigusr1(int sig){
    printf("haha, u cant kill me using %d signal. Continue...", sig); 
}

int main(){
    signal(SIGUSR1, handle_sigusr1);

    printf("process(%d) started\n", getpid()); 

    while(1){
        printf("infinite loop from process(%d)\n", getpid());
        sleep(2);
    }
    return 0; 
}
