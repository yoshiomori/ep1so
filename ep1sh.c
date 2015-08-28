#include "mallocSafe.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>

#define NUM_MAX_INICIAL_CARAC_A_LER 100
#define PATH_MAX 4096
#define ARG_MAX 131072

int main(){
  char *pcwd;
  char *args[ARG_MAX];
  pid_t childPid;
  int numMaxCaracALer = NUM_MAX_INICIAL_CARAC_A_LER;
  char *entrada = mallocSafe(numMaxCaracALer * sizeof(char));
  char *c; /* Iterador de string */
  int i, ii; /* Iterador */
  while(1){
    /* Imprimir o diretório */
    pcwd = getcwd(NULL, 0);
    printf("[ %s ]  ", pcwd);
    
    /* Tratar entrada do usuário (scanf) */
    fgets(entrada, numMaxCaracALer, stdin);        
    
    /* Enviar comandos para SO (fork) */
    switch (childPid = fork()){
    case -1:
      perror("fork failed");
      exit(1);
    case 0: /* Processo do filho */
      /* Parser */
      for(c = entrada, i = 0; *c; c++){
	if(*c == ' ' || *c == '\n'){
	  *c = '\0';
	  if(*(c + 1) && *(c + 1) != ' ' && i < ARG_MAX)
	    args[i++] = c + 1;
	}
      }
      /* Interpretador */
      if(!strcmp("ls", entrada)){
	args[0] = "-1";
	execve("/bin/ls", args, 0);
	perror("execve");
	exit(EXIT_FAILURE);
      }
      else if(!strcmp("pwd", entrada))
	printf("%s\n", pcwd);
      else if(!strcmp("./ep1", entrada)){
	printf("Executa ep1 ");
	for(ii = 0; ii < i; ii++)
	  printf("%s ", args[ii]);
	printf("\n");
      }
      else if(!strcmp("cd", entrada)){
	printf("Executa cd ");
	for(ii = 0; ii < i; ii++)
	  printf("%s ", args[ii]);
	printf("\n");
      }
      break;
    default:
      /* Processo do pai */
      free(entrada);
      wait(NULL);
      break;
    }
  }
  return 0;
}
