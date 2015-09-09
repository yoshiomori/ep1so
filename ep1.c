#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv){
  FILE *pfile;
  int modo;
  if(argc < 4){
    printf("Falta argumentos\n");
    exit(0);
  }
  pfile = fopen(argv[2], "r");
  if(!pfile){
    perror(argv[1]);
    exit(EXIT_FAILURE);
  }
  
  fclose(pfile);
  return 0;
}
