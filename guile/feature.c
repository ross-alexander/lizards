#include <stdlib.h>
#include <libguile.h>

#include "lizards.h"

static long feature_tag;

static SCM feature_make (SCM s_x, SCM s_y, SCM s_owner)
{
  struct feature *feature;
  SCM feature_smob;
  int width, height;

  SCM_ASSERT (SCM_INUMP (s_x), s_x, SCM_ARG1, "make-feature");
  SCM_ASSERT (SCM_INUMP (s_y), s_y,  SCM_ARG2, "make-feature");
  SCM_ASSERT (SCM_INUMP (s_owner), s_owner, SCM_ARG3, "make-feature");

  feature = (struct feature*)scm_must_malloc(sizeof(struct feature), "feature");
  feature->x = SCM_INUM (s_x);
  feature->y = SCM_INUM (s_y);
  feature->owner = SCM_INUM(s_owner);
  feature->age = 0;
  SCM_NEWCELL (feature_smob);
  SCM_SETCDR (feature_smob, feature);
  SCM_SETCAR (feature_smob, feature_tag);

  return feature_smob;
}

static SCM feature_age (SCM feature_smob)
{
  struct feature *feature;
  SCM_ASSERT ((SCM_NIMP (feature_smob)
	       && SCM_CAR (feature_smob) == feature_tag),
	      feature_smob, SCM_ARG1, "age-feature");
  feature = (struct feature*) SCM_CDR (feature_smob);
  feature->age++;
  return SCM_UNSPECIFIED;
}

static SCM feature_mark (SCM feature_smob)
{
  struct feature *feature = (struct feature*) SCM_CDR (feature_smob);
  return SCM_UNSPECIFIED;
}

static scm_sizet feature_free (SCM feature_smob)
{
  struct feature *feature = (struct feature*) SCM_CDR (feature_smob);
  scm_sizet size = sizeof (struct feature);
  free (feature);
  return size;
}

static int feature_print (SCM feature_smob, SCM port, scm_print_state *pstate)
{
  struct feature *feature = (struct feature *) SCM_CDR (feature_smob);
  scm_puts ("#<feature ", port);
  scm_display (scm_long2num(feature->x), port); scm_puts (" ", port);
  scm_display (scm_long2num(feature->y), port); scm_puts (" ", port);
  scm_display (scm_long2num(feature->owner), port); scm_puts (" ", port);
  scm_display (scm_long2num(feature->age), port); scm_puts (">", port);

  /* non-zero means success */
  return 1;
}

static scm_smobfuns feature_funs = {
  feature_mark, feature_free, feature_print, 0
};

void feature_init ()
{
  feature_tag = scm_newsmob (&feature_funs);

  scm_make_gsubr ("make-feature", 3, 0, 0, feature_make);
  scm_make_gsubr ("age-feature", 1, 0, 0, feature_age);
}
