#include <stdio.h>
#include <stdlib.h>
#include "../filaDePrioridade.h"

int cmp(int a, int b){
  return a < b ? 1 : (a = b ? 0 : -1);
}

int main(){
  int v[3] = {2, 1, 3};
  int v1[3] = {2, 3, 1};
  int v2[3] = {2, 1, 3};
  peneira (0, 3, v1, cmp);
  peneira (0, 3, v2, cmp);
  heapsort(3, v, cmp);
  return 0;
}
