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
    return (char) (
        mem != NULL &&
        mem->size >= size &&
        // This is here because of the global header -> check if there is block allocated after it
        // Cuz global header is still set FREE and
        ((header *) ((char *) mem + sizeof(header)))->type != ALLOCATED
    );
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
    
    header *actual = memory;
    header *before = NULL;
    
    while (!can_allocate(actual, size)) {
        // Not suitable last chunk of free memory
        if (actual->next == NULL)
            return NULL;
        else {
            before = actual;
            actual = actual->next;
        }
    };
    
    // if (header+size+footer) won't fit into the split chunk, return the whole block
    if ((actual->size - memsize(size)) < 0) {
        if (actual == memory)
            return NULL;
        
        actual->type = ALLOCATED;
        before->next = actual->next;
        return actual;
    }
    
    // static copy of header of free memory (to keep the data)
    header h_former_free = *actual;
    
    header *h_alloc = NULL;
    footer *f_alloc = NULL;
    
    if (head(actual) == memory) {
        // Special case, when the first free block is the beginning of memory
        h_alloc = head(((char *) actual + sizeof(header)));
        f_alloc = foot(h_alloc, size);
    }
    else {
        // Set the actual (allocated) block info, replace the old header
        h_alloc = head(actual);
        f_alloc = foot(actual, size);
    }
    
    h_alloc->size = size;
    h_alloc->type = ALLOCATED;
    h_alloc->next = NULL;
    f_alloc->size = size;
    
    // Move the old (freemem) header by offset (size)
    header *h_free = head(((char *) f_alloc + sizeof(footer)));
    footer *f_free = foot(actual, h_former_free.size);
    
    h_free->size = h_former_free.size - memsize(size);
    
    // global header stays here in the case of first block (therefore we need to substract it)
    if (head(actual) == memory) {
        h_free->size = h_free->size - sizeof(header);
    }
    
    h_free->next = h_former_free.next;
    h_free->type = FREE;
    f_free->size = h_free->size;
    
    if (head(actual) == memory) {
        head(actual)->next = h_free;
        return h_alloc;
    }
    
    before->next = h_free;
    return actual;
};

int memory_free (void *valid_ptr) {};

int memory_check (void *ptr) {
    return (
        (ptr != NULL) &&
        (ptr >= memory) &&
        (ptr < (void *) (foot(memory, head(memory)->size + sizeof(footer)))) &&
        ((header *) ptr)->type == ALLOCATED
    );
};

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
    char *pointer2 = (char *) memory_alloc(7);
    
    int result = memory_check(pointer);
    int result2 = memory_check(pointer2);
    
    return 0;
}

