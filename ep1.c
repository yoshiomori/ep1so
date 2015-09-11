#define _GNU_SOURCE
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>

#include "mallocSafe.h"

#define NUM_MAX_PROC (INT_MAX - 10)
#define TAM_MAX_NOME_PROC 1000

void ***proc;
/* proc[j] contem as informacoes do j-esimo processo no
 * arquivo de traces:
 * t0, nome, dt, deadline, p, (pthread_t) thread que executa o processo,
 * r := dt - tempo de CPU consumido, mutex1, mutex2, terminou
 * (nessa ordem)
 */


/* PROTOTIPOS DAS FUNCOES AUXILIARES */

void *trabalha(void* arg);

void criaThread(int j);

void despausaThread(int j);

void pausaThread(int j);

int main(int argc, char **argv){
  FILE *arqTraces;
  /* 1. Verificamos numero de argumentos */
  if(argc < 4){
    printf("Falta argumentos\n");
    exit(0);
  }

  /* 2. Abrimos arquivo de traces */
  arqTraces = fopen(argv[2], "r");
  if(!arqTraces){
    perror(argv[1]);
    exit(EXIT_FAILURE);
  }
  
  /* 3. Armazenamos traces na tabela proc */
  proc = mallocSafe(NUM_MAX_PROC);
  int j;
  for(j = 0; j < NUM_MAX_PROC; j++) {
    proc[j] = mallocSafe(10*sizeof(void**));
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

  /* 4. Criamos vetor de threads de cada CPU */
  int numCPUs = sysconf(_SC_NPROCESSORS_ONLN);
  int* threadDaCPU = mallocSafe(numCPUs * sizeof(int));
    /* threadDaCPU[k] eh tq proc[threadDaCPU[k]] tem as informacoes
     * da thread trabalhadora que estah executando na cpu k,
     * threadDaCPU[k] eh -1 se nao houver thread executando na cpu k
     */
  int k;
  for(k = 0; k < numCPUs; k++) {
    threadDaCPU[k] = -1;
  }

  /* 5. Para cada CPU k, criamos um cpu_set_t que contem k e apenas k */
  cpu_set_t* cpuSet = mallocSafe(numCPUs * sizeof(cpu_set_t));
  for(k = 0; k < numCPUs; k++) {
    CPU_ZERO(&cpuSet[k]);
    CPU_SET(k, &cpuSet[k]);
  }

  /* 6. Criamos uma fila de prioridade threadsPausadas inicialmente vazia */
  /* TODO */

  /* 7. */
  
  
  


  fclose(arqTraces);
  return 0;
}


/* funcoes auxiliares */


int fatorial(int n) {
    if(n == 0) return 1;
    return n*fatorial(n-1);
}

/* funcao das threads trabalhadoras 
 * Recebe ponteiro para inteiro j
 * onde j eh o indice da thread na tabela proc
 */
void *trabalha(void* arg) {
  int meuIndice = *(int*)arg;

  while(1) {
    pthread_mutex_lock((pthread_mutex_t*)proc[meuIndice][7]); /* lock(mutex1) */
    pthread_mutex_unlock((pthread_mutex_t*)proc[meuIndice][7]); /* unlock(mutex1) */
    while(1) {
        fatorial(3); /* fazemos qualquer coisa para gastar tempo de CPU */
        if(pthread_mutex_trylock((pthread_mutex_t*)proc[meuIndice][8]) == 0) { /* se trylock(mutex2) */
            /* isto eh, demos trylock no mutex2 com sucesso */
            pthread_mutex_unlock((pthread_mutex_t*)proc[meuIndice][8]); /* unlock(mutex2) */
            break;
        }
    }
  }   

  free((int*)arg);
}


/* cria thread para processo proc[j], colocando os valores adequados para 
 * proc[j][5..9] */
void criaThread(int j) {
  proc[j][5] = (void*)mallocSafe(sizeof(pthread_t));
  proc[j][6] = (void*)mallocSafe(sizeof(double));
  proc[j][7] = (void*)mallocSafe(sizeof(pthread_mutex_t));
  proc[j][8] = (void*)mallocSafe(sizeof(pthread_mutex_t));
  proc[j][9] = (void*)mallocSafe(sizeof(pthread_mutex_t));
  
  *(double*)proc[j][6] = *(double*)proc[j][2]; /* r:= dt */
  pthread_mutex_init((pthread_mutex_t*)proc[j][7], NULL);
  pthread_mutex_init((pthread_mutex_t*)proc[j][8], NULL);
  pthread_mutex_init((pthread_mutex_t*)proc[j][9], NULL);
  
  pthread_mutex_lock(proc[j][7]); /* lock(mutex1); */
  
  int *indiceDaThread = mallocSafe(sizeof(int));
  *indiceDaThread = j;

  pthread_create((pthread_t*)proc[j][5], NULL, &trabalha, (void*)indiceDaThread); /* criamos thread */ 
}

/* despausa a thread do processo proc[j] */
void despausaThread(int j) {
  pthread_mutex_lock(proc[j][8]); /* lock(mutex2) */
  pthread_mutex_unlock(proc[j][7]); /* unlock(mutex1) */
}

/* pausa a thread do processo proc[j] */
void pausaThread(int j) {
  pthread_mutex_lock(proc[j][7]); /* lock(mutex1) */
  pthread_mutex_unlock(proc[j][8]); /* unlock(mutex2) */
}
