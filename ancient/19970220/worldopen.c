#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "lizard.h"
#include "map.h"
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
int lizard_pop_growth (int present_pop, int max_pop);
int ProcessHex(hex_t world_hex);

unsigned int spawn_turn_switch = 0,
	     capture_switch = 0,
	     volcano_dormant_switch = 0,
	     peak_active_switch = 0,
	     world_wrap_switch = 0,
             victory_points_switch = 0,
             home_owned_worth = 0,
	     home_victory_points = 0,
	     give_switch = 0;

/* ----------------------------------------------------------------------
--
-- lizard_pop_growth
--
---------------------------------------------------------------------- */

int lizard_pop_growth (int present_pop, int max_pop)
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

    if (spawn_turn_switch)
      fwork2 *= 10.0;

    work = (float) present_pop * fwork2;
  }

  return (work);
}

/* ---------------------------------------------------------------------- */

int ProcessHex(hex_t world_hex)
{
  FILE *fptr;

  char work_string [80];

  int work3,
    work4,
    hex_change;
  
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
	  
  if (world_hex.terrain == CURSED && world.turn > 0)
    {
      if (lizards_in_hex (&world_hex) > 0)
	{
	  open_player_mesg (world_hex.hex_owner, &fptr);
		  
	  work3 = (float) lizards_in_hex (&world_hex) * CURSED_KILL_LEVEL;
		  
	  if (work3 < 1) work3 = 1;

	  fprintf (fptr, "Cursed: %s, %d %d %d %d %d %d\n",
		   work_string,
		   lizards_in_hex(&world_hex),
		   world_hex.red_lizards,
		   world_hex.green_lizards,
		   world_hex.grey_lizards,
		   world_hex.black_lizards,
		   world_hex.yellow_lizards);

	  kill_lizards_xy (&world_hex, work3);
	  hex_change = 1;
	  fclose (fptr);
	}

      for (work3 = 0; work3 < number_spys; work3 ++)
	if (spy_list [work3]->x_hex == world_hex.x &&
	    spy_list [work3]->y_hex == world_hex.y)
	  {
	    open_player_mesg (spy_list [work3]->player, &fptr);
	    fprintf (fptr, "SpyCursed: %s\n",
		     work_string);
	    spy_list [work3]->player = 0;
	    fclose (fptr);
	  }
    }
  
  if (world_hex.terrain == DEN || world_hex.terrain == HOME_DEN)
    {
      work3 = terrain_adjacent (world_hex.x, world_hex.y, FERTILE);
	      
      if (world_hex.hex_owner > 0)
	open_player_mesg (world_hex.hex_owner, &fptr);
      else
	fptr = NULL;

      if (work3 == 0)
	{
	  if (world.turn > 0)
	    {
	      if (world_hex.den_lizard_type == 0)
		{
		  if (fptr != NULL)
		    fprintf (fptr, "NomadStarve: %s %d",
			     work_string,
			     world_hex.lizard_pop);
		  world_hex.lizard_pop = 0;
		  hex_change = 1;
		  world_hex.terrain = RUIN;
 		}
	      else
		{
		  work4 = ((double) world_hex.lizard_pop *
 			   (DEN_STARVE_LEVEL + (world_hex.turns_den_hungry *
						DEN_INCREASE_STARVE_LEVEL)));
		  if ((world_hex.lizard_pop - work4) < MINIMUM_POP_LEVEL)
		    {
		      world_hex.terrain = RUIN;
		      world_hex.lizard_pop = 0;
			      
		      if (world_hex.hex_owner == 0)
			{
			  world_hex.red_lizards = 0;
			  world_hex.green_lizards = 0;
			  world_hex.grey_lizards = 0;
			  world_hex.black_lizards = 0;
			  world_hex.yellow_lizards = 0;
			}
		      else
			{
			  if (world_hex.terrain == HOME_DEN)
			    fprintf (fptr, "HomeDenStarveRuin: %s %d\n", 
				     work_string,
				     world_hex.den_lizard_type);
			  else
			    fprintf (fptr, "DenStarveRuin: %s %d\n", 
				     work_string,
				     world_hex.den_lizard_type);
			}
		      world_hex.den_lizard_type = 0;
		      world_hex.turns_den_hungry = 0;
		      hex_change = 1;
		    }
		  else 
		    {
		      if (fptr != NULL)
			if (world_hex.terrain == HOME_DEN)
			  fprintf (fptr, "HomeDenStarve: %s %d %d %d\n",
				   work_string,
				   world_hex.den_lizard_type,
				   work4,
				   world_hex.lizard_pop);
			else
			  fprintf (fptr, "DenStarve: %s %d %d %d\n",
				   work_string,
				   world_hex.den_lizard_type,
				   work4,
				   world_hex.lizard_pop);
		      world_hex.turns_den_hungry ++;
		      world_hex.lizard_pop -= work4;
		      hex_change = 1;
		    }
		}
	    }
	}
      else
	{
	  if (world_hex.den_lizard_type == 0)
	    {
	      work4 = POP_ADDITION;
		      
	      world_hex.lizard_pop += work4;

	      fprintf (fptr, "NomadsJoinDen: %s %d\n",
		       work_string,
		       work4);
	      if (world_hex.lizard_pop > MINIMUM_POP_LEVEL)
		{
		  world_hex.den_lizard_type = random (5) + 1;
		  fprintf (fptr, "DenSettled: %s %d\n",
			   work_string,
			   world_hex.den_lizard_type);
		}
	      hex_change = 1;
	    }
	  else 
	    {
	      if (world_hex.lizard_pop < (work3 * DEN_LIZARD_LEVEL))
		{
		  work4 = lizard_pop_growth (world_hex.lizard_pop,
					     work3 * DEN_LIZARD_LEVEL);
		  
		  world_hex.lizard_pop += work4;
			  
		  hex_change = 1;
		}
	      else
		{
		  if (world_hex.lizard_pop > (work3 * DEN_LIZARD_LEVEL) &&
		      world.turn > 0)
		    {
		      work4 = lizard_pop_growth (world_hex.lizard_pop,
						 work3 * DEN_LIZARD_LEVEL);
			      
		      if (work4 == 0)
			work4 = -1;
			      
		      world_hex.lizard_pop += work4;
		      if (fptr != NULL)
			fprintf (fptr, "DenOverpopulation: %s %d\n",
				 work_string,
				 -work4);
		      hex_change = 1;
		    }
		}
	    }
	}
      
      if (fptr != NULL)
	fclose (fptr);
    }
  if (world_hex.terrain != FERTILE &&
      lizards_in_hex (&world_hex) > 0)
    {
      if (world_hex.turns_hungry < MAX_HUNGER_POINTS && world.turn > 0)
	{
	  world_hex.turns_hungry ++;
	  hex_change = 1;
	}
    }
  else
    if (world_hex.terrain == FERTILE &&
	lizards_in_hex (&world_hex) > 0)
      {
	world_hex.turns_hungry = 0;
	hex_change = 1;
      }
  return hex_change;
}

