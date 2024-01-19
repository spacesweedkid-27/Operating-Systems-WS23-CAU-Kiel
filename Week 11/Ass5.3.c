#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

enum {READ=0, WRITE=1};

void piping(int argc, char* argv[]);

int main(int argc, char* argv[]) {
    // I hope this does not break anything
    // (he doesn't know)
    argv = &argv[1];
    argc--;

    // REMEMBER: '|' is left-associative.
    

    piping(argc, argv);
    return 0;
}

/**
 * @brief Things(tm) went terrible wrong, print an error using a passed message 
 * and exit the programme.
 * 
 * @param msg the message (as a string) that is used as a prefix in our perror call
 */
/*
static void die(char *msg) {
  perror(msg);
  exit(EXIT_FAILURE);
}
*/

int pipe_helper(int argc, char* argv[], int input_pipe[2]){
  if(argc == 0){
    return 0;
  }

  int output_pipe[2];
  if(pipe(output_pipe))
      perror("Outcoming pipe creation has failed");

  int next_input_pipe[2];
  if(pipe(next_input_pipe))
      perror("Next incoming pipe creation has failed");

  if(errno){
      printf("Exiting.\n");
      exit(1);
  }

  pid_t pid = fork();
  // fork
  if (pid > 0){
    // parent
    pid_t second_child = fork();
    if (second_child > 0){
      close(input_pipe[WRITE]);
      dup2(input_pipe[READ], 0);
      close(input_pipe[READ]);

      close(output_pipe[READ]);
      dup2(output_pipe[WRITE], 1);
      close(output_pipe[WRITE]);
    } else if (second_child == 0){
      waitpid(second_child, NULL, 0);
    } else {
      perror("Fork failed");
    }

    execlp(argv[0], argv[0], NULL);
  } else if (pid == 0){
    // child
    close(output_pipe[WRITE]);
    dup2(output_pipe[READ], 0);
    close(output_pipe[READ]);

    if (argc > 1){
      close(next_input_pipe[READ]);
      dup2(next_input_pipe[WRITE], 1);
      close(next_input_pipe[WRITE]);
    }

    execlp(argv[1], argv[1], NULL);
  } else {
    perror("Fork failed");
  }

  printf("Sucess\n");

  argv = &argv[1];
  argc--;

  return pipe_helper(argc, argv, next_input_pipe);
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
    if (argc < 2){
      printf("Less than 2 arguments passed.\n");
      exit(1);
    }

    // create the first input pipe
    int input_pipe[2];

    if(pipe(input_pipe))
      perror("Incoming pipe creation has failed");

    // write "" into it
    write(input_pipe[WRITE], "", 1);

    // pass it to a helper
    pipe_helper(argc, argv, input_pipe);
}
