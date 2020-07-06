#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "lizard.h"
#include "map.h"
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

char player_code [MAX_PLAYERS + 1][6];
char player_name [MAX_PLAYERS + 1][37];
char player_clan_name [MAX_PLAYERS + 1][37];
char player_home_den [MAX_PLAYERS + 1];
char players_in_game;

char player_road [MAX_PLAYERS + 1][37];
char player_burb [MAX_PLAYERS + 1][37];
char player_city [MAX_PLAYERS + 1][37];
char player_country [MAX_PLAYERS + 1][37];
char player_cred [MAX_PLAYERS + 1][10];
char player_output_mode [MAX_PLAYERS + 1];
char game_due_date [20];

order_t *order_list [MAX_ORDERS_PER_TURN];

spy_t *spy_list [MAX_SPYS];
int number_spys;

char dir [7][3] = { "", "N", "NE", "SE", "S", "SW", "NW" };

char lizard_colours [6][12] = { "Unsettled", "Red", "Green", "Grey", "Black", "Yellow" };

char liz_cp_desc [MAX_COMBAT_POINTS + 1][10] = { "Average",
						 "Good",
						 "Dangerous",
						 "Ferocious",
						 "Fearsome",
						 "Veteran" };

char liz_hunger_desc [MAX_HUNGER_POINTS + 1][10] = { "Sated",
						     "Peckish",
						     "Hungry",
						     "Famished",
						     "STARVED",
						     "STARVING" };

char *message_list [MAX_RANDOM_MESSAGES];

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
  FILE *fptr,
       *player_fptr;

  char filename [80],
       temp_string [255];

  int work,
      work2,
      order_num,
      pass;

  order_t work_order;

  randomize ();

  show_title ();

  printf ("LIZARDS! World Master. %s Last compiled %s.\n\n",
          VER_STRING, __DATE__);

  if (argc == 4)
  {
    data_path = LzCheckDir(argv[1], 0);
    turn_number = atoi (argv[2]);
    game_code = (char*)malloc(strlen(argv[3] + 1));
    strcpy (game_code, argv[3]);
    strupr (game_code);
  } else {
    printf ("FATAL ERROR: Turn and Game Number not specified on command line!\n"
             "             Command Line Format: EXEFILE data_path turn_number game.\n"
             "             e.g. WORLMAST C:\\ 1 A\n");

    exit (EXIT_FAILURE);
  }

  get_switch_list ();

  get_message_file ();

  get_world ();

  get_player_list ();

  if (players_in_game == 0)
  {
    fprintf (stderr, "FATAL ERROR: Game %s has no players.\n", game_code);
    exit (EXIT_FAILURE);
  }

  get_home_den_status ();

  get_spy_list ();

  sprintf (filename, "%s%s%03d.%s", data_path, ORDS_SHUF_F, turn_number,
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
    /* give everyone the pass header */

    for (work2 = 1; work2 < players_in_game + 1; work2 ++)
    {
      open_player_mesg (work2, &player_fptr);

      switch (pass)
      {
	case 0:
	  fprintf (player_fptr, "\n * ARTILLERY PHASE (all Chants, Lobs and Free orders).\n");

	  break;
	case 1:
	  fprintf (player_fptr, "\n * MOVEMENT PHASE (all Moves, Splits, Sails and Sneaks orders).\n");

	  break;
	case 2:
	  fprintf (player_fptr, "\n * DEVELOPMENT PHASE (all Builds, Destroy, Recruit and Give orders).\n");

	  break;
      }

      fclose (player_fptr);
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

  printf ("PASS %d END.\n", pass);
  for (work = 0; work < number_orders; work ++)
    free (order_list [work]);

  put_spy_list ();

  put_home_den_status ();

  for (work = 0; work < number_spys; work ++)
    free (spy_list [work]);

  for (work = 0; work < MAX_RANDOM_MESSAGES; work ++)
    if (message_list [work] != NULL)
      free (message_list [work]);

  sprintf (filename, "%s%s%03d.%s", data_path, ORDS_SHUF_F, turn_number,
    game_code);

  remove (filename); /* delete orders file once finished with it */

  printf ("Finished!\n");

  return (EXIT_SUCCESS);
}
