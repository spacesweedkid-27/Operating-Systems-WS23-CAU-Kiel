#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

// Define the struct for the data
struct Data {
    int id;
    const char* name;
    int amount;
};

// Function for setting up the array and filling it with example data
// Note that binary search requires that the data is sorted
// It should exit if the allocation fails
// Otherwise it should return the array
struct Data* setup_array(int length){
    // allocate the needed memory
    struct Data* temp = malloc(sizeof(struct Data) * length);
    // If malloc returns the nullpointer,
    // then we failed to allocate enough memory so exit
    if (temp == NULL){
        exit(1);
    }

    // go through each struct Data and set example values
    for (int i = 0; i < length; ++i){
        temp[i].id = i;
        temp[i].name = "Examplevalues";
        temp[i].amount = -i;
    }
    // return the pointer to the first cell of the array
    return temp;
}

// Fill the array with data (sorted by id)
void fill_array(struct Data* array){
    // You may change this to your liking :)
    array[0] = (struct Data){4, "Slipper", 10};
    array[1] = (struct Data){10, "Glove", 15};
    array[2] = (struct Data){22, "Window Cleaner", 20};
    array[3] = (struct Data){23, "Apple", 8};
    array[4] = (struct Data){26, "Frozen Pizza", 12};
}


// Binary search function
// It should return the amount of search steps a search took
// Or -1 if the element is not in the array
// If the element is found it should be stored in the variable pointed to by found_element
// Otherwise assign found_element with an instance of the struct filled with 0, NULL, 0
int binary_search(struct Data* arr, int length, int target_id, struct Data* found_element) {
    int steps = 0;
    int low = 0;
    int high = length;
    int mid = low + (high - low) / 2;
    int oldmid = -1;

    while (low != high && low - high != 1 && oldmid != mid)
    {
        oldmid = mid;

        printf("Steps: %d, low: %d, high: %d\n", steps, low, high);
        steps++;
        if (arr[mid].id < target_id){
            low = mid;
            mid = low + (high - low) / 2; 
        } else if (arr[mid].id > target_id){
            high = mid;
            mid = low + (high - low) / 2; 
        } else {
            *found_element = arr[mid];
            return steps;
        }
    }
    
    // just an example
    // *found_element = arr[0];


    // well we failed to find it, so do the thing and return -1
    found_element = NULL;
    return -1;
}


int main(int argc, char** argv) {

    // If the user fails to pass one argument, exit
    if (argc != 2){
        printf("%s <target_id>\n", argv[0]);
        return 1;
    }

    int isnum = 1;
    // you have to use strlen, because sizeof argv[n] always returns 8, don't ask me why
    for (long unsigned int i = 0; i < strlen(argv[1]); ++i){
        // check if digits are being used, if not return 0
        if (argv[1][i] > '9' || argv[1][i] < '0'){
            isnum = 0;
        }
    }

    if (!isnum){
        printf("%s <target_id>\n", argv[0]);
        return 1;
    }

    int target_id = atoi(argv[1]);
    printf("Target ID: %d\n", target_id);

    // Adjust the length if you change the array length
    int array_length = 5;
    struct Data* data_array = setup_array(array_length);
    
    fill_array(data_array);

    // printDataArr(data_array, array_length);

    struct Data found_element;
    int steps = binary_search(data_array, array_length, target_id, &found_element);

    // We don't need the space for data_array anymore.
    // BUT ONLY IF &found_element IS NOT IN THE ALLOCATED SPACE FOR data_array
    free(data_array);

    if (steps < 0) {
        printf("Element could not be found...\n");
        return 1;
    }

    printf("Steps: %d\n", steps);

    printf("id: %d\nName: %s\nAmount: %d\n", found_element.id, found_element.name, found_element.amount);
    

    // TODO: Implement printing the result of the search
    // If nothing is found this should be printed
    // If the element is found print its content and print how many steps were needed to find it.
    // Remember dealing with memory

    return 0;
}