#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "lizard.h"
#include "order.h"

#include "orders.h"
#include "moreords.h"
#include "terrain.h"
#include "blddest.h"

#include "filefunc.h"

#include "lizgame.h"
#include "lizmesg.h"
#include "lizmisc.h"
#include "lizord.h"

int main (int argc, char *argv []);
void DoPass(World *, int);

order_t *order_list [MAX_ORDERS_PER_TURN];

char *message_list [MAX_RANDOM_MESSAGES];

/* ----------------------------------------------------------------------
--
-- DoPass
--
---------------------------------------------------------------------- */
void DoPass(World *world, int pass)
{
  int player, order_num;
  FILE *fptr;

  for (player = 1; player <= world->num_players; player++)
    {
      open_player_mesg (player, &fptr);
      
      switch (pass)
	{
	case 0:
	  fprintf (fptr, "Phase(artillery)\n");
	  break;
	case 1:
	  fprintf (fptr, "Phase(movement)\n");
	  break;
	case 2:
	  fprintf (fptr, "Phase(development)\n");
	  break;
	}
      fclose (fptr);
    }

  if (pass == -1)
    printf ("LIZARDS! SYSTEM PASS:\n");
  else
    printf ("PASS %d:\n", pass);

  for (order_num = 0; order_num < number_orders; order_num ++)
    {
      switch (order_list [order_num]->order_type)
	{
	case MOVE:
	  if (pass == 1)
	    {
	      show_order (order_list [order_num]);
	      
	      open_player_mesg (order_list [order_num]->player, &fptr);
	      fprintf (fptr, "\n");
	      fclose (fptr);
	      
	      move_order (order_list [order_num]);
	    }
	  break;

	case SPLIT:
	  if (pass == 1)
	    {
	      show_order (order_list [order_num]);
	      
	      open_player_mesg (order_list [order_num]->player, &fptr);
	      fprintf (fptr, "\n");
	      fclose (fptr);
	      
	      split_order (order_list [order_num]);
	    }
	  break;
	  
	case BUILD_FERTILE:
	  if (pass == 2)
	    {
	      show_order (order_list [order_num]);
	      
	      open_player_mesg (order_list [order_num]->player, &fptr);
	      fprintf (fptr, "\n");
	      fclose (fptr);
		  
	      build_fertile_order (order_list [order_num]);
	    }
	  break;
	  
	case BUILD_DEN:
	  if (pass == 2)
	    {
	      show_order (order_list [order_num]);
	      
	      open_player_mesg (order_list [order_num]->player, &fptr);
	      fprintf (fptr, "\n");
	      fclose (fptr);
	      
	      build_den_order (order_list [order_num]);
	    }
	  break;

	case BUILD_BRIDGE:
	  if (pass == 2)
	    {
	      show_order (order_list [order_num]);
	      
	      open_player_mesg (order_list [order_num]->player, &fptr);
	      fprintf (fptr, "\n");
	      fclose (fptr);
	      
	      build_bridge_order (order_list [order_num]);
	    }
	  break;
	
	case DESTROY_FERTILE:
	  if (pass == 2)
	    {
	      show_order (order_list [order_num]);
	      
	      open_player_mesg (order_list [order_num]->player, &fptr);
	      fprintf (fptr, "\n");
	      fclose (fptr);
	      
	      destroy_fertile_order (order_list [order_num]);
	    }
	  break;
	  
	case DESTROY_BRIDGE:
	  if (pass == 2)
	    {
	      show_order (order_list [order_num]);
	      
	      open_player_mesg (order_list [order_num]->player, &fptr);
	      fprintf (fptr, "\n");
	      fclose (fptr);
	      
	      destroy_bridge_order (order_list [order_num]);
	    }
	  break;
	  
	case RECRUIT:
	  if (pass == 2)
	    {
	      show_order (order_list [order_num]);
	      
	      open_player_mesg (order_list [order_num]->player, &fptr);
	      fprintf (fptr, "\n");
	      fclose (fptr);
	      
	      recruit_order (order_list [order_num]);
	    }
	  break;
	  
	case ERUPT_VOLCANO:
	  if (pass == 1)
	    {
	      show_order (order_list [order_num]);
	      erupt_volcano_order (order_list [order_num]);
	    }
	  break;
	  
	case COLLAPSE_BRIDGE:
	  if (pass == 1)
	    {
	      printf ("Collapse Bridge Order.\n");
	      collapse_bridge_order (order_list [order_num], 0, 0);
	    }

	  break;
	case MOVE_WHIRLPOOL:
	  if (pass == 1)
	    {
	      show_order (order_list [order_num]);
	      move_whirlpool_order (order_list [order_num]);
	    }
	  break;

	case CHANT:
	  if (pass == 0)
	    {
	      show_order (order_list [order_num]);
	      
	      open_player_mesg (order_list [order_num]->player, &fptr);
	      fprintf (fptr, "\n");
	      fclose (fptr);
	      chant_order (order_list [order_num]);
	    }
	  break;

	case GIVE:
	  if (pass == 2)
	    {
	      show_order (order_list [order_num]);
	      
	      open_player_mesg (order_list [order_num]->player, &fptr);
	      fprintf (fptr, "\n");
	      fclose (fptr);
	      
	      give_order (order_list [order_num]);
	    }
	  break;
	  
	case FREE:
	  if (pass == 0)
	    {
	      show_order (order_list [order_num]);
	      
	      open_player_mesg (order_list [order_num]->player, &fptr);
	      fprintf (fptr, "\n");
	      fclose (fptr);
	      
	      free_order (order_list [order_num]);
	    }
	  break;
	
	case LOB:
	  if (pass == 0)
	    {
	      show_order (order_list [order_num]);
	      
	      open_player_mesg (order_list [order_num]->player, &fptr);
	      fprintf (fptr, "\n");
	      fclose (fptr);
	      
	      lob_order (order_list [order_num]);
	    }
	  break;

	case BUILD_RAFT:
	  if (pass == 2)
	    {
	      show_order (order_list [order_num]);
	      
	      open_player_mesg (order_list [order_num]->player, &fptr);
	      fprintf (fptr, "\n");
	      fclose (fptr);
	      
	      build_raft_order (order_list [order_num]);
	    }
	  break;
	  
	case SAIL:
	  if (pass == 1)
	    {
	      show_order (order_list [order_num]);
	      
	      open_player_mesg (order_list [order_num]->player, &fptr);
	      fprintf (fptr, "\n");
	      fclose (fptr);
	      
	      move_order (order_list [order_num]);
	    }
	  
	  break;
	case ERUPT_MOUNTAIN:
	  if (pass == 1)
	    {
	      show_order (order_list [order_num]);
	      erupt_mountain_order (order_list [order_num]);
	    }
	  break;

	case DORMANT_VOLCANO:
	  if (pass == 1)
	    {
	      show_order (order_list [order_num]);	      
	      dormant_volcano_order (order_list [order_num]);
	    }
	  break;

	case KILL_PLAYER:
	  if (pass == -1)
	    {
	      show_order (order_list [order_num]);
       	      kill_player_order (order_list [order_num]);
	    }
	  break;

	case SNEAK:
	  if (pass == 1)
	    {
	      show_order (order_list [order_num]);

	      open_player_mesg (order_list [order_num]->player, &fptr);
	      fprintf (fptr, "\n");
	      fclose (fptr);

	      sneak_order (order_list [order_num]);
	    }
	  break;

	default:
	  printf ("Inactive Order.\n");
	  break;
	}
    }
}

