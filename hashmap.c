#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "hashmap.h"


typedef struct HashMap HashMap;
int enlarge_called=0;

struct HashMap {
    Pair ** buckets;
    long size; 
    long capacity;
    long current; 
};

Pair *createPair(char *key, void *value) {
    Pair *new = (Pair *)malloc(sizeof(Pair));
    new->key = key;
    new->value = value;
    return new;
}

long hash(char *key, long capacity) {
    unsigned long hash = 0;
    char *ptr;
    for (ptr = key; *ptr != '\0'; ptr++) {
        hash += hash * 32 + tolower(*ptr);
    }
    return hash % capacity;
}

int is_equal(void *key1, void *key2) {
    if (key1 == NULL || key2 == NULL) return 0;
    if (strcmp((char *)key1, (char *)key2) == 0) return 1;
    return 0;
}

void insertMap(HashMap *map, char *key, void *value) {
    long idx = hash(key, map->capacity);

    while (map->buckets[idx] != NULL && map->buckets[idx]->key != NULL) {
        if (is_equal(map->buckets[idx]->key, key)) {
            map->buckets[idx]->value = value;
            return;
        }
        idx = (idx + 1) % map->capacity;
    }

    map->buckets[idx] = createPair(key, value);
    map->size++;

    if ((double)map->size / map->capacity > 0.75) {
        enlarge(map); 
    }
}
void enlarge(HashMap *map) {
    enlarge_called = 1; 

    Pair **old_buckets = map->buckets;
    long old_capacity = map->capacity;

    map->capacity *= 2;
    map->buckets = (Pair **)calloc(map->capacity, sizeof(Pair *));
    map->size = 0;

    for (long i = 0; i < old_capacity; i++) {
        if (old_buckets[i] != NULL && old_buckets[i]->key != NULL) {
            insertMap(map, old_buckets[i]->key, old_buckets[i]->value);
            free(old_buckets[i]);
        }
    }

    free(old_buckets);
}

HashMap * createMap(long capacity) {
    HashMap *map = (HashMap *)malloc(sizeof(HashMap));
    map->buckets = (Pair **)calloc(capacity, sizeof(Pair *));
    map->size = 0;
    map->capacity = capacity;
    map->current = -1;
    return map;
}

void eraseMap(HashMap * map,  char * key) { 
    long idx = hash(key, map->capacity);

    while (map->buckets[idx] != NULL) {
        if (is_equal(map->buckets[idx]->key, key)) {
            map->buckets[idx]->key = NULL; 
            map->size--;
            return;
        }
        idx = (idx + 1) % map->capacity;
    }


}

Pair * searchMap(HashMap * map,  char * key) {
    long idx = hash(key, map->capacity);
    while (map->buckets[idx] != NULL) {
        if (is_equal(map->buckets[idx]->key, key)) {
            map->current = idx;
            return map->buckets[idx];
        }
        idx = (idx + 1) % map->capacity;
    }

    return NULL;
}

Pair * firstMap(HashMap * map) {
    for (long i = 0; i < map->capacity; i++) {
        if (map->buckets[i] != NULL && map->buckets[i]->key != NULL) {
            map->current = i;
            return map->buckets[i];
        }
    }
    return NULL;
}

Pair * nextMap(HashMap * map) {
    for (long i = map->current + 1; i < map->capacity; i++) {
        if (map->buckets[i] != NULL && map->buckets[i]->key != NULL) {
            map->current = i;
            return map->buckets[i];
        }
    }
    return NULL;
}
