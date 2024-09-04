#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <limits.h>
#include <assert.h>
#include "HMM.h"
#include <unistd.h>

struct block *List_Of_Free_Blocks = NULL; /* Initialized to NULL */


void split(struct block *fitting_slot, size_t size) {

    // Ensure the fitting_slot has enough space to split
    if (fitting_slot->size <= size + sizeof(struct block)) {
        return; // Not enough space to split
    }

    // Calculate the address of the new block
    struct block *new_block = (struct block *)((char *)fitting_slot + sizeof(struct block) + size);

    // Ensure the new block address is within the allocated memory
    if ((char *)new_block >= (char *)fitting_slot + fitting_slot->size) {
        return; // Invalid split, would result in out-of-bounds memory access
    }

    // Set up the new block's metadata
    new_block->size = fitting_slot->size - size - sizeof(struct block);
    new_block->free = 1;
    new_block->next = fitting_slot->next;

    // Update the fitting_slot to reflect the allocated size
    fitting_slot->size = size;
    fitting_slot->free = 0;
    fitting_slot->next = new_block;
}



void* MyMalloc(size_t num_of_bytes) {
    struct block* curr = List_Of_Free_Blocks;
    struct block* prev = NULL;
    void* result = NULL;
	if(num_of_bytes%8 != 0)
		num_of_bytes = ALIGN(num_of_bytes);
    // Initialize heap if not already initialized
    if (List_Of_Free_Blocks == NULL) 
    { //>>>>>>>>>>>>>>>>>>>>>
        void* heap_start = sbrk(PROG_BRK_INC);
        if (heap_start == (void *)-1) {
            perror("sbrk failed");
            return NULL;
        }

        // Initialize the first block
        List_Of_Free_Blocks = (struct block*)heap_start;
        List_Of_Free_Blocks->size = PROG_BRK_INC - sizeof(struct block);
        List_Of_Free_Blocks->free = 1;
        List_Of_Free_Blocks->next = NULL;
    }

    curr = List_Of_Free_Blocks;

    // Traverse the free list to find a suitable block
    while (curr != NULL) 
    {    	
        // Check if the current block is large enough and free
        if ((curr->size >= num_of_bytes) && curr->free) {
            break;  // Suitable block found
        }

        prev = curr;
        curr = curr->next;
    }

    // If no suitable block was found, request more memory
    if (curr == NULL) 
    {
        void* heap_start = sbrk(sizeof(struct block) + num_of_bytes + PROG_BRK_INC);
        if (heap_start == (void *)-1) {
            perror("sbrk failed");
            return NULL;
        }

        // Initialize the new block
        curr = (struct block*)heap_start;
        curr->size = num_of_bytes;
        curr->free = 0;
        
	struct block * next = (struct block*)((char*)curr + sizeof(struct block) + curr->size);
	
	curr->next = next;
	
	next->free = 1;
	next->size = PROG_BRK_INC - sizeof(struct block);
	next->next = NULL;
        // Add the new block to the free list
        if (prev != NULL) {
            prev->next = curr;
        } else {
            List_Of_Free_Blocks = curr;
        }
        return (void*)(curr+1);
    }

    // If the block is a perfect fit
    if (curr->size == num_of_bytes && curr->free) 
    {
        curr->free = 0;
        result = (void*)(curr + 1);  // Return memory after the block header
    } 
    // If the block is larger, split it
    else if (curr->size > num_of_bytes + 2*sizeof(struct block)) {
        split(curr, num_of_bytes);
        curr->free = 0;
        result = (void*)(curr + 1);  // Return memory after the block header
    } 
    // If the block is too small, return NULL (shouldn't happen due to previous checks)
    else {
        result = NULL;
    }

    return result;
}


