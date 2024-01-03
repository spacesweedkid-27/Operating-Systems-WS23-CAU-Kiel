#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <pthread.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <semaphore.h>
#include <limits.h>
#include "triangle.h"
#include <assert.h>

// The shared state in our concurrent scenario
struct state {
  int boundary, interior;

  int active, finished;
} state;

volatile char should_output = 0;

/**
 * @brief Callback function for the countPoints() function of triangle.h.
 * This function should increment the number of found points by the given amount and signal the
 * output thread to update the progress report on stdout.
 * @param boundary Found points on the boundary of the triangle
 * @param interior Found points in the interior of the triangle
 */
static void calc_finished_cb(int boundary, int interior){
  // TODO: Add semaphore
  // This is what has to be done
  state.boundary += boundary;
  state.interior += interior;
  state.active--;
  state.finished++;
}

/**
*\brief Start routine of our worker threads of this problem.
* Remember the threads are meant to run detached.
* This has the advantage that no join is necessary but 
* you need to bookkeep yourself if a thread has finished its workload.
* @param param the param of our worker threads
*/
static void *worker(void *param){}

/**
*\brief Start routine of the thread that is meant to present the results.
*@param param the param of our thread
**/
static void *printer(void *param) {
  for (; ;)
    while (!should_output)
      ; // wait for output req
    // TODO: Add semaphore
    should_output = 0;
    printf("Found %d boundary and %d interior points, %d active threads, %d finished threads\r", state.boundary, state.interior, state.active, state.finished);

}

// Returns a pointer to a triangle from a string input
// If the input is wrong return NULL
// the string should be in the following format:
// "(num,num,num),(num,num,num),(num,num,num)", whereas num is a number.
struct triangle* parse(char* input){
  struct triangle* temp = malloc(sizeof(struct triangle));
  // this looks so stupid
  if (sscanf(input, "(%d,%d),(%d,%d),(%d,%d)", &temp->point[0].x, &temp->point[0].y, &temp->point[1].x, &temp->point[1].y, &temp->point[2].x, &temp->point[2].y) != 6) {
    errno = EINVAL;
    return NULL;
  }
    
  return temp;
}

void print_triangle(struct triangle input){
  printf("(%d,%d),(%d,%d),(%d,%d)\n", input.point[0].x, input.point[0].y, input.point[1].x, input.point[1].y, input.point[2].x, input.point[2].y);
}


int main(int argc, char *argv[]) {

  struct triangle* test = parse("(1,1),(2,4)GASBB,(7,5)");
  if (test)
    print_triangle(*test);
  else {
    perror("Parse failed");
  }
    

  return;
  // TODO: Parse args and check for bad input
  int THREAD_NUM = 1;

  pthread_t threads[THREAD_NUM];
  pthread_t output_thread;

  struct triangle thread_args[THREAD_NUM];

  // line length should be not more than 17 (alloc 18 bc of 0-terminator)
  char* line_buf = malloc(18);

  for (; ;){
    // Get 17 characters
    fgets(line_buf, 17, stdin);
    
    // Parse them
    
  }

  //assert(!pthread_create(&threads[i], NULL, printer, &thread_args[i]))


  return 1;
}
