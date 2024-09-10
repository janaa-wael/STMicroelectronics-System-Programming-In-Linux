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
	
		
	/* Handling corner case of free list not being initialized. */	
	if(List_Of_Free_Blocks == NULL)
	{
		//printf("List of free blocks isn't initialized.\n");
		return NULL;
	}
	
	/*If the memory pool is not initialized, initialize() is called to set it up. */
	if(!(List_Of_Free_Blocks->size)){ 
		initialize();
		//printf("Memory initialized\n");
	}
	
	
	while(num_of_bytes > ProgBrkOffset())
	{
		if(program_break + PROGRAM_BREAK_INCREMENT < program_break)
		{
			//printf("Overflow flow in the value of program break");
			return NULL;
		}
		program_break += PROGRAM_BREAK_INCREMENT;
	}
	
	curr = List_Of_Free_Blocks;
	
	if(curr == NULL || curr < simulated_heap || curr >= simulated_heap + HEAP_SIZE)
		return NULL;
		
	/* Traverse the free list to find a suitable block */
	while((((curr->size)<num_of_bytes)||((curr->free)==0))&&(curr->next!=NULL))
	{
		prev=curr;
		curr=curr->next;
		//printf("One block checked\n");
		if (curr == NULL || curr < simulated_heap || curr >= simulated_heap + HEAP_SIZE) 
		{
        	    return NULL;
        	}
	}

	if(curr == NULL)
	{
		program_break += PROGRAM_BREAK_INCREMENT;
		while((((curr->size)<num_of_bytes)||((curr->free)==0))&&(curr->next!=NULL))
		{
		prev=curr;
		curr=curr->next;
		//printf("One block checked\n");
		}
	}
	
	if ((curr->size) == num_of_bytes) 
	{
        	curr->free = 0;
        	result = (void*)(curr);
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
		return NULL;
}


