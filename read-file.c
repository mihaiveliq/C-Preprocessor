#include "read-file.h"

/*
 * Functie care verifica daca o cheie exista in dictionar.
 */
int key_exist(char* string, storage_t *key_strg) {
    int i;

    for (i = 0; i < key_strg->length; ++i) {
        if (!strcmp(key_strg->names_bucket[i], string)) {
            return 1;
        }
    }
    return 0;
}

/*
 * Functie care citeste intr-un fisier auxiliar
 * dintr-un fisier.
 */
void read_file(FILE *fd_in, FILE *fd_aux, hashmap_t *map, 
    storage_t *key_strg, storage_t *dir_strg) {
    /* Declarari */
    int i, found_file = 0;
    char *rw_buf, *multiline_buf, *include_file, *token;
    FILE *fd_include;
    
    rw_buf = calloc(MAX_LEN_RW_BUF + 1, sizeof(char));
    if (!rw_buf)
        exit(12);

    
    /* File input */
    while (feof(fd_in)) {
        fgets(rw_buf, 256, fd_in);
        /* Defines */
        if (!strcmp(check_for_directives(rw_buf), "define")) {
            if (rw_buf[strlen(rw_buf) - 1] != '\\') {
                fgets(rw_buf, 256, fd_in);
                while (rw_buf[strlen(rw_buf) - 1] != '\\') {
                    multiline_buf = strncat((char*) 
                        realloc(multiline_buf, strlen(multiline_buf) 
                            + strlen(rw_buf)),
                        rw_buf, strlen(multiline_buf) - 1);
                    fgets(rw_buf, 256, fd_in);
                }
                manage_define(map, key_strg, multiline_buf);
                continue;
            }
            manage_define(map, key_strg, rw_buf);
        /* Includes */
        } else if (!strcmp(check_for_directives(rw_buf), "include")) {
            include_file = get_file_name_from_include(rw_buf);
            fd_include = fopen(include_file, "r+t");
            if (fd_include == NULL) {
                for (i = 0; i < dir_strg->length; ++i) {
                    fd_include = fopen(concat_file_with_directory(
                        dir_strg->names_bucket[i], include_file), "r+t");
                    if (fd_include == NULL) {
                        continue;
                    } else {
                        found_file = 1;
                        read_file(fd_include, fd_aux, map, key_strg, dir_strg);
                        fclose(fd_include);
                    }
                }
            } else {
                found_file = 1;
                read_file(fd_include, fd_aux, map, key_strg, dir_strg);
                fclose(fd_include);
            }
            if (found_file == 0) {
                exit(1);
            }
        /* Conditionals */
        } else if (!strcmp(check_for_directives(rw_buf), "conditional")) {
            token = strtok(rw_buf, " ");

            if (!strcmp(token, "#if") 
                || !strcmp(token, "#elif")
                || !strcmp(token, "#else")) {

                token = strtok(NULL, " ");
                if (token != NULL) {
                    if (get_from_map(map, token) != NULL
                        && get_from_map(map, token) != 0) {
                        fgets(rw_buf, 256, fd_in);
                        while (!strcmp(strtok(rw_buf, " "), "#endif")) {
                            fwrite(rw_buf, sizeof(char), strlen(rw_buf) + 1, fd_aux);
                            fgets(rw_buf, 256, fd_in);
                        }
                    }
                    continue;
                }
            } else if (!strcmp(token, "#ifdef") 
                || !strcmp(token, "ifndef")
                || !strcmp(token, "#else")) {
                
                token = strtok(NULL, " ");
                if (token != NULL) {
                    if (key_exist(token, key_strg)) {
                        fgets(rw_buf, 256, fd_in);
                        while (!strcmp(strtok(rw_buf, " "), "#endif")) {
                            fwrite(rw_buf, sizeof(char), strlen(rw_buf) + 1, fd_aux);
                            fgets(rw_buf, 256, fd_in);
                        }
                    }
                    continue;
                }
            }
        } else {
            fwrite(rw_buf, sizeof(char), strlen(rw_buf) + 1, fd_aux);
        }
        fgets(rw_buf, 256, fd_in);
    }
}

/*
 * Functie care citeste intr-un fisier auxiliar
 * de la stdin.
 */
