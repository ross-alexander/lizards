#include "shared.h"

struct Feature {
  Instance *name;
};

const char *Name(void) { return "Feature"; }

size_t Size(void *ptr) {return 0;}

void Display(void *ptr, FILE *stream, int level)
{
  struct Feature *tmp = (struct Feature*)ptr;
  fprintf(stream, "(Feature ");
  InstanceDisplay(tmp->name, stream, level);
  fprintf(stream, ")");
}

void* Realise(void *ptr)
{
  struct Feature* feature = (struct Feature*)calloc(sizeof(struct Feature), 1); 
  Node *tmp = (Node*)ptr;
  tmp = tmp->tail;
  feature->name = tmp->head;
  return (void*)feature;
}
