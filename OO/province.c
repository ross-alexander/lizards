#include "shared.h"

const char *Name(void)
{
  return "Province";
}

/* ---------------------------------------------------------------------- 
--
--
---------------------------------------------------------------------- */
struct Province {
  Instance *name;
  Instance *owner;
  Instance *terrain;
  Instance *fertile;
  Instance *features;
  Instance *borders;
};

size_t Size(void *ptr) {return 0;}

void Display(void *ptr, FILE *stream, int level)
{
  struct Province *tmp = (struct Province*)ptr;
  int i;
  for (i = 0; i < level; i++)
    fprintf(stream, "\t");
  fprintf(stream, "(Province ");
  InstanceDisplay(tmp->name, stream, level + 1);
  fprintf(stream, " ");
  InstanceDisplay(tmp->owner, stream, level + 1);
  fprintf(stream, " ");
  InstanceDisplay(tmp->terrain, stream, level + 1);
  fprintf(stream, " ");
  InstanceDisplay(tmp->fertile, stream, level + 1);
  fprintf(stream, " ");
  InstanceDisplay(tmp->features, stream, level + 1);
  fprintf(stream, " ");
  InstanceDisplay(tmp->borders, stream, level + 1);
  fprintf(stream, ")\n");
}

void* Realise(void *ptr)
{
  struct Province* hex = (struct Province*)calloc(sizeof(struct Province), 1); 
  Node *tmp = (Node*)ptr;
  tmp = tmp->tail;
  hex->name = tmp->head; tmp = tmp->tail;
  hex->owner = tmp->head; tmp = tmp->tail;
  hex->terrain = tmp->head; tmp = tmp->tail;
  hex->fertile = tmp->head; tmp = tmp->tail;
  hex->features = tmp->head; tmp = tmp->tail;
  hex->borders = tmp->head; tmp = tmp->tail;
  return (void*)hex;
}

void* Self(void *ptr)
{
  return ptr;
}
