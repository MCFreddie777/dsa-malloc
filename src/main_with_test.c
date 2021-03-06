//
// Created by Bc. František Gič on 26/02/2020.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

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
        !((mem == memory) && ((header *) ((char *) mem + sizeof(header)))->type == ALLOCATED) &&
        // If whole memory is set as allocated (special case)
        mem->type != ALLOCATED
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
    
    // If (header+size+footer) + next header won't fit into the split chunk, return the whole block
    if (
        actual->size <= memsize(size) ||
        (actual == memory && actual->size <= memsize(size) + sizeof(header))
        ) {
        
        actual->type = ALLOCATED;
        
        if (actual != memory)
            before->next = actual->next;
        
        return ((char *) actual + sizeof(header));
    }
    
    // Static copy of header of free memory (to keep the data)
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
    
    // Global header stays here in the case of first block (therefore we need to substract it)
    if (head(actual) == memory) {
        h_free->size = h_free->size - sizeof(header);
    }
    
    h_free->next = h_former_free.next;
    h_free->type = FREE;
    f_free->size = h_free->size;
    
    // If we created block right after start of memory, return that
    if (head(actual) == memory) {
        head(actual)->next = h_free;
        return (char *) h_alloc + sizeof(header);
    }
    
    // If we took the replacement of free header, return that
    before->next = h_free;
    return (char *) actual + sizeof(header);
};

/**
 * This function tests whether the pointer points to a memory inside our "simulated" memory
 *
 * @param ptr The pointer to be checked
 * @return A boolean value represented
 */
short out_of_bounds (void *ptr) {
    return (short) (
        (ptr < memory) ||
        (ptr >= (void *) (foot(memory, head(memory)->size + sizeof(footer))))
    );
};

/**
 * This function checks whether the memory which the pointer pointing to
 * was previously allocated by our memory_alloc function or not
 *
 * @param ptr The pointer to a memory to be checked
 * @return A boolean value
 */
int memory_check (void *ptr) {
    header *h = head(((char *) ptr - sizeof(header)));
    return (
        (ptr != NULL) &&
        !out_of_bounds(ptr) &&
        h->type == ALLOCATED
    );
};

/**
 * This function frees the memory previously allocated by memory_alloc()
 * If memory is preceded or succeeded by another free chunk it'll merge those chunks together
 * If all chunks are free, it will merge the whole memory within the global header and footer
 *
 * @param valid_ptr A pointer to the memory which was previously allocated by memory_alloc
 * @return '1' if memory was freed
 *         '0' otherwise
 */
