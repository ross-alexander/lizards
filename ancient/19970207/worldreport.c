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

int main (int argc, char *argv []);
int lizard_pop_growth (int present_pop, int max_pop);

char player_code [MAX_PLAYERS + 1][6];
char player_name [MAX_PLAYERS + 1][37];
char player_home_den [MAX_PLAYERS + 1];
char player_clan_name [MAX_PLAYERS + 1][37];

char players_in_game;

char player_road [MAX_PLAYERS + 1][37];
char player_burb [MAX_PLAYERS + 1][37];
char player_city [MAX_PLAYERS + 1][37];
char player_country [MAX_PLAYERS + 1][37];
char player_cred [MAX_PLAYERS + 1][10];
char player_output_mode [MAX_PLAYERS + 1];
char game_due_date [20];

spy_t *spy_list [MAX_SPYS];
int number_spys;

char dir [7][3] = { "", "N", "NE", "SE", "S", "SW", "NW" };

char lizard_colours [6][7] = { "", "Red", "Green", "Grey", "Black", "Yellow" };

unsigned int spawn_turn_switch = 0,
	     capture_switch = 0,
	     volcano_dormant_switch = 0,
	     peak_active_switch = 0,
	     world_wrap_switch = 0,
             victory_points_switch = 0,
             home_owned_worth = 0,
	     home_victory_points = 0,
	     give_switch = 0;

