#include "hash-map.h"

/*Functie de hash:
 *
 * Se stabileste indexul unui entry prin
 * calcularea sumei codurilor ASCII
 * ale caracterelor cheii.
 *
 */
int hash_func(char *key) {
    int i, index_in_map = 0;

    for (i = 0; i < strlen(key); ++i) {
        if (key[i] > 'a' && key[i] < 'z') {
            index_in_map += key[i] - 'a';
        } else if (key[i] > 'A' && key[i] < 'Z') {
            index_in_map += key[i] - 'A';
        } else {
            index_in_map += key[i] - 32;
        }
    }

    return index_in_map % MAXLEN;
}

/*Functie de returnare a valorii unui entry din map:
 *
 * Se cauta in bucketul corespunzator indexului cheii date ca parametru
 * entry-ul din care face parte si se returneaza valoarea.
 *
 */
char *get_from_map(hashmap_t *self, char *key) {
    int i, index, list_len;
    entry_t **entries;

    index = hash_func(key);

    if (self->list_of_buckets[index] && self->list_of_buckets[index]->list) {
        entries = self->list_of_buckets[index]->list;
        list_len = self->list_of_buckets[index]->length;

        for (i = 0; i < list_len; ++i) {
            if (entries[i] && entries[i]->key) {
                if (!strcmp(entries[i]->key, key)) {
                    return entries[i]->value;
                }
            }
        }
    }

    return NULL;
}

/*Functie de adaugare a unui entry in map:
 *
 *  @La inceput se verifica daca exista un bucket la indexul corespunzator
 * cheii date ca parametru.
 *
 *  (A1)Daca nu exista:
 *  -Se aloca memorie pentru bucketul corespunzator indexului cheii date
 *  ca parametru (pentru inceput cu dimensiunea default 20).
 *  -In continuare se creeaza un prim entry in bucket pe baza cheii si a 
 *  valorii date ca parametrii. Se initializeaza lungimea cu 1 (numarul
 *  de entry-uri din bucket).
 *
 *  (B2)Daca exista:
 *  -Daca exista deja un entry ce contine cheia data ca parametru, i se 
 *  inlocuieste valoarea anterioara cu cea data ca parametru.
 *  -Daca nu, se verifica daca acesta este plin. In caz afirmativ i se
 *  dubleaza capacitatea.
 *  -In continuare se creeaza noul entry pe baza cheii si a valorii date 
 *  ca parametrii si se adauga in bucket. De asemenea se actualizeaza
 *  numarul de entry-uri din bucket (lungimea).
 *
 */
void put_in_map(hashmap_t *self, char *key, char *value) {
    int index, list_len, list_capacity, i;
    entry_t **entries;

    index = hash_func(key);
    
    /* A1 */
    if (!self->list_of_buckets[index]) {

        self->list_of_buckets[index] = calloc(1, sizeof(bucket_t));
        if (!self->list_of_buckets[index]) {
            exit(12);
        }

        self->list_of_buckets[index]->list 
            = calloc(INITIAL_BKT_CAPACITY, sizeof(entry_t*));
        if (!self->list_of_buckets[index]->list) {
            exit(12);
        }
        self->list_of_buckets[index]->capacity = INITIAL_BKT_CAPACITY;

        list_len = self->list_of_buckets[index]->length = 1;
        self->list_of_buckets[index]->list[list_len - 1] 
            = calloc(1, sizeof(entry_t));
        if (!self->list_of_buckets[index]->list[list_len - 1])
            exit(12);

        self->list_of_buckets[index]->list[list_len - 1]->key 
            = strcpy((char*) calloc(strlen(key) + 1, sizeof(char)), key);
        if (!self->list_of_buckets[index]->list[list_len - 1]->key)
            exit(12);
        
        if (value != NULL) {  
            self->list_of_buckets[index]->list[list_len - 1]->value 
                = strcpy((char*) calloc(strlen(value) + 1, sizeof(char)),
                    value);
            if (!self->list_of_buckets[index]->list[list_len - 1]->value)
                exit(12);
        }

    /* B2 */   
    } else {
        
        entries = self->list_of_buckets[index]->list;
        list_len = self->list_of_buckets[index]->length;
        
        for (i = 0; i < list_len; ++i) {
            if (entries[i] && entries[i]->key 
                && !strcmp(entries[i]->key, key)) {
                if (entries[i]->value) {
                    free(entries[i]->value);
                    entries[i]->value = NULL;
                }
                
                if (value != NULL) {  
                    entries[i]->value 
                        = strcpy((char*) calloc(strlen(value) + 1, 
                            sizeof(char)), value);
                    if (!entries[i]->value)
                        exit(12);
                } 

                return;
            }
        }

        if (list_len >= self->list_of_buckets[index]->capacity) {
            self->list_of_buckets[index]->capacity *= 2;
            list_capacity = self->list_of_buckets[index]->capacity;
            entries = realloc(entries, list_capacity * sizeof(entry_t*));
            if (!entries)
                exit(12);
            
            for (i = list_len - 1; i < list_capacity; ++i) {
                self->list_of_buckets[index]->list[i] = NULL;
            }
        }

        entries[list_len] = calloc(1, sizeof(entry_t));
        if (!entries[list_len])
            exit(12);

        entries[list_len]->key 
            = strcpy((char*) calloc(strlen(key) + 1, sizeof(char)), key);
        if (!entries[list_len]->key)
            exit(12);

        if (value != NULL) {  
            entries[list_len]->value 
                = strcpy((char*) calloc(strlen(value) + 1, 
                    sizeof(char)), value);
            if (!entries[list_len]->value)
                exit(12);
        }

        self->list_of_buckets[index]->length++;
    }
}

