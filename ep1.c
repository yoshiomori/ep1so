#define _GNU_SOURCE
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>

#include "mallocSafe.h"
#include "filaDePrioridade.h"

#define NUM_MAX_PROC (INT_MAX - 10)
#define TAM_MAX_NOME_PROC 1000
#define QUANTUM 0.1

void ***proc;
/* proc[j] contem as informacoes do j-esimo processo no
 * arquivo de traces:
 * t0, nome, dt, deadline, p, (pthread_t) thread que executa o processo,
 * r := dt - tempo de CPU consumido, mutex1, mutex2, terminou
 * (nessa ordem)
 */


/* PROTOTIPOS DAS FUNCOES AUXILIARES */

int menor1(int i, int j);
int menor2(int i, int j);
int menor3(int i, int j);
int menor5(int i, int j);
int menor6(int i, int j);

void *trabalha(void* arg);

void criaThread(int j);

void despausaThread(int j);

void pausaThread(int j);

/* MAIN */
int main(int argc, char **argv){
  FILE *arqTraces, *arqSaida;

  /* 1. Verificamos numero de argumentos */
  if(argc != 4 && argc != 5){
    printf("Numero de argumentos incorreto. Era para recebermos 3 ou 4 argumentos.\n");
    exit(0);
  }
  /* 2. Lemos os argumentos e abrimos arquivos de entrada e saida */
  /* 2.1. Lemos o primeiro argumento em escalonador */
  int escalonador = atoi(argv[1]);
  /* printf("escalonador = %d\n", escalonador); */
  /* 2.2. Abrimos arquivo de traces */
  arqTraces = fopen(argv[2], "r");
  if(!arqTraces){
    perror(argv[0]);
    exit(EXIT_FAILURE);
  }
  /* 2.3. Abrimos arquivo de saida */
  arqSaida = fopen(argv[3], "w");
  if(!arqSaida) {
      perror(argv[0]);
      exit(EXIT_FAILURE);
  }
  /* 2.4. Vemos se devemos rodar no modo depuracao */
  int modoDepuracao = 0;
  if(argc == 5 && strcmp(argv[4], "d") == 0) {
    modoDepuracao = 1;
  }
  /* printf("modoDepuracao == %d\n", modoDepuracao);*/
  
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
  /* TESTE COMECA 
  for(j = 0; j < numProc; j++) {
    printf("%lf %s %lf %lf %d\n", *(double*)proc[j][0], 
            (char*)proc[j][1], *(double*)proc[j][2],
            *(double*)proc[j][3], *(int*)proc[j][4]);
  }
  TESTE TERMINA */

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

  /* 6. Criamos uma fila de prioridade inicialmente vazia
   * que contera as threads pausadas (e possivelmente
   * algumas threads terminadas) */
  if(escalonador == 1) {
    PQinit(numProc, menor1);
  } else if(escalonador == 2) {
    PQinit(numProc, menor2);
  } else if(escalonador == 3) {
    PQinit(numProc, menor3);
  } else if(escalonador == 5) {
    PQinit(numProc, menor5);
  } else if(escalonador == 6) {
    PQinit(numProc, menor6);
  }
  
  /* fprintf(stderr, "vamos comecar o loop de escalonamento\n");*/
  /* 7. Loop de escalonamento */
  int linhaDaSaida = 0;
  int mudancasDeContexto = 0;
  time_t tComeco, tAtual, tDecorrido;
  tComeco = (tAtual = time(NULL));
  tDecorrido = tAtual - tComeco;
  int threadsTerminadas = 0;
  int proxProc = 0;
  while(threadsTerminadas < numProc) {
    printf("LOOP! threadsTerminadas = %d, numProc = %d\n", threadsTerminadas, numProc);
    printf("tAtual - tComeco = %lf, tDecorrido = %lf\n", (double)(tAtual - tComeco), (double)tDecorrido);
    /* 7.1 Atualizamos proc[j] para cada j processo velho (em execucao) */
    /* printf("Vamos atualizar proc[j] para j processo velho\n");*/
    for(k = 0; k < numCPUs; k++) {
      if(threadDaCPU[k] != -1) {
          *(double*)proc[threadDaCPU[k]][6] -= tDecorrido; /* r -= tDecorrido */
      }
    }
    /* 7.2 Atualizamos proc[j] para cada j processo novo */
    /* printf("Vamos atualizar proc[j] para j processo novo\n"); */
    for(j = proxProc; j < numProc; j++) {
        if(*(double*)proc[j][0] > tAtual - tComeco) { /* se processo j nao chegou ainda */
          printf("processo nao chegou ainda!\n");
          break;
        }
        if(modoDepuracao == 1) {
          fprintf(stderr, "Processo de indice %d (%s) chegou no sistema!\n", j, (char*)proc[j][1]);
        }
        criaThread(j); /* criamos thread para executar novo processo
                          (a thread comeca pausada) */
        PQinsert(j); /* inserimos o novo processo na fila de prioridade */
    }
    proxProc = j;
    /* 7.3 Limpamos threads que jah terminaram */
    /* printf("Vamos limpar threads que jah terminaram\n"); */
    for(k = 0; k < numCPUs; k++) {
      if(threadDaCPU[k] != -1 && *(int*)proc[threadDaCPU[k]][9] == 1) {
        /* se thread executando na CPU k jah indicou que terminou */
        /* esperamos ela realmente terminar */
        if(pthread_join(*(pthread_t*)proc[threadDaCPU[k]][5], NULL) != 0) {
          fprintf(stderr, "Erro ao dar join com thread trabalhadora terminada\n");
          exit(EXIT_FAILURE);
        }
        if(modoDepuracao == 1) {
          fprintf(stderr, "Processo de indice %d terminou e vai aparecer na linha %d da saida", threadDaCPU[k], linhaDaSaida);
        }
        fprintf(arqSaida, "%s %lf %lf\n", (char *)proc[threadDaCPU[k]][1], (double)(tAtual - tComeco),
                (double)(tAtual - tComeco - *(double *)proc[threadDaCPU[k]][0]));
        linhaDaSaida++;
        threadsTerminadas++;
        /* liberamos a CPU */
        threadDaCPU[k] = -1;
      }
    }
    /* printf("antes de 7.4, temos threadsTerminadas = %d\n", threadsTerminadas); */
    /* 7.4 Colocamos threads para serem executadas nas CPUs livres */
    /* printf("Vamos colocar threads para serem executadas nas CPUs livres\n"); */
    for(k = 0; k < numCPUs && !PQempty(); k++) {
      if(threadDaCPU[k] == -1) {
        PQprint();
        threadDaCPU[k] = PQdelmin();
        printf("delmin retornou %d\n", threadDaCPU[k]);
        pthread_setaffinity_np(*(pthread_t*)proc[threadDaCPU[k]][5], sizeof(cpu_set_t), &cpuSet[k]);
        despausaThread(threadDaCPU[k]);
        if(modoDepuracao == 1) {
          fprintf(stderr, "Processo do %d-esimo trace (%s) comecou a usar a CPU %d, que estava livre!\n", threadDaCPU[k],
                  (char*)proc[threadDaCPU[k]][1], k);
        }
      }
    }
    /* 7.5.1 Fazemos mudanca de contexto se o escalonamento for preemptivo com criterio de comparacao double */
    /* printf("Mudanca de contexto\n"); */
    if(escalonador == 3 || escalonador == 6) {
      while(!PQempty()) {
        /* 7.5.1.1 vamos encontrar indMax tal que threadDaCPU[k] tem o menor privilegio possivel */
        /* max eh o valor de r:= dt - tempo consumido, se escalonador == 3
         * max eh o valor de deadline, se escalonador == 6
         */
        int indMax; double max;
        /* inicializamos indMax e max */
        indMax = 0;
        /* como PQ != vazio, segue, pelo passo 7.4, que todas as cpus estao ocupadas */
        if(escalonador == 3) max = *(double*)proc[threadDaCPU[0]][6];
        if(escalonador == 6) max = *(double*)proc[threadDaCPU[0]][3];
        for(k = 1; k < numCPUs; k++) {
          double valor;
          if(escalonador == 3) valor = *(double*)proc[threadDaCPU[k]][6];
          if(escalonador == 6) valor = *(double*)proc[threadDaCPU[k]][3];
          if(valor > max) {
            indMax = k;
            max = valor;
          }
        }
        /* 7.5.1.2 Se a primeira thread da fila de prioridade tem prioridade sobre threadDaCPU[indMax]
         * entao colocamos aquela no lugar desta
         */
        if(PQfirst() >= max) {
            break;
        }
        PQprint();
        int min = PQdelmin();
        pausaThread(threadDaCPU[indMax]);
        if(modoDepuracao == 1) {
          fprintf(stderr, "Processo do %d-esimo trace (%s) liberou a CPU %d, devido a mudanca de contexto!\n",
                  threadDaCPU[indMax], (char*)proc[threadDaCPU[indMax]][1], indMax);
        }
        PQinsert(threadDaCPU[indMax]);
        pthread_setaffinity_np(*(pthread_t*)proc[min][5], sizeof(cpu_set_t), &cpuSet[indMax]);
        despausaThread(min);
        if(modoDepuracao == 1) {
          fprintf(stderr, "Processo do %d-esimo trace (%s) comecou a usar a CPU %d, devido a mudanca de contexto!\n",
                  min, (char*)proc[min][1], indMax);
        }
        threadDaCPU[indMax] = min;

        mudancasDeContexto++;

      }
    }
    /* 7.5.2 Fazemos mudanca de contexto se o escalonamento for preemptivo com criterio de comparacao int */
    if(escalonador == 5) {
      while(!PQempty()) {
        /* 7.5.2.1 vamos encontrar indMax tal que threadDaCPU[k] tem o menor privilegio possivel */
        /* max eh o valor de r:= dt - tempo consumido, se escalonador == 3
         * max eh o valor de deadline, se escalonador == 6
         */
        int indMax; int max;
        /* inicializamos indMax e max */
        indMax = 0;
        /* como PQ != vazio, segue, pelo passo 7.4, que todas as cpus estao ocupadas */
        max = *(int*)proc[threadDaCPU[0]][4];
        for(k = 1; k < numCPUs; k++) {
          int valor;
          valor = *(int*)proc[threadDaCPU[k]][4];
          if(valor > max) {
            indMax = k;
            max = valor;
          }
        }
        /* 7.5.2.2 Se a primeira thread da fila de prioridade tem prioridade sobre threadDaCPU[indMax]
         * entao colocamos aquela no lugar desta
         */
        if(PQfirst() >= max) {
            break;
        }
        PQprint();
        int min = PQdelmin();
        pausaThread(threadDaCPU[indMax]);
        if(modoDepuracao == 1) {
          fprintf(stderr, "Processo do %d-esimo trace (%s) liberou a CPU %d, devido a mudanca de contexto!\n",
                  threadDaCPU[indMax], (char*)proc[threadDaCPU[indMax]][1], indMax);
        }
        PQinsert(threadDaCPU[indMax]);
        pthread_setaffinity_np(*(pthread_t*)proc[min][5], sizeof(cpu_set_t), &cpuSet[indMax]);
        despausaThread(min);
        if(modoDepuracao == 1) {
          fprintf(stderr, "Processo do %d-esimo trace (%s) comecou a usar a CPU %d, devido a mudanca de contexto!\n",
                  min, (char*)proc[min][1], indMax);
        }
        threadDaCPU[indMax] = min;

        mudancasDeContexto++;

      }
    }

    /* 7.6 Aguardamos e calculamos tempo */
    /* printf("Calculamos o tempo\n"); */
    struct timespec tempoADormir;
    tempoADormir.tv_sec = 1;
    tempoADormir.tv_nsec = 0;
    nanosleep(&tempoADormir, NULL);
    tDecorrido = time(NULL) - tAtual;
    tAtual += tDecorrido;

    printf("\n");
  }

  fprintf(arqSaida, "%d\n", mudancasDeContexto);
  if(modoDepuracao == 1) {
    fprintf(stderr, "mudancas de contexto: %d\n", mudancasDeContexto);
  }
  
  
  
  


  fclose(arqTraces);
  fclose(arqSaida);
  return 0;
}


