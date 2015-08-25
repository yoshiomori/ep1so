#include "mallocSafe.h"

#define NUM_MAX_INICIAL_CARAC_A_LER 100

int main(){
	char *pcwd;
    while(1){
        /* Imprimir o diretório */
		pcwd = getcwd();
		printf("%s\n", pcwd);
        /* Tratar entrada do usuário (scanf) */
        int numMaxCaracALer = NUM_MAX_INICIAL_CARAC_A_LER;
        char *entrada = mallocSafe(numMaxCaracALer * sizeof(char));
        
        
        
        /* Enviar comandos para SO (fork) */

        /* Enviar mensagens para o terminal ? */

    }
    return 0;
}
