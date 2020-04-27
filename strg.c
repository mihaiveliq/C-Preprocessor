#include "strg.h"

/* 
 * Functie care aloca memorie pentru structura
 * de stocare a numelor de directoare.
 */
storage_t *alloc_strg(storage_t *strg) {

    strg = (storage_t*) calloc(1, sizeof(storage_t));
    if (!strg)
        exit(12);

    strg->names_bucket
        = (char**) calloc(INITIAL_DIR_BKT_CAPACITY, sizeof(char*));
    if (!strg->names_bucket)
        exit(12);
    
    strg->capacity = INITIAL_DIR_BKT_CAPACITY;

    strg->length = 0;

    return strg;
}

/* 
 * Functie care adauga un nume de director sub forma de string
 * in structura de stocare a numelor de directoare.
 */
void put_in_strg(storage_t *strg, char *name) {
    int capacity, length, i;

    if (!strg || !strg->names_bucket || !name)
        return;
    
    capacity = strg->capacity;
    length = strg->length;

    if (length >= capacity) {
        strg->names_bucket 
            = realloc(strg->names_bucket, 2 * capacity * sizeof(char*));
        if (!strg->names_bucket)
            exit(12);

        for (i = length - 1; i < 2 * capacity; ++i) {
            strg->names_bucket[i] = NULL;
        }
    }

    if (!strg->names_bucket[length]) {
        strg->names_bucket[length] 
            = strcpy((char*) calloc(strlen(name) + 1, sizeof(char)), 
                name);
        strg->length++;
    }
}

/* 
 * Functie care elibereaza memorie alocata pentru
 * structura de stocare a numelor de directoare.
 */
void free_strg(storage_t *strg) {
    int i;

    if (!strg)
        return;
    
    if (!strg->names_bucket) {
        free(strg);
        return;
    }

    for (i = 0; i < strg->capacity; ++i) {
        if (!strg->names_bucket[i]) {
            continue;
        }
        free(strg->names_bucket[i]);
    }
    
    free(strg->names_bucket);
    free(strg);
}
