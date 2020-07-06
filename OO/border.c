#include "shared.h"

struct Border {
  Instance *dir;
  Instance *neighbour;
};

const char *Name(void) { return "Border"; }

size_t Size(void *ptr) {return sizeof(struct Border);}

void Display(void *ptr, FILE *stream, int level)
{
  struct Border *tmp = (struct Border*)ptr;
  fprintf(stream, "(Border ");
  InstanceDisplay(tmp->dir, stream, level);
  fprintf(stdout, " ");
  InstanceDisplay(tmp->neighbour, stream, level);
  fprintf(stream, ")");
}

void* Realise(void *ptr)
{
  struct Border* border = (struct Border*)calloc(sizeof(struct Border), 1); 
  Node *tmp = (Node*)ptr;
  tmp = tmp->tail; border->dir = tmp->head;
  tmp = tmp->tail; border->neighbour = tmp->head;
  return (void*)border;
}
