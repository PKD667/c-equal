#pragma once

typedef struct pair {
    void* key;
    long int value;
} pair;

typedef struct {
    pair* data;
    int size;
    int capacity;
} item;

typedef struct {
    item *items;
    int capacity;
} hashtable;


hashtable *hm_create(int capacity);
void hm_destroy(hashtable *hm);
int hm_add(hashtable *hm, char *key, long int value);
long int hm_get(hashtable *hm, char *key);
char* hm_getv(hashtable *hm, long int value);
int hm_rm(hashtable *hm, char *key);
int hm_visualize(hashtable *hm);
hashtable* hm_init(struct pair* kvlist,int size);

unsigned int hm_hash(hashtable *hm, char *key);;