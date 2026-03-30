#define _DEFAULT_SOURCE
#define PATH_SIZE 4096

#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>



int change_eol(const char* file_path);

/*
Proszę utworzyć program, który wczyta jako parametr wywołania nazwę katalogu. Następnie zamieni w występujących w nim plikach z rozszerzeniem txt znaki końca linii ze standardu Windows na UNIX (CRLF->LF). Dla każdego przetwarzanego pliku powinien zostać stworzony osobny proces.
*/
int main(int argc, char* argv[]){
    if(argc != 2){
        fprintf(stderr, "Usage: %s <catalog_name>\n", argv[0]);
        return 1;
    }
    
    char* catalog_name = argv[1]; 
    struct dirent* entity; 
    DIR* entity_stream; 

    entity_stream = opendir(catalog_name); 

    if(entity_stream == NULL){
        perror("error during opening catalog");
        return 1; 
    }

    while((entity = readdir(entity_stream)) != NULL){
        char* entity_name = entity -> d_name; 
        printf("Parent process(%d) <- %s\n", getpid(), entity_name); 
        int en_len = strlen(entity_name);
        if(strcmp(".", entity_name) == 0 || 
            (strcmp("..", entity_name) == 0) || 
            entity -> d_type != DT_REG ||
            en_len < 4 || 
            (strcmp(".txt", entity_name + en_len - 4) != 0)
        ) {
            printf("%s is not .txt file\n", entity_name);
            continue;
        }

        char full_path[PATH_SIZE]; 
        snprintf(full_path, PATH_SIZE, "%s/%s", catalog_name, entity_name); 

        //creating new process
        //pid < 0 - error
        //pid == 0 - inside child process
        //pid > 0 - inside parent process
        pid_t pid = fork();
        if(pid < 0){
            perror("error during opening new process"); 
            continue;
        }
        else if(pid == 0){
            printf("Process(%d) is working on file '%s'\n", getpid(), full_path); 
            int result = change_eol(full_path); 
            printf("Process(%d) finished with file '%s'\n", getpid(), full_path); 
            _exit(result);
        }
    }

    closedir(entity_stream);
    while (wait(NULL) > 0) {}
    return 0;
}

int change_eol(const char* file_path){
    int in = open(file_path, O_RDONLY);
    if(in < 0){
        perror("error during opening file for reading"); 
        return 1; 
    }

    char temp_path[PATH_SIZE];
    //redirect stream to temp_path 
    snprintf(temp_path, PATH_SIZE, "%s.tmp", file_path);
    int out = open(temp_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (out < 0) {
        perror("error during opening or creating file for writing"); 
        close(in);
        return 1;
    }

    unsigned char b1;
    unsigned char b2;
    ssize_t n1;
    ssize_t n2;

    while((n1 = read(in, &b1, 1)) == 1){
        if(b1 == '\r'){
            n2 = read(in, &b2, 1);
            if(n2 == 1){
                if(b2 == '\n'){
                    unsigned char lf = '\n';
                    if(write(out, &lf, 1) != 1){
                        perror("error during writing to file");  
                        close(in);
                        close(out); 
                        return 1; 
                    }
                } else {
                    //b1 = \r, b2 = some other byte
                    if((write(out, &b1, 1) != 1) || (write(out, &b2, 1) != 1)){
                        perror("error during writing to file");  
                        close(in);
                        close(out); 
                        return 1; 
                    }
                }
            } else if(n2 == 0){
                //file ends with \r
                if(write(out, &b1, 1) != 1){
                    perror("error during writing to file"); 
                    close(in);
                    close(out); 
                    return 1; 
                }
                break;
            } else {
                perror("error during reading file");
                close(in);
                close(out);
                return 1;
            }
        } else {
            if(write(out, &b1, 1) != 1){
                perror("error during writing to file"); 
                close(in);
                close(out); 
                return 1; 
            }
        }
    }
    if(n1 < 0){
        perror("error during reading file");
        close(in);
        close(out);
        return 1; 
    }

    close(in);
    close(out);

    if (rename(temp_path, file_path) != 0) {
        perror("error during renaming files"); 
        return 1;
    }
    return 0;
}

