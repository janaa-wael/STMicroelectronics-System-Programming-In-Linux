#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <limits.h>
#include <assert.h>
#include "HMM.h"

char simulated_heap[HEAP_SIZE];
struct block *List_Of_Free_Blocks = (void*)(simulated_heap); /* Metadata Block Structure; Block contains info of data about the data */
static char *program_break = simulated_heap; /* Program break initially points to the beginning of the HEAP */

void initialize()
{
	/* the size of the block that the metadata describes is the size of the heap - the size of the metadata block*/
	List_Of_Free_Blocks -> size = HEAP_SIZE - sizeof(struct block);
	
	/* The free flag = 1 indicates that the block is initially free */
	List_Of_Free_Blocks -> free = 1;
	
	/* The metadata block pointer doesn't point to another metadata block because there's no other blocks are allocated yet */
	List_Of_Free_Blocks -> next = NULL;
}



/*
 * If we find a free block which exactly fits the required size, we don't need to do the splitting. So this function is  
 * only required if we have what is more than required.
 * The arguments accepted by this function are --- A pointer to the metadata block which refers to the block of memory 
 * whose size is more than required.(fitting_slot) and the required size of the memory chunk to be allocated.
 */
void split(struct block *fitting_slot,size_t size)
{
	/* new is pointer to a new metadata block. It points to the location provided by passing a block of memory which is equal to the size of metadata block + the size requested to be allocated */
	struct block *new=(void*)((void*)fitting_slot+size+sizeof(struct block));
	new->size=(fitting_slot->size)-size-sizeof(struct block);
	new->free=1;
	new->next=fitting_slot->next;
	fitting_slot->size=size;
	fitting_slot->free=0;
	fitting_slot->next=new;
}


void* MyMalloc(size_t num_of_bytes)
{
	struct block* curr, *prev;
	void *result;
	
	
	if(num_of_bytes == 0)
		return NULL;
		
	/* Handling corner case of free list not being initialized. */	
	if(List_Of_Free_Blocks == NULL)
	{
		//printf("List of free blocks isn't initialized.\n");
		return;
	}
	
	/*If the memory pool is not initialized, initialize() is called to set it up. */
	if(!(List_Of_Free_Blocks->size)){ 
		initialize();
		//printf("Memory initialized\n");
	}
	
	curr = List_Of_Free_Blocks;
	while(num_of_bytes > ProgBrkOffset())
	{
		if(program_break + PROGRAM_BREAK_INCREMENT < program_break)
		{
			//printf("Overflow flow in the value of program break");
			return NULL;
		}
		program_break += PROGRAM_BREAK_INCREMENT;
	}
	
	while((((curr->size)<num_of_bytes)||((curr->free)==0))&&(curr->next!=NULL))
	{
		prev=curr;
		curr=curr->next;
		//printf("One block checked\n");
	}
	
	if ((curr->size) == num_of_bytes) 
	{
        	curr->free = 0;
        	result = (void*)(++curr);
        	//printf("Exact fitting block allocated\n");
        	return result;
    	} 
    	else if((curr->size) > (num_of_bytes + sizeof(struct block))) 
    	{
        	split(curr, num_of_bytes);
        	result = (void*)(++curr);
        	//printf("Fitting block allocated with a split\n");
        	return result;
    	} 
    	else 
    	{
        	result = NULL;
        	//printf("Sorry. No sufficient memory to allocate\n");
        	return result;
    	}
}


void merge() {
    struct block *curr, *prev;
    curr = List_Of_Free_Blocks;
    if (curr == NULL) {
        // No blocks to merge
        return;
    }

    while (curr != NULL && curr->next != NULL) {
        // Ensure pointers are valid
        assert(curr != NULL);
        assert(curr->next != NULL);

        //printf("Current block: size = %zu, free = %d\n", curr->size, curr->free);
        //printf("Next block: size = %zu, free = %d\n", curr->next->size, curr->next->free);

        // Check if both blocks are free
        if (curr->free && curr->next->free) {
            // Ensure we don't have size overflow
            if (curr->size > SIZE_MAX - (curr->next->size + sizeof(struct block))) {
                printf("Size overflow detected. Skipping merge.\n");
                curr = curr->next; // Move to next block
                continue;
            }

            // Merge the blocks
            curr->size += (curr->next->size) + sizeof(struct block);
            curr->next = curr->next->next;
        } else {
            // Move to the next block
            prev = curr;
            curr = curr->next;
        }
    }
}

void MyFree(void *ptr) {
	
	if(isBlockAllocated(ptr)==0)
	{
		printf("Block already freed.\n");
		return;
	}
	if(ptr == NULL)
		printf("Pointer is NULL\n");
	if(((struct block*)ptr)->free == 1)
	{
		printf("Memory block isn't allocated. It's already free");
	}
    /* Verifies if the pointer provided is within the range of the memory pool. */
	if(((void*)simulated_heap <= ptr) && (ptr <= (void*)(simulated_heap + HEAP_SIZE))) {
        struct block* curr = ptr;
        --curr;
        curr->free = 1;
        merge();
    } else {
        printf("Please provide a valid pointer allocated by MyMalloc\n");
    }
}

int isBlockAllocated(void* ptr)
{
	struct block* metadata_block = simulated_heap;
	
	
	while(metadata_block != NULL)
	{
		void* block_start = (char*)metadata_block + sizeof(struct block);
		if(block_start == ptr)
			return 1;
		
		metadata_block = metadata_block -> next;
		
	}
	return 0;
}

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

int ProgBrkOffset()
{
	return (program_break - simulated_heap);
}