/* ---------------------------------------------------------------------- */


int main (int argc, char *argv [])
{
  FILE *fptr;

  int work,
      work2,
      hex_change,
      header = 0;

  hex_t world_hex;

  randomize ();

  show_title ();

  printf ("LIZARDS! World Open. %s Last compiled %s.\n\n",
          VER_STRING, __DATE__);

  if (argc == 4)
  {
    data_path = LzCheckDir(argv[1], 0);
    world.turn = atoi (argv[2]);
    game_code = (char*)malloc(strlen(argv[3]) + 1);
    strcpy (game_code, argv[3]);
    strupr (game_code);
  }
  else
    {
      fprintf (stderr, "FATAL ERROR: Turn and Game Number not specified on command line!\n"
	       "             Command Line Format: EXEFILE data_path turn game.\n"
	       "                             e.g. WORLOPEN C:\\ 1 A\n");
      exit (EXIT_FAILURE);
    }
  
  get_switch_list ();
    get_player_list ();
    if (players_in_game == 0)
    {
      fprintf (stderr, "FATAL ERROR: Game %s has no players.\n", game_code);
      exit (EXIT_FAILURE);
    }
  
  get_spy_list ();
  get_world (&world);

  /* Set up headers for PLAYER message files! */

  printf ("Messaging players...");

  for (work = 1; work < players_in_game + 1; work ++)
    {
      open_player_mesg (work, &fptr);
      
      if (header)
	fprintf (fptr, "Header: on\n");
      else
	fprintf(fptr, "Header: off\n");
      
      fprintf (fptr, "ClanID: %s\n", player[work].code);
      if (spawn_turn_switch)
	fprintf (fptr, "Spawning: on\n");
      else
	fprintf (fptr, "Spawning: off\n");
      fprintf (fptr, "Phase: world\n");
      fclose (fptr);
    }

  open_player_mesg (0, &fptr);
  {
      if (header)
	fprintf (fptr, "Header: on\n");
      else
	fprintf(fptr, "Header: off\n");
      fprintf (fptr, "Turn: %d\n", world.turn);
      fprintf (fptr, "Game: %s\n", game_code);
      fprintf (fptr, "DueDate: %s\n", game_due_date);
  }
  fclose (fptr);
  
  printf (" \nOpening world...");
  
  /* ---------
     --
     -- process each work hex in turn
     --
     --------- */

  for (work = 0; work < world.x; work ++)
    {
      for (work2 = 0; work2 < world.y; work2 ++)
	{
	  get_hex (work, work2, &world_hex);
	  hex_change = ProcessHex(world_hex);
	  if (hex_change)
	    put_hex (work, work2, &world_hex);
	}
    }

  printf (" \nUpdating spys...");

  /* Update spy turns alone */

  for (work = 0; work < number_spys; work ++)
    {
      spy_list [work]->turns_alone ++;
      get_hex (spy_list [work]->x_hex, spy_list [work]->y_hex, &world_hex);
      
      /* Free immobile Spys */
      
      if (spy_list [work]->mired > 0)
	spy_list [work]->mired -= 1;
      
      /* Clear Spy moved flags */
      
      spy_list [work]->moved_this_turn = 0;
    }
  
  put_spy_list ();
  
  printf (" \n\nFinished!\n");
  
  return (EXIT_SUCCESS);
}
