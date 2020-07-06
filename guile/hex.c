#include <stdlib.h>
#include <libguile.h>

#include "lizards.h"

long hex_tag;

static SCM hex_make (SCM s_x, SCM s_y, SCM s_title, SCM s_owner,
		     SCM s_type, SCM s_fertile, SCM s_features,
		     SCM s_neighbours)
{
  struct hex *hex;
  SCM hex_smob;
  int width, height;

  SCM_ASSERT (SCM_INUMP (s_x), s_x, SCM_ARG1, "make-hex");
  SCM_ASSERT (SCM_INUMP (s_y), s_y,  SCM_ARG2, "make-hex");
  SCM_ASSERT (SCM_STRINGP (s_title), s_title, SCM_ARG3, "make-hex");
  SCM_ASSERT (SCM_STRINGP (s_owner), s_owner, SCM_ARG4, "make-hex");
  SCM_ASSERT (SCM_STRINGP (s_type), s_type, SCM_ARG5, "make-hex");
  SCM_ASSERT (SCM_INUMP (s_fertile), s_fertile, SCM_ARG6, "make-hex");
  SCM_ASSERT (scm_list_p (s_features), s_features, SCM_ARG7, "make-hex");
  SCM_ASSERT (scm_list_p (s_neighbours), s_neighbours, SCM_ARGn, "make-hex");

  hex = (struct hex*)scm_must_malloc(sizeof(struct hex), "hex");
  hex->x = SCM_INUM (s_x);
  hex->y = SCM_INUM (s_y);
  SCM_NEWCELL (hex_smob);
  SCM_SETCDR (hex_smob, hex);
  SCM_SETCAR (hex_smob, hex_tag);

  return hex_smob;
}

static SCM hex_mark (SCM hex_smob)
{
  /* FIX */
  struct hex *hex = (struct hex*) SCM_CDR (hex_smob);
  return SCM_UNSPECIFIED;
}

static scm_sizet hex_free (SCM hex_smob)
{
  /* FIX */
  struct hex *hex = (struct hex*) SCM_CDR (hex_smob);
  scm_sizet size = sizeof (struct hex);
  free (hex);
  return size;
}

static int hex_print (SCM hex_smob, SCM port, scm_print_state *pstate)
{
  struct hex *hex = (struct hex *) SCM_CDR (hex_smob);
  scm_puts ("#<hex ", port);
  scm_display (scm_long2num(hex->x), port); scm_puts (" ", port);
  scm_display (scm_long2num(hex->y), port); scm_puts (">", port);
  /* non-zero means success */
  return 1;
}

static scm_smobfuns hex_funs = {
  hex_mark, hex_free, hex_print, 0
};

void hex_init ()
{
  hex_tag = scm_newsmob (&hex_funs);
  scm_make_gsubr ("make-hex", 8, 0, 0, hex_make);
}
