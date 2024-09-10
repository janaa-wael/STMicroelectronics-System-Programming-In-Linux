#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "HMM.h"
#include <unistd.h>
// Define constants
#define NUM_ALLOCS 100
#define MAX_ITERATIONS 1000
#define MAX_SIZE 1024

// Fill memory with a specific pattern
void fill_memory(void* ptr, size_t size, unsigned char pattern) {
    if (ptr != NULL) {
        memset(ptr, pattern, size);
    }
}

// Check memory content against an expected pattern
int check_memory(void* ptr, size_t size, unsigned char pattern) {
    if (ptr == NULL) return 0;
    unsigned char* bytePtr = (unsigned char*)ptr;
    for (size_t i = 0; i < size; ++i) {
        if (bytePtr[i] != pattern) {
            return 0; // Memory content doesn't match the expected pattern
        }
    }
    return 1; // Memory content matches the expected pattern
}

// Print memory content for debugging
void print_memory(void* ptr, size_t size) {
    if (ptr == NULL) return;
    unsigned char* bytePtr = (unsigned char*)ptr;
    printf("Memory content at %p:", ptr);
    for (size_t i = 0; i < size; ++i) {
        printf(" %02X", bytePtr[i]);
    }
    printf("\n");
}

// Stress test function
void random_alloc_realloc_calloc_test() {
    srand((unsigned int)time(NULL));
    
    void* pointers[NUM_ALLOCS] = {NULL};
    size_t sizes[NUM_ALLOCS] = {0};  // Array to store the size of each allocation
    
    for (int i = 0; i < MAX_ITERATIONS; ++i) {
        int index = rand() % NUM_ALLOCS;
        size_t size = (size_t)(rand() % MAX_SIZE) + 1;
        
        // Randomly choose between allocation, reallocation, and freeing
        int operation = rand() % 4;
        switch (operation) {
            case 0: // Allocate new memory
                if (pointers[index] == NULL) {
                    if (rand() % 2) {
                        // Use MyMalloc
                        pointers[index] = MyMalloc(size);
                        if (pointers[index] != NULL) {
                            sizes[index] = size;
                            fill_memory(pointers[index], size, 0xAA);
                            printf("Allocated memory of size %zu at address %p using MyMalloc\n", size, pointers[index]);
                            print_memory(pointers[index], size);
                        } else {
                            fprintf(stderr, "Allocation failed for size %zu using MyMalloc\n", size);
                        }
                    } else {
                        // Use MyCalloc
                        pointers[index] = MyCalloc(1, size);
                        if (pointers[index] != NULL) {
                            sizes[index] = size;
                            // Verify zero-initialization
                            if (check_memory(pointers[index], size, 0)) {
                                printf("Allocated zero-initialized memory of size %zu at address %p using MyCalloc\n", size, pointers[index]);
                                print_memory(pointers[index], size);
                            } else {
                                fprintf(stderr, "Memory at %p is not zero-initialized\n", pointers[index]);
                            }
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
                        sizes[index] = newSize;
                        // Check memory integrity
                        if (!check_memory(newPointer, newSize, 0xAA)) {
                            fprintf(stderr, "Memory corruption detected during realloc\n");
                        }
                        printf("Reallocated memory to size %zu at address %p (old address was %p) using MyRealloc\n", newSize, newPointer, pointers[index]);
                        pointers[index] = newPointer;
                        print_memory(pointers[index], newSize);
                    } else {
                        fprintf(stderr, "Reallocation failed for new size %zu\n", newSize);
                    }
                } else {
                    // If pointer is NULL, treat it as allocation
                    pointers[index] = MyMalloc(size);
                    if (pointers[index] != NULL) {
                        sizes[index] = size;
                        fill_memory(pointers[index], size, 0xAA);
                        printf("Allocated memory of size %zu at address %p using MyMalloc\n", size, pointers[index]);
                        print_memory(pointers[index], size);
                    } else {
                        fprintf(stderr, "Allocation failed for size %zu using MyMalloc\n", size);
                    }
                }
                break;

            case 2: // Free existing memory
                if (pointers[index] != NULL) {
                    printf("Freeing memory at address %p\n", pointers[index]);
                    // Verify memory content before freeing
                    if (!check_memory(pointers[index], sizes[index], 0xAA)) {
                        fprintf(stderr, "Memory corruption detected before freeing\n");
                    } else {
                        print_memory(pointers[index], sizes[index]);
                    }
                    MyFree(pointers[index]);
                    pointers[index] = NULL;
                    sizes[index] = 0;
                } else {
                    // If pointer is NULL, treat it as allocation
                    pointers[index] = MyMalloc(size);
                    if (pointers[index] != NULL) {
                        sizes[index] = size;
                        fill_memory(pointers[index], size, 0xAA);
                        printf("Allocated memory of size %zu at address %p using MyMalloc\n", size, pointers[index]);
                        print_memory(pointers[index], size);
                    } else {
                        fprintf(stderr, "Allocation failed for size %zu using MyMalloc\n", size);
                    }
                }
                break;

            case 3: // Test MyCalloc
                if (pointers[index] == NULL) {
                    pointers[index] = MyCalloc(1, size);
                    if (pointers[index] != NULL) {
                        sizes[index] = size;
                        // Verify zero-initialization
                        if (check_memory(pointers[index], size, 0)) {
                            printf("Allocated zero-initialized memory of size %zu at address %p using MyCalloc\n", size, pointers[index]);
                            print_memory(pointers[index], size);
                        } else {
                            fprintf(stderr, "Memory at %p is not zero-initialized\n", pointers[index]);
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
            // Verify memory content before freeing
            if (!check_memory(pointers[i], sizes[i], 0xAA)) {
                fprintf(stderr, "Memory corruption detected before freeing\n");
            } else {
                print_memory(pointers[i], sizes[i]);
            }
            MyFree(pointers[i]);
            pointers[i] = NULL;
            sizes[i] = 0;
        }
    }
}

int main()
{
	printf("Starting the heap memory manager test...\n");
    
    // Run the stress test
    random_alloc_realloc_calloc_test();
    
    printf("Heap memory manager test completed.\n");
    return 0;
}

/*
int main() {
	initialize();
	debug_heap();
    // Run the stress test
    char* ptr1 = MyMalloc(10);
    for(int i = 0 ; i < 10 ; i++)
    {
    	*(ptr1+i) = i;
    	
    }
    char*ptr2 = ptr1;
    for(char i = 0 ; i < 10 ; i++)
    {
    	printf("\nelement %d : %d",i,*(ptr1+i));
    } 
    //debug_heap();
    ptr2 = MyCalloc(2,4);
    //debug_heap();
    for(char i = 0 ; i < 8 ; i++)
    {
    	*(ptr2+i) = 'A' + i;
    	
    }
    for(char i = 0 ; i < 8 ; i++)
    {
    	printf("\nelement %d : %c",i,*(ptr2+i));
    } 
    for(char i = 0 ; i < 10 ; i++)
    {
    	printf("\nelement %d : %d",i,*(ptr1+i));
    } 
    MyFree(ptr1);
    for(char i = 0 ; i < 8 ; i++)
    {
    	printf("\nelement %d : %c",i,*(ptr2+i));
    } 
    //debug_heap();
    MyFree(ptr2);
    //debug_heap();
    return 0;
}*/

