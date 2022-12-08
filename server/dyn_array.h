#include <stdio.h>
#include <malloc.h>

typedef struct S_DYN_ARRAY{
    void * * array;
    int size;
} DYN_ARRAY, * pDYN_ARRAY;

int da_init(pDYN_ARRAY, int);

void * da_get(DYN_ARRAY, int);

int da_insert(pDYN_ARRAY, int, void *);

int da_free(pDYN_ARRAY);