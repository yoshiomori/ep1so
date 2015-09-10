#include "mallocSafe.h"
#include "leituraArq.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>

#define NUM_MAX_CARAC_A_LER 10000
#define PATH_MAX 4096
#define ARG_MAX 131072

int main(){
  char *pcwd;
  char *arg[ARG_MAX];
  pid_t childPid;
  char *entrada = mallocSafe(NUM_MAX_CARAC_A_LER * sizeof(char));
  char *str;
  int i, ii; /* Iterador */

  while(1){
    /* 1. Imprimir o diretório */
    pcwd = getcwd(NULL, 0);
    printf("[%s] ", pcwd);
    
    /* 2. Ler entrada do usuário */
    leLinha(entrada, NUM_MAX_CARAC_A_LER, stdin); /* le ateh NUM_MAX_CARAC_A_LER - 1 caracteres,
                                                     jogando fora os demais caracteres */
    
    /* 3. Parsear entrada */
    i = 0;
    str = strtok(entrada, " \n");
    while(str && i < ARG_MAX - 1){
      arg[i++] = str;
      str = strtok(NULL, " \n");
    }
    
    arg[i] = NULL;

    /* neste ponto, vale
     * arg[0] == comando ou binario, arg[1] == primeiro argumento, ..., 
     * arg[i-1] == (i-1)-esimo argumento, ..., arg[i] == NULL
     */

    /* 4. Executar comando/binario */
    /* 4.1 Comando cd */
    if(!strcmp("cd", arg[0])) {
      /* TESTE COMECA 
      printf("Executa cd ");
      for(ii = 0; ii < i; ii++)
        printf("%s ", args[ii]);
      printf("\n");
      TESTE TERMINA */
      chdir(arg[1]);
    }

    /* 4.2 Comando pwd */
    else if(!strcmp("pwd", arg[0])) {
      printf("%s\n", pcwd);
    }

    /* 4.3 Binario (/bin/ls ou ./ep1) */
    else {
      switch (childPid = fork()){
        case -1:
          perror("fork falhou");
          exit(1);
          break;
        case 0: /* Processo filho */
          /* 4.3.1 Binario /bin/ls -1 */
          if(!strcmp("/bin/ls", arg[0]) && !strcmp("-1", arg[1])){ 
            printf("vou executar /bin/ls com o seguinte arg:");
            for(ii = 0; ii <= i; ii++) {
              printf("%s,", arg[ii]);
            }
            printf("\n");
            execve(arg[0], arg, 0);
            perror("execve");
            exit(EXIT_FAILURE);
          }
          /* 4.3.2 Binario ./ep1 */
          else if(!strcmp("./ep1", arg[0])){ // TODO: Interpretação do ./ep1
            /* TESTE COMECA 
            printf("Executa ep1 ");
            for(ii = 0; ii < i; ii++)
              printf("%s ", args[ii]);
            printf("\n");
            TESTE TERMINA */
          }
          exit(0); /* nao achamos o que executar, entao nos matamos */
          break;
        default:
          /* Processo pai */
          free(entrada);
          wait(NULL);
          break;
      }
    }
  }
  return 0;
}
