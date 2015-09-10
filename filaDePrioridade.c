#include <stdlib.h>

static int *pq; 
static int N;

static int (*less)(int, int);

void fixUp(int a[], int k){
  int tmp;
  while (k > 1 && less(a[k], a[k/2])){
    tmp = a[k];
    a[k] = a[k/2];
    a[k/2] = tmp;
    k = k/2;
  }
}

void fixDown(int a[], int k, int N){
  int j, tmp;
  while (2*k <= N){
    j = 2*k;
    if (j < N && less(a[j+1], a[j])) j++;
    if (!less(a[j], a[k])) break;
    tmp = a[k];
    a[k] = a[j];
    a[j] = tmp;
    k = j;
  }
}
void PQinit(int maxN, int (*tmp)(int, int)){
  pq = malloc((maxN+1)*sizeof(int));
  N = 0;
  less = tmp;
}
int PQempty(){
  return N == 0;
}
void PQinsert(int v){
  pq[++N] = v;
  fixUp(pq, N);
}
int PQdelmax(){ 
  int tmp;
  tmp = pq[1];
  pq[1] = pq[N];
  pq[N] = tmp; 
  fixDown(pq, 1, N-1); 
  return pq[N--]; 
}
