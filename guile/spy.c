#include <stdlib.h>
#include <libguile.h>

#include "lizards.h"

static long spy_tag;

static SCM spy_make (SCM s_x, SCM s_y, SCM s_owner)
{
  Spy *spy;
  SCM spy_smob;
  int width, height;

  SCM_ASSERT (SCM_INUMP (s_x), s_x, SCM_ARG1, "make-spy");
  SCM_ASSERT (SCM_INUMP (s_y), s_y,  SCM_ARG2, "make-spy");
  SCM_ASSERT (SCM_INUMP (s_owner), s_owner, SCM_ARG3, "make-spy");

  spy = (Spy*)scm_must_malloc(sizeof(Spy), "spy");
  spy->x = SCM_INUM (s_x);
  spy->y = SCM_INUM (s_y);
  spy->owner = SCM_INUM(s_owner);
  spy->age = 0;
  SCM_NEWCELL (spy_smob);
  SCM_SETCDR (spy_smob, spy);
  SCM_SETCAR (spy_smob, spy_tag);

  return spy_smob;
}

static SCM spy_age (SCM spy_smob)
{
  Spy *spy;
  SCM_ASSERT ((SCM_NIMP (spy_smob)
	       && SCM_CAR (spy_smob) == spy_tag),
	      spy_smob, SCM_ARG1, "age-spy");
  spy = (Spy*) SCM_CDR (spy_smob);
  spy->age++;
  return SCM_UNSPECIFIED;
}

static SCM spy_mark (SCM spy_smob)
{
  Spy *spy = (Spy*) SCM_CDR (spy_smob);
  return SCM_UNSPECIFIED;
}

static scm_sizet spy_free (SCM spy_smob)
{
  Spy *spy = (Spy*) SCM_CDR (spy_smob);
  scm_sizet size = sizeof (Spy);
  free (spy);
  return size;
}

static int spy_print (SCM spy_smob, SCM port, scm_print_state *pstate)
{
  Spy *spy = (Spy *) SCM_CDR (spy_smob);
  scm_puts ("#<spy ", port);
  scm_display (scm_long2num(spy->x), port); scm_puts (" ", port);
  scm_display (scm_long2num(spy->y), port); scm_puts (" ", port);
  scm_display (scm_long2num(spy->owner), port); scm_puts (" ", port);
  scm_display (scm_long2num(spy->age), port); scm_puts (">", port);

  /* non-zero means success */
  return 1;
}

static scm_smobfuns spy_funs = {
  spy_mark, spy_free, spy_print, 0
};

void spy_init ()
{
  spy_tag = scm_newsmob (&spy_funs);

  scm_make_gsubr ("make-spy", 3, 0, 0, spy_make);
  scm_make_gsubr ("age-spy", 1, 0, 0, spy_age);
}