/* DEFINICOES DAS FUNCOES AUXILIARES */

/* Retorna 1 se processo de indice
 * i tem precedencia em relacao ao de indice j
 * segundo o escalonamento (1) FCFS
 * Retorna 0 se i e j tem a mesma precedencia
 * ou j tem precedencia sobre i
 */
int menor1(int i, int j) {
  /* comparamos os t0 do i e do j */
  if(*(double*)proc[i][0] < *(double*)proc[j][0]) return 1; 
  return 0;
}

int menor2(int i, int j) {
  /* comparamos os dt do i e do j */
  /* printf("menor2(%d, %d);\n", i, j); */
  if(*(double*)proc[i][2] < *(double*)proc[j][2]) {
    /* printf("é menor!\n"); */
    return 1;
  }
  /* printf("não é menor \n"); */
  return 0;
}

int menor3(int i, int j) {
  /* comparamos os r (= dt - tempo consumido) do i e do j */
  if(*(double*)proc[i][6] < *(double*)proc[j][6]) return 1;
  return 0;
}

int menor5(int i, int j) {
  /* comparamos os p do i e do j */
  if(*(int*)proc[i][4] < *(int*)proc[j][4]) return 1;
  return 0;
}

int menor6(int i, int j) {
  /* comparamos os deadlines do i e do j */
  if(*(double*)proc[i][3] < *(double*)proc[j][3]) return 1;
  return 0;
}

