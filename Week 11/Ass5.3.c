#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

enum {READ=0, WRITE=1}

void piping(int argc, char* argv[]);

static int pip[2];

int main(int argc, char* argv[]) {
    piping(argc, argv);
    return 0;
}


/**
 * @brief Things(tm) went terrible wrong, print an error using a passed message 
 * and exit the programme.
 * 
 * @param msg the message (as a string) that is used as a prefix in our perror call
 */
static void die(char *msg) {
  perror(msg);
  exit(EXIT_FAILURE);
}


/**
 * @brief Function replicating the behaviour of the pipe command |
 * You can either implement the original pipe that only works for two commands
 * or implement a "multi" pipe that works for n commands
 * Your programme has only to work for commands without arguments.
 * 
 * @param argc 
 * @param argv 
 */
void piping(int argc,  char* argv[]){
    // Parse commands and check for errors in them.
    int command_number = argc - 2;

    // REMEMBER: '|' is left-associative.
    

}
