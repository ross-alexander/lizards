#ifndef LZ_COMPAT
#define LZ_COMPAT

#define P_WAIT 0

struct ffblk {
  char *ff_name;
};



#define random(x) (rand() % x)
extern void randomize(void);
extern char *itoa(int, char*, int);
extern char *strupr(char *);

/* ----------------------------------------------------------------------
--
-- Prototype for new functions
--
---------------------------------------------------------------------- */
char *LzCheckDir(char *, int);

#endif

