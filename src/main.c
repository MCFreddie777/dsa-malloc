//
// Created by Bc. František Gič on 26/02/2020.
//

#include <stdio.h>
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
#define foot(pointer, size) ((footer*) ((char *) pointer + sizeof(header) +size))
#define memsize(size) (size + sizeof(header) + sizeof(footer))

/**
 * This function checks for the internal size of a block of memory and decides whether the block
 * can be allocated
 *
 * @param mem pointer at a memory to be checked
 * @param size an amount of bytes needed to be allocated
 * @return '1' if a blocks internal memory is larger or equal to the amount needed
 *         '0' otherwise
 */
char can_allocate (header *mem, unsigned int size) {
    return (char) (mem != NULL && mem->size >= size);
}

/**
 * Function which allocates the simulated memory
 *
 * @param size an amount of blocks needed to allocate
 * @return pointer to an "allocated" memory or NULL if cannot be allocated
 */
void *memory_alloc (unsigned int size) {
    
    // Uninitialized memory
    if (memory == NULL)
        return NULL;
    
    header *actual = (header *) memory;
    
    while (!can_allocate(actual, size)) {
        // Not suitable last chunk of free memory
        if (actual->next == NULL)
            return NULL;
        else
            actual = actual->next;
    };
    
    // if (header+size+footer) won't fit into the split chunk, return the whole block
    if ((actual->size - memsize(size)) < 0) {
        memory = actual->next;
        actual->type = ALLOCATED;
        return actual;
    }
    
    // static copy of header of free memory (to keep the data)
    header h_former_free = *actual;
    
    // Set the actual (allocated) block info
    header *h_alloc = head(actual);
    footer *f_alloc = foot(actual, size);
    
    h_alloc->size = size;
    h_alloc->type = ALLOCATED;
    h_alloc->next = NULL;
    f_alloc->size = size;
    
    // Move the old (freemem) header by offset (size)
    header *h_free = head(((char *) f_alloc + sizeof(footer)));
    footer *f_free = foot(actual, h_former_free.size);
    
    h_free->size = h_former_free.size - memsize(size);
    h_free->next = h_former_free.next;
    h_free->type = FREE;
    f_free->size = h_free->size;
    
    // if actual block was first in the chain of free mem
    if ((char *) actual == memory)
        memory = h_free;
    
    return actual;
};

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
    if (ptr != NULL && (size >= memsize(0))) {
        memory = ptr;
        head(ptr)->size = size - memsize(0);
        head(ptr)->next = NULL;
        head(ptr)->type = FREE;
        foot(ptr, head(ptr)->size)->size = head(ptr)->size;
    }
    else exit(1);
}

int main () {
    char region[100];
    memset(region, 'F', 100);
    memory_init(region, 100);
    
    char *pointer = (char *) memory_alloc(5);
    
    return 0;
}

