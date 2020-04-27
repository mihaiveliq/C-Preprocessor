#include "input.h"

#define MAX_LEN_RW_BUF 256

char *concat_file_with_directory(char *str1, char *str2);

char *get_file_name_from_include(char *some_string);

entry_t *parse_define_in_entry(hashmap_t *self, char *define_string);

char *take_out_flag(char *long_string_name);

char *check_for_directives(char *some_string);

void manage_define(hashmap_t *self, storage_t *key_strg, char *some_string);

int pointer_to_offset(char *pointer, char *pointer_ref);

char *update_string(hashmap_t *self, char *some_string);
