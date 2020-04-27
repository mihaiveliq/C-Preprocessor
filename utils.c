#include "utils.h"

/*
 * Functie care parseaza stringul de forma "cheie=valoare" primit
 * ca parametru din linia de comanda intr-un entry.
 */
entry_t *parse_define_in_entry(hashmap_t *self, char *define_string) {
    char *token;
    entry_t *entry;

    entry = (entry_t*) calloc(1, sizeof(entry_t));
    if (!entry)
        exit(12);


    token = strtok(define_string, "=");

    if (token != NULL) {
        entry->key = calloc(strlen(token) + 1, sizeof(char));
        if (!entry->key)
            exit(12);
            
        strcpy(entry->key, token);
    }

    if (token != NULL)
        token = strtok(NULL, "=");
    
    if (token != NULL) {
        if (get_from_map(self, token) != NULL) {
            entry->value 
                = calloc(strlen(get_from_map(self, token)) + 1, sizeof(char));
            if (!entry->value)
                exit(12);

            strcpy(entry->value, get_from_map(self, token));
        } else {
            entry->value = calloc(strlen(token) + 1, sizeof(char));
            if (!entry->value)
                exit(12);

            strcpy(entry->value, token);
        }
    }

    return entry;
}

/* 
 * Functie care elimina flagurile din numele 
 * parametrului dat in linia de comanda.
 */
char *take_out_flag(char *long_string_name) {
    char *parsed_string;
    int i, j;

    parsed_string = calloc(strlen(long_string_name) - 1, sizeof(char));
    if (!parsed_string)
        exit(12);

    for (i = 2, j = 0; i < strlen(long_string_name); ++i, ++j) {
        parsed_string[j] = long_string_name[i];
    }

    return parsed_string;
}

/*
 * Functie care concateneaza 2 siruri
 */
char *concat_file_with_directory(char *str1, char *str2) {
    int new_len;

    new_len = strlen(str1) + strlen(str2) + 1;
    str1 = realloc(str1, new_len);
    if (!str1)
        exit(12);
    strcat(str1, str2);

    free(str2);

    return str1;
}

/* 
 * Functie care returneaza numele fisierului inclus.
 */
char *get_file_name_from_include(char *some_string) {
    char *token;
    char *updated_string;

    token = strtok(some_string, "\" ");

    if (token != NULL) {
        token = strtok(NULL, "\" ");
        if (token != NULL) {
            updated_string 
                = strcpy((char*) calloc(strlen(token) + 1, 
                    sizeof(char)), token);
        }
    
    }

    return updated_string;
}

/* 
 * Functie care determina daca avem de a face cu vreo directiva.
 */
char *check_for_directives(char *some_string) {
    char *token;

    if (some_string[0] != '#') {
        return "no_directive";
    }

    token = strtok(some_string, " ");
    if (token == NULL) {
        return "wrong_code";
    }

    if (!strcmp(token, "#define")) {
        return "define";
    }

    if (!strcmp(token, "#include")) {
        return "include";
    }

    if (!strcmp(token, "#if")
        || !strcmp(token, "#elif")
        || !strcmp(token, "#else")
        || !strcmp(token, "#ifdef")
        || !strcmp(token, "#ifndef")
        || !strcmp(token, "#else")
        || !strcmp(token, "#endif")) {
        return "conditional";
    }

    return "wrong_code";
}

/*
 * Functie care converteste un pointer in offset int.
 */
int pointer_to_offset(char *pointer, char *pointer_ref) {
    return (int) (pointer - pointer_ref);
}

/* 
 * Functie care prelucreaza defineurile.
 * Se verifica daca in valoare sunt MACRO-uri
 * si se inlocuiesc daca este cazul.
 */
void manage_define(hashmap_t *self, storage_t *key_strg, char *some_string) {
    char *token, *key = NULL, *value = NULL, *buf, *value_index,
        *key_in_value_index, *copy_some_string;
    char *delims = "\t[]{}<>=+-*/%%!&|^.,:;() ";
    int quotation_mark = -1, i, j, temp_size, temp_offset, value_index_offset,
        after_concat_index_offset, key_in_value_index_offset,
        old_key_in_value_index_offset;

    if (!some_string) {
        return;
    }

    copy_some_string = strcpy((char*) calloc(strlen(some_string) + 1,
         sizeof(char)), some_string);
    token = strtok(some_string, delims);

    if (token != NULL) {
        token = strtok(NULL, delims);
        if (token != NULL) {
            value_index = token + strlen(token) + 1;
            value_index_offset 
                = pointer_to_offset(value_index, some_string);
            after_concat_index_offset = value_index_offset;
            key_in_value_index_offset = value_index_offset;
            old_key_in_value_index_offset = value_index_offset;
            key = strcpy((char*) calloc(1, strlen(token) + 1), token);
            if (!key)
                exit(12);
        }
    }
    
    while(token != NULL) {
       token = strtok(NULL, delims);
       printf("%s \n", token);
        if (token != NULL) {
            for (i = 0; i < strlen(token); ++i) {
                if (token[i] == '\"') {
                    quotation_mark *= -1;
                }
            }
            if ((quotation_mark == -1) && get_from_map(self, token) != NULL) {

                buf = strcpy((char*) 
                    calloc(strlen(get_from_map(self, token)) + 1,
                        sizeof(char)), get_from_map(self, token));
                if (!buf)
                    exit(12);
                key_in_value_index = token;
                key_in_value_index_offset 
                    = pointer_to_offset(key_in_value_index, some_string);
                if (value == NULL) {
                    temp_size 
                        = key_in_value_index_offset - value_index_offset;
                    value = (char*) calloc(temp_size + 1, sizeof(char));
                    if (!value)
                        exit(12);
                    temp_offset = value_index_offset;
                    for (j = 0; j < temp_size; ++j) {
                        value[j] = copy_some_string[j + temp_offset];
                    }
                    old_key_in_value_index_offset 
                        = key_in_value_index_offset + strlen(token);
                } else {
                    temp_size = strlen(value) 
                        + strlen(buf) 
                        + key_in_value_index_offset
                        - old_key_in_value_index_offset; 
                    value = realloc(value, (temp_size + 1) * sizeof(char));
                    if (!value)
                        exit(12);
                    temp_offset = strlen(value);
                    temp_size = key_in_value_index_offset
                        - old_key_in_value_index_offset;
                    for (j = 0, i = 0; j < temp_size; ++j, ++i) {
                        value[temp_offset + i] 
                            = copy_some_string[i 
                                + old_key_in_value_index_offset];
                    }
                    old_key_in_value_index_offset 
                        = key_in_value_index_offset + strlen(token);
                }
                temp_offset = strlen(value);
                temp_size = strlen(buf);
                for (j = 0; j < temp_size; ++j) {
                    value[j + temp_offset] = buf[j];
                }
                after_concat_index_offset 
                    = key_in_value_index_offset + strlen(token);
                key_in_value_index_offset += strlen(token);
                free(buf);
            }
        }
    }

    if (value != NULL) {
        temp_offset = strlen(value);
        temp_size = strlen(copy_some_string) 
            - after_concat_index_offset + strlen(value);
    } else {
        temp_offset = 0;
        temp_size = strlen(copy_some_string) - value_index_offset;
    }
    if (value == NULL) {
        value = (char*) calloc(temp_size + 1, sizeof(char));
        if (!value)
            exit(12);
    } else {
        value = realloc(value, (temp_size + 1) * sizeof(char));
        if (!value)
            exit(12);
    }

    for (j = temp_offset, i = 0; j < temp_size; ++j, ++i) {
       value[j] = copy_some_string[i + key_in_value_index_offset];
    }
    
    put_in_map(self, key, value);
    put_in_strg(key_strg, key);
    free(key);
    free(value);
    free(copy_some_string);
}

