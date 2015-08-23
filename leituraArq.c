#include "leituraArq.h"

#define NUM_MAX_INICIAL_CARAC_A_LER 100


char *leLinha(FILE *arq) {
    int numMaxCaracALer = NUM_MAX_INICIAL_CARAC_A_LER;
    int i = 0;
    char *entrada = mallocSafe(numMaxCaracALer * sizeof(char));
    
    while( (ch = fgetc(arq)) != EOF) {
        if(ch == '\n') {
            
        }
    }

    if(ferror(arq) != 0) {
        fprintf(stderr, "Erro na leitura do arquivo\n");
        exit(-1);
    }  
    
    
    
}