/*Functie de stergere a unui entry din map:
 *
 * Se identifica entry-ul pe baza cheii si se elimina
 * din bucket.
 * Restul entry-urilor se shifteaza inspre stanga.
 *
 */
void remove_from_map(hashmap_t *self, char *key) {
    int i, j, index, list_len;
    entry_t *temp;
    entry_t **entries;

    index = hash_func(key);

    if (self->list_of_buckets[index] && self->list_of_buckets[index]->list) {
        entries = self->list_of_buckets[index]->list;
        list_len = self->list_of_buckets[index]->length;
    } else {
        return;
    }

    for (i = 0; i < list_len; ++i) {
        if (!entries[i]) {
            return;
        }

        if (!entries[i]->key) {
            return;
        }
        
        if (!strcmp(entries[i]->key, key)) {
            temp = entries[i];
            if (i < list_len - 1) {
                for (j = i + 1; j < list_len; ++j, ++i) {
                    entries[i] = entries[j];
                }
            }
            entries[list_len - 1] = temp;
            free(entries[list_len - 1]->key);
            free(entries[list_len - 1]->value);
            free(entries[list_len - 1]);
            entries[list_len - 1]->key = NULL;
            entries[list_len - 1]->value = NULL;
            entries[list_len - 1] = NULL;
            self->list_of_buckets[index]->length--;
            return;
        }
    }
}

/*Functie de alocare a memoriei pentru map:
 *
 * Se aloca memorie pentru hashmap si pentru
 * vectorul de pointeri catre bucketuri.
 * 
 */
hashmap_t *alloc_map() {
    hashmap_t *self;

    self = calloc(1, sizeof(hashmap_t));
    if (self == NULL) {
        exit(12);
    }

    self->list_of_buckets = calloc(MAXLEN, sizeof(bucket_t *));
    if (self->list_of_buckets == NULL) {
        exit(12);
    }

    return self;
}

/*Functie de eliberare a memoriei alocate pentru map:
 *
 * Se parcurg succesiv toate zonele de memorie 
 * alocate si se elibereaza.
 * 
 */
void free_map(hashmap_t *self) {
    int index, i;
    bucket_t *bkt;

    if (!self)
        return;
    
    if (!self->list_of_buckets) {
        free(self);
        return;
    }

    for (index = 0; index < MAXLEN; ++index) {
        bkt = self->list_of_buckets[index];
        
        if (!bkt)
            continue;

        if (!bkt->list)
            continue;

        for (i = 0; i < bkt->capacity; ++i) {
            if (bkt->list[i] == NULL)
                continue;

            if (bkt->list[i]->key != NULL)
                free(bkt->list[i]->key);

            if (bkt->list[i]->value != NULL)
                free(bkt->list[i]->value);

            free(bkt->list[i]);
        }

        free(bkt->list);
        free(bkt);
    }

    free(self->list_of_buckets);
    free(self);
}
