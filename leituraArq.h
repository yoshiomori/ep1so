#include <stdio.h>

/* se arq nao contem um \n, dah exit(-1)
 * caso contrario, le todos os caracteres ateh o \n e armazena os
 * max{ateh size - 1, numero de caracteres ateh o \n} primeiros
 * em s
 */
void leLinha(char *s, int size, FILE *arq);
