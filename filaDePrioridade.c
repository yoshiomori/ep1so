#include "filaDePrioridade.h"
void peneira (int p, int m, int v[], int(*cmp)(int,int)){
  int f = 2*p, x = v[p];
  while(f < m){
    if(f < m - 1 && cmp(v[f], v[f + 1]) == 1) ++f;
    if(cmp(x, v[f]) != -1) break;
    v[p] = v[f];
    p = f, f = 2 * p;
  }
  v[p] = x;
}
void heapsort(int n, int v[], int(*cmp)(int, int)){
  int p, m, x;
  for (p = n/2; p >= 0; --p)
    peneira(p, n, v, cmp);
  for(m = n - 1; m >= 1; --m){
    x = v[0], v[0] = v[m], v[m] = x;
    peneira(0, m-1, v, cmp);
  }
}
