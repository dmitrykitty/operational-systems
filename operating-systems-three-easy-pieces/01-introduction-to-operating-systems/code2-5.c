#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

volatile int counter = 0; 
int loops; 

void* worker(void* arg){
    for(int i = 0; i < loops; i++){
        counter++; 
    }
    return NULL; 
}

int main(int argc, char* argv[]){
    if(argc < 2){
        perror("usage: ./code <value>\n");
        return 1; 
    }

    loops = atoi(argv[1]);
    pthread_t p1, p2; 

    printf("Initial value: %d\n", counter);

    // pthread_create(thread, attr, start_routine, arg)
    // thread         -> pointer to variable where thread ID will be stored
    // attr           -> thread attributes (NULL = default attributes)
    // start_routine  -> function that the new thread will execute
    // arg            -> single argument passed to that function
    pthread_create(&p1, NULL, worker, NULL);
    pthread_create(&p2, NULL, worker, NULL);

    // pthread_join(thread, retval)
    // thread -> thread to wait for
    // retval -> pointer used to receive the return value of the thread function
    //           (NULL = we do not care about the returned value)
    pthread_join(p1, NULL);
    pthread_join(p2, NULL);

    printf("Final value: %d\n", counter);
    return 0;
}