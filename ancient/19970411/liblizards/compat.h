#ifndef LZ_COMPAT
#define LZ_COMPAT

#include <unistd.h>

#define random(x) (rand() % x)
extern void randomize(void);
extern char *itoa(int, char*, int);
extern char *strupr(char *);
extern int spawnlp (int blah, char* a, char* b, char* c, char* d);

/* ----------------------------------------------------------------------
--
-- Prototype for new functions
--
---------------------------------------------------------------------- */
char *LzCheckDir(char *, int);

#endif

