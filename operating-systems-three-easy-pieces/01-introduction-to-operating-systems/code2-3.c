#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(){

    //int* p = malloc(sizeof(int));
    int* p = calloc(1, sizeof(int));
    if(p == NULL){
        perror("Allocating went wrong");
        return 1; 
    }

    //getpid() -> return int process ID 
    //printf requires (void*) as pointer
    printf("(%d) memory address of p:%p\n", getpid(), (void*)p);
    int n = 10; 
    while(n > 0) {
        *p= *p + 1;     
        printf("(%d)p:%d\n", getpid(), *p);
        n--;
    }
    free(p);
    return 0;
}