int main (int argc, char *argv [])
{
  FILE *fptr;

  char work_string [80],
       temp_string [80];

  int work,
      work2,
      work3,
      work4,
      hex_change,
      header = 0;

  hex_t world_hex;

  randomize ();

  show_title ();

  printf ("LIZARDS! World Open. %s Last compiled %s.\n\n",
          VER_STRING, __DATE__);

  /*
     Note: Fontasy support is PRESENTLY disabled. Fontasy support used to
     be via:

     WORLOPEN C:\\ 1 A H - For headers
     WORLOPEN C:\\ 1 A N - No headers

     and the 'header' variable, which is now by default OFF.
  */

  if (argc == 4)
  {
    data_path = LzCheckDir(argv[1], 0);
    turn_number = atoi (argv[2]);
    game_code = (char*)malloc(strlen(argv[3]) + 1);
    strcpy (game_code, argv[3]);
    strupr (game_code);

    /* for FONTASY support

    if (toupper (argv [4][0]) == 'H')
      header = 1;
    else
      header = 0;

    */
  } else {
    printf ("FATAL ERROR: Turn and Game Number not specified on command line!\n"
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

  get_world ();

  /* Set up headers for PLAYER message files! */

  printf ("Messaging players...");

  for (work = 1; work < players_in_game + 1; work ++)
  {
    open_player_mesg (work, &fptr);

    if (header)
    {
      fprintf (fptr, ".lb lnam.blk\n.ke f10\n.bf mono\n.cs 1\n.lh 10\n.kr OFF\n"
		     "\n\n\n(c) Copyright Circle Games and Stuart Nicholson, 1991.\n\n");
      fprintf (fptr, "\n\n\n\n\n");

      fprintf (fptr, ".bf spl\n.mf2\n");
      fprintf (fptr, "      %s\n", player_name [work]);
      fprintf (fptr, "      %s\n", player_road [work]);
      fprintf (fptr, "      %s\n", player_burb [work]);
      fprintf (fptr, "      %s\n", player_city [work]);
      fprintf (fptr, "      %s\n", player_country [work]);
      fprintf (fptr, ".bf mono\n.cs 1\n.lh 10\n.kr OFF\n\n");
    }

    fprintf (fptr, " * You are player %d (%s) with '%s' Clan!\n * You have Credit of $%s.\n",
	     work, player_name [work], player_clan_name [work], player_cred [work]);
    fprintf (fptr, " * This is Turn %d of LIZARDS! Game %s.\n"
		   " * ORDERS FOR NEXT TURN DUE BY %s.\n",
	     turn_number, game_code, strupr (game_due_date));

    fprintf (fptr, "\n * WORLD PHASE (all Den Growth and Settling).\n");

    if (spawn_turn_switch)
      fprintf (fptr, "\n - THIS IS A SPAWNING TURN! Den Lizards reproduce at TEN times normal rate.\n");

    fclose (fptr);
  }

  open_player_mesg (0, &fptr);

  fprintf (fptr, " * This is the Circle Games Header for Turn %d of LIZARDS! Game %s.\n"
		 " * ORDERS FOR NEXT TURN DUE BY %s.\n",
	   turn_number, game_code, strupr (game_due_date));

  fclose (fptr);

  printf (" \nOpening world...");

  for (work = 0; work < world_x; work ++)
    {
      for (work2 = 0; work2 < world_y; work2 ++)
	{
	  get_hex (work, work2, &world_hex);
	  
	  hex_change = 0;
	  
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
	  
	  if (world_hex.terrain == CURSED && turn_number > 0)
	    {
	      xy_to_string (work, work2, work_string);
	      
	      if (lizards_in_hex (&world_hex) > 0)
		{
		  open_player_mesg (world_hex.hex_owner, &fptr);
		  
		  work3 = (float) lizards_in_hex (&world_hex) * CURSED_KILL_LEVEL;
		  
		  if (work3 < 1) work3 = 1;
		  
		  fprintf (fptr, "\n - %d Lizards in CURSED hex at %s choke and collapse, dead. ",
			   work3, work_string);
		  
		  kill_lizards_xy (&world_hex, work3);
		  
		  hex_change = 1;
		  
		  if (lizards_in_hex (&world_hex) == 0)
		    fprintf (fptr, " NO Lizards resist the CURSE.\n");
		  else
		    fprintf (fptr, "\n - %s Lizards resist the CURSE.\n",
			     show_lizards (world_hex.red_lizards, world_hex.green_lizards,
					   world_hex.grey_lizards, world_hex.black_lizards,
					   world_hex.yellow_lizards, temp_string));
		  
		  fclose (fptr);
		}
	      
	      for (work3 = 0; work3 < number_spys; work3 ++)
		if (spy_list [work3]->x_hex == work &&
		    spy_list [work3]->y_hex == work2)
		  {
		    open_player_mesg (spy_list [work3]->player, &fptr);
		    
		    fprintf (fptr, " - Spy Lizard in CURSED hex at %s is poisoned and collapses, dead.\n",
			     work_string);
		    
		    spy_list [work3]->player = 0;
		    
		    fclose (fptr);
		  }
	    }

	  if (world_hex.terrain == DEN || world_hex.terrain == HOME_DEN)
	    {
	      work3 = terrain_adjacent (work, work2, FERTILE);
	      
	      if (world_hex.hex_owner > 0)
		open_player_mesg (world_hex.hex_owner, &fptr);
	      else
		fptr = NULL;
	      
	      xy_to_string (work, work2, work_string);
	      
	      if (work3 == 0)
		{
		  if (turn_number > 0)
		    {
		      if (world_hex.den_lizard_type == 0)
			{
			  if (world_hex.lizard_pop > 0)
			    {
			      if (fptr != NULL)
				fprintf (fptr, "\n - %d nomad Lizards at un-settled Den at %s leave as there's no food available.\n",
					 world_hex.lizard_pop, work_string);
			    }
			  else
			    {
			      if (fptr != NULL)
				fprintf (fptr, "\n - No nomad Lizards enter un-settled Den at %s as there's no food available.\n",
					 work_string);
			    }
			  world_hex.lizard_pop = 0;
			  hex_change = 1;
			  world_hex.terrain = RUIN;
			  if (fptr != NULL)
			    fprintf (fptr, " - Un-settled Den falls into Ruins.\n");
			}
		      else
			{
			  work4 = ((float) world_hex.lizard_pop *
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
				    fprintf (fptr, "\n - %s Home Den at %s is abandoned and becomes a Ruin due to lack of food!\n",
					     lizard_colours [world_hex.den_lizard_type], work_string);
				  else
				    fprintf (fptr, "\n - %s Den at %s is abandoned and becomes a Ruin due to lack of food!\n",
					     lizard_colours [world_hex.den_lizard_type], work_string);
				}
			      world_hex.den_lizard_type = 0;
			      world_hex.turns_den_hungry = 0;
			      
			      hex_change = 1;
			    }
			  else 
			    {
			      if (fptr != NULL)
				if (world_hex.terrain == HOME_DEN)
				  fprintf (fptr, "\n - %s Home Den at %s starves! ",
					   lizard_colours [world_hex.den_lizard_type],
					   work_string);
				else
				  fprintf (fptr, "\n - %s Den at %s starves! ",
					   lizard_colours [world_hex.den_lizard_type],
					   work_string);
			      
			      world_hex.turns_den_hungry ++;
			      
			      world_hex.lizard_pop -= work4;
			      
			      if (fptr != NULL)
				fprintf (fptr, " %d Den Lizards die leaving %d survivors...\n",
					 work4, world_hex.lizard_pop);
			      
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
		      
		      fprintf (fptr, "\n - %d nomad Lizards join un-settled Den at %s.",
			       work4, work_string);
		      
		      if (world_hex.lizard_pop > MINIMUM_POP_LEVEL)
			{
			  world_hex.den_lizard_type = random (5) + 1;
			  
			  fprintf (fptr, " Den is settled by nomad Lizards and becomes a %s Den!\n",
				   lizard_colours [world_hex.den_lizard_type]);
			} 
		      else
			fprintf (fptr, "\n");

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
			      turn_number > 0)
			    {
			      work4 = lizard_pop_growth (world_hex.lizard_pop,
							 work3 * DEN_LIZARD_LEVEL);
			      
			      if (work4 == 0)
				work4 = -1;
			      
			      world_hex.lizard_pop += work4;
			      
			      if (fptr != NULL)
				fprintf (fptr, "\n - %d Den Lizards at %s starve due to over-population!\n",
					 -work4, work_string);
			      
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
	      if (world_hex.turns_hungry < MAX_HUNGER_POINTS && turn_number > 0)
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
