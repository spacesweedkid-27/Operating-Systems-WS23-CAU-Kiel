#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

int get_size_of_file(char* path){
    char BUFFER = '\0';
    int FILE = open(path, O_RDONLY);
    if (FILE == -1){
        return -1;
    }
    int SIZE = 0;
    // bytes read = 0 -> File end reached
    while (read(FILE, &BUFFER, 1) != 0){
        //printf("Reading bytes: %c\n", BUFFER);
        // Increase the byte size of the File.
        SIZE++;
    }
    close(FILE);
    return SIZE;
}

int read_file(char* path, char** read_string){
    int SIZE = get_size_of_file(path);

    printf("%s has size %d.\n", path, SIZE);

    char* BUFFER = (char*)malloc(SIZE);

    int FILE = open(path, O_RDONLY);
    // If one of them is -1 then there was a read error.
    if (read(FILE, BUFFER, SIZE) != SIZE || SIZE == -1){
        free(BUFFER);
        return 1;
    }
    close(FILE);
    *read_string = BUFFER;
    return 0;
}

int wc(char* path){
    // char* to store the files bytes
    char* read_string;
    read_file(path, &read_string);
    // Now the data is prepared.
    printf("%s\n", read_string);

    return (int)path[0];
}

int main(int argc, char* argv[]){
    if (argc == 1) {
        printf("The program simplewc expects a path to a file as argument!\n");
        return 1;
    }

    char* path = argv[1];
    printf("Set path to: \"%s\"\n", path);

    return wc(path);
    
}