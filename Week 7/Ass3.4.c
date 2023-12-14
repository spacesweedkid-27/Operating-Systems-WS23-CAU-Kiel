#include <stdio.h>    // IO such as println and puts
#include <stdlib.h>   // malloc, free and more
#include <pthread.h>  // POSIX threads and useful functions for them
#include <sys/mman.h> // mmap for shared memory

// to supress warnings
int fork();
pid_t wait();


void* create_shared_memory(size_t size) {
    // Our memory buffer will be readable and writable:
    int protection = PROT_READ | PROT_WRITE;

    // and be readable by other processes, but only in this group
    int visibility = MAP_SHARED | MAP_ANONYMOUS;

    // actually allocate the memory
    return mmap(NULL, size, protection, visibility, -1, 0);
}

/* 
The input for our threads as a struct since thread_create expects exactly one input
 Having a typedef makes it a bit more comfortable to use*/
typedef struct Thread_input{
    // We have to divide the array somehow between all threads
    int lower_bound, upper_bound;
    // Store the result of the computation in res
    long res;
    // The reference to the array
    long *arr;
} Thread_input;

/* 
\brief The start routine for all our threads
@param thread_input void pointer as  sole argument
@return The start routine has to return a void pointer - just return NULL in this task :)
*/
void *thread_routine(void *thread_input){
    Thread_input* converted = (Thread_input*)thread_input;  // cast to right type
    long tempsum = 0;   // store a temporary sum
    
    for (int index = converted->lower_bound; index < converted->upper_bound; index++){
        tempsum += converted->arr[index];   // add all wanted elements
    }

    converted->res += tempsum;

    return NULL;    // I want to know why, but god said no.
}

/*
\brief Sums an array in a threaded fashion using a passed number of threads.
@param thread_num the number of threads
@param input_arr The array that has to be summed
@return the sum
*/
long threaded_sum(int thread_num, long input_arr[], size_t array_len){
    if (thread_num < 1){
        return 1;
    }


    // set passed thread_num to array_len if it is too big.
    if ((int)array_len < thread_num){
        thread_num = (int)array_len;
    }

    // devide input_arr into thread_num segments
    int arr_width = array_len / thread_num;

    // give the last segment more size if it was not divisible
    int remainder = array_len % thread_num;

    // allocate shared memory to store the results
    long* sums = create_shared_memory(sizeof(long) * thread_num);

    // start thread_num threads
    for (int i = 0; i < thread_num; ++i)
    {
        pid_t pid = fork();

        if (pid == 0){
            Thread_input thread_input = {i * arr_width, (i+1) * arr_width, 0, input_arr};

            // If we're at the last thread-job, then make it's job a bit bigger.
            if (i == thread_num - 1){
                thread_input.upper_bound += remainder;   
            }

            thread_routine(&thread_input);
            printf("Thread finished with result %ld\n", thread_input.res);
            sums[i] = thread_input.res; // let's see if this works
            exit(0);
        }

        if (pid == -1){
            perror("Something bad happened. Failed to create new thread.\n");
            return -1;
        } else {
            printf("Started thread %d out of %d with pid %d\n", i + 1, thread_num, pid);
        }
    }
    
    // wait for all children to die
    for (int i = 0; i < thread_num; ++i)
        wait(NULL);

    long temp = 0;

    // sum all sums to temp
    for (int i = 0; i < thread_num; ++i){
        temp += sums[i];
    }

    return temp;
}


int main(){
    long arr[] = {1,2,3,4,5,6,7,8,9,10};
    printf("Sum of arr = %ld\n", threaded_sum(3, arr, 10));
}