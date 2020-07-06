typedef struct Node {
  int type;
  void *value;
} Node;

typedef struct List {
  Node *head;
  struct List *tail;
} List;

extern Node *parse;
