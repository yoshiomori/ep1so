#include "mallocSafe.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>

#define NUM_MAX_INICIAL_CARAC_A_LER 100
#define PATH_MAX 4096

int main(){
	char *pcwd;
	pid_t childPid;
    while(1){
        /* Imprimir o diretório */
	pcwd = getcwd(NULL, 0);
	printf("[ %s ]  ", pcwd);

        /* Tratar entrada do usuário (scanf) */
        int numMaxCaracALer = NUM_MAX_INICIAL_CARAC_A_LER;
        char *entrada = mallocSafe(numMaxCaracALer * sizeof(char));
        
	fgets(entrada, numMaxCaracALer, stdin);        
        
        /* Enviar comandos para SO (fork) */
	switch (childPid = fork()){
	case -1:
	  perror("fork failed");
	  exit(1);
	case 0:
	  /* Processo do filho */
	  printf("execve para o comand %s\n", entrada);
	  break;
	default:
	  /* Processo do pai */
	  wait(NULL);
	  break;
	}

        /* Enviar mensagens para o terminal ? */

    }
    return 0;
}
