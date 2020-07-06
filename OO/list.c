/* ---------------------------------------------------------------------- 
--
--
---------------------------------------------------------------------- */
#include "shared.h"

const char *Name(void){return "List";}

size_t Size(void *ptr)
{
  int i = 0;
  Node *tmp;
  for (tmp = (Node*)ptr; tmp; tmp = tmp->tail)
    i++;
  return i;
}

void Display(void* ptr, FILE *stream, int level)
{
  Node *tmp = ptr;
  fprintf(stream, "(");
  while (tmp && tmp->tail)
    {
      InstanceDisplay(tmp->head, stream, level + 1);
      fprintf(stream, " ");
      tmp = tmp->tail;
    }
  if (tmp)
    InstanceDisplay(tmp->head, stream, level + 1);
  fprintf(stream, ")");
}

