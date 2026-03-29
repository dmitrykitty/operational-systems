#include <stdio.h>

/*
To execute few processes in one command
./cpu "A" &  ./cpu "B" &  ./cpu "C" &  ./cpu "D"
*/

int main(int argc, char* argv[]){
    if(argc < 2){
        perror("usage: cpu <string>\n");
    }

    char* str = argv[1]; 
    int n = 10; 
    while(n > 0){
        printf("%s\n", str); 
        n--;
    }
    return 0;
}