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


int main(int argc, char *argv[]) {
  // TODO: Parse args and check for bad input
  int THREAD_NUM = 1;

  pthread_t threads[THREAD_NUM];
  pthread_t output_thread;

  struct triangle thread_args[THREAD_NUM];

  // line length should be not more than 17 (alloc 18 bc of 0-terminator)
  char* line_buf = malloc(18);

  for (; ;){
    fgets(line_buf, 17, stdin);
    printf("%s", line_buf);
  }

  //assert(!pthread_create(&threads[i], NULL, printer, &thread_args[i]))


  return 1;
}