void merge() {
    struct block *curr, *prev;

    curr = List_Of_Free_Blocks;
    prev = NULL;

    if (curr == NULL) {
        // No blocks to merge
        return;
    }

    while (curr != NULL) {
        // Check if both blocks are free and adjacent
        if (curr->free && curr->next != NULL && curr->next->free) {
            // Ensure we don't have size overflow
            if (curr->size > SIZE_MAX - (curr->next->size + sizeof(struct block))) {
                prev = curr;
                curr = curr->next; // Move to next block
                continue;
            }

            // Merge the blocks
            curr->size += curr->next->size + sizeof(struct block);
            curr->next = curr->next->next;
        } else {
            // Move to the next block
            prev = curr;
            curr = curr->next;
        }
    }
}


void MyFree(void *ptr) {
	
	
	if(ptr == NULL)
	{
		return;
	}
	struct block* metadata_block = (struct block*)ptr - 1;
	
	if(metadata_block->free == 1)
	{
		return;
	}
	metadata_block->free = 1;
	
    	/* Verifies if the pointer provided is within the range of the memory pool. */
	if(((void*)List_Of_Free_Blocks <= ptr) && (ptr <= (void*)sbrk(0))) 
	{
        	struct block* curr = ptr;
        	--curr;
        	curr->free = 1;
        	merge();
    	} 
}


void debug_heap() {
    struct block* curr = (struct block*)List_Of_Free_Blocks;
    int block_number = 0;

    printf("Heap Debugging Information:\n");
    printf("-----------------------------------------------------------------\n");
    printf("| Block # | Address      | Size        | Free/Allocated | Next Address |\n");
    printf("-----------------------------------------------------------------\n");

    while (curr != NULL) 
    {
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


void* MyCalloc(size_t nmemb, size_t size)
{
	size = ALIGN(size);
\
	if(nmemb == 0 || size == 0)
		return malloc(sizeof(struct block));
	else if(nmemb != 0 && size > SIZE_MAX / nmemb)
	{
		void* ptr = malloc(nmemb*size);
		struct block* metadata_block = (struct block*)((char*)ptr - sizeof(struct block));
		if(ptr != NULL)
		{
			memset(ptr, 0, nmemb*size);
			return ptr;
		}
		else 
			return NULL;
	}
}

void* MyRealloc(void* ptr, size_t size)
{
	size = ALIGN(size);
	if((ptr == NULL || ptr< (void*)List_Of_Free_Blocks || ptr > (void*)sbrk(0)) && size!=0)
	{
		return malloc(size);
	}
	else if((ptr == NULL || ptr< (void*)List_Of_Free_Blocks || ptr > (void*)sbrk(0))  && size ==0)
	{
		size = sizeof(struct block);
		return malloc(size);
	}	
	else if(size == 0 && ptr!= NULL)
	{
		MyFree(ptr);
		return NULL;
	}
	else if (size == ((struct block*)ptr - 1)->size) {
        	return ptr; // No need to reallocate
    	}
	else
	{
		if(ptr <= (void*)sbrk(0) && (ptr >= (void*)List_Of_Free_Blocks))
		{			
			struct block* metadata_block = (struct block*)((char*)ptr - sizeof(struct block));
			 if (metadata_block == NULL || metadata_block->size == 0 ) 
			 {
        	    		fprintf(stderr, "Invalid metadata block or size.\n");
        	    		return NULL;
        		}
			void* ptr2 = malloc(size);
			if(ptr2 != NULL)
			{
				memcpy(ptr2,ptr, metadata_block->size < size ? metadata_block->size : size);
				MyFree(ptr);
				return ptr2;
			}
			else
			{
				return NULL;
			}
		}		
	}
	return ptr;
}

void* malloc(size_t size)
{
	if(size ==0)
		size = sizeof(struct block);
		void* ptr = MyMalloc(size);
	return ptr;	
}

void free(void* ptr)
{
	MyFree(ptr);	
}
void* calloc(size_t nmemb, size_t size)
{
	return MyCalloc(nmemb,size);
}

void *realloc(void * ptr, size_t size)
{
	return MyRealloc(ptr, size);
}