void read_stdin(FILE *fd_aux, hashmap_t *map, storage_t *key_strg, 
    storage_t *dir_strg) {
    /* Declarari */
    int i, found_file = 0;
    char *rw_buf, *multiline_buf, *include_file, *token;
    FILE *fd_include;
    
    rw_buf = calloc(MAX_LEN_RW_BUF + 1, sizeof(char));
    if (!rw_buf)
        exit(12);

    /* Stdin input */
    while (feof(stdin)) {
        fgets(rw_buf, 256, stdin);
        /* Defines */
        if (!strcmp(check_for_directives(rw_buf), "define")) {
            if (rw_buf[strlen(rw_buf) - 1] != '\\') {
                multiline_buf = calloc(strlen(rw_buf) + 1, sizeof(char));
                fgets(rw_buf, 256, stdin);
                while (rw_buf[strlen(rw_buf) - 1] != '\\') {
                    multiline_buf = strncat((char*) 
                        realloc(multiline_buf, strlen(multiline_buf) 
                            + strlen(rw_buf)),
                        rw_buf, strlen(multiline_buf) - 1);
                    fgets(rw_buf, 256, stdin);
                }
                manage_define(map, key_strg, multiline_buf);
                continue;
            }
            manage_define(map, key_strg, rw_buf);
        /* Includes */
        } else if (!strcmp(check_for_directives(rw_buf), "include")) {
            include_file = get_file_name_from_include(rw_buf);
            fd_include = fopen(include_file, "r+t");
            if (fd_include == NULL) {
                for (i = 0; i < dir_strg->length; ++i) {
                    fd_include = fopen(concat_file_with_directory(
                        dir_strg->names_bucket[i], include_file), "r+t");
                    if (fd_include == NULL) {
                        continue;
                    } else {
                        found_file = 1;
                        read_file(fd_include, fd_aux, map, key_strg, dir_strg);
                        fclose(fd_include);
                    }
                }
            } else {
                found_file = 1;
                read_file(fd_include, fd_aux, map, key_strg, dir_strg);
                fclose(fd_include);
            }
            if (found_file == 0) {
                exit(1);
            }
        /* Conditionals */
        } else if (!strcmp(check_for_directives(rw_buf), "conditional")) {
            token = strtok(rw_buf, " ");

            if (!strcmp(token, "#if") 
                || !strcmp(token, "#elif")
                || !strcmp(token, "#else")) {

                token = strtok(NULL, " ");
                if (token != NULL) {
                    if (get_from_map(map, token) != NULL
                        && get_from_map(map, token) != 0) {
                        fgets(rw_buf, 256, stdin);
                        while (!strcmp(strtok(rw_buf, " "), "#endif")) {
                            fwrite(rw_buf, sizeof(char), strlen(rw_buf) + 1, fd_aux);
                            fgets(rw_buf, 256, stdin);
                        }
                    }
                    continue;
                }
            } else if (!strcmp(token, "#ifdef") 
                || !strcmp(token, "ifndef")
                || !strcmp(token, "#else")) {
                
                token = strtok(NULL, " ");
                if (token != NULL) {
                    if (key_exist(token, key_strg)) {
                        fgets(rw_buf, 256, stdin);
                        while (!strcmp(strtok(rw_buf, " "), "#endif")) {
                            fwrite(rw_buf, sizeof(char), strlen(rw_buf) + 1, fd_aux);
                            fgets(rw_buf, 256, stdin);
                        }
                    }
                    continue;
                }
            }
        } else {
            fwrite(rw_buf, sizeof(char), strlen(rw_buf) + 1, fd_aux);
        }
        fgets(rw_buf, 256, stdin);
    }
}

/*
 * Functie care scrie intr-un fisier dintr-un
 * fisier auxiliar.
 */
void write_in_output_file(FILE *fd_out, FILE *fd_aux, hashmap_t *map) {
    /* Declarari */
    char *rw_buf;
    
    rw_buf = calloc(MAX_LEN_RW_BUF + 1, sizeof(char));
    if(!rw_buf)
        exit(12);

    
    /* File input */
    while (feof(fd_aux)) {
        fgets(rw_buf, 256, fd_aux);
        rw_buf = update_string(map, rw_buf);
        fwrite(rw_buf, sizeof(char), strlen(rw_buf) + 1, fd_aux);
        fgets(rw_buf, 256, fd_aux);
    }
}

/*
 * Functie care scrie la consola dintr-un fisier auxiliar.
 */
void write_in_stdout(FILE *fd_aux, hashmap_t *map) {
    /* Declarari */
    char *rw_buf;
    
    rw_buf = calloc(MAX_LEN_RW_BUF + 1, sizeof(char));
    if(!rw_buf)
        exit(12);
    
    /* File input */
    while (feof(fd_aux)) {
        fgets(rw_buf, 256, fd_aux);
        rw_buf = update_string(map, rw_buf);
        fprintf(stdout, "%s\n", rw_buf);
        fgets(rw_buf, 256, fd_aux);
    }
}
