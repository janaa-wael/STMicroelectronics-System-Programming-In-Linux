#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "HMM.h"

extern char simulated_heap[HEAP_SIZE];

/* Metadata Block Structure; Block contains info of data about the data */

struct block *List_Of_Free_Blocks = (void*)(simulated_heap);

static char *program_break = simulated_heap;

void initialize()
{
	/* the size of blthe ock that the metadata describes is the size of the heap - the size of the metadata block*/
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
	
	/*If the memory pool is not initialized, initialize() is called to set it up. */
	if(!(List_Of_Free_Blocks->size)){ 
		initialize();
		printf("Memory initialized\n");
	}
	curr = List_Of_Free_Blocks;
	while((((curr->size)<num_of_bytes)||((curr->free)==0))&&(curr->next!=NULL)){
		prev=curr;
		curr=curr->next;
		printf("One block checked\n");
	}
	
	if ((curr->size) == num_of_bytes) {
        curr->free = 0;
        result = (void*)(++curr);
        printf("Exact fitting block allocated\n");
        return result;
    } else if ((curr->size) > (num_of_bytes + sizeof(struct block))) {
        split(curr, num_of_bytes);
        result = (void*)(++curr);
        printf("Fitting block allocated with a split\n");
        return result;
    } else {
        result = NULL;
        printf("Sorry. No sufficient memory to allocate\n");
        return result;
    }
}


void merge() {
    struct block *curr, *prev;
    curr = List_Of_Free_Blocks;
    while ((curr->next) != NULL) {
        if ((curr->free) && (curr->next->free)) {
            curr->size += (curr->next->size) + sizeof(struct block);
            curr->next = curr->next->next;
        }
        prev = curr;
        curr = curr->next;
    }
}

void MyFree(void *ptr) {
	
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


