/* ---------------------------------------------------------------------- 
--
--
---------------------------------------------------------------------- */
#include "shared.h"

const char *Name(void){return "String";}

size_t Size(void *ptr)
{
  return strlen((char*)ptr);
}

void Display(void *ptr, FILE *stream, int level)
{
  if (debug) printf("StringDisplay %08lX\n", ptr);
  fprintf(stream, "\"%s\"", (char*)ptr);
}