int fatorial(int n) {
    /*printf("FATORIAL!\n");*/
    if(n == 0) return 1;
    return n*fatorial(n-1);
}

/* funcao das threads trabalhadoras 
 * Recebe ponteiro para inteiro j
 * onde j eh o indice da thread na tabela proc
 */
void *trabalha(void* arg) {
  int meuIndice = *(int*)arg;

  printf("TRAB! meuIndice = %d\n", meuIndice);

  while(1) {
    pthread_mutex_lock((pthread_mutex_t*)proc[meuIndice][7]); /* lock(mutex1) */
    pthread_mutex_unlock((pthread_mutex_t*)proc[meuIndice][7]); /* unlock(mutex1) */
    while(1) {
        fatorial(3); /* fazemos qualquer coisa para gastar tempo de CPU */
        if(*(double*)proc[meuIndice][6] <= 0) { /* se r = dt - tempo consumido <= 0 */
          goto fim;
        }
        if(pthread_mutex_trylock((pthread_mutex_t*)proc[meuIndice][8]) == 0) { /* se trylock(mutex2) */
            /* isto eh, demos trylock no mutex2 com sucesso */
            pthread_mutex_unlock((pthread_mutex_t*)proc[meuIndice][8]); /* unlock(mutex2) */
            break;
        }
    }
  }   

fim:
  free((int*)arg);
  *(int*)proc[meuIndice][9] = 1; /* terminou := 1 */
  return NULL;
}


