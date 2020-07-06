#include <stdio.h>
#include <stdlib.h>
#include "lizards.h"
#include "lizgame.h"
#include "lizmisc.h"

/* ----------------------------------------------------------------------
--
-- Function: new_combat
--
-- Return codes:
--      -1 attackers flee
--       0 neither side flees
--      +1 defenders flee
--
---------------------------------------------------------------------- */

int new_combat (world_t *world, feature_t *attack, feature_t *defend,
		hex_t *attack_hex, hex_t *defend_hex, int flee_flag)
{
  FILE *fptr1,
       *fptr2;

  double attack_percent,
    defend_percent;
  
  int attacking_deaths,
      defending_deaths,
      attack_hit,
      defend_hit,
      round,
      work,
      lizards;

  round = 0;

  open_player_mesg (world, attack_hex->owner, &fptr1);

  if (defend_hex->owner > 0)
    open_player_mesg (world, defend_hex->owner, &fptr2);
  else
    fptr2 = NULL;
  if (has_feature(defend_hex, DEN))
    attack_hit = DEN_ATTACK;
  else
    attack_hit = BASE_ATTACK;

  defend_hit = BASE_ATTACK;

  attack_hit += (attack->f.band.combat * CP_MOD_LEVEL) -
		(defend->f.band.combat * CP_MOD_LEVEL);
  do
    {
      defending_deaths = 0;
      attacking_deaths = 0;
      lizards = lizards_in_band(attack) / 2;

      if (lizards == 0)
	lizards = 1;

      for (work = 0; work < lizards; work ++)
	if (random (100) <= attack_hit &&
	    defending_deaths < lizards_in_band (defend))
	  defending_deaths ++;

      lizards = (lizards_in_band (defend)) / 2;

      if (lizards == 0)
	lizards = 1;

      for (work = 0; work < lizards; work ++)
	if (random (100) <= defend_hit &&
	    attacking_deaths < lizards_in_band(attack))
	  attacking_deaths ++;
      round ++;

      kill_lizards(attack, attacking_deaths);
      kill_lizards(defend, defending_deaths);

      if (lizards_in_band(attack) > 0 && lizards_in_band(defend) > 0)
	{
	  attack_percent = 100 * ((double)lizards_in_band (defend) /
				  (double)lizards_in_band (attack));
	  attack_percent = 100 * ((double)lizards_in_band (attack) /
				  (double)lizards_in_band (defend));

	  if (attack_percent > 150 && flee_flag != -1)
	    {
	      attack_percent -= 125;
	      if (random (100) <= attack_percent)
		{
		  fprintf (fptr1, " - Combat round %d! ", round);
		  fprintf (fptr1, "Standing Lizards: %d. Standing enemy Lizards: %d.\n",
			   lizards_in_band(attack),
			   lizards_in_band(defend));
		  if (fptr2 != NULL)
		    {
		      fprintf (fptr2, " - Combat round %d! ", round);
		      fprintf (fptr2, "Standing Lizards: %d. Standing enemy Lizards: %d.\n",
			       lizards_in_band(attack),
			       lizards_in_band(defend));
	    fprintf (fptr2, " - Attacking Lizards flee!\n");
		    }
		  return (-1);
		}
	    }
	  if (!has_feature(defend_hex, BAND) &&
	      defend_percent > 150 &&
	      flee_flag == 1)
	    {
	      defend_percent -= 125;
	      if (random (100) <= defend_percent)
		{
		  fprintf (fptr1, " - Combat round %d! ", round);
		  
		  fprintf (fptr1, "Standing Lizards: %d. Standing enemy Lizards: %d.\n",
			   lizards_in_band(attack),
			   lizards_in_band(defend));
		  fprintf (fptr1, " - Enemy Lizards attempt to flee!\n");
		  
		  if (fptr2 != NULL)
		    {
		      fprintf (fptr2, " - Combat round %d! ", round);
		      
		      fprintf (fptr2, "Standing Lizards: %d. Standing enemy Lizards: %d.\n",
			   lizards_in_band(defend),
			   lizards_in_band(attack));
		    }
		  return (1);
		}
	    }
	}
    } while (lizards_in_band (attack) > 0 && lizards_in_band (defend) > 0);

  fprintf (fptr1, " - Combat round %d! ", round);

  fprintf (fptr1, "Standing Lizards: %d. Standing enemy Lizards: %d.\n",
	   lizards_in_band (attack),
	   lizards_in_band (defend));

  if (fptr2 != NULL)
    {
      fprintf (fptr2, " - Combat round %d! ", round);
      
      fprintf (fptr2, "Standing Lizards: %d. Standing enemy Lizards: %d.\n",
	       lizards_in_band (defend),
	       lizards_in_band (attack));
    }
  return (0);
}
  
