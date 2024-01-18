#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

// Our pipe file descriptors
static int pipefd_ptc[2], pipefd_ctp[2];
enum {READ=0, WRITE=1};

/**
 * @brief Create twp pipes and call perror and exit, if the creation fails.
 */
void create_pipes() {
  if (pipe(pipefd_ptc) || pipe(pipefd_ctp)){
    perror("Pipe creation failed");
    exit(errno);
  }
  printf("Pipe creation successful\n");
}
/**
 * @brief The child process sends the parent Pong
 */
void tell_parent() {
  char message[] = "pong";
  write(pipefd_ctp[WRITE], &message, 5);
  printf("Child sent a message to parent!\n");

}

/**
 * @brief Child reads a message out of the paper that was sent by the parent
 */
void wait_parent() {
  char buf[5];
  read(pipefd_ptc[READ], &buf, 5);
  printf("Child received %s from the parent!\n", buf);
}
/**
 * @brief Parent sends the child a message through the pipe
 * 
 */
void tell_child() {
  char message[] = "ping";
  write(pipefd_ptc[WRITE], &message, 5);
  printf("Parent sent a message to child!\n");
}

/**
 * @brief Read the message out of the pipe that was sent by the child
 * 
 */
void wait_child() {
  char buf[5];
  read(pipefd_ctp[READ], &buf, 5);
  printf("Parent received %s from the child!\n", buf);
}

int main(int argc, char* argv[]) {
  create_pipes();
  pid_t pid = fork();
  if(pid < 0) {
    perror("fork failed");
    exit(EXIT_FAILURE);
  }

  // Set a timer, the process will end after 10 seconds - sends an SIGALRM after the passage of this time.
  // The default disposition for SIGALRM is TERM(ination) ==> alarm kills our parent process
  // The OS will reap our zombie child (yeah admittdedly no clean solution)
  alarm(10);
  if (pid == 0) {
    // Parent process
    // this process doesn't use this
    close(pipefd_ptc[READ]);
    close(pipefd_ctp[WRITE]);

    while (1) {
      sleep(rand() % 2 + 1);
      tell_child();
      wait_child();
    }
  }
  else {
    // Child process
    // this process doesn't use this
    close(pipefd_ctp[READ]);
    close(pipefd_ptc[WRITE]);

    while (1) {
      sleep(rand() % 2 + 1);
      wait_parent();
      tell_parent();
    }
  }
  return 0;
}