typedef struct node* link;
typedef struct _CQ* CQ;
struct _CQ {
  int N;
  link first;
  link last;
};

CQ CQinit();

void CQinsert(CQ,int);

int CQsize(CQ);

int CQremove(CQ);

int CQnext(CQ);
