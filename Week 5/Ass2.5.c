#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

struct Res{
    int lines, words, bytes;
};

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

    // printf("%s has size %d.\n", path, SIZE);

    char* BUFFER = (char*)malloc(SIZE);

    int FILE = open(path, O_RDONLY);
    // If one of them is -1 then there was a read error.
    if (read(FILE, BUFFER, SIZE) != SIZE || SIZE == -1){
        free(BUFFER);
        return 1;
    }

    // Add Termination to Buffer to insure format.
    BUFFER[SIZE] = '\0';

    close(FILE);
    *read_string = BUFFER;
    return 0;
}

int internal_wc(char* path, struct Res *result){
    // char* to store the files bytes
    char* read_string;
    read_file(path, &read_string);
    // Now the data is prepared.
    //printf("%s\n", read_string);

    // SIZE GOT, inefficiant but I DON'T CARE
    int SIZE = get_size_of_file(path);
    result->bytes = SIZE;

    int word_count = 0;
    int line_count = 0;
    int is_reading_word = 0;
    
    // now calculate word count
    for (int i = 0; i < SIZE; ++i){
       // printf("%d", read_string[i] == '\n');
        if (read_string[i] != '\n' && read_string[i] != ' ' && read_string[i] != '\t'){
            if (!is_reading_word){
                word_count++;
                is_reading_word = 1;
            } else {
                // Do nothing
            }
        } else {
            if (read_string[i] == '\n') {
                line_count++;
        }
            is_reading_word = 0;
        }
    }

    result->words = word_count;
    result->lines = line_count;

    if (SIZE == -1) return -1;
    return 0;
}

void prettify_wc(struct Res* result, char* path){
    printf("%d\t%d\t%d\t%s\n",result->lines, result->words, result->bytes, path);
}

int main(int argc, char* argv[]){
    if (argc == 1) {
        printf("The program simplewc expects a path to a file as argument!\n");
        return 1;
    }

    char* path = argv[1];
    // printf("Set path to: \"%s\"\n", path);

    struct Res result = {0,0,0};

    // Calculate the result, if we get an error, exit
    if (internal_wc(path, &result)){
        return 1;
    }

    prettify_wc(&result, path);

    return 0;
    
}