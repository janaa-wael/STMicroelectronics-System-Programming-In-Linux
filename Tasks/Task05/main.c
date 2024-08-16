#include <stdio.h>
#include <stdlib.h>
#include "HMM.h"

char simulated_heap;

void debug_heap() {
    struct block* curr = (struct block*)simulated_heap;
    int block_number = 0;

    printf("Heap Debugging Information:\n");
    printf("-----------------------------------------------------------------\n");
    printf("| Block # | Address      | Size        | Free/Allocated | Next Address |\n");
    printf("-----------------------------------------------------------------\n");

    while (curr != NULL) {
        printf("| %7d | %12p | %11zu | %14s | %12p |\n", 
               block_number,
               (void*)curr,
               curr->size,
               curr->free ? "Free" : "Allocated",
               (void*)curr->next);
        curr = curr->next;
        block_number++;
    }

    printf("-----------------------------------------------------------------\n");
}

int main()
{
	// Initialize the heap
    initialize();

    // Perform allocations, deallocations, and debugging
    void* ptr1 = MyMalloc(100);
    void* ptr2 = MyMalloc(200);

    debug_heap();

    MyFree(ptr1);

    debug_heap();
}