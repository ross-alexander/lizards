#ifdef HAVE_MALLOC_H
#include <malloc.h>
#endif

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#define YYSTYPE char*

extern YYSTYPE yylval;

  typedef struct {
    int lineNo;
  } Lexicon;

  typedef struct {
    size_t size;
    size_t head;
    char *ptr;
  } StringRing;

StringRing *RingCreate(size_t size);
void RingFree(StringRing *ptr);
char* RingInsert(StringRing *ring, int len, char *ptr);

extern Lexicon lexicon;
