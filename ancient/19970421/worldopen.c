#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "lizard.h"
#include "order.h"

#include "lizgame.h"
#include "lizhex.h"
#include "lizmisc.h"

/* ----------------------------------------------------------------------
--
-- Prototypes
--
---------------------------------------------------------------------- */

int main (int argc, char *argv []);
int lizard_pop_growth (World *, int present_pop, int max_pop);
void ProcessHex(World *, int x, int y);

/* ----------------------------------------------------------------------
--
-- lizard_pop_growth
--
---------------------------------------------------------------------- */

int lizard_pop_growth (World *world, int present_pop, int max_pop)
{
  float fwork,
	fwork2;

  int work;

  if (present_pop > max_pop)
  {
    if (present_pop - max_pop < 1)
      return (1);
    else
      return (-((present_pop - max_pop) / 2));
  } else {
    fwork = ((float) present_pop / (float) max_pop);

    fwork2 = 0.6 - ((fwork / 0.10) * 0.05);

    if (world->flags & LZ_SPAWN)
      fwork2 *= 10.0;

    work = (float) present_pop * fwork2;
  }

  return (work);
}
/* ----------------------------------------------------------------------
--
-- ProcessDen
--
---------------------------------------------------------------------- */
int ProcessDen(World *world, hex_t *world_hex);
int ProcessDen(World *world, hex_t *world_hex)
{
  int hex_change, work3, work4;
  char work_string[80];
  int owner = world_hex->hex_owner;
  
  work3 = terrain_adjacent (world, world_hex->x, world_hex->y, FERTILE);
  if (work3 == 0)
    {
      if (world->turn > 0)
	{
	  if (world_hex->den_lizard_type == 0)
	    {
	      if (world_hex->hex_owner > 0)
		fprintf (world->player[world_hex->hex_owner].fptr,
			 "NomadStarve: %s %d",
			 work_string,
			 world_hex->lizard_pop);
	      world_hex->lizard_pop = 0;
	      hex_change = 1;
	      world_hex->terrain = RUIN;
	    }
	  else
	    {
	      work4 = ((double) world_hex->lizard_pop *
		       (DEN_STARVE_LEVEL + (world_hex->turns_den_hungry *
					    DEN_INCREASE_STARVE_LEVEL)));
	      if ((world_hex->lizard_pop - work4) < MINIMUM_POP_LEVEL)
		{
		  world_hex->terrain = RUIN;
		  world_hex->lizard_pop = 0;
		  
		  if (world_hex->hex_owner == 0)
		    {
		      world_hex->red_lizards = 0;
		      world_hex->green_lizards = 0;
		      world_hex->grey_lizards = 0;
		      world_hex->black_lizards = 0;
		      world_hex->yellow_lizards = 0;
		    }
		  else
		    {
		      if (world_hex->terrain == HOME_DEN)
			fprintf (world->player[world_hex->hex_owner].fptr,
				 "HomeDenStarveRuin: %s %d\n", 
				 work_string,
				 world_hex->den_lizard_type);
		      else
			fprintf (world->player[world_hex->hex_owner].fptr,
				 "DenStarveRuin: %s %d\n", 
				 work_string,
				 world_hex->den_lizard_type);
		    }
		  world_hex->den_lizard_type = 0;
		  world_hex->turns_den_hungry = 0;
		  hex_change = 1;
		}
	      else 
		{
		  if (world_hex->hex_owner > 0)
		    if (world_hex->terrain == HOME_DEN)
		      fprintf (world->player[owner].fptr,
			       "HomeDenStarve: %s %d %d %d\n",
			       work_string,
			       world_hex->den_lizard_type,
			       work4,
			       world_hex->lizard_pop);
		    else
		      fprintf (world->player[owner].fptr,
			       "DenStarve: %s %d %d %d\n",
			       work_string,
			       world_hex->den_lizard_type,
			       work4,
			       world_hex->lizard_pop);
		  world_hex->turns_den_hungry ++;
		  world_hex->lizard_pop -= work4;
		  hex_change = 1;
		}
	    }
	}
    }
  else
    {
      if (world_hex->den_lizard_type == 0)
	{
	  work4 = POP_ADDITION;
	  
	  world_hex->lizard_pop += work4;
	  
	  fprintf (world->player[owner].fptr,
		   "NomadsJoinDen: %s %d\n",
		   work_string,
		   work4);
	  if (world_hex->lizard_pop > MINIMUM_POP_LEVEL)
	    {
	      world_hex->den_lizard_type = random (5) + 1;
	      fprintf (world->player[owner].fptr,
		       "DenSettled: %s %d\n",
		       work_string,
		       world_hex->den_lizard_type);
	    }
	  hex_change = 1;
	}
      else 
	{
	  if (world_hex->lizard_pop < (work3 * DEN_LIZARD_LEVEL))
	    {
	      work4 = lizard_pop_growth (world, world_hex->lizard_pop,
					 work3 * DEN_LIZARD_LEVEL);
	      
	      world_hex->lizard_pop += work4;
	      hex_change = 1;
	    }
	  else
	    {
	      if (world_hex->lizard_pop > (work3 * DEN_LIZARD_LEVEL) &&
		  world->turn > 0)
		{
		  work4 = lizard_pop_growth (world, world_hex->lizard_pop,
					     work3 * DEN_LIZARD_LEVEL);
		  
		  if (work4 == 0)
		    work4 = -1;
		  
		  world_hex->lizard_pop += work4;
		  if (owner > 0)
		    fprintf (world->player[owner].fptr,
			     "DenOverpopulation: %s %d\n",
			     work_string,
			     -work4);
		  hex_change = 1;
		}
	    }
	}
    }
  return hex_change;
}

