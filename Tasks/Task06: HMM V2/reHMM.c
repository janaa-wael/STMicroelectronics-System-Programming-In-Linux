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
    struct block *new_block = (void*)((void*)fitting_slot + sizeof(struct block) + size);

    // Ensure the new block address is within the allocated memory
    /*if ((char *)new_block >= (char *)fitting_slot + fitting_slot->size) {
        return; // Invalid split, would result in out-of-bounds memory access
    }*/

    // Set up the new block's metadata
    new_block->size = fitting_slot->size - size - sizeof(struct block);
    new_block->free = 1;
    new_block->next = fitting_slot->next;

    // Update the fitting_slot to reflect the allocated size
    fitting_slot->size = size;
    fitting_slot->free = 0;
    fitting_slot->next = new_block;
}

/*
void* MyMalloc(size_t num_of_bytes) 
{
    struct block* curr = List_Of_Free_Blocks;
    struct block* prev = NULL;
    void* result = NULL;
    
    num_of_bytes = ALIGN(num_of_bytes);
    
    // Ensure non-zero size for allocation
    if(num_of_bytes == 0)
    	num_of_bytes = sizeof(struct block);
    	
    // Initialize heap if not already initialized
    if (List_Of_Free_Blocks == NULL) 
    {
        size_t count = (PROG_BRK_INC > (num_of_bytes + sizeof(struct block))) ? PROG_BRK_INC : num_of_bytes + sizeof(struct block);
        void* heap_start = sbrk(count);
        if (heap_start == (void *)-1) 
        {
            perror("sbrk failed");
            return NULL;
        }

        List_Of_Free_Blocks = (struct block*)heap_start;
        List_Of_Free_Blocks->size = count - sizeof(struct block);
        List_Of_Free_Blocks->free = 1;
        List_Of_Free_Blocks->next = NULL;
    }

    curr = List_Of_Free_Blocks;

    while (curr != NULL && curr->next != NULL && (!curr->free || curr->size < num_of_bytes)) 
    {
        prev = curr;
        curr = curr->next;
    }

    if (curr->next == NULL && curr->size < num_of_bytes && curr->free) 
    {
        //size_t count = (PROG_BRK_INC > (num_of_bytes ) ? PROG_BRK_INC : num_of_bytes;
        size_t count = 0;
    	while(count < num_of_bytes)
    	{
    		count += PROG_BRK_INC;
    		void * sbrk_ret = sbrk(PROG_BRK_INC);
    		if (sbrk_ret == (void *)-1) 
     		{
           	 	perror("sbrk failed");
           		return NULL;
        	}

    	}
        curr->size = count;
        curr->free = 0;
        curr->next = NULL;

        if (prev != NULL) 
        {
            prev->next = curr;
        }
        else 
        {
            List_Of_Free_Blocks = curr;
        }
    }

    if (curr->size == num_of_bytes && curr->free) 
    {
        curr->free = 0;
        result = (void*)(curr + 1);
        return result;
    } 
    else if (curr->size >= (num_of_bytes + sizeof(struct block)) && curr->free) 
    {
        split(curr, num_of_bytes);
        curr->free = 0;
        result = (void*)(curr + 1);
        return result;
    } 
    else if(curr->size < (num_of_bytes + sizeof(struct block)) && curr->free)
    {
    	size_t count = PROG_BRK_INC;
    	while(count < num_of_bytes)
    	{
    		count += PROG_BRK_INC;
    		void * sbrk_ret = sbrk(PROG_BRK_INC);
    		if (sbrk_ret == (void *)-1) 
     		{
           	 	perror("sbrk failed");
           		return NULL;
        	}

    	}
    	curr->size = count;
    	curr->free = 0;
    	curr->next = 0;
    	result = (void*)(curr + 1);
        return result;
    	
    }
    return NULL;  // In case no suitable block is found
}*/

