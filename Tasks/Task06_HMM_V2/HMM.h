#ifndef HMM_H
#define HMM_H
#include <string.h>


#define PROG_BRK_INC			1000*1000*136
#define PROG_BRK_DEC			8000
// Ensure alignment of the block
#define ALIGNMENT 			8
#define ALIGN(size) 			(((size+7)/8)*8)

struct block{
    size_t size;			//size of block it refers to  
    int free;   			//check whether block is free or allocated                        
    struct block *next; 	//Points to the next metadata block
};

void split(struct block *fitting_slot,size_t size);
void* MyMalloc(size_t num_of_bytes);
void merge();
void MyFree(void* ptr);
void* MyCalloc(size_t nmemb, size_t size);
void* MyRealloc(void* ptr, size_t size);
void debug_heap();
void* malloc(size_t size);
void* calloc(size_t nmemb, size_t size);
void free(void* ptr);
void* realloc(void * ptr, size_t size);
#endif
