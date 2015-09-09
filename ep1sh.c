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
  char *argv[ARG_MAX + 2];
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
    while(str && i < ARG_MAX){
      argv[i++] = str;
      str = strtok(NULL, " \n");
    }

    /* neste ponto, entrada contem o comando/binario que o usuario deseja executar
     * args[0], ..., args[i-1] contem os argumentos para o comando/binario */
    
    /* vamos criar um vetor argv, com argv[0] == entrada, argv[1] == args[0], ..., 
     * argv[i] == args[i-1], argv[i+1] == NULL
     */

    /* 4. Criar argv */
    

    /* 5. Executar comando/binario */
    /* 5.1 Comando cd */
    if(!strcmp("cd", argv[0])) {
      /* TESTE COMECA 
      printf("Executa cd ");
      for(ii = 0; ii < i; ii++)
        printf("%s ", args[ii]);
      printf("\n");
      TESTE TERMINA */
      chdir(argv[1]);
    }

    /* 5.2 Comando pwd */
    else if(!strcmp("pwd", argv[0])) {
      printf("%s\n", pcwd);
    }

    /* 5.3 Binario (/bin/ls ou ./ep1) */
    else {
      switch (childPid = fork()){
        case -1:
          perror("fork falhou");
          exit(1);
          break;
        case 0: /* Processo filho */
          /* 5.3.1 Binario /bin/ls -1 */
          if(!strcmp("/bin/ls", argv[0]) && !strcmp("-1", argv[1])){ 
            printf("vou executar /bin/ls com o seguinte argv:");
            for(ii = 0; ii <= i; ii++) {
              printf("%s,", argv[ii]);
            }
            printf("\n");
            execve(argv[0], argv, 0);
            perror("execve");
            exit(EXIT_FAILURE);
          }
          /* 5.3.2 Binario ./ep1 */
          else if(!strcmp("./ep1", argv[0])){ // TODO: Interpretação do ./ep1
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
