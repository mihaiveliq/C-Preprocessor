#include "strg.h"

#define MAXLEN 256
#define INITIAL_BKT_CAPACITY 20

int hash_func(char *key);

typedef struct {
    char *key;
    char *value;
} entry_t;

typedef struct {
    int capacity;
    int length;
    entry_t **list;
} bucket_t;

typedef struct {
    bucket_t **list_of_buckets;
} hashmap_t;

char *get_from_map(hashmap_t *self, char *key);

void put_in_map(hashmap_t *self, char *key, char *value);

void remove_from_map(hashmap_t *self, char *key);

hashmap_t *alloc_map();

void free_map(hashmap_t *self);
