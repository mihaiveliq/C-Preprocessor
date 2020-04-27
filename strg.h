#include "libraries.h"

#define INITIAL_DIR_BKT_CAPACITY 10

typedef struct {
    int capacity;
    int length;
    char **names_bucket;
} storage_t;

storage_t *alloc_strg(storage_t *strg);

void put_in_strg(storage_t *strg, char *name);

void free_strg(storage_t *strg);
