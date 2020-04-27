#include "parse-input.h"

input_t *parse_input(int argc, char **argv) {
    int i, got_input = 0, got_output = 0;
    char *out_name, *in_name;
    hashmap_t *map = NULL;
    storage_t *dir_strg = NULL;
    storage_t *key_strg = NULL;
    input_t *input;
    entry_t *entry;

    /* Alocari */
    map = alloc_map(map);
    dir_strg = alloc_strg(dir_strg);
    key_strg = alloc_strg(key_strg);

    if (argc > 1) {
        /* Prelucrarea parametrilor din linia de comanda */
        for (i = 1; i < argc; ++i) {
            /* Introducerea define-urilor in map */
            if (strcmp(argv[i], "-D") == 0) {
                ++i;
                entry = parse_define_in_entry(map, argv[i]);
                put_in_map(map, entry->key, entry->value);
                put_in_strg(key_strg, entry->key);

                if (entry != NULL) {
                    if (entry->key != NULL)
                        free(entry->key);
                    if (entry->value != NULL)
                        free(entry->value);
                    free(entry);
                }

            } else if ((argv[i][0] == '-') && (argv[i][1] == 'D')) {
                entry = parse_define_in_entry(map, take_out_flag(argv[i]));
                put_in_map(map, entry->key, entry->value);
                put_in_strg(key_strg, entry->key);

                if (entry != NULL) {
                    if (entry->key != NULL)
                        free(entry->key);
                    if (entry->value != NULL)
                        free(entry->value);
                    free(entry);
                }
            
            /* Stocarea numelor de directoare */
            } else if (strcmp(argv[i], "-I") == 0) {
                ++i;
                put_in_strg(dir_strg, argv[i]);

            } else if ((argv[i][0] == '-') && (argv[i][1] == 'I')) {
                put_in_strg(dir_strg, take_out_flag(argv[i]));

            /* Stocarea numelui fisierului de output */
            } else if (strcmp(argv[i], "-o") == 0) {
                if (got_output) {
                    /* eroare prea multe fisiere de output */
                    exit(2);
                }
                ++i;
                got_output = 1;

                out_name = strcpy((char*) calloc(strlen(argv[i]) + 1,
                    sizeof(char)), argv[i]);
                if (!out_name)
                    exit(12);

            } else if ((argv[i][0] == '-') && (argv[i][1] == 'o')) {
                if (got_output) {
                    /* eroare prea multe fisiere de output */
                    exit(2);
                }
                got_output = 1;

                out_name = strcpy((char*) calloc(strlen(argv[i]) - 1,
                    sizeof(char)), take_out_flag(argv[i]));
                if (!out_name)
                    exit(12);

            /* Verific introducerea eronata a parametrilor */
            } else if ((argv[i][0] == '-') && ((argv[i][1] != 'o') ||
                (argv[i][1] != 'D') || (argv[i][1] != 'I'))) {

                /* Tratare eroare parametrii cu flag eronat*/
                exit(3);
            
            /* Verific daca se primeste fisierul de input sau cel de output */
            } else {
                if (!got_input) {
                    /* Stocare fisier input */
                    got_input = 1;

                    in_name = strcpy((char*) calloc(strlen(argv[i]) + 1,
                        sizeof(char)), argv[i]);
                    if (!in_name)
                        exit(12);

                } else if (got_input && !got_output) {
                    /* Stocare fisier output */
                    got_output = 1;

                    out_name = strcpy((char*) calloc(strlen(argv[i]) + 1,
                        sizeof(char)), argv[i]);
                    if (!out_name)
                        exit(12);

                } else {
                    /* Tratare eroare wrong output file */
                    exit(2);
                }
            }
        }
    }

    input = (input_t*) calloc(1, sizeof(input_t));

    input->map = map;
    input->dir_strg = dir_strg;
    input->key_strg = key_strg;
    input->got_input = got_input;
    input->got_output = got_output;
    input->in_name = in_name;
    input->out_name = out_name;

    return input;
}