/* ----------------------------------------------------------------------
--
-- main
--
---------------------------------------------------------------------- */

int main (int argc, char *argv [])
{
  FILE *fptr;

  char filename [80];

  int work,
    pass;

  order_t work_order;

  randomize ();

  show_title ();

  printf ("LIZARDS! World Master. %s Last compiled %s.\n\n",
	  VER_STRING, __DATE__);

  if (argc == 3)
    {
      data_path = LzCheckDir(argv[1], 0);
      game_code = (char*)malloc(strlen(argv[2] + 1));
      strcpy (game_code, argv[2]);
      strupr (game_code);
    } else {
      printf ("FATAL ERROR: Turn and Game Number not specified on command line!\n"
	      "             Command Line Format: EXEFILE data_path world.turn game.\n"
	      "             e.g. %s C:\\ 1 A\n", argv[0]);

      exit (EXIT_FAILURE);
    }

  world = (World*)malloc(sizeof(World));
  get_world(world);

  if (world->num_players == 0)
    {
      fprintf (stderr, "FATAL ERROR: Game %s has no players.\n", game_code);
      exit (EXIT_FAILURE);
    }

  sprintf (filename, "%s%s%03d.%s", data_path, ORDS_SHUF_F, world->turn,
	   game_code);

  if ((fptr = fopen (filename, "rb")) == NULL)
    {
      fprintf (stderr, "FATAL ERROR: Unable to read %s file.\n", filename);
      exit (EXIT_FAILURE);
    }

  number_orders = num_records (fptr, sizeof (order_t));
      
  for (work = 0; work < number_orders; work ++)
    {
      get_rec (fptr, sizeof (order_t), work, &work_order);
	  
      if ((order_list [work] = malloc (sizeof (order_t))) == NULL)
	{
	  fprintf (stderr, "FATAL ERROR: Out of memory.\n");
	  exit (EXIT_FAILURE);
	}

      *(order_list [work]) = work_order;
    }

  fclose (fptr);

  for (pass = -1; pass < 3; pass ++)
    {
      DoPass(world, pass);
      printf ("PASS %d END.\n", pass);
    }
      
  for (work = 0; work < number_orders; work ++)
    free (order_list [work]);

  for (work = 0; work < world->num_spies; work ++)
    free (world->spylist [work]);

  for (work = 0; work < MAX_RANDOM_MESSAGES; work ++)
    if (message_list [work] != NULL)
      free (message_list [work]);

  sprintf (filename, "%s%s%03d.%s", data_path, ORDS_SHUF_F, world->turn,
	   game_code);

  remove (filename); /* delete orders file once finished with it */

  put_world(world);
  printf ("Finished!\n");

  return (EXIT_SUCCESS);
}
