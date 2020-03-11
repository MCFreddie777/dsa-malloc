//
// Created by Bc. František Gič on 26/02/2020.
//

#include <stdlib.h>
#include <string.h>

/*
 * Global variable, pointer to the start of our simulated memory
 */
void *memory = NULL;

typedef struct header {
    struct header *next;
    unsigned int size;
    char type;
} header;

typedef struct footer {
    unsigned int size;
} footer;

#define FREE 0
#define ALLOCATED 1
#define head(pointer) ((header*) pointer)
#define foot(pointer, size) ((footer *) ( pointer+sizeof(header) + size))

void *memory_alloc (unsigned int size) {};

int memory_free (void *valid_ptr) {};

int memory_check (void *ptr) {};

/**
 * This method initialises the memory by pushing the header and footer in it
 * Also, it sets the pointer to the fictive memory to the predefined global variable
 *
 * @param ptr Pointer to the fictive memory to be allocated
 * @param size Number of blocks to be allocated
 */
void memory_init (void *ptr, unsigned int size) {
    if (ptr != NULL && (size >= sizeof(header) + sizeof(footer))) {
        memory = ptr;
        head(ptr)->size = size - sizeof(header) - sizeof(footer);
        head(ptr)->next = NULL;
        head(ptr)->type = FREE;
        foot(ptr, head(ptr)->size)->size = head(ptr)->size;;
    }
    else exit(1);
}

int main () {
    char region[100];
    memset(region, 'F', 100);
    memory_init(region, 100);
    
    return 0;
}

