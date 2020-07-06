#include "shared.h"

struct Player {
  Instance *code;
  Instance *name;
};

const char *Name(void) { return "Player"; }

size_t Size(void *ptr) {return sizeof(struct Player);}

void Display(void *ptr, FILE *stream, int level)
{
  struct Player *tmp = (struct Player*)ptr;
  fprintf(stream, "(Player ");
  InstanceDisplay(tmp->code, stream, level);
  fprintf(stream, " ");
  InstanceDisplay(tmp->name, stream, level);
  fprintf(stream, ")\n");
}

void* Realise(void *ptr)
{
  struct Player* player = (struct Player*)calloc(sizeof(struct Player), 1); 
  Node *tmp = (Node*)ptr;
  tmp = tmp->tail; player->code = tmp->head;
  tmp = tmp->tail; player->name = tmp->head;
  return (void*)player;
}
