#ifndef HMM_H
#define HMM_H

#define HEAP_SIZE			1024*1024
#define PROGRAM_BREAK_INCREMENT		500 
struct block{
    size_t size;			//size of block it refers to  
    int free;   			//check whether block is free or allocated                        
    struct block *next; 	//Points to the next metadata block
};


void initialize();
void split(struct block *fitting_slot,size_t size);
void* MyMalloc(size_t num_of_bytes);
void merge();
void MyFree(void* ptr);
int isBlockAllocated(void* ptr);
int ProgBrkOffset();
void* malloc(size_t size);
void free(void* ptr);
void* MyCalloc(size_t nmemb, size_t size);
void* MyRealloc(void* ptr, size_t size);
void* increase_block_size(void* ptr, size_t requested_size);
void* dec_block_size(void* ptr, size_t size);
void* Realloc2(void * ptr, size_t size);
#endif
