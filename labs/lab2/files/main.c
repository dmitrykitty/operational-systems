#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

/*
Proszę utworzyć program, który wczyta plik, 
którego nazwa przekazywana jest przy wywołaniu programu 
i zamieni występujące w nim polskie znaki na odpowiedniki z 
alfabetu angielskiego (np: ą na a, ć na c itd.).
*/

char convert_char(unsigned char b1, unsigned char b2) {
    if (b1 == 0xC4 && b2 == 0x85) return 'a'; // ą
    if (b1 == 0xC4 && b2 == 0x84) return 'A'; // Ą

    if (b1 == 0xC4 && b2 == 0x87) return 'c'; // ć
    if (b1 == 0xC4 && b2 == 0x86) return 'C'; // Ć

    if (b1 == 0xC4 && b2 == 0x99) return 'e'; // ę
    if (b1 == 0xC4 && b2 == 0x98) return 'E'; // Ę

    if (b1 == 0xC5 && b2 == 0x82) return 'l'; // ł
    if (b1 == 0xC5 && b2 == 0x81) return 'L'; // Ł

    if (b1 == 0xC5 && b2 == 0x84) return 'n'; // ń
    if (b1 == 0xC5 && b2 == 0x83) return 'N'; // Ń

    if (b1 == 0xC3 && b2 == 0xB3) return 'o'; // ó
    if (b1 == 0xC3 && b2 == 0x93) return 'O'; // Ó

    if (b1 == 0xC5 && b2 == 0x9B) return 's'; // ś
    if (b1 == 0xC5 && b2 == 0x9A) return 'S'; // Ś

    if (b1 == 0xC5 && b2 == 0xBA) return 'z'; // ź
    if (b1 == 0xC5 && b2 == 0xB9) return 'Z'; // Ź

    if (b1 == 0xC5 && b2 == 0xBC) return 'z'; // ż
    if (b1 == 0xC5 && b2 == 0xBB) return 'Z'; // Ż

    return 0;
}

int main(int argc, char* argv[]){
    if(argc < 2){
        printf("Usage: %s input_file\n", argv[0]); 
        return 1;
    }
    
    char* input_file = argv[1]; 
    char* output_file = "output.txt";  // Output file name
    unsigned char b1;
    unsigned char b2;
    
    // Open input file for reading
    int in_file = open(input_file, O_RDONLY);
    if(in_file < 0) {
        printf("Error durinmg opening the file '%s'\n", input_file);
        return 1;
    }
    
    //Create a new file and open it
	//if it exists (write-only access)
	//File permissions: rw-r--r--
    //O_TRUNC - clean before writing 
    int out_file = open(output_file, O_WRONLY | O_CREAT | O_TRUNC , 0644);
    if(out_file < 0) {
        printf("Error during creating a file '%s'\n", output_file);
        close(in_file);
        return 1;
    }    
    // Read from input, convert, and write to output
    //read(from, &adres_to, how_many_bytes) -> amount of readen bytes
    //1 - ok, 0 - EOF, -1 - error
    ssize_t n; 
    while((n = read(in_file, &b1, 1)) == 1) {
        //ASCII [0-127]
        if(b1 < 128){
            if(write(out_file, &b1, 1) != 1){
                perror("error during writing");
                close(in_file);
                close(out_file);
                return 1; 
            }
        } else {
            if(read(in_file, &b2, 1) != 1){
                perror("error during reading second byte");
                close(in_file); 
                close(out_file);
                return 1;
            }
            unsigned char converted = convert_char(b1, b2);       
            if(converted == 0){
                //it means it two or more bytes char but not polish one
                if((write(out_file, &b1, 1) != 1) || (write(out_file, &b2, 1) != 1)){
                    perror("error during writing two bytes of non polish char");
                    close(in_file); 
                    close(out_file);
                    return 1;
                }
            } else {
                if(write(out_file, &converted, 1) != 1){
                    perror("error during writing changeg polish char");
                    close(in_file); 
                    close(out_file);
                    return 1;
                }
            } 
        }
    }

    if(n < 0){
        perror("error during reading");
        close(in_file);
        close(out_file);
        return 1; 
    }
    
    close(in_file);
    close(out_file);
    
    printf("Output saved to '%s'\n", output_file);
    return 0;
}