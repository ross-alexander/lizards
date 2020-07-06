#include "shared.h"

struct World {
  Instance *name;
  Instance *turn;
  Instance *provinces;
  Instance *players;
};

const char *Name(void) { return "World"; }

size_t Size(void *ptr) {return sizeof(struct World);}

void Display(void *ptr, FILE *stream, int level)
{
  struct World *tmp = (struct World*)ptr;
  fprintf(stream, "(World ");
  InstanceDisplay(tmp->name, stream, level);
  fprintf(stdout, " ");
  InstanceDisplay(tmp->turn, stream, level);
  fprintf(stdout, "\n");
  InstanceDisplay(tmp->players, stream, level + 1);
  fprintf(stream, "\n");
  InstanceDisplay(tmp->provinces, stream, level + 1);
  fprintf(stream, ")\n");
}

void* Realise(void *ptr)
{
  struct World* world = (struct World*)calloc(sizeof(struct World), 1); 
  Node *tmp = (Node*)ptr;
  tmp = tmp->tail; world->name = tmp->head;
  tmp = tmp->tail; world->turn = tmp->head;
  tmp = tmp->tail; world->players = tmp->head;
  tmp = tmp->tail; world->provinces = tmp->head;
  return (void*)world;
}

int Open(struct World *world, void *ptr)
{
  printf("Opening world ");
  InstanceDisplay(world->name, stdout, 0);
  printf("\n");
  return 0;
}