/* ---------------------------------------------------------------------- */

void ProcessHex(World *world, int x, int y)
{
  hex_t world_hex;
  int owner, work3, hex_change;
  char work_string [80];

  get_hex(world, x, y, &world_hex);
  owner = world_hex.hex_owner;
  hex_change = 0;
  xy_to_string (world_hex.x, world_hex.y, work_string);

  if (world_hex.turns_undisturbed > 0)
    {
      world_hex.turns_undisturbed --;
      hex_change = 1;
    }

  if (world_hex.lizards_immobile > 0)
    {
      if (world_hex.lizards_immobile == 3)
	world_hex.lizards_immobile = 0;
      else
	world_hex.lizards_immobile -= 1;
      hex_change = 1;
    }
	  
  if ((world_hex.terrain == CURSED) && (world->turn > 0))
    {
      if (lizards_in_hex (&world_hex) > 0)
	{
	  work3 = (float) lizards_in_hex (&world_hex) * CURSED_KILL_LEVEL;
		  
	  if (work3 < 1) work3 = 1;

	  fprintf (world->player[world_hex.hex_owner].fptr, 
		   "Cursed: %s, %d %d %d %d %d %d\n",
		   work_string,
		   lizards_in_hex(&world_hex),
		   world_hex.red_lizards,
		   world_hex.green_lizards,
		   world_hex.grey_lizards,
		   world_hex.black_lizards,
		   world_hex.yellow_lizards);

	  kill_lizards_xy (&world_hex, work3);
	  hex_change = 1;
	}

      for (work3 = 0; work3 < world->num_spies; work3 ++)
	if (world->spylist [work3]->x_hex == world_hex.x &&
	    world->spylist [work3]->y_hex == world_hex.y)
	  {
	    fprintf (world->player[world_hex.hex_owner].fptr,
		     "SpyCursed: %s\n",
		     work_string);
	    world->spylist [work3]->player = 0;
	  }
    }
  
  if ((world_hex.terrain == DEN) || (world_hex.terrain == HOME_DEN))
    hex_change = ProcessDen(world, &world_hex);
  if (lizards_in_hex(&world_hex) > 0)
    {
      if (world_hex.terrain != FERTILE)
	{
	  if (world_hex.turns_hungry < MAX_HUNGER_POINTS && (world->turn > 0))
	    {
	      world_hex.turns_hungry ++;
	      hex_change = 1;
	    }
	}
      else
	{
	  world_hex.turns_hungry = 0;
	  hex_change = 1;
	}
    }
  if (hex_change)
    put_hex (world, x, y, &world_hex);
}

/* ---------------------------------------------------------------------- */


int main (int argc, char *argv [])
{
  int loop, x, y;
  hex_t world_hex;

  randomize ();
  show_title ();

  printf ("LIZARDS! World Open. %s Last compiled %s.\n\n",
          VER_STRING, __DATE__);

  if (argc == 3)
  {
    data_path = LzCheckDir(argv[1], 0);
    game_code = (char*)malloc(strlen(argv[2]) + 1);
    strcpy (game_code, argv[2]);
    strupr (game_code);
  }
  else
    {
      fprintf (stderr, "FATAL ERROR: Turn and Game Number not specified on command line!\n"
	       "             Command Line Format: EXEFILE data_path turn game.\n"
	       "                             e.g. WORLOPEN C:\\ 1 A\n");
      exit (EXIT_FAILURE);
    }
  world = (World*)malloc(sizeof(World));
  get_world (world);
  OpenPlayerFiles(world);

  if (world->num_players == 0)
    {
      fprintf (stderr, "FATAL ERROR: Game %s has no players.\n", game_code);
      exit (EXIT_FAILURE);
    }
  
  /* Set up headers for PLAYER message files! */

  printf ("Messaging players...");

  for (loop = 0; loop <= world->num_players; loop ++)
    {
      fprintf (world->player[loop].fptr, "Header(%s, %d)\n",
	       game_code, world->turn);
      fprintf (world->player[loop].fptr,
	       "ClanID(%s,%s)\n",
	       world->player[loop].code,
	       world->player[loop].clan_name);
      if (world->flags & LZ_SPAWN)
	fprintf (world->player[loop].fptr, "Spawning(on)\n");
      else
	fprintf (world->player[loop].fptr, "Spawning(off)\n");
    }

  for (loop = 1; loop <= world->num_players; loop++)
    fprintf (world->player[loop].fptr, "Phase(world)\n");
  printf (" \nOpening world...");
  
  /* ---------
     --
     -- process each work hex in turn
     --
     --------- */

  for (y = 0; y < world->y; y++)
    for (x = 0; x < world->x; x++)
      ProcessHex(world, x, y);

  /* Update spy turns alone */

  printf (" \nUpdating spys...");
  for (loop = 0; loop < world->num_spies; loop++)
    {
      world->spylist [loop]->turns_alone ++;
      get_hex (world,
	       world->spylist [loop]->x_hex,
	       world->spylist [loop]->y_hex, &world_hex);
      
      /* Free immobile Spys */
      
      if (world->spylist [loop]->mired > 0)
	world->spylist [loop]->mired -= 1;
       
      /* Clear Spy moved flags */
      
      world->spylist[loop]->moved_this_turn = 0;
    }
  ClosePlayerFiles(world);
  put_world(world);
  printf (" \n\nFinished!\n");
  
  return (EXIT_SUCCESS);
}
