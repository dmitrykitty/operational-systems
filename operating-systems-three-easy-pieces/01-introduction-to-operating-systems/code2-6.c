#include <stdio.h>
#include <fcntl.h>

int main(){
    char* file_name = "file.txt";
    //0700 = S_IRWXU = Read + Write + Execute for USER(OWNER)
    int file = open(file_name, O_WRONLY | O_CREAT | O_TRUNC, 0700);

    if(file < 0){
        printf(stderr, "Error during opening '%s' occured.", file_name); 
        return 1; 
    }

    // Write text into the file.
    // "hello world\n" has 12 visible bytes; 13 also includes '\0'
    int in = write(file, "Hello World\n", 12);
    if(in != 13){
        printf(stderr, "Error during writing to '%s' occured: expected bytes - 12, got - %d.", file_name, in); 
    }

    close(file); 
    return 0; 
}