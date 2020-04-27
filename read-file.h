#include "parse-input.h"

int key_exist(char* string, storage_t *key_strg);

void read_file(FILE *fd_in, FILE *fd_aux, hashmap_t *map, 
    storage_t *key_strg, storage_t *dir_strg);

void read_stdin(FILE *fd_aux, hashmap_t *map, storage_t *key_strg, 
    storage_t *dir_strg);

void write_in_output_file(FILE *fd_out, FILE *fd_aux, hashmap_t *map);

void write_in_stdout(FILE *fd_aux, hashmap_t *map);
