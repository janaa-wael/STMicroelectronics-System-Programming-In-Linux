#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include "HMM.h"

#define NUM_ALLOCS 10000
#define MAX_SIZE 10240
#define MAX_ITERATIONS 10000000

void random_alloc_realloc_calloc_test() {
    srand((unsigned int)time(NULL));
    
    void* pointers[NUM_ALLOCS] = {NULL};
    
    for (int i = 0; i < MAX_ITERATIONS; ++i) {
        int index = rand() % NUM_ALLOCS;
        size_t size = (size_t)(rand() % MAX_SIZE) + 1;
        
        // Randomly choose between allocation, reallocation, and freeing
        int operation = rand() % 4; // Added a fourth operation for realloc
        switch (operation) {
            case 0: // Allocate new memory
                if (pointers[index] == NULL) {
                    if (rand() % 2) {
                        // Use MyMalloc
                        pointers[index] = MyMalloc(size);
                        if (pointers[index] != NULL) {
                            printf("Allocated memory of size %zu at address %p using MyMalloc\n", size, pointers[index]);
                        } else {
                            fprintf(stderr, "Allocation failed for size %zu using MyMalloc\n", size);
                        }
                    } else {
                        // Use MyCalloc
                        pointers[index] = MyCalloc(1, size); // Allocate memory for an array of 1 element of size 'size'
                        if (pointers[index] != NULL) {
                            printf("Allocated zero-initialized memory of size %zu at address %p using MyCalloc\n", size, pointers[index]);
                        } else {
                            fprintf(stderr, "Allocation failed for size %zu using MyCalloc\n", size);
                        }
                    }
                }
                break;

            case 1: // Reallocate existing memory
                if (pointers[index] != NULL) {
                    size_t newSize = (size_t)(rand() % MAX_SIZE) + 1;
                    void* newPointer = MyRealloc(pointers[index], newSize);
                    if (newPointer != NULL) {
                        printf("Reallocated memory to size %zu at address %p (old address was %p) using MyRealloc\n", newSize, newPointer, pointers[index]);
                        pointers[index] = newPointer;
                    } else {
                        fprintf(stderr, "Reallocation failed for new size %zu\n", newSize);
                    }
                } else {
                    // If pointer is NULL, treat it as allocation
                    pointers[index] = MyMalloc(size);
                    if (pointers[index] != NULL) {
                        printf("Allocated memory of size %zu at address %p using MyMalloc\n", size, pointers[index]);
                    } else {
                        fprintf(stderr, "Allocation failed for size %zu using MyMalloc\n", size);
                    }
                }
                break;

            case 2: // Free existing memory
                if (pointers[index] != NULL) {
                    printf("Freeing memory at address %p\n", pointers[index]);
                    MyFree(pointers[index]);
                    pointers[index] = NULL;
                } else {
                    // If pointer is NULL, treat it as allocation
                    pointers[index] = MyMalloc(size);
                    if (pointers[index] != NULL) {
                        printf("Allocated memory of size %zu at address %p using MyMalloc\n", size, pointers[index]);
                    } else {
                        fprintf(stderr, "Allocation failed for size %zu using MyMalloc\n", size);
                    }
                }
                break;

            case 3: // Test MyCalloc
                if (pointers[index] == NULL) {
                    pointers[index] = MyCalloc(1, size);
                    if (pointers[index] != NULL) {
                        printf("Allocated zero-initialized memory of size %zu at address %p using MyCalloc\n", size, pointers[index]);
                        // Verify zero-initialization
                        unsigned char* bytePtr = (unsigned char*)pointers[index];
                        for (size_t j = 0; j < size; ++j) {
                            if (bytePtr[j] != 0) {
                                fprintf(stderr, "Memory at %p is not zero-initialized\n", pointers[index]);
                                break;
                            }
                        }
                    } else {
                        fprintf(stderr, "Allocation failed for size %zu using MyCalloc\n", size);
                    }
                }
                break;
        }
    }
    
    // Free remaining allocated memory
    for (int i = 0; i < NUM_ALLOCS; ++i) {
        if (pointers[i] != NULL) {
            printf("Freeing remaining memory at address %p\n", pointers[i]);
            MyFree(pointers[i]);
            pointers[i] = NULL;
        }
    }
}

void fill_memory(void* ptr, size_t size, char pattern) {
    if (ptr != NULL) {
        memset(ptr, pattern, size);
    }
}

// Test function to repeatedly allocate and reallocate memory
void test_realloc(int num_iterations, size_t initial_size, size_t increment_size) {
    void* ptr = NULL;

    for (int i = 0; i < num_iterations; ++i) {
        // Allocate initial block
        if (ptr == NULL) {
            ptr = MyMalloc(initial_size);
            if (ptr == NULL) {
                perror("MyMalloc failed");
                return;
            }
            fill_memory(ptr, initial_size, 'A' + i % 26); // Fill with pattern
        } else {
            // Reallocate the block with increased size
            size_t new_size = initial_size + (i + 1) * increment_size;
            ptr = MyRealloc(ptr, new_size);
            if (ptr == NULL) {
                perror("MyRealloc failed");
                return;
            }
            fill_memory(ptr, new_size, 'A' + i % 26); // Fill with pattern
        }

        // Debug the state of the heap
        debug_heap();
    }

    // Free the allocated block
    MyFree(ptr);
    printf("Freed memory.\n");

    // Final debug of heap state
    debug_heap();
}
/*
int main() {
printf("HEllo");
    // Example parameters

    size_t initial_size = 128;       // Initial size of the block
    size_t increment_size = 64;      // Increment size for reallocation

    // Test the realloc function
    test_re    int num_iterations = 10;alloc(num_iterations, initial_size, increment_size);

    return 0;
}*/

int main() { 
    initialize();
    printf("Starting random allocation, reallocation, and deallocation test...\n");
    random_alloc_realloc_calloc_test();
    printf("Test complete.\n");
    return 0;
}

