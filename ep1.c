#define _GNU_SOURCE
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>

#include "mallocSafe.h"

#define NUM_MAX_PROC (INT_MAX - 10)
#define TAM_MAX_NOME_PROC 1000

void ***proc;

int main(int argc, char **argv){
  FILE *arqTraces;
  /* 1. Verificar numero de argumentos */
  if(argc < 4){
    printf("Falta argumentos\n");
    exit(0);
  }

  /* 2. Abrir arquivo de traces */
  arqTraces = fopen(argv[2], "r");
  if(!arqTraces){
    perror(argv[1]);
    exit(EXIT_FAILURE);
  }
  
  /* 3. Armazenar traces na tabela proc */
  proc = mallocSafe(NUM_MAX_PROC);
  int j;
  for(j = 0; j < NUM_MAX_PROC; j++) {
    proc[j] = mallocSafe(9*sizeof(void**));
    proc[j][0] = (void*)mallocSafe(sizeof(double));
    proc[j][1] = (void*)mallocSafe(TAM_MAX_NOME_PROC*sizeof(char));
    proc[j][2] = (void*)mallocSafe(sizeof(double));
    proc[j][3] = (void*)mallocSafe(sizeof(double));
    proc[j][4] = (void*)mallocSafe(sizeof(int));
    if(fscanf(arqTraces, "%lf %s %lf %lf %d\n", (double*)proc[j][0],
            (char*)proc[j][1], (double*)proc[j][2],
            (double*)proc[j][3], (int*)proc[j][4]) != 5) {
      /* esta linha eh a primeira linha vazia do arquivo
       * entao liberamos a memoria que alocamos desnecessariamente
       */
      int l;
      for(l = 0; l < 5; l++) {
        free(proc[j][l]);
      }
      free(proc[j]);
      break;
    }
  }

  int numProc = j;
  /* TESTE COMECA */
  for(j = 0; j < numProc; j++) {
    printf("%lf %s %lf %lf %d\n", *(double*)proc[j][0], 
            (char*)proc[j][1], *(double*)proc[j][2],
            *(double*)proc[j][3], *(int*)proc[j][4]);
  }


  /* TESTE TERMINA */


  
  


  fclose(arqTraces);
  return 0;
}