void merge() {
    struct block *curr, *prev;
    curr = List_Of_Free_Blocks;
    if (curr == NULL) {
        // No blocks to merge
        return;
    }

    while (curr != NULL && curr->next < program_break) {
        // Ensure pointers are valid
        assert(curr != NULL);
        assert(curr->next != NULL);

        //printf("Current block: size = %zu, free = %d\n", curr->size, curr->free);
        //printf("Next block: size = %zu, free = %d\n", curr->next->size, curr->next->free);

        // Check if both blocks are free
        if (curr->free && curr->next->free) {
            // Ensure we don't have size overflow
            if (curr->size > SIZE_MAX - (curr->next->size + sizeof(struct block))) {
                //printf("Size overflow detected. Skipping merge.\n");
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
	
	
	if(ptr == NULL)
	{
		//printf("Pointer is NULL\n");
		return NULL;
	}
	struct block* metadata_block = (struct block*)ptr - 1;
	//assert(metadata_block->free==0);
	if(metadata_block->free == 1)
	{
		//printf("Memory block isn't allocated. It's already free");
		return;
	}
	metadata_block->free = 1;
    /* Verifies if the pointer provided is within the range of the memory pool. */
	if(((void*)simulated_heap <= ptr) && (ptr <= (void*)(simulated_heap + HEAP_SIZE))) {
        struct block* curr = ptr;
        --curr;
        curr->free = 1;
        merge();
    } else {
        //printf("Please provide a valid pointer allocated by MyMalloc\n");
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

int ProgBrkOffset()
{
	return (program_break - simulated_heap);
}

void* malloc(size_t size)
{
	return MyMalloc(size);	
}

void free(void* ptr)
{
	MyFree(ptr);	
}

void* MyCalloc(size_t nmemb, size_t size)
{
	
	/*if(nmemb == 0 || size==0)
	{
		struct block* ptr = List_Of_Free_Blocks;
		while(ptr!= NULL && ptr->free == 0)
		{
			ptr = ptr->next;
		}
		return ptr;
	}*/
	void* start_of_block = MyMalloc(nmemb*size);
	if(start_of_block == NULL)
		return NULL;
	char* ptr = (char*)start_of_block;
	struct block* metadata_block = (char*)ptr - sizeof(struct block);
	memset(ptr, 0, nmemb*size);
	return start_of_block;
}

void* MyRealloc(void* ptr, size_t size)
{

	if(ptr == NULL)
	{
		return MyMalloc(size);
	}
	//assert(isBlockAllocated(ptr));
	struct block* metadata_block = (char*) ptr - sizeof(struct block);
	if(size == 0)
	{
		MyFree(ptr);
		return NULL;
	}
	void* new_ptr = MyMalloc(size);
	return new_ptr;
	
	
}

void* Realloc2(void * ptr, size_t size)
{
    if (ptr == NULL) {
        // If ptr is NULL, behave like malloc
        return MyMalloc(size);
    }

    if (size == 0) {
        // If size is 0, free the block and return NULL
        MyFree(ptr);
        return NULL;
    }

    // Retrieve the metadata block associated with ptr
    struct block* metadata_block = (struct block*)((char*)ptr - sizeof(struct block));

    if (metadata_block->size >= size) {
        // If the current block is large enough, return the same pointer
        return ptr;
    }

    // Check if we can merge with the next block
    if (metadata_block->next != NULL && metadata_block->next->free) {
        size_t total_free_space = metadata_block->size + metadata_block->next->size + sizeof(struct block);

        if (total_free_space >= size) {
            struct block* next_block = metadata_block->next->next;

            metadata_block->size = size;
            if (total_free_space > size + sizeof(struct block)) {
                struct block* new_block = (struct block*)((char*)metadata_block + sizeof(struct block) + size);
                new_block->size = total_free_space - size - sizeof(struct block);
                new_block->free = 1;
                new_block->next = next_block;
                metadata_block->next = new_block;
            } else {
                metadata_block->next = next_block;
            }
            return (char*)metadata_block + sizeof(struct block);
        }
    }

    // Allocate a new block of the requested size
    void* new_ptr = MyMalloc(size);
    if (new_ptr == NULL) {
        return NULL; // Allocation failed
    }

    // Copy data from the old block to the new block
    memcpy(new_ptr, ptr, metadata_block->size);

    // Free the old block
    MyFree(ptr);

    return new_ptr;

}

void* merge_with_next(void* ptr, size_t size)
{
	struct block* metadata_block = (struct block*)ptr - 1;
	if(metadata_block->next != NULL && metadata_block->free)
	{
		size_t total_space = metadata_block->size + sizeof(struct block) + metadata_block->next->size;
		size_t remaining_size = total_space - metadata_block->size;
		if(total_space > size)
		{
			metadata_block->size = total_space;
			struct block* next_metadata_block = (char*)metadata_block + metadata_block->size;
			next_metadata_block->size = remaining_size-sizeof(struct block) > 0 ? remaining_size-sizeof(struct block) : 0;
			next_metadata_block->next = metadata_block->next;
			metadata_block->next = next_metadata_block;
			next_metadata_block->free = 1;
			return metadata_block + 1;
		}
		else if(total_space == size)
		{
			metadata_block->next = metadata_block->next->next;
			metadata_block->size = size;
		}
	}
	return NULL;

}

void* increase_block_size(void* ptr, size_t requested_size)
{
	struct block* metadata_block = (struct block*)((char*) ptr - sizeof(struct block));
	struct block* next_metadata_block = metadata_block->next;
	struct block* curr = List_Of_Free_Blocks;
	struct block* prev = List_Of_Free_Blocks;

	/*while(prev->next != metadata_block && prev!= NULL)
	{
		prev = prev->next;
	}*/

    if (metadata_block->next != NULL && metadata_block->next->free == 1) {
        size_t total_free_space = metadata_block->size + metadata_block->next->size + sizeof(struct block);
        
        if (total_free_space >= requested_size) {
            struct block* next_block = metadata_block->next->next;
            
            metadata_block->size = requested_size;
            if (total_free_space > requested_size + sizeof(struct block)) {
                struct block* new_block = (struct block*)((char*)metadata_block + sizeof(struct block) + requested_size);
                new_block->size = total_free_space - requested_size - sizeof(struct block);
                new_block->free = 1;
                new_block->next = next_block;
                metadata_block->next = new_block;
            } else {
                metadata_block->next = next_block;
            }
            
            return (char*)metadata_block + sizeof(struct block);
        }
    }
	
	while(curr->next != NULL)
	{
		
		if (curr->free == 1 && curr->size >= requested_size) 
		{
            copy_block_contents(metadata_block + 1, curr + 1);
            curr->free = 0;
            curr->size = requested_size;
            metadata_block->free = 1;
            merge();
        	return (char*)curr + sizeof(struct block);
        }
			
		
		curr = curr->next;
	}
	/* the function returns NULL if reallocation of a larger size fails */
	return NULL;
}

void copy_block_contents(void* ptr1, void* ptr2)
{
	struct block* start1 = (char*) ptr1 - sizeof(struct block);
	struct block* start2 = (char*) ptr2 - sizeof(struct block);
	size_t size_to_copy = (start1->size < start2->size) ? start1->size : start2->size;
	
	for (size_t i = 0; i < size_to_copy; i++) 
	{
        ((char*)ptr2)[i] = ((char*)ptr1)[i];
    }
	start1->free = 0;
}

void* dec_block_size(void* ptr, size_t size)
{
	struct block* metadata_block = (char*)ptr - sizeof(struct block);
	
	struct block* new_metadata_block = (char*)metadata_block + sizeof(struct block) + metadata_block->size;
	
	if(metadata_block->size > size)
	{
		size_t remaining_size = metadata_block->size - size - sizeof(struct block);
		
		new_metadata_block -> size = remaining_size;
		new_metadata_block->free = 1;
		new_metadata_block->next = metadata_block->next;

		metadata_block->next = new_metadata_block;
		metadata_block->size = size;
		merge();
		return (char*)metadata_block + sizeof(struct block);
	}
	return ptr;
}

