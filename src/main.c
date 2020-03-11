//
// Created by František Gič on 26/02/2020.
//
#include <string.h>

void *memory_alloc (unsigned int size) {
    // mem = alloc ( size );
    // if (mem == NULL) return NULL
    // return mem;
};

int memory_free (void *valid_ptr) {};

int memory_check (void *ptr) {};

void memory_init (void *ptr, unsigned int size);

int main () {
    char region[50];
    memory_init(region, 50);
    
    return 0;
}