static void capture_lizards (world_t *world, hex_t *hex, int no_captured,
		      int *red, int *green, int *grey,
		      int *black, int *yellow,
		      int *red_captured, int *green_captured,
		      int *grey_captured, int *black_captured,
		      int *yellow_captured)
{
#ifdef IgnoreThisStuff
  int work,
      work2,
      work3,
      work4;

  work = no_captured;

  *red_captured = 0;
  *green_captured = 0;
  *grey_captured = 0;
  *black_captured = 0;
  *yellow_captured = 0;

  if ((world->flags & LIZ_CAPTURE) == 0)
    return;

  while (work > 0 && lizards_in_hex (hex) < MAX_LIZARDS_IN_HEX)
  {
    if (work > 10)
      work2 = random (work / 10) + 1;
    else
      work2 = 1;

    do
    {
      work3 = random (LIZARD_TYPES) + 1;

      switch (work3)
      {
	case RED_LIZARD:
	  work4 = *red;

	  break;
	case GREEN_LIZARD:
	  work4 = *green;

	  break;
	case GREY_LIZARD:
	  work4 = *grey;

	  break;
	case BLACK_LIZARD:
	  work4 = *black;

	  break;
	case YELLOW_LIZARD:
	  work4 = *yellow;

	  break;
      }
    } while (work4 == 0);

    if (work2 > work4)
      work2 = work4;

    if (lizards_in_hex (hex) + work2 > MAX_LIZARDS_IN_HEX)
      work2 = MAX_LIZARDS_IN_HEX - lizards_in_hex (hex);

    switch (work3)
    {
      case RED_LIZARD:
	*red -= work2;
	*red_captured += work2;
	hex->red_lizards += work2;

	break;
      case GREEN_LIZARD:
	*green -= work2;
	*green_captured += work2;
	hex->green_lizards += work2;

	break;
      case GREY_LIZARD:
	*grey -= work2;
	*grey_captured += work2;
	hex->grey_lizards += work2;

	break;
      case BLACK_LIZARD:
	*black -= work2;
	*black_captured += work2;
	hex->black_lizards += work2;

	break;
      case YELLOW_LIZARD:
	*yellow -= work2;
	*yellow_captured += work2;
	hex->yellow_lizards += work2;
    }

    work -= work2;
  }
#endif
}

/* ----------------------------------------------------------------------
--
-- Function: combat_sea_monster
--
-- Return values;
--     0 monster dies
--     1 band dies
--
---------------------------------------------------------------------- */
int combat_sea_monster (world_t *w, feature_t *band, coord xy, int monster)
{
  FILE *fptr;
  int attacking_deaths,
      defending_deaths,
      attack_hit,
      defend_hit,
      round,
      work,
      lizards;

  hex_t *hex = get_hex(w, xy);
  round = 0;
  open_player_mesg (w, hex->owner, &fptr);
  attack_hit = BASE_ATTACK + (band->f.band.combat * CP_MOD_LEVEL);
  defend_hit = MONSTER_DEFEND;
  do
    {
      defending_deaths = 0;
      attacking_deaths = 0;
      lizards = lizards_in_band(band);
      if (lizards == 0)
	lizards = 1;
      for (work = 0; work < lizards; work ++)
	if (random (100) <= attack_hit &&
	    defending_deaths < monster)
	  defending_deaths ++;
      for (work = 0; work < (monster / 2); work ++)
	if (random (100) >= defend_hit &&
	    attacking_deaths < lizards_in_band(band))
	  attacking_deaths ++;
      round ++;

    kill_lizards (band, attacking_deaths);
    monster -= defending_deaths;
    } while (lizards_in_band(band) > 0 && monster > 0);

  fprintf (fptr, " - Combat round %d! ", round);
  fprintf (fptr, "Surviving Lizards: %d.\n", lizards_in_band(band));
  return (monster > 0);
}

