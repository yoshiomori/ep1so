#include <stdio.h>
#include <stdlib.h>

#include "../filaCircular.h"

int main(){
  CQ cq = CQinit();
  int i = 0;

  CQinsert(cq, 1);
  CQinsert(cq, 2);
  printf("%d\n", CQnext(cq));
  printf("%d\n", CQsize(cq));
  printf("%d\n", CQnext(cq));
  printf("%d\n", CQnext(cq));

  for(i = 0; i < 10; i++)
    CQinsert(cq, rand()%100);

  printf("%d\n", CQsize(cq));

  printf("%d\n", CQnext(cq));
  printf("%d\n", CQnext(cq));
  
  for(i = 0; i < CQsize(cq); i++){
    printf("item %d do first que acabou de ser removido\n", CQremove(cq));
    printf("size %d\n", CQsize(cq));
  }
  return 0;
}
