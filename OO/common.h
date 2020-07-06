typedef struct Type {
  void *dlhandle;
  const char* (*name)(void);
  size_t (*size)(void*);
  void (*display)(void*, FILE*, int);
  void* (*realise)(void*);
} Type;

typedef struct Instance {
  int refcnt;
  Type *type;
  void *ptr;
} Instance;

typedef struct Node {
  Instance* head;
  struct Node *tail;
} Node;

extern Node* MkList(Instance *, Node*);

extern void InstanceDisplay(Instance *, FILE *, int);
extern Instance *InstanceRealise(const char *type, void *ptr);
extern size_t InstanceSize(Instance *);
extern HashTable *types;
extern int common;
extern int debug;
extern Instance *world;
