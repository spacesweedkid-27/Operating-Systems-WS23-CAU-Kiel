#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>

char* concat(const char *s1, const char *s2)
{
    char *result = malloc(strlen(s1) + strlen(s2) + 1); // +1 for the null-terminator
    // TODO: check for errors in malloc here
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}


int lshere(char* path, int allow_rec){
    DIR* dir = opendir(path);
    if (!dir) {
        // No directory
        return 1;
    }
    struct dirent* e = NULL;
    while ((e = readdir(dir))){
        // strcmp returns 0 when strings are equal
        // "The special entries .. and . in directories shall be skipped while exploring a directory. (This is tested!)"
        if (strcmp(e->d_name, "..") && strcmp(e->d_name, ".")) {
            
            // If it does not start with '/', change the path. This fixes the first element of / being adressed as "//..."
            // (which is not even wrong, but does not have to be there and is asymmetric)
            if (e->d_name[0] != '/'){
                // Am I happy that path is a local variable
                path = concat(path, "/");
            }

            int skip = 0;
            if (path[1] == '/'){
                skip = 1;
            }
            printf("%s\n", concat(path, e->d_name) + skip);

            // We have to remove the suffix of the input if it is '/'
            // because we would have some "//" when sls-ing to "/"
            if (path[strlen(path) - 1] == '/'){
                // This does not delete the last character or free space.
                path[strlen(path) - 1] = '\0';
            }
            if (allow_rec){
                lshere(concat(concat(path, "/"), e->d_name), allow_rec - 1);
            }
        }
    }
    free(e);
    closedir(dir);
    
    return 0;
}

int main(int argc, char* argv[]){

    if (argc > 3){
        printf("Too many arguments...\nUsage: sls <path> (depth)\nor:    sls\n");
        return 1;
    }

    // Set to maximum if no argument passed.
    int maxDepth = 4096;

    // Else read...
    if (argc > 2) {
        maxDepth = atoi(argv[2]);
    }

    // If the user did not pass anything, then we want to execute ls in "."
    // If we fail because argv[1] is not a path to a directory, we try "."
    if (argc == 1 || lshere(realpath(argv[1], NULL), maxDepth)){
        lshere(realpath(".", NULL), maxDepth);
    }
    return 0;
}
