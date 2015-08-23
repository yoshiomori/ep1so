#include "mallocSafe.h"

#define NUM_MAX_INICIAL_CARAC_A_LER 100

int main(){
    while(1){
        /* Imprimir o diretório */

        /* Tratar entrada do usuário (scanf) */
        int numMaxCaracALer = NUM_MAX_INICIAL_CARAC_A_LER;
        char *entrada = mallocSafe(numMaxCaracALer * sizeof(char));
        
        fgets(entrada, numMaxCaracALer, stdin);
        
        /* Enviar comandos para SO (fork) */

        /* Enviar mensagens para o terminal ? */

    }
    return 0;
}
