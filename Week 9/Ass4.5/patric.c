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

// The shared state in our concurrent scenario
struct state {
  int boundary, interior;

  int active, finished;
} state;

short should_output = 0;

sem_t semaphore;

/**
 * @brief Callback function for the countPoints() function of triangle.h.
 * This function should increment the number of found points by the given amount and signal the
 * output thread to update the progress report on stdout.
 * @param boundary Found points on the boundary of the triangle
 * @param interior Found points in the interior of the triangle
 */
static void calc_finished_cb(int boundary, int interior){
  // ENTERING CRITICAL AREA
  sem_wait(&semaphore);
  state.boundary += boundary;
  state.interior += interior;
  state.active--;
  state.finished++;

  should_output = 1;
  sem_post(&semaphore);
  // EXITING CRITICAL AREA
}

/**
*\brief Start routine of our worker threads of this problem.
* Remember the threads are meant to run detached.
* This has the advantage that no join is necessary but 
* you need to bookkeep yourself if a thread has finished its workload.
* @param param the param of our worker threads
*/
static void *worker(void *param){
  // cast input back to right format
  struct triangle* input = (struct triangle*) param;
  countPoints(input, &calc_finished_cb);
  return NULL;
}

/**
*\brief Start routine of the thread that is meant to present the results.
*@param param the param of our thread
**/
static void *printer(void *param) {
  for (; ;){
    while (!should_output)
      ; // wait for output req
    // ENTERING CRITICAL AREA
    sem_wait(&semaphore);
    should_output = 0;
    sem_post(&semaphore);
    // EXITING CRITICAL AREA
    printf("Found %d boundary and %d interior points, %d active threads, %d finished threads\n", state.boundary, state.interior, state.active, state.finished);
  }
  return NULL;
}

// Returns a pointer to a triangle from a string input
// If the input is wrong return NULL
// the string should be in the following format:
// "(num,num,num),(num,num,num),(num,num,num)", whereas num is a number.
struct triangle* parse(char* input){
  // allocate mem for a triangle
  struct triangle* temp = malloc(sizeof(struct triangle));
  // this looks so stupid, give a 24 char buffer so that if somebody has a valid input but spills their food on the keyboard just after that, the input is discarded.
  // passing an input too long confusingly gives the error: free(): double free detected in tcache 2, and aborts.
  char should_be_empty[24]; // non-heap object, does not have to be freed!
  // this may or may not be the most cursed line I have written ever:
  // scan the input for the formula below, and check if sscanf could parse 6 arguments, which are the 6 decimals, if it could parse 7, then the string after the last ')' exists, which
  // makes the input unacceptable. There is no possiblilty for the case that we parsed 5 arguments correctly and pass a wrong string, because of the brackets seperating the last decimal and the string.
  if (sscanf(input, "(%d,%d),(%d,%d),(%d,%d)%s\n", &temp->point[0].x, &temp->point[0].y, &temp->point[1].x, &temp->point[1].y, &temp->point[2].x, &temp->point[2].y, &should_be_empty) != 6) {
    // return the nullpointer
    return NULL;
  }

  return temp;
}

// simple printer, prints the triangle as it should be parsed.
void print_triangle(struct triangle input){
  printf("(%d,%d),(%d,%d),(%d,%d)\n", input.point[0].x, input.point[0].y, input.point[1].x, input.point[1].y, input.point[2].x, input.point[2].y);
}


int main(int argc, char *argv[]) {
  // dec THREAD_NUM
  int THREAD_NUM;
  // buffer to detect wrong args
  char should_be_empty[24];

  if (argc != 2){
    printf("Wrong amount of arguments.\n");
    return 1;
  }
  // check if the string exists, then display error and exit otherwise init THREAD_NUM
  if (sscanf(argv[1], "%d%s", &THREAD_NUM, &should_be_empty) != 1){
    printf("Garbage arguments detected.\n");
    return 2;
  }

  // Init semaphore for threads with maximum access being 1.
  sem_init(&semaphore, 0, 1);

  // Init output thread
  pthread_t output_thread;

  printf("Starting output thread...\n");
  pthread_create(&output_thread, NULL, &printer, NULL);

  size_t buf_size = 32;
  char* line_buf = malloc(buf_size);

  for (; ;){
    // Fill the line buffer.
    getline(&line_buf, &buf_size, stdin);
    
    // Parse it.
    struct triangle* next = parse(line_buf);
    // Check for wrong input.
    if (next){
      printf("Parse completed, result is: ");
      print_triangle(*next);
    } else {
      // Display warning and continue (aka, skip this turn)
      printf("Parse failed, will ignore input\n");
      continue;
    }

    printf("Adding new triangle...\n");

    while (state.active >= THREAD_NUM || state.active < 0){
      // Wait
    }

    // ENTERING CRITICAL AREA
    sem_wait(&semaphore);
    state.active++;
    sem_post(&semaphore);
    // EXITING CRITICAL AREA
    
    // dec new thread
    pthread_t thread;
    // start it // TODO: CHECK FOR ERRORS
    pthread_create(&thread, NULL, &worker, next);
    // and detach it. // TODO: CHECK FOR ERRORS
    pthread_detach(thread);
    }

  // lol, I accidentally put the free in the while loop and wondered why I got a double free about before I could see "Parse completed", IO is slow...
  free(line_buf);


  return 1;
}
