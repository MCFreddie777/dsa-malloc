//
// Created by František Gič on 26/02/2020.
//
#include <string.h>

/*
 * Funkcia memory_alloc má poskytovať služby analogické štandardnému malloc.
 * Teda, vstupné parametre sú veľkosť požadovaného súvislého bloku pamäte
 * a funkcia mu vráti:
 * ukazovateľ na úspešne alokovaný kus voľnej pamäte, ktorý sa vyhradil,
 * alebo NULL, keď nie je možné súvislú pamäť požadovanej veľkosť vyhradiť.
 */
void *memory_alloc (unsigned int size) {
    // mem = alloc ( size );
    // if (mem == NULL) return NULL
    // return mem;
};

/*
 * Funkcia memory_free slúži na uvoľnenie vyhradeného bloku pamäti,
 * podobne ako funkcia free.
 * Funkcia vráti 0, ak sa podarilo (funkcia zbehla úspešne) uvoľniť blok pamäti,
 * inak vráti 1.
 * Môžete predpokladať, že parameter bude vždy platný ukazovateľ,
 * vrátený z predchádzajúcich volaní funkcie memory_alloc,
 * ktorý ešte nebol uvoľnený.
 */
int memory_free (void *valid_ptr) {};

/*
 * Funkcia memory_check slúži na skontrolovanie, či parameter (ukazovateľ)
 * je platný ukazovateľ, ktorý bol v nejakom z predchádzajúcich volaní
 * vrátený funkciou memory_alloc a zatiaľ nebol uvoľnený funkciou memory_free.
 * Funkcia vráti 0, ak je ukazovateľ neplatný, inak vráti 1.
 */
int memory_check (void *ptr) {};

/*
 * Funkcia memory_init slúži na inicializáciu spravovanej voľnej pamäte.
 * Predpokladajte, že funkcia sa volá práve raz pred všetkými inými volaniami
 * memory_alloc, memory_free a memory_check. Viď testovanie nižšie.
 * Ako vstupný parameter funkcie príde blok pamäte,
 * ktorú môžete použiť pre organizovanie a aj pridelenie voľnej pamäte.
 * Vaše funkcie nemôžu používať globálne premenné okrem jednej globálnej premennej
 * na zapamätanie ukazovateľa na pamäť, ktorá vstupuje do funkcie memory_init.
 * Ukazovatele, ktoré prideľuje vaša funkcia memory_alloc
 * musia byť výhradne z bloku pamäte, ktorá bola pridelená funkcii memory_init.
 */
void memory_init (void *ptr, unsigned int size);

int main () {
    char region[50];
    
    memory_init(region, 50);
    char *pointer = (char *) memory_alloc(10);
    if (pointer)
        memset(pointer, 0, 10);
    if (pointer)
        memory_free(pointer);
 
    return 0;
}

