#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_STDIO_H
#include <stdio.h>
#endif

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#ifdef HAVE_DLFCN_H
#include <dlfcn.h>
#endif

#include "hash.h"
#include "common.h"

/* ---------------------------------------------------------------------- 
--
--
---------------------------------------------------------------------- */
int debug;
HashTable *types;

/* ---------------------------------------------------------------------- 
--
--
---------------------------------------------------------------------- */
Node *MkList(Instance *head, Node *tail)
{
  Node *tmp = (Node*)calloc(sizeof(Node), 1);
  tmp->head = head;
  tmp->tail = tail;
  return tmp;
}

void InstanceDisplay(Instance *in, FILE *stream, int level)
{
  if (debug) printf("Display %08lX %08lX\n", in, in->ptr);
  (*in->type->display)(in->ptr, stream, level);
}

/* ---------------------------------------------------------------------- 
--
--
---------------------------------------------------------------------- */
size_t InstanceSize(Instance *in)
{
  return (*in->type->size)(in->ptr);
}

Instance *InstanceRealise(const char *type, void *ptr)
{
  Instance *in = (Instance*)calloc(sizeof(Instance), 1);
  Type *typeptr = HashSearch(types, strlen(type), (void*)type);
  if (!typeptr)
    {
      fprintf(stderr, "Illegal type %s.\n", type);
      exit(1);
    }
  if (debug) printf("Realise %08lX %08lX\n", in, ptr);
  in->type = typeptr;
  if (typeptr->realise)
    in->ptr = (*typeptr->realise)(ptr);
  else
    in->ptr = ptr;
  return in;
}

int SignalInt(Instance *in, const char *func, void *ptr)
{
  int (*funcptr)(void*, void*);
  if (in->type->dlhandle && (funcptr = dlsym(in->type->dlhandle, func)))
    return (*funcptr)(in->ptr, ptr);
  else
    return -1;
}
void* SignalPtr(Instance *in, const char *func, void *ptr)
{
  void* (*funcptr)(void*, void*);
  if (in->type->dlhandle && (funcptr = dlsym(in->type->dlhandle, func)))
    return (*funcptr)(in->ptr, ptr);
  else
    return NULL;
}

void TypeInsert(Type *t)
{
  HashInsert(types, strlen((*t->name)()), (void*)(*t->name)(), (void*)t);
}

void Dynamic(const char *path)
{
  void *dl;
  if ((dl = dlopen(path, RTLD_LAZY)))
    {
      Type *t = (Type*)calloc(sizeof(Type), 1);
      void *dltmp;
      t->dlhandle = dl;
      if (dltmp = dlsym(dl, "Name")) t->name = dltmp;
      if (dltmp = dlsym(dl, "Size")) t->size = dltmp;
      if (dltmp = dlsym(dl, "Display")) t->display = dltmp;
      if (dltmp = dlsym(dl, "Realise")) t->realise = dltmp;
      TypeInsert(t);
    }
  else
    {
      fprintf(stderr, "Dynamic load error: %s\n", dlerror());
    }
}

Instance *world;

int main(int argc, char *argv[])
{
  Instance *in;
  void *dl;
  types = HashCreate(1023);
  Dynamic("/home/rossa/lizards/string.so");
  Dynamic("/home/rossa/lizards/list.so");
  Dynamic("/home/rossa/lizards/province.so");
  Dynamic("/home/rossa/lizards/feature.so");
  Dynamic("/home/rossa/lizards/player.so");
  Dynamic("/home/rossa/lizards/world.so");
  Dynamic("/home/rossa/lizards/border.so");
  yyparse();
  SignalInt(world, "Open", NULL);
}