/* 
 * Functie care inlocuieste cheile din stinguri
 * cu valorile lor din map.
 */
char *update_string(hashmap_t *self, char *some_string) {
    char *token, *value = NULL, *buf, *key_in_value_index, *copy_some_string;
    char *delims = "\t[]{}<>=+-*/%%!&|^.,:;() ";
    int quotation_mark = -1, i, j, temp_size, temp_offset, value_index_offset,
        after_concat_index_offset, key_in_value_index_offset,
        old_key_in_value_index_offset;

    if (!some_string) {
        return NULL;
    }

    copy_some_string = strcpy((char*) calloc(strlen(some_string) + 1,
         sizeof(char)), some_string);
    token = strtok(some_string, delims);
    
    value_index_offset = 0;
    after_concat_index_offset = 0;
    key_in_value_index_offset = 0;
    old_key_in_value_index_offset = 0;
    
    while(token != NULL) {

        for (i = 0; i < strlen(token); ++i) {
            if (token[i] == '\"') {
                quotation_mark *= -1;
            }
        }
            
        if ((quotation_mark == -1) && get_from_map(self, token) != NULL) {

            buf = strcpy((char*) 
                calloc(strlen(get_from_map(self, token)) + 1,
                    sizeof(char)), get_from_map(self, token));
            if (!buf)
                exit(12);
            key_in_value_index = token;
            key_in_value_index_offset 
                = pointer_to_offset(key_in_value_index, some_string);
            
            if (value == NULL) {
                temp_size 
                    = key_in_value_index_offset - value_index_offset;
                value = (char*) calloc(temp_size + 1, sizeof(char));
                if (!value)
                    exit(12);
                temp_offset = value_index_offset;
                for (j = 0; j < temp_size; ++j) {
                    value[j] = copy_some_string[j + temp_offset];
                }
                old_key_in_value_index_offset 
                    = key_in_value_index_offset + strlen(token);
            } else {
                temp_size = strlen(value) 
                    + strlen(buf) 
                    + key_in_value_index_offset
                    - old_key_in_value_index_offset; 
                value = realloc(value, (temp_size + 1) * sizeof(char));
                if (!value)
                     exit(12);
                temp_offset = strlen(value);
                temp_size = key_in_value_index_offset 
                    - old_key_in_value_index_offset;
                for (j = 0, i = 0; j < temp_size; ++j, ++i) {
                    value[temp_offset + i] 
                        = copy_some_string[i 
                            + old_key_in_value_index_offset];
                }
                old_key_in_value_index_offset 
                    = key_in_value_index_offset + strlen(token);
            }
            temp_offset = strlen(value);
            temp_size = strlen(buf);
            for (j = 0; j < temp_size; ++j) {
                value[j + temp_offset] = buf[j];
            }
            after_concat_index_offset 
                = key_in_value_index_offset + strlen(token);
            key_in_value_index_offset += strlen(token);
            free(buf);
        }
        token = strtok(NULL, delims);
    }

    if (value != NULL) {
        temp_offset = strlen(value);
        temp_size = strlen(copy_some_string) 
            - after_concat_index_offset + strlen(value);
    } else {
        temp_offset = 0;
        temp_size = strlen(copy_some_string) - value_index_offset;
    }
    if (value == NULL) {
        value = (char*) calloc(temp_size + 1, sizeof(char));
        if (!value)
            exit(12);
    } else {
        value = realloc(value, (temp_size + 1) * sizeof(char));
        if (!value)
            exit(12);
    }
    for (j = temp_offset, i = 0; j < temp_size; ++j, ++i) {
       value[j] = copy_some_string[i + key_in_value_index_offset];
    }

    free(some_string);
    free(copy_some_string);

    return value;
}
