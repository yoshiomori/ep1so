#include <stdio.h>
#include <stdlib.h>
#include "mallocSafe.h"

/* faz malloc verificando se este retorna NULL */
void *mallocSafe(size_t n) {
    void *pt;
    pt = malloc(n);
    if(pt == NULL) {
        fprintf(stderr, "Nao consegimos alocar memoria\n");
        exit(-1);
    }
    return pt;
}
