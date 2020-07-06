#define HOME_DEN_SCORE 50
#define DEN_SCORE 25
#define BRIDGE_SCORE 10
#define TEMPLE_SCORE 30

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "lizards.h"
#include "lizmisc.h"

static void calculate_points (world_t *world)
{
  int i, x, y, *lizard_heads;
  hex_t *hex;
  feature_t *f;

  lizard_heads = (int*)alloca(sizeof(int) * world->num_players);
  for (i = 1; i < world->num_players; i++)
    {
      world->player[i]->home_den = 0;
      world->player[i]->points = 0;
    }

  for (y = 0; y < world->y; y++)
    for (x = 0; x < world->x; x++)
      {
	hex = get_hex(world, coord_cons(x, y));
	if ((f = has_feature(hex, DEN)))
	  {
	    world->player[hex->owner]->points += f->f.den.pop;
      	    lizard_heads[hex->owner] += f->f.den.pop;
	    if (f->f.den.home)
	      {
		world->player[hex->owner]->points += HOME_DEN_SCORE;
		world->player[hex->owner]->home_den++;
	      }
	    else
	      world->player[hex->owner]->points += DEN_SCORE;
	  }
	world->player[hex->owner]->points += lizards_in_hex (hex);
	lizard_heads [hex->owner] += lizards_in_hex (hex);
	
	if (has_feature(hex, BRIDGE) && hex->owner > 0)
	  world->player[hex->owner]->points += BRIDGE_SCORE;
	
	if (has_feature(hex, TEMPLE) && hex->owner > 0)
	  world->player[hex->owner]->points += TEMPLE_SCORE;
      }

  for (i = 0; i < world->num_spies; i++)
    {
      if (world->spylist[i]->player > 0)
	{
	  world->player[world->spylist[i]->player]->points += 1;
	  lizard_heads [world->spylist[i]->player] += 1;
	}
    }
  
  for (i = 1; i < world->num_players; i++)
    if (lizard_heads [i] == 0)
      world->player[i]->points = 0;  
}

void ClanRank(world_t *world)
{
  int work,
    i, j,
    total_home_dens = 0,
    home_dens_for_victory = 0,
    victory_points_for_victory = 0,
    winner_score = 0,
    winner_player = 0,
    tie_flag = LIZ_FALSE;
  player_t **scorers;

  scorers = (player_t**)alloca(sizeof(player_t*) * world->num_players);

  printf ("Messaging Players...\n");

  for (i = 1; i < world->num_players; i++)
	player_mesg(world, 0,
		 " %d - Player %02d, '%s' (%3s) Clan.\n",
		 world->player[i]->home_den,
		 i,
		 world->player[i]->clan,
                 world->player[i]->code);

  for (i = 0; i < world->num_players; i++)
    scorers [i] = world->player[i];

  calculate_points(world);

  for (i = 1, total_home_dens = 0; i < world->num_players; i++)
    total_home_dens += world->player[i]->home_den;

  printf (" Sorting Clans...\n");

  /* braindead bubble sort */

  for (i = 1; i < world->num_players; i++)
    for (j = i + 1; j < world->num_players; j++)
      if (scorers[i]->points < scorers[j]->points)
	{
	  player_t *ptmp = scorers[i];
	  scorers[i] = scorers[j];
	  scorers[j] = ptmp;
	}

  printf ("Messaging Players...\n");

  
  for (i = 0; i < world->num_players; i++)
    {
      player_mesg(world, i, "(clan-rank '(");
      for (j = 1; j < world->num_players; j++)
	player_mesg (world, i,
		     "(rank %02d %d %d \"%s\" \"%s\")\n",
		     j,
		     scorers[j]->points,
		     scorers[j]->num,
		     scorers[j]->clan,
		     scorers[j]->code);
      player_mesg(world, i, ")");
    }

  /* check victory conditions if required! */
  
  printf ("Checking VICTORY CONDITIONS...\n");
  {
      /* old, 'half the home dens' victory condition */
      
      /* Check Victory Conditions! */
      
    home_dens_for_victory = total_home_dens / 2;
      
    if (2 * home_dens_for_victory <= total_home_dens)
      home_dens_for_victory++;
      
    for (i = 1; i < world->num_players; i++)
      if (world->player[i]->home_den >= home_dens_for_victory &&
	  home_dens_for_victory > 0)
	winner_player = i;
    if (winner_player)
      {
	for (j = 0; j < world->num_players; j++)
	  {
	    if (j == winner_player)
	      {
		player_mesg (world, j, "\n * '%s' Clan YOU HAVE WON THIS GAME OF LIZARDS!\n",
			     world->player[j]->clan);
		player_mesg (world, j,
			     " * CONGRATULATIONS '%s' Clan!\n\n",
			     world->player[i]->clan);
		player_mesg (world, j,
			     " - Game %s has reached victory conditions and has ended.\n",
			     world->game);
	      }
	    else
	      {
		player_mesg (world, j,
			     "\n * '%s' Clan HAS WON THIS GAME OF LIZARDS!\n",
			     world->player[winner_player]->clan);
		player_mesg (world, j,
			     " * CONGRATULATIONS '%s' Clan!\n\n",
			     world->player[winner_player]->clan);
		player_mesg (world, j,
			     " - Game %s has reached victory conditions and has ended.\n",
			     world->game);
	      }
	  }
	printf (" \n\n'%s' Clan\nHAS WON THIS GAME OF LIZARDS!\n",
		world->player[winner_player]->clan);
	printf ("Game %s has reached victory conditions and has ended.\n",
		world->game);
      }
    else
      {
	for (i = 1; i < world->num_players; i++)
	  player_mesg (world, i,
		       "(victory-dens-continue %d %d %d)\n",
		       total_home_dens,
		       home_dens_for_victory,
		       world->player[work]->home_den);
      }
  }
  for (i = 0; i < world->num_players; i++)
    player_mesg(world, i, ")\n");
}
