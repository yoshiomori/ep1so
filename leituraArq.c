#include "leituraArq.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


void leLinha(char *s, int size, FILE *arq) {
  if(fgets(s, size, arq) == NULL || strlen(s) <= 0) {
    fprintf(stderr, "erro na leitura de linha\n");
    exit(-1);
  }
  if(s[strlen(s) - 1] == '\n') /* se lemos a linha inteira */
    return;

  /* lemos e jogamos fora o restante da linha */
  while(1) {
    int ch = fgetc(arq); 
    if(ch == EOF) { /* erro */
      fprintf(stderr, "erro na leitura de linha\n");
      exit(-1); 
    }
    if(ch == '\n') { /* lemos a linha inteira */
      break;  
    }
  }
}
