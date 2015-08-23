#include <stdio.h>

/* estrutura que permite strings contendo o caractere nulo */
typedef struct stringComFimMarcado {
    char *str;
    int indiceFim; /* indice do caractere nulo marcador de final */
} stringComFimMarcado;


/* retorna uma stringComFimMarcado
   contendo todos os caracteres do comeco do arquivo ateh o primeiro
   \n ou EOF. A string eh terminada com um caractere nulo. */
char *leLinha(FILE *arq);
