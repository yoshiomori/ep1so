#include <stdio.h>
#include <stdlib.h>
#include "../filaDePrioridade.h"

int less(int a, int b){
  if(a < b) return 1;
  return 0;
}

int main(){
  int tmp, i;
  PQinit(10, &less);
  if (PQempty()) printf("Fila vazia\n");
  else printf("Fila não está vazia\n");
  PQinsert(10);
  if (PQempty()) printf("Fila vazia\n");
  else{
    printf("Fila não está mais vazia\n");
    printf("%d\n", PQdelmin());
    for (i = 0; i < 10; i++){
      tmp = rand() % 100;
      PQinsert(tmp);
      printf("%d foi inserido\n", tmp);
    }
    while (!PQempty()) printf("%d foi removido\n", PQdelmin());
  }
  printf("%d\n", PQfirst());
  return 0;
}