void* MyMalloc(size_t num_of_bytes) 
{
    struct block* curr = List_Of_Free_Blocks;
    struct block* prev = NULL;
    void* result = NULL;
    
    num_of_bytes = ALIGN(num_of_bytes);
    
    // Ensure non-zero size for allocation
    if(num_of_bytes == 0)
    	num_of_bytes = sizeof(struct block);
    	
    // Initialize heap if not already initialized
    if (List_Of_Free_Blocks == NULL) 
    {
        
        void* heap_start = sbrk(PROG_BRK_INC);
        if (heap_start == (void *)-1) 
        {
            perror("sbrk failed");
            return NULL;
        }

        List_Of_Free_Blocks = (struct block*)heap_start;
        List_Of_Free_Blocks->size = PROG_BRK_INC - sizeof(struct block);
        List_Of_Free_Blocks->free = 1;
        List_Of_Free_Blocks->next = NULL;
    }
    
    curr = List_Of_Free_Blocks;
    
    while ( (curr != NULL) && (curr->size < num_of_bytes || curr->free != 1) ) 
    {
        prev = curr;
        curr = curr->next;
    }
    
    if(curr == NULL)
    {
    	if(prev->free == 1)
	{
	    	curr = prev;
    	
    		size_t count = PROG_BRK_INC;
    		
    		while(curr->size < num_of_bytes)
    		{
    			void* ret_ptr = sbrk(PROG_BRK_INC);
    			curr->size += PROG_BRK_INC;
    			if (ret_ptr == (void *)-1) 
     			{
        	   	 	perror("sbrk failed");
        	   		return NULL;
        		}
    		}
    	}
    	else if(prev->free == 0)
    	{
    		curr = (struct block*)((char*)(prev + 1) + prev->size);
    		void* ret_ptr = sbrk(PROG_BRK_INC);
    		if (ret_ptr == (void *)-1) 
     		{
        	   	 	perror("sbrk failed");
        	   		return NULL;
        	}
    		curr->size = PROG_BRK_INC - sizeof(struct block);
    		while(curr->size < (num_of_bytes))
    		{
    			ret_ptr = sbrk(PROG_BRK_INC);
    			curr->size += PROG_BRK_INC;
    			if (ret_ptr == (void *)-1) 
     			{
        	   	 	perror("sbrk failed");
        	   		return NULL;
        		}
    		}
    		
    	}
    	prev->next = curr;
    	curr->free = 0;
    	curr->next = NULL;
    	return (void*)(curr+1);
    }

    if (curr->size == num_of_bytes && curr->free) 
    {
        curr->free = 0;
        result = (void*)(curr + 1);
        return result;
    } 
    else if ((curr->size >= (num_of_bytes + 2*sizeof(struct block))) && curr->free) 
    {
        split(curr, num_of_bytes);
        curr->free = 0;
        result = (void*)(curr + 1);
        return result;
    }
    else if( (curr->size >= num_of_bytes) && curr->free )
    {
    	curr->free = 0;
    	result = (void*)(curr + 1);
    	return result;
    }
    return NULL;
}  



void merge() {
    struct block *curr, *prev;

    curr = List_Of_Free_Blocks;
    prev = NULL;

    if (curr == NULL) {
        // No blocks to merge
        return;
    }

    while (curr->next != NULL) 
    {
        // Check if both blocks are free and adjacent
        if (curr->free && curr->next != NULL && curr->next->free) {
            // Merge the blocks
            curr->size += curr->next->size + sizeof(struct block);
            curr->next = curr->next->next;
        } 
        // Move to the next block
        prev = curr;
        curr = curr->next;
        if(curr == NULL)
        {
        	prev->next = NULL;
        	break;
        }
    }
    if(curr != NULL)
    {
    	if(curr->free && prev->free && curr->next == NULL)
    	{
    		prev->size += curr->size + sizeof(struct block);
    		prev->next = NULL;
    	}
    }
    
    while( (prev->size >=  PROG_BRK_DEC) && (prev->free == 1) && (prev->next == NULL) )
    {
		void* ret_ptr = sbrk (-PROG_BRK_DEC);
		if (ret_ptr == (void *)-1) 
     		{
        	   	 	perror("sbrk failed");
        	 
        	}
		prev->size-= PROG_BRK_DEC;
    }
}


void MyFree(void *ptr) {
	
	struct block* curr = List_Of_Free_Blocks;
	if(ptr == NULL || curr == NULL)
	{
		return;
	}
	if(curr->free == 1)
	{
		return;
	}
	
	while((curr->next) != NULL)
	{
			if( (curr == (struct block*)ptr - 1) && curr->free == 0)
			{
				curr->free = 1;
				merge();
				break;			
			}
			curr = curr->next;
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
	//size = ALIGN(size);
	if(nmemb == 0 || size == 0 || nmemb*size <= 0)
		return malloc(sizeof(struct block));
	else
	{
		void* ptr = malloc(nmemb*size);
		if(ptr != NULL)
		{
			memset(ptr, 0, nmemb*size);
			
		}
		return ptr;
	}
}

void* MyRealloc(void* ptr, size_t size)
{
	size = ALIGN(size);
	if((ptr == NULL) && size!=0)
	{
		return malloc(size);
	}
	else if((ptr == NULL)  && size ==0)
	{
		size = sizeof(struct block);
		return malloc(size);
	}	
	else if(size == 0 && ptr!= NULL)
	{
		MyFree(ptr);
		return NULL;
	}
	else if (size <= (((struct block*)ptr) - 1)->size) {
        	return ptr; // No need to reallocate
    	}
	else
	{
		struct block* metadata_block = ((struct block*)ptr) - 1;
		 if (metadata_block == NULL || metadata_block->size == 0 ) 
		 {
	    		fprintf(stderr, "Invalid metadata block or size.\n");
	    		return NULL;
		}
		void* ptr2 = malloc(size);
		if(ptr2 != NULL)
		{
			memcpy(ptr2,ptr, metadata_block->size);
			MyFree(ptr);
			return ptr2;
		}
		else
		{
			return NULL;
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


