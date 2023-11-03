#include <stdio.h>
#include <stdlib.h>


// The data that is stored within a node of the doubly-linked list
struct Data {
    int id;
    int length;
};

// Struct for the nodes of the doubly-linked list
struct Node {
    struct Node* former;
    struct Data data;
    struct Node* next;
};

// Define the structure of the doubly-linked list
// TODO: implement me
struct DoublyLinkedList {
    struct Node* first;
    struct Node* last;
};

// Function to create a new node
// Return a pointer to the node
// Exit if allocation fails.
struct Node* createNode(struct Data data){
    // Allocate
    struct Node* temp = malloc(sizeof(struct Node));
    // Check if NULL
    if (temp == NULL){
        exit(1);
    }
    
    // Set values
    temp->data = data;
    temp->former = NULL;
    temp->next = NULL;

    return temp;
}

// Function to initialize a doubly-linked list with NULL
void initializeDoublyList(struct DoublyLinkedList* list){
    list->first = NULL;
    list->last = NULL;
}

void printHelperForward(struct Node* currentNode){
    if (currentNode == NULL)
    {
        return;
    } else {
        printf("ID: %d, Length: %d\n",currentNode->data.id, currentNode->data.length);
        printHelperForward(currentNode->next);
    }
    
}

// Function to print the doubly-linked list in forward direction
void printListForward(const struct DoublyLinkedList* list){
    printHelperForward(list->first);
}

void printHelperReverse(struct Node* currentNode){
    if (currentNode == NULL)
    {
        return;
    } else {
        printf("ID: %d, Length: %d\n",currentNode->data.id, currentNode->data.length);
        printHelperReverse(currentNode->former);
    }
    
}

// Function to print the doubly-linked list in reverse direction
void printListReverse(const struct DoublyLinkedList* list){
    printHelperReverse(list->last);
}

void freeHelper(struct Node* currentNode){

    printf("Pointer to be freed: %p\n", (void* )currentNode);
        
    // Copy the pointer to the next node
    struct Node* next = currentNode->next;

    if (currentNode != NULL)
        free(currentNode);

    if (next != NULL){
        freeHelper(next);
    }
    
}

// Function to free the memory allocated for the doubly-linked list
void freeDoublyList(struct DoublyLinkedList* list){
    freeHelper(list->first);
    printf("Internal list freed\n");
}

// Function to insert a node at the end of the doubly-linked list
// A list cannot be full so this operation theoretically cannot fail.

void append(struct DoublyLinkedList* list, struct Data data){
    if (list->last == NULL || list->first == NULL){
        list->last = createNode(data);
        list->first = list->last;
        return;
    }

    struct Node* newLast = createNode(data);
    struct Node* oldLast = list->last;

    oldLast->next = newLast;
    newLast->former = oldLast;
    
    list->last = newLast;
}

// Function to insert a node at the head of the doubly-linked list
void appendLeft(struct DoublyLinkedList* list, struct Data data){
    if (list->last == NULL || list->first == NULL){
        list->last = createNode(data);
        list->first = list->last;
        return;
    }

    struct Node* newFirst = createNode(data);
    struct Node* oldFirst = list->first;

    oldFirst->former = newFirst;
    newFirst->next = oldFirst;

    list->first = newFirst;
}

// Pops the current head of the linked list
// Stores the popped data in the variable pointed to by popped
// Return 0 if the popping worked, 1 if you tried to pop from an empty list (do not exit)
int pop(struct DoublyLinkedList* list, struct Data* popped){
    // is the list empty?
    if (list == NULL || list->first == NULL || list->last == NULL){
        return 1;
    }

    // Copy the value into popped
    // printf("%d",list->last->data.id);

    *popped = list->first->data;

    
    // Copy the adress to the last element
    struct Node* oldFirst = list->first;
    struct Node* newFirst = list->first->next;


    // New last element is the former from the popped one

    list->first = newFirst;

    free(oldFirst);

    return 0;
}


int popAndPrintAll(struct DoublyLinkedList* list){
    struct Data* currElem = malloc(sizeof(struct Data));
    
    // pop -> 0 (= false) when succeding
    while (!pop(list, currElem))
    {
        printf("ID: %d, Length: %d\n",currElem->id, currElem->length);
    }

    return 0;
}


// LinkedList.c
// The data that is stored within a node of the doubly-linked list

// Example main
int main() {
    struct DoublyLinkedList list;
    initializeDoublyList(&list);

    // Insert some nodes at the beginning
    struct Data data1 = {1, 11};
    struct Data data2 = {2, 25};
    struct Data data3 = {3, 55};
    struct Data data4 = {0, 1};

    appendLeft(&list, data1);
    appendLeft(&list, data2);
    appendLeft(&list, data3);
    append(&list, data4);

    // Print the doubly-linked list in forward and reverse directions
    printListForward(&list);
    printf("_________________\n");
    printListReverse(&list);
    
    printf("_________________\n");

    popAndPrintAll(&list);
    printf("Popping complete\n");
    
    appendLeft(&list, data1);
    

    // Segementation fault
    freeDoublyList(&list);
    
    printf("Complete\n");
    // TODO: Complete
    return 0;
}