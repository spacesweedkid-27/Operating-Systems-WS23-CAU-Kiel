#include <stdio.h> // changed

// f is meant to initialise an as pointer declared array
void f (int **ptr) { // changed
    int ar[] = {1,2,3,4};
    *ptr = ar; // changed
}

int main() {
    int *arr;
    f(&arr); // changed
    // Print all the elements of the array 
    for(int i=0; i < 4; i++) { //changed
        int d = arr[i];
        printf("The %d-th element is %d.\n", i, d); // changed
    } //changed
    return 0; // changed
}



