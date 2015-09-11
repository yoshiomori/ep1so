#include <stdlib.h>
#include "filaCircular.h"

struct node { int item; link next; };
typedef link Node;

CQ CQinit(){
  CQ cq = malloc(sizeof*cq);
  cq->first = NULL;
  cq->last = NULL;
  cq->N = 0;
  return cq;
}


link newNodes(){
  link node = malloc(sizeof *node);
  node->next = node;
  return node;
}

void CQinsert(CQ queue, int v){
  if(!queue->N){
    queue->first = newNodes();
    queue->last = queue->first;
  }
  queue->last->next = newNodes();
  queue->last = queue->last->next;
  queue->last->item = v;
  queue->last->next = queue->first;
  queue->N++;
}

int CQsize(CQ queue){
  return queue->N;
}

int CQremove(CQ queue){
  int res = queue->first->item;
  queue->first = queue->first->next;
  free(queue->last->next);
  queue->last->next = queue->first;
  queue->N--;
  return res;
}

int CQnext(CQ queue){
  int res = queue->first->item;
  queue->last = queue->first;
  queue->first = queue->first->next;
  return res;
}