static void whirlpool_kill (world_t *world, int x, int y)
{
#ifdef IgnoreThisStuff
  FILE *fptr;

  char work_string [80],
       temp_string [80];

  int work;

  hex_t hex;

  get_hex(world, x, y, &hex);

  if (lizards_in_hex (&hex) > 0)
  {
    xy_to_string (x, y, work_string);

    open_player_mesg (world, hex.hex_owner, &fptr);

    work = (int)((double)lizards_in_hex (&hex) * WHIRLPOOL_KILL_LEVEL);

    if (work < 1)
      work = 1;

    fprintf (fptr, " - %d Lizards at %s are sucked down to their deaths in Whirlpool!\n",
		   work, work_string);

    kill_lizards_xy (&hex, work);

    put_hex(world, x, y, &hex);

    if (lizards_in_hex (&hex) == 0)
    {
      fprintf (fptr, " - NO Lizards resist the Whirlpool!\n");

      /* fclose(fptr); */
    } else
      fprintf (fptr, " - %s Lizards at %s resist the Whirlpool!\n",
	       show_lizards (hex.red_lizards,
	       hex.green_lizards, hex.grey_lizards,
	       hex.black_lizards, hex.yellow_lizards,
	      temp_string), work_string);

    /* fclose(fptr); */
  }

  for (work = 0; work < world->num_spies; work ++)
    if (world->spylist [work]->x_hex == x && world->spylist [work]->y_hex == y &&
	world->spylist [work]->player > 0)
    {
      open_player_mesg (world, world->spylist [work]->player, &fptr);

      xy_to_string (x, y, work_string);

      fprintf (fptr, " - Spy Lizard at %s sucked down it's death in Whirlpool!\n",
	       work_string);

      /* fclose(fptr); */

      world->spylist [work]->player = 0;
    }
#endif
}

static void bridge_collapse_kill (world_t *world, int x, int y)
{
#ifdef IgnoreThisStuff
  FILE *fptr;

  char work_string [80],
       temp_string [80];
  int work;

  hex_t hex;

  get_hex(world, x, y, &hex);

  if (lizards_in_hex (&hex) > 0)
  {
    xy_to_string (x, y, work_string);

    open_player_mesg (world, hex.hex_owner, &fptr);

    work = (int)((double) lizards_in_hex (&hex) * COLLAPSE_KILL_LEVEL);

    if (work > 1)
      work = random (work) + 1;
    else
      work = 1;

    fprintf (fptr, " - %d Lizards at %s are killed by falling wreckage!\n",
		   work, work_string);

    kill_lizards_xy (&hex, work);

    put_hex(world, x, y, &hex);

    if (lizards_in_hex (&hex) == 0)
    {
      fprintf (fptr, " - NO Lizards survive the fall!\n");

      /* fclose(fptr); */
    } else
      fprintf (fptr, " - %s Lizards at %s survived the fall.\n",
	       show_lizards (hex.red_lizards,
	       hex.green_lizards, hex.grey_lizards,
	       hex.black_lizards, hex.yellow_lizards,
	      temp_string), work_string);

    /* fclose(fptr); */
  }

  for (work = 0; work < world->num_spies; work ++)
    if (world->spylist [work]->x_hex == x && world->spylist [work]->y_hex == y &&
	world->spylist [work]->player > 0)
    {
      open_player_mesg (world, world->spylist [work]->player, &fptr);

      xy_to_string (x, y, work_string);

      fprintf (fptr, " - Spy Lizard at %s killed by falling wreckage!\n",
	       work_string);

      /* fclose(fptr); */

      world->spylist [work]->player = 0;
    }
#endif
}