int memory_free (void *valid_ptr) {
    
    header *valid_h = head(((char *) valid_ptr - sizeof(header)));
    
    if (memory_check(valid_ptr)) {
        
        
        footer *valid_f = foot(valid_ptr, valid_h->size - sizeof(header));
        
        header *next_h = head(((char *) valid_f + sizeof(footer)));
        footer *next_f = foot(next_h, next_h->size);
        
        footer *prev_f = (footer *) ((char *) valid_h - sizeof(footer));
        header *prev_h = head(((char *) prev_f - prev_f->size - sizeof(header)));
        
        // If the block after is free
        if (!out_of_bounds(next_h) && next_h->type == FREE) {
            
            // Omit the the blocks footer and next header
            valid_h->size = valid_h->size + sizeof(footer) + sizeof(header) + next_h->size;
            next_f->size = valid_h->size;
            
            // Traverse to link the free memory chain
            header *actual = (header *) memory;
            while (actual->next != next_h) {
                actual = actual->next;
            }
            
            actual->next = valid_h;
        }
        
        // If the block before is free, expand its size by this block, omit the footer and header
        if (!out_of_bounds(prev_h) && prev_h->type == FREE) {
            prev_h->size = prev_h->size + sizeof(footer) + sizeof(header) + valid_h->size;
            valid_f->size = prev_h->size;
            
            // If the block before points to the block to be freed, skip this.
            if (prev_h->next == valid_h) {
                prev_h->next = valid_h->next;
            }
        }
        
        // Set the chunk as free (the main aim of this function lol)
        valid_h->type = FREE;
        
        int free_chunk_n = 0;
        unsigned int free_chunk_size = 0;
        
        // Traverse to link the free memory chain
        header *actual = head(memory)->next;
        while (actual != NULL) {
            free_chunk_n++;
            free_chunk_size += actual->size;
            actual = actual->next;
        }
        
        /*
         * If size of all free chunks including its' headers and footer is equal to the total
         * size between global boundaries, then we can merge all chunks
         * Formula = N * (sizeof(header) + sizeof(footer)) + total_inner_free_size - global_footer
         */
        unsigned int raw_size_free_chunks =
            free_chunk_n * (sizeof(header) + sizeof(footer)) + free_chunk_size - sizeof(footer);
        
        if (raw_size_free_chunks == head(memory)->size) {
            head(memory)->next = NULL;
            foot(memory, head(memory)->size)->size = head(memory)->size;
        }
        
        return 1;
    }
    else return 0;
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

void test (
    char *description,
    unsigned int min_block_size,
    unsigned int max_block_size,
    unsigned int memory_size
) {
    unsigned int actual_memory_size = head(memory)->size;
    
    char *ptr = NULL;
    unsigned int requested_block_size = 0;
    unsigned int total_requested_bytes = 0;
    unsigned int total_allocated_bytes = 0;
    unsigned int total_misc_space = 0;
    unsigned int allocated_blocks_n = 0;
    
    do {
        // Generate random blocksize
        requested_block_size = rand() % (max_block_size - min_block_size + 1) + min_block_size;
        
        // Alloc the memory
        ptr = (char *) memory_alloc(requested_block_size);
        
        if (ptr != NULL) {
            header *allocated = head(((char *) ptr - sizeof(header)));
            total_requested_bytes += requested_block_size;
            total_allocated_bytes += allocated->size;
            total_misc_space += sizeof(header) + sizeof(footer);
            allocated_blocks_n++;
        }
    } while (ptr != NULL);
    
    // Traverse the free memory chain
    unsigned int empty_memory = 0;
    header *actual = head(memory)->next;
    while (actual != NULL) {
        empty_memory += actual->size;
        total_misc_space += sizeof(header) + sizeof(footer);
        actual = actual->next;
    }
    
    // Don't forget the global header (in case the whole memory isn't used)
    if (head(memory)->type == FREE)
        total_misc_space += sizeof(header);
    
    printf("\nTEST %s\n", description);
    printf("Range [%u - %u] (bytes) \n", min_block_size, max_block_size);
    printf("Memory size: %u bytes\n", memory_size);
    printf("Memory size (within global header and footer): %u bytes\n", actual_memory_size);
    printf("Allocated blocks (count): %u\n", allocated_blocks_n);
    printf("Allocated (requested): %u/%u bytes (%.2f%%)\n",
           total_requested_bytes, actual_memory_size,
           ((float) total_requested_bytes / actual_memory_size) * 100);
    printf("Allocated (actual): %u/%u bytes (%.2f%%) (%.2f%% of total size)\n",
           total_allocated_bytes,
           actual_memory_size,
           ((float) total_allocated_bytes / actual_memory_size) * 100,
           ((float) total_allocated_bytes / memory_size) * 100
    );
    printf("Memory used by misc (footers and headers): %u/%u bytes (%.2f%%)\n",
           total_misc_space, memory_size, ((float) total_misc_space / memory_size) * 100);
    printf("Empty memory: %u/%u bytes (%.2f%%)\n",
           empty_memory, actual_memory_size, ((float) empty_memory / actual_memory_size) * 100);
}

int main () {
    unsigned int memory_size = 100000;
    
    char region[memory_size];
    memset(region,0,memory_size);
    memory_init(region, memory_size);
    
    srand(time(0));
    
    test(
        "5",
        8,
        10000,
        memory_size
    );
    return 0;
}

