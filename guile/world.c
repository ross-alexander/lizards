#include <stdlib.h>
#include <guile/gh.h>
#include <libguile.h>

#include "lizards.h"

static long world_tag;

static SCM World_make (SCM s_game, SCM s_x, SCM s_y, SCM s_turn, SCM s_players, SCM s_map)
{
  int len;
  struct world *world;
  SCM world_smob;

  SCM_ASSERT (SCM_STRINGP (s_game), s_game, SCM_ARG1, "make-world");
  SCM_ASSERT (SCM_INUMP (s_x), s_x, SCM_ARG2, "make-world");
  SCM_ASSERT (SCM_INUMP (s_y), s_y,  SCM_ARG3, "make-world");
  SCM_ASSERT (SCM_INUMP (s_turn), s_turn, SCM_ARG4, "make-world");
  SCM_ASSERT (scm_list_p (s_players), s_players, SCM_ARG5, "make-world");
  SCM_ASSERT (scm_list_p (s_map), s_map, SCM_ARG6, "make-world");

  world = (World*)scm_must_malloc(sizeof(World), "world");
  world->game = gh_scm2newstr(s_game, &len);
  world->x = SCM_INUM (s_x);
  world->y = SCM_INUM (s_y);
  world->turn = SCM_INUM(s_turn);
  printf("World %s (%d %d %d) loaded\n", world->game,
	 world->x, world->y, world->turn);
  world->players = s_players;
  world->map = (Hex*)scm_must_malloc(sizeof(Hex) * world->x * world->y, "map");
  world->hexlist = s_map;
  SCM_NEWCELL (world_smob);
  SCM_SETCDR (world_smob, world);
  SCM_SETCAR (world_smob, world_tag);
  return world_smob;
}

static SCM World_mark (SCM world_smob)
{
  World *world = (World*) SCM_CDR (world_smob);
  scm_gc_mark (world->players);
  scm_gc_mark (world->hexlist);
  return SCM_UNSPECIFIED;
}

static scm_sizet World_free (SCM world_smob)
{
  World *world = (World*) SCM_CDR (world_smob);
  /* needs correction */
  scm_sizet size = sizeof (World);
  free (world);
  return size;
}

static int World_print (SCM world_smob, SCM port, scm_print_state *pstate)
{
  World *world = (World *) SCM_CDR (world_smob);
  scm_puts ("#<world ", port);
  scm_display (gh_str02scm(world->game), port); scm_puts (" ", port);
  scm_display (scm_long2num(world->x), port); scm_puts (" ", port);
  scm_display (scm_long2num(world->y), port); scm_puts (" ", port);
  scm_display (scm_long2num(world->turn), port); scm_puts (" ", port);
  scm_display (world->players, port); scm_puts (">", port);

  /* non-zero means success */
  return 1;
}

static scm_smobfuns world_funs = {
  World_mark, World_free, World_print, 0
};

static SCM world_map (SCM world_smob)
{
  struct world *world = (struct world*) SCM_CDR (world_smob);
  return world->hexlist;
}

static SCM world_players (SCM world_smob)
{
  struct world *world = (struct world*) SCM_CDR (world_smob);
  return world->players;
}

static SCM world_players_set (SCM world_smob, SCM players)
{
  struct world *world = (struct world*) SCM_CDR (world_smob);
  world->players = players;
  return world->players;
}

static SCM world_game (SCM world_smob)
{
  struct world *world = (struct world*) SCM_CDR (world_smob);
  return gh_str02scm(world->game);
}

static SCM world_set_hex (SCM world_smob, SCM hex_smob)
{
  struct world *world = (struct world*) SCM_CDR (world_smob);
  struct hex *hex = (struct hex*) SCM_CDR (hex_smob);

  SCM_ASSERT ((SCM_NIMP (world_smob)
	       && SCM_CAR (world_smob) == world_tag),
	      world_smob, SCM_ARG1, "world-set-hex");
  SCM_ASSERT ((SCM_NIMP (hex_smob)
	       && SCM_CAR (hex_smob) == hex_tag),
	      hex_smob, SCM_ARG2, "world-set-hex");

  if ((hex->x >= 0) && (hex->x < world->x) &&
      (hex->y >= 0) && (hex->y < world->y))
    {
      printf("Hex %d %d set\n", hex->x, hex->y);
      memcpy(&world->map[world->y * hex->y + hex->x], hex, sizeof(hex));
      return gh_bool2scm(1);
    }
  return gh_bool2scm(0);
}  

void world_init ()
{
  world_tag = scm_newsmob (&world_funs);
  scm_make_gsubr ("make-world", 6, 0, 0, World_make);
  scm_make_gsubr ("world-map", 1, 0, 0, world_map);
  scm_make_gsubr ("world-players", 1, 0, 0, world_players);
  scm_make_gsubr ("world-players!", 2, 0, 0, world_players_set);
  scm_make_gsubr ("world-game", 1, 0, 0, world_game);
  scm_make_gsubr ("world-hex!", 2, 0, 0, world_set_hex);
}
