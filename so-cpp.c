#include "read-file.h"

int main(int argc, char **argv) {
    /* Declarari */
    int i, got_input, got_output;
    char *out_name, *in_name;
    hashmap_t *map;
    storage_t *dir_strg, *key_strg;
    input_t *input;
    FILE *fd_in, *fd_out, *fd_aux; 

    /* Prelucrarea parametrilor din linia de comanda */
    input = parse_input(argc, argv);

    map = input->map;
    dir_strg = input->dir_strg;
    key_strg = input->key_strg;
    got_input = input->got_input;
    got_output = input->got_output;
    in_name = input->in_name;
    out_name = input->out_name;

    free(input);

    fd_aux = fopen("aux_file", "w+t");

    if (!got_input) {
        /* Stdin input */
        read_stdin(fd_aux, map, key_strg, dir_strg);
    } else {
        /* File input */
        fd_in = fopen(in_name, "rt");
        read_file(fd_in, fd_aux, map, key_strg, dir_strg);
        fclose(fd_in);
    }

    for (i = 0; i < key_strg->length; ++i) {
        put_in_map(map, key_strg->names_bucket[i], 
            update_string(map, get_from_map(map, key_strg->names_bucket[i])));
    }

    fseek(fd_aux, 0, SEEK_SET);

    if (!got_output) {
        /* Stdout output */
        write_in_stdout(fd_aux, map);
        
    } else {
        fd_out = fopen(out_name, "wt");
        write_in_output_file(fd_out, fd_aux, map);
        fclose(fd_out);
    }

    fclose(fd_aux);

    /* Eliberari */
    free_map(map);
    free_strg(dir_strg);
    free_strg(key_strg);
    if (out_name != NULL) {
        free(out_name);
    }
    if (out_name != NULL) {
        free(in_name);
    }

    return 0;
}
