#include <stdlib.h>
#include <libguile.h>

#include "lizards.h"

static long player_tag;

static SCM player_make (SCM s_id, SCM s_clan, SCM s_name)
{
  struct player *player;
  SCM player_smob;
  int len;

  SCM_ASSERT (SCM_INUMP (s_id), s_id, SCM_ARG1, "make-player");
  SCM_ASSERT (SCM_STRINGP (s_clan), s_clan,  SCM_ARG2, "make-player");
  SCM_ASSERT (SCM_STRINGP (s_name), s_name, SCM_ARG3, "make-player");

  player = (struct player*)scm_must_malloc(sizeof(struct player), "player");
  player->id = SCM_INUM (s_id);
  player->clan = gh_scm2newstr(s_clan, &len);
  player->name = gh_scm2newstr(s_name, &len);
  SCM_NEWCELL (player_smob);
  SCM_SETCDR (player_smob, player);
  SCM_SETCAR (player_smob, player_tag);
  return player_smob;
}

static SCM player_mark (SCM player_smob)
{
  struct player *player = (struct player*) SCM_CDR (player_smob);
  return SCM_UNSPECIFIED;
}

static scm_sizet player_free (SCM player_smob)
{
  struct player *player = (struct player*) SCM_CDR (player_smob);
  scm_sizet size = sizeof (struct player) + strlen(player->name) +
    strlen(player->clan) + 2;
  free (player);
  return size;
}

static int player_print (SCM player_smob, SCM port, scm_print_state *pstate)
{
  struct player *player = (struct player *) SCM_CDR (player_smob);
  scm_puts ("#<player ", port);
  scm_display (scm_long2num(player->id), port); scm_puts (" ", port);
  scm_display (gh_str02scm(player->clan), port); scm_puts (" ", port);
  scm_display (gh_str02scm(player->name), port); scm_puts (">", port);

  /* non-zero means success */
  return 1;
}

static scm_smobfuns player_funs = {
  player_mark, player_free, player_print, 0
};

void player_init ()
{
  player_tag = scm_newsmob (&player_funs);
  scm_make_gsubr ("make-player", 3, 0, 0, player_make);
}
