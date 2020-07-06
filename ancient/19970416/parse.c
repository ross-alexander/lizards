#include "parse.h"

#ifdef HAVE_STRING_H
#include <string.h>
#endif

  StringRing *RingCreate(size_t size)
    {
      StringRing *sr = (StringRing*)malloc(sizeof(StringRing));
      sr->size = size;
      sr->head = 0;
      sr->ptr = (char*) malloc(size);
      return sr;
    }

  void RingFree(StringRing *ptr)
    {
      free(ptr->ptr);
      free(ptr);
    }

  char* RingInsert(StringRing *ring, int len, char *ptr)
    {
      char *rval;
      if (len >= ring->size)
	return NULL;
      if ((ring->head + len + 1) > ring->size)
	ring->head = 0;
      strncpy(&ring->ptr[ring->head], ptr, len);
      ring->ptr[ring->head + len] = '\0';
      rval = &ring->ptr[ring->head];
      ring->head += len + 1;
      return rval;
    }

Lexicon lexicon;
StringRing *ring;

extern int yyparse(void);

int main(int argc, char *argv[])
{
  lexicon.lineNo = 1;
  ring = RingCreate(4096);
  yyparse();
  return 0;
}