/* cria thread para processo proc[j], colocando os valores adequados para 
 * proc[j][5..9] */
void criaThread(int j) {
  proc[j][5] = (void*)mallocSafe(sizeof(pthread_t));
  proc[j][6] = (void*)mallocSafe(sizeof(double));
  proc[j][7] = (void*)mallocSafe(sizeof(pthread_mutex_t));
  proc[j][8] = (void*)mallocSafe(sizeof(pthread_mutex_t));
  proc[j][9] = (void*)mallocSafe(sizeof(int));
  
  *(double*)proc[j][6] = *(double*)proc[j][2]; /* r := dt */
  pthread_mutex_init((pthread_mutex_t*)proc[j][7], NULL);
  pthread_mutex_init((pthread_mutex_t*)proc[j][8], NULL);
  *(int*)proc[j][9] = 0; /* terminou := 0 */
  
  pthread_mutex_lock((pthread_mutex_t*)proc[j][7]); /* lock(mutex1); */
  
  int *indiceDaThread = mallocSafe(sizeof(int));
  *indiceDaThread = j;

  pthread_create((pthread_t*)proc[j][5], NULL, &trabalha, (void*)indiceDaThread); /* criamos thread */ 
}

/* despausa a thread do processo proc[j] */
void despausaThread(int j) {
  pthread_mutex_lock((pthread_mutex_t*)proc[j][8]); /* lock(mutex2) */
  pthread_mutex_unlock((pthread_mutex_t*)proc[j][7]); /* unlock(mutex1) */
}

/* pausa a thread do processo proc[j] */
void pausaThread(int j) {
  pthread_mutex_lock((pthread_mutex_t*)proc[j][7]); /* lock(mutex1) */
  pthread_mutex_unlock((pthread_mutex_t*)proc[j][8]); /* unlock(mutex2) */
}
