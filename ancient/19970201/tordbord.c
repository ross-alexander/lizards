#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "lizard.h"
#include "map.h"
#include "order.h"

#include "filefunc.h"
#include "strgfunc.h"

#include "lizgame.h"
#include "lizhex.h"
#include "lizmisc.h"

#define NUM_POSSIBLE_LINES 44

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

unsigned int spawn_turn_switch = 0,
             capture_switch = 0,
	     volcano_dormant_switch = 0,
	     peak_active_switch = 0,
	     world_wrap_switch = 0,
	     victory_points_switch = 0,
	     home_owned_worth = 0,
	     home_victory_points = 0,
	     give_switch = 0;

spy_t *spy_list [MAX_SPYS];
int number_spys;

order_t *order_list [MAX_ORDERS_PER_TURN];
int number_of_orders;

char dir [7][3] = { "", "N", "NE", "SE", "S", "SW", "NW" };

char chant_code [NUMBER_CHANTS][5] = { "EYES", "REVE", "SHUN", "MOUL",
				       "COER", "TRAN", "DARK", "FREE",
				       "SUMM", "GATE" };

int main (int argc, char *argv []);
void add_order (order_t *order);

int main (int argc, char *argv [])
{
  FILE *fptr,
       *in_fptr,
       *bin_fptr,
       *temp_fptr;

  char possible_lines [NUM_POSSIBLE_LINES][20] = { "\x0", "/*",
						   "PLAYER", "PL",
						   "MESSAGE", "MS",
						   "MOVE", "MO",
						   "SPLIT", "SP",
						   "BUILD_FERTILE", "BF",
						   "BUILD_DEN", "BD",
						   "BUILD_BRIDGE", "BB",
						   "DESTROY_FERTILE", "DF",
						   "DESTROY_BRIDGE", "DB",
						   "RECRUIT", "RE",
						   "CHANT", "CH",
						   "MOD_MESG", "MMS",
						   "GIVE", "GI",
						   "LOB", "LO",
						   "FREE", "FR",
						   "BUILD_RAFT", "BR",
						   "SAIL", "SA",
						   "KILL", "KI",
						   "SNEAK", "SN",
						   "ERUPT", "ER",
						   "SPAWN_TURN", "ST" };

  char lizard_colours [5][4] = { "RED", "GRN", "GRY", "BLK", "YEL" };

  char filename [255],
       number [10],
       work_string [255],
       temp_string [255],
       temp_string2 [255],
       temp_string3 [255],
       mesg_string [40][255],
       screwed [255];

  int line_type;

  char **token_table;

  int loop,
      present_player = 0,
      work,
      work2,
      work3,
      work_x,
      work_y;

  int no_move_returned_flags [MAX_PLAYERS + 1];

  order_t work_order;

  hex_t world_hex;

  show_title ();

  printf ("LIZARDS! Text Orders Convertor. %s Last compiled %s.\n\n",
	  VER_STRING, __DATE__);

  if (argc == 4)
  {
    data_path = LzCheckDir(argv[1], 0);
    turn_number = atoi(argv[2]);
    game_code = (char*)malloc(strlen(argv[3]) + 1);
    strcpy (game_code, argv [3]);
    strupr (game_code);
  } else {
    printf ("FATAL ERROR: Turn and Game Number not specified on command line!\n"
	     "             Command Line Format: EXEFILE data_path turn_number game.\n"
	     "                             e.g. TORDBORD C:\\ 1 A\n");

    exit (EXIT_FAILURE);
  }

  get_world ();

  get_player_list ();

  if (players_in_game == 0)
  {
    printf ("FATAL ERROR: Game %s has no players. Press SPACE.\n", game_code);
    exit (EXIT_FAILURE);
  }

  get_spy_list ();

  sprintf (filename, "%s%s%03d.%s", data_path, TEXT_ORDS_F, turn_number,
    game_code);

  if ((in_fptr = fopen (filename, "rt")) == '\0')
  {
    fprintf (stderr, "FATAL ERROR: Unable to read %s file.\n", filename);
    exit (EXIT_FAILURE);
  }

  for (loop = 1; loop < players_in_game + 1; loop ++)
  {
    send_player_mesg (loop, "\n * LIZARDS! ORDER ERRORS\n\n");
    no_move_returned_flags [loop] = LZ_TRUE;
  }

  while (fgets (work_string, 140, in_fptr) != '\0')
  {
    work_string [strlen (work_string) - 1] = '\0';

    chop_space (work_string);

    strupr (work_string);

    strtoken (temp_string, work_string, " ");

    flay_spaces (work_string);

    for (line_type = 0; line_type < NUM_POSSIBLE_LINES; line_type ++)
      if (strcmp (temp_string, possible_lines [line_type]) == 0)
	break;

    strcpy (temp_string, work_string);

    switch (line_type)
    {
      case 0:
      case 1:
	break;		/* Comment line types, so ignore them! */
      case 2:		/* Player Command */
      case 3:
	work = atoi (temp_string);

	if (work >= 1 && work <= players_in_game)
	{
	  present_player = work;

	  printf ("Present Player: %d - %s.\n\n", work, player_name [work]);

	  no_move_returned_flags [present_player] = LZ_FALSE;
	}

	break;
      case 4:           /* Mesg Order */
      case 5:
	if (present_player == 0)
	{
	  printf ("FATAL ERROR: Order issued when player number unknown. Press SPACE.\n");
	  exit (EXIT_FAILURE);
	}

	strcpy (temp_string, work_string);

	work = 0;

	while (fgets (temp_string2, 140, in_fptr) != '\0')
	{
	  strcpy (mesg_string [work], temp_string2);

	  work ++;

	  if (strcmp (strupr (temp_string2), "ENDMESG\n") == 0)
	  {
	    work --;

	    break;
	  }

	  if (work == 40)
	    break;
	}

	sprintf (filename, "%s%s%02d_%03d.%s", data_path,
	  PLYPLY_MESG_F, present_player, turn_number, game_code);

	if ((fptr = fopen (filename, "at")) == '\0')
	  {
	    printf ("FATAL ERROR: Unable to append to %s file. Press SPACE.\n",
		    filename);
	    exit (EXIT_FAILURE);
	  }

	if (temp_string [0] == '\0')
	  fprintf (fptr, " - Message SENT by you to ALL Players\n");
	else
	  fprintf (fptr, " - Message SENT by you to Player(s): %s\n", temp_string);

	for (work2 = 0; work2 < work; work2 ++)
	  fprintf (fptr, "   %s", mesg_string [work2]);

	fprintf (fptr, "\n");

	fclose (fptr);

	if (temp_string [0] == '\0')
	{
	  for (work3 = 1; work3 < players_in_game + 1; work3 ++)
	  {
	    if (work3 != present_player)
	    {
	      sprintf (filename, "%s%s%02d_%03d.%s", data_path, PLYPLY_MESG_F,
		work3, turn_number, game_code);

	      if ((fptr = fopen (filename, "at")) == '\0')
	      {
                printf ("FATAL ERROR: Unable to append to %s file. Press SPACE.\n",
			filename);
		exit (EXIT_FAILURE);
	      }

	      fprintf (fptr, " - Message from Player %d, '%s' Clan:\n",
		       present_player, player_clan_name [present_player]);

	      for (work2 = 0; work2 < work; work2 ++)
		fprintf (fptr, "   %s", mesg_string [work2]);

	      fprintf (fptr, "\n");

	      fclose (fptr);
	    }
	  }
	} else {
	  while (1)
	  {
            strtoken (temp_string2, temp_string, " ");

            if (temp_string2 [0] == '\0')
	      break;

	    if (atoi (temp_string2) != present_player &&
		atoi (temp_string2) != 0)
	    {
	      sprintf (filename, "%s%s%02d_%03d.%s", data_path,
		PLYPLY_MESG_F, atoi (temp_string2), turn_number, game_code);

	      if ((fptr = fopen (filename, "at")) == '\0')
	      {
                printf ("FATAL ERROR: Unable to append to %s file. Press SPACE.\n",
			filename);
		exit (EXIT_FAILURE);
	      }

	      fprintf (fptr, " - Message from Player %d, '%s' Clan:\n",
		       present_player, player_clan_name [present_player]);

	      for (work2 = 0; work2 < work; work2 ++)
		fprintf (fptr, "   %s", mesg_string [work2]);

	      fprintf (fptr, "\n");

	      fclose (fptr);
	    }
	  }
	}

	break;
      case 6:		/* Move Order */
      case 7:
	if (present_player == 0)
        {
          printf ("FATAL ERROR: Order issued when player number unknown. Press SPACE.\n");
	  exit (EXIT_FAILURE);
	}

	work_order.player = present_player;
	work_order.order_type = MOVE;

	strtoken (temp_string2, temp_string, " ");

	string_to_loc (temp_string2, &work_x, &work_y);

	work_order.x_hex = work_x;
	work_order.y_hex = work_y;

	get_hex (work_x, work_y, &world_hex);

	if (world_hex.hex_owner == present_player)
	{
	  work_order.red_lizards = world_hex.red_lizards;
	  work_order.green_lizards = world_hex.green_lizards;
	  work_order.grey_lizards = world_hex.grey_lizards;
	  work_order.black_lizards = world_hex.black_lizards;
	  work_order.yellow_lizards = world_hex.yellow_lizards;
	} else {
	  work_order.red_lizards = 0;
	  work_order.green_lizards = 0;
	  work_order.grey_lizards = 0;
	  work_order.black_lizards = 0;
	  work_order.yellow_lizards = 0;
	}

	work_order.moves [0] = 0;
	work_order.moves [1] = 0;

	strtoken (temp_string2, temp_string, " ");

	work_order.moves [0] = string_to_direction (temp_string2);

	work_order.moves [1] = string_to_direction (temp_string);

	if (work_order.player > 0 && work_order.moves [0] > 0 &&
	    work_order.x_hex >= 0 && work_order.y_hex >= 0 &&
	    work_order.x_hex < world_x && work_order.y_hex < world_y)
	{
	  add_order (&work_order);
	} else {
	  send_player_mesg (present_player, " - 'MO %s' order is incorrectly written, or incomplete. Please refer to your Rulebook.\n",
	    work_string);
	}

	break;
      case 8:		/* Split Order */
      case 9:
	if (present_player == 0)
	{
	  printf ("FATAL ERROR: Order issued when player number unknown. Press SPACE.\n");

          getch ();

	  exit (EXIT_FAILURE);
	}

	work_order.player = present_player;
	work_order.order_type = SPLIT;

	strtoken (temp_string2, temp_string, " ");

	string_to_loc (temp_string2, &work_x, &work_y);

	work_order.x_hex = work_x;
	work_order.y_hex = work_y;

	work_order.red_lizards = 0;
	work_order.green_lizards = 0;
	work_order.grey_lizards = 0;
	work_order.black_lizards = 0;
	work_order.yellow_lizards = 0;

	work_order.moves [0] = 0;
	work_order.moves [1] = 0;

	while (1)
	{
	  strtoken (temp_string2, temp_string, " ");

	  work = atoi (temp_string2);

	  if (work == 0)
	    break;

	  strsplt (temp_string3, temp_string2, strlen (temp_string2) - 3);

	  for (work2 = 0; work2 < LIZARD_TYPES; work2 ++)
	    if (strcmp (temp_string3, lizard_colours [work2]) == 0)
	      break;

	  switch (work2)
	  {
	    case 0:
	      work_order.red_lizards = work;

	      break;
	    case 1:
	      work_order.green_lizards = work;

	      break;
	    case 2:
	      work_order.grey_lizards = work;

	      break;
	    case 3:
	      work_order.black_lizards = work;

	      break;
	    case 4:
	      work_order.yellow_lizards = work;

	      break;
	    default:
	      break;
	  }
	}

	work_order.moves [0] = string_to_direction (temp_string2);

	work_order.moves [1] = string_to_direction (temp_string);

	if (work_order.player > 0 && work_order.moves [0] > 0 &&
	    work_order.x_hex >= 0 && work_order.y_hex >= 0 &&
	    work_order.x_hex < world_x && work_order.y_hex < world_y &&
	    (work_order.red_lizards > 0 || work_order.green_lizards > 0 ||
	    work_order.grey_lizards > 0 || work_order.black_lizards > 0 ||
	    work_order.yellow_lizards > 0))
	{
	  add_order (&work_order);
	} else {
	  send_player_mesg (present_player, " - 'SP %s' order is incorrectly written, or incomplete. Please refer to your Rulebook.\n",
	    work_string);
	}

	break;
      case 10:		/* Build Fertile Order */
      case 11:
	if (present_player == 0)
        {
          printf ("FATAL ERROR: Order issued when player number unknown. Press SPACE.\n");
	  exit (EXIT_FAILURE);
	}

	work_order.player = present_player;
	work_order.order_type = BUILD_FERTILE;

	strtoken (temp_string2, temp_string, " ");

	string_to_loc (temp_string2, &work_x, &work_y);

	work_order.x_hex = work_x;
	work_order.y_hex = work_y;

	get_hex (work_x, work_y, &world_hex);

	work_order.red_lizards = world_hex.red_lizards;
	work_order.green_lizards = world_hex.green_lizards;
	work_order.grey_lizards = world_hex.grey_lizards;
	work_order.black_lizards = world_hex.black_lizards;
	work_order.yellow_lizards = world_hex.yellow_lizards;

	work_order.moves [0] = 0;
	work_order.moves [1] = 0;

	if (work_order.player > 0 && work_order.x_hex >= 0 &&
	    work_order.y_hex >= 0 && work_order.x_hex < world_x &&
	    work_order.y_hex < world_y)
	{
	  add_order (&work_order);
	} else {
	  send_player_mesg (present_player, " - 'BF %s' order is incorrectly written, or incomplete. Please refer to your Rulebook.\n",
	    work_string);
	}

	break;
      case 12:		/* Build Den Order */
      case 13:
	if (present_player == 0)
        {
          printf ("FATAL ERROR: Order issued when player number unknown. Press SPACE.\n");
	  exit (EXIT_FAILURE);
	}

	work_order.player = present_player;
	work_order.order_type = BUILD_DEN;

        strtoken (temp_string2, temp_string, " ");

	string_to_loc (temp_string2, &work_x, &work_y);

	work_order.x_hex = work_x;
	work_order.y_hex = work_y;

	get_hex (work_x, work_y, &world_hex);

	work_order.red_lizards = world_hex.red_lizards;
	work_order.green_lizards = world_hex.green_lizards;
	work_order.grey_lizards = world_hex.grey_lizards;
	work_order.black_lizards = world_hex.black_lizards;
	work_order.yellow_lizards = world_hex.yellow_lizards;

	work_order.moves [0] = 0;
	work_order.moves [1] = 0;

	if (work_order.player > 0 && work_order.x_hex >= 0 &&
	    work_order.y_hex >= 0 && work_order.x_hex < world_x &&
	    work_order.y_hex < world_y)
	{
	  add_order (&work_order);
	} else {
	  send_player_mesg (present_player, " - 'BD %s' order is incorrectly written, or incomplete. Please refer to your Rulebook.\n",
	    work_string);
	}

	break;
      case 14:	 	/* Build Bridge Order */
      case 15:
	if (present_player == 0)
        {
          printf ("FATAL ERROR: Order issued when player number unknown. Press SPACE.\n");

	  getch ();

	  exit (EXIT_FAILURE);
	}

        work_order.player = present_player;
	work_order.order_type = BUILD_BRIDGE;

	strtoken (temp_string2, temp_string, " ");

	string_to_loc (temp_string2, &work_x, &work_y);

	work_order.x_hex = work_x;
	work_order.y_hex = work_y;

        get_hex (work_x, work_y, &world_hex);

	work_order.red_lizards = world_hex.red_lizards;
	work_order.green_lizards = world_hex.green_lizards;
	work_order.grey_lizards = world_hex.grey_lizards;
	work_order.black_lizards = world_hex.black_lizards;
	work_order.yellow_lizards = world_hex.yellow_lizards;

        work_order.moves [0] = 0;
	work_order.moves [1] = 0;

	work_order.moves [0] = string_to_direction (temp_string);

	if (work_order.player > 0 && work_order.x_hex >= 0 &&
	    work_order.y_hex >= 0 && work_order.x_hex < world_x &&
	    work_order.y_hex < world_y && work_order.moves [0] != 0)
	{
	  add_order (&work_order);
	} else {
	  send_player_mesg (present_player, " - 'BB %s' order is incorrectly written, or incomplete. Please refer to your Rulebook.\n",
	    work_string);
	}

	break;
      case 16:           /* Destroy Fertile Order */
      case 17:
	if (present_player == 0)
	{
          printf ("FATAL ERROR: Order issued when player number unknown. Press SPACE.\n");

          getch ();

	  exit (EXIT_FAILURE);
	}

        work_order.player = present_player;
	work_order.order_type = DESTROY_FERTILE;

        strtoken (temp_string2, temp_string, " ");

	string_to_loc (temp_string2, &work_x, &work_y);

	work_order.x_hex = work_x;
	work_order.y_hex = work_y;

        get_hex (work_x, work_y, &world_hex);

	work_order.red_lizards = world_hex.red_lizards;
	work_order.green_lizards = world_hex.green_lizards;
	work_order.grey_lizards = world_hex.grey_lizards;
	work_order.black_lizards = world_hex.black_lizards;
	work_order.yellow_lizards = world_hex.yellow_lizards;

	work_order.moves [0] = 0;
	work_order.moves [1] = 0;

	if (work_order.player > 0 && work_order.x_hex >= 0 &&
	    work_order.y_hex >= 0 && work_order.x_hex < world_x &&
	    work_order.y_hex < world_y)
	{
	  add_order (&work_order);
	} else {
	  send_player_mesg (present_player, " - 'DF %s' order is incorrectly written, or incomplete. Please refer to your Rulebook.\n",
	    work_string);
	}

	break;
      case 18:	        /* Destroy Bridge Order */
      case 19:
	if (present_player == 0)
        {
          printf ("FATAL ERROR: Order issued when player number unknown. Press SPACE.\n");

	  getch ();

	  exit (EXIT_FAILURE);
	}

	work_order.player = present_player;
	work_order.order_type = DESTROY_BRIDGE;

        strtoken (temp_string2, temp_string, " ");

	string_to_loc (temp_string2, &work_x, &work_y);

	work_order.x_hex = work_x;
	work_order.y_hex = work_y;

	get_hex (work_x, work_y, &world_hex);

	work_order.red_lizards = world_hex.red_lizards;
	work_order.green_lizards = world_hex.green_lizards;
	work_order.grey_lizards = world_hex.grey_lizards;
	work_order.black_lizards = world_hex.black_lizards;
	work_order.yellow_lizards = world_hex.yellow_lizards;

        work_order.moves [0] = 0;
	work_order.moves [1] = 0;

	work_order.moves [0] = string_to_direction (temp_string);

	if (work_order.player > 0 && work_order.x_hex >= 0 &&
	    work_order.y_hex >= 0 && work_order.x_hex < world_x &&
	    work_order.y_hex < world_y && work_order.moves [0] != 0)
	{
	  add_order (&work_order);
	} else {
	  send_player_mesg (present_player, " - 'DB %s' order is incorrectly written, or incomplete. Please refer to your Rulebook.\n",
	    work_string);
	}

	break;
      case 20:		/* Recruit Order */
      case 21:
	if (present_player == 0)
	{
          printf ("FATAL ERROR: Order issued when player number unknown. Press SPACE.\n");

          getch ();

	  exit (EXIT_FAILURE);
	}

        work_order.player = present_player;
	work_order.order_type = RECRUIT;

        strtoken (temp_string2, temp_string, " ");

	string_to_loc (temp_string2, &work_x, &work_y);

	work_order.x_hex = work_x;
	work_order.y_hex = work_y;

	work_order.red_lizards = 0;
	work_order.green_lizards = 0;
	work_order.grey_lizards = 0;
	work_order.black_lizards = 0;
	work_order.yellow_lizards = 0;

	work_order.moves [0] = 0;
	work_order.moves [1] = 0;

	if (work_order.player > 0 && work_order.x_hex >= 0 &&
	    work_order.y_hex >= 0 && work_order.x_hex < world_x &&
	    work_order.y_hex < world_y)
	{
	  add_order (&work_order);
	} else {
	  send_player_mesg (present_player, " - 'RE %s' order is incorrectly written, or incomplete. Please refer to your Rulebook.\n",
	    work_string);
	}

	break;
      case 22:          /* Chant Order */
      case 23:
	if (present_player == 0)
        {
          printf ("FATAL ERROR: Order issued when player number unknown. Press SPACE.\n");

          getch ();

	  exit (EXIT_FAILURE);
	}

	work_order.player = present_player;
	work_order.order_type = CHANT;

	strtoken (temp_string2, temp_string, " ");

	string_to_loc (temp_string2, &work_x, &work_y);

	work_order.x_hex = work_x;
	work_order.y_hex = work_y;

	get_hex (work_x, work_y, &world_hex);

	work_order.red_lizards = 0;
	work_order.green_lizards = 0;
	work_order.grey_lizards = 0;
	work_order.black_lizards = 0;
	work_order.yellow_lizards = 0;

        work_order.moves [0] = 0;
	work_order.moves [1] = 0;

	strtoken (temp_string2, temp_string, " ");

	if ((work_order.black_lizards = atoi (temp_string2)) == 0)
	  work_order.black_lizards = world_hex.black_lizards;
	else
	  strtoken (temp_string2, temp_string, " ");

	if (work_order.black_lizards > world_hex.black_lizards)
	  work_order.black_lizards = world_hex.black_lizards;

	for (work2 = 0; work2 < NUMBER_CHANTS; work2 ++)
	  if (strcmp (temp_string2, chant_code [work2]) == 0)
	  {
	    work2 ++;
	    break;
	  }

	if (work2 == (NUMBER_CHANTS + 1))
	  work_order.yellow_lizards = 0;
	else
	  work_order.yellow_lizards = work2;

	strtoken (temp_string2, temp_string, " ");

	if (work2 == 10)
	  if (temp_string [0] == '\0')
	    string_to_loc (temp_string2, &work_order.red_lizards,
			   &work_order.green_lizards);
	  else {
	    strcat (temp_string2, temp_string);

	    string_to_loc (temp_string2, &work_order.red_lizards,
			   &work_order.green_lizards);
	  }
	else {
	  work_order.moves [0] = string_to_direction (temp_string2);

	  work_order.moves [1] = string_to_direction (temp_string);
	}

	if (work_order.player > 0 && work_order.x_hex >= 0 &&
	    work_order.y_hex >= 0 && work_order.x_hex < world_x &&
	    work_order.y_hex < world_y && (work_order.moves [0] != 0 ||
	    work_order.yellow_lizards == 10) && work_order.yellow_lizards > 0)
	{
	  add_order (&work_order);
	} else {
	  send_player_mesg (present_player, " - 'CH %s' order is incorrectly written, or incomplete. Please refer to your Rulebook.\n",
	    work_string);
	}

	break;
      case 24:		/* System Message */
      case 25:
	strcpy (temp_string, work_string);

	work = 0;

	while (fgets (temp_string2, 140, in_fptr) != '\0')
	{
	  strcpy (mesg_string [work], temp_string2);

	  work ++;

	  if (strcmp (strupr (temp_string2), "ENDMESG\n") == 0)
	  {
	    work --;

	    break;
	  }

	  if (work == 40)
	    break;
	}

	/* send 'the other bit' to PLAYER 0 ONLY - given to the rest of the
	   players by MESSAPND.EXE later! */

	sprintf (filename, "%s%s00_%03d.%s", data_path, PLYPLY_MESG_F,
	  turn_number, game_code);

	if ((fptr = fopen (filename, "at")) == '\0')
	{
          printf ("FATAL ERROR: Unable to append to %s file. Press SPACE.\n",
		   filename);

          getch ();

	  exit (EXIT_FAILURE);
	}

	fprintf (fptr, " * The Other Bit:\n\n");

	for (work2 = 0; work2 < work; work2 ++)
	  fprintf (fptr, "   %s", mesg_string [work2]);

	fclose (fptr);

	break;
      case 26:		/* Give Order */
      case 27:
	if (present_player == 0)
	{
          printf ("FATAL ERROR: Order issued when player number unknown. Press SPACE.\n");

	  getch ();

	  exit (EXIT_FAILURE);
	}

	work_order.player = present_player;
	work_order.order_type = GIVE;

        strtoken (temp_string2, temp_string, " ");

	string_to_loc (temp_string2, &work_x, &work_y);

	work_order.x_hex = work_x;
	work_order.y_hex = work_y;

	work_order.red_lizards = 0;
	work_order.green_lizards = 0;
	work_order.grey_lizards = 0;
	work_order.black_lizards = 0;
	work_order.yellow_lizards = 0;

        work_order.moves [0] = 0;
	work_order.moves [1] = 0;

	work_order.moves [0] = atoi (temp_string);

	if (work_order.player > 0 && work_order.x_hex >= 0 &&
	    work_order.y_hex >= 0 && work_order.x_hex < world_x &&
	    work_order.y_hex < world_y && work_order.moves [0] != 0)
	{
	  add_order (&work_order);
	} else {
	  send_player_mesg (present_player, " - 'GI %s' order is incorrectly written, or incomplete. Please refer to your Rulebook.\n",
	    work_string);
	}

	break;
      case 28:		/* Lob Order */
      case 29:
	if (present_player == 0)
	{
	  printf ("FATAL ERROR: Order issued when player number unknown. Press SPACE.\n");

	  getch ();

	  exit (EXIT_FAILURE);
	}

	work_order.player = present_player;
	work_order.order_type = LOB;

        strtoken (temp_string2, temp_string, " ");

	string_to_loc (temp_string2, &work_x, &work_y);

	work_order.x_hex = work_x;
	work_order.y_hex = work_y;

	get_hex (work_x, work_y, &world_hex);

	work_order.red_lizards = 0;
	work_order.green_lizards = 0;
	work_order.grey_lizards = 0;
	work_order.black_lizards = 0;
	work_order.yellow_lizards = world_hex.yellow_lizards;

        work_order.moves [0] = 0;
	work_order.moves [1] = 0;

        strtoken (temp_string2, temp_string, " ");

	work_order.moves [0] = string_to_direction (temp_string2);

	work_order.moves [1] = string_to_direction (temp_string);

	if (work_order.player > 0 && work_order.x_hex >= 0 &&
	    work_order.y_hex >= 0 && work_order.x_hex < world_x &&
	    work_order.y_hex < world_y && work_order.moves [0] != 0)
	{
	  add_order (&work_order);
	} else {
	  send_player_mesg (present_player, " - 'LO %s' order is incorrectly written, or incomplete. Please refer to your Rulebook.\n",
	    work_string);
	}

	break;
      case 30:		/* Free Order */
      case 31:
	if (present_player == 0)
        {
          printf ("FATAL ERROR: Order issued when player number unknown. Press SPACE.\n");

          getch ();

	  exit (EXIT_FAILURE);
	}

	work_order.player = present_player;
	work_order.order_type = FREE;

	strtoken (temp_string2, temp_string, " ");

	string_to_loc (temp_string2, &work_x, &work_y);

	work_order.x_hex = work_x;
	work_order.y_hex = work_y;

	work_order.red_lizards = 0;
	work_order.green_lizards = 0;
	work_order.grey_lizards = 0;
	work_order.black_lizards = 0;
	work_order.yellow_lizards = 0;

	while (1)
	{
          strtoken (temp_string2, temp_string, " ");

	  work = atoi (temp_string2);

	  if (work == 0)
	    break;

	  strsplt (temp_string3, temp_string2, strlen (temp_string2) - 3);

	  for (work2 = 0; work2 < LIZARD_TYPES; work2 ++)
	    if (strcmp (temp_string3, lizard_colours [work2]) == 0)
	      break;

	  switch (work2)
	  {
	    case 0:
	      work_order.red_lizards = work;

	      break;
	    case 1:
	      work_order.green_lizards = work;

	      break;
	    case 2:
	      work_order.grey_lizards = work;

	      break;
	    case 3:
	      work_order.black_lizards = work;

	      break;
	    case 4:
	      work_order.yellow_lizards = work;

	      break;
	    default:
	      break;
	  }
	}

	work_order.moves [0] = 0;
	work_order.moves [1] = 0;

	if (work_order.player > 0 && work_order.x_hex >= 0 &&
	    work_order.y_hex >= 0 && work_order.x_hex < world_x &&
	    work_order.y_hex < world_y)
	{
	  add_order (&work_order);
	} else {
	  send_player_mesg (present_player, " - 'FR %s' order is incorrectly written, or incomplete. Please refer to your Rulebook.\n",
	    work_string);
	}

	break;
      case 32:		/* Build Raft Order */
      case 33:
	if (present_player == 0)
        {
          printf ("FATAL ERROR: Order issued when player number unknown. Press SPACE.\n");

          getch ();

	  exit (EXIT_FAILURE);
	}

	work_order.player = present_player;
	work_order.order_type = BUILD_RAFT;

	strtoken (temp_string2, temp_string, " ");

	string_to_loc (temp_string2, &work_x, &work_y);

	work_order.x_hex = work_x;
	work_order.y_hex = work_y;

	get_hex (work_x, work_y, &world_hex);

	work_order.red_lizards = world_hex.red_lizards;
	work_order.green_lizards = world_hex.green_lizards;
	work_order.grey_lizards = world_hex.grey_lizards;
	work_order.black_lizards = world_hex.black_lizards;
	work_order.yellow_lizards = world_hex.yellow_lizards;

        work_order.moves [0] = 0;
	work_order.moves [1] = 0;

        strtoken (temp_string2, temp_string, " ");

	work_order.moves [0] = string_to_direction (temp_string2);

	if (work_order.player > 0 && work_order.x_hex >= 0 &&
	    work_order.y_hex >= 0 && work_order.x_hex < world_x &&
	    work_order.y_hex < world_y && work_order.moves [0] != 0)
	{
	  add_order (&work_order);
	} else {
	  send_player_mesg (present_player, " - 'BR %s' order is incorrectly written, or incomplete. Please refer to your Rulebook.\n",
	    work_string);
	}

	break;
      case 34:		/* Sail Order */
      case 35:
	if (present_player == 0)
        {
	  printf ("FATAL ERROR: Order issued when player number unknown. Press SPACE.\n");

          getch ();

	  exit (EXIT_FAILURE);
	}

	work_order.player = present_player;
	work_order.order_type = SAIL;

        strtoken (temp_string2, temp_string, " ");

	string_to_loc (temp_string2, &work_x, &work_y);

	work_order.x_hex = work_x;
	work_order.y_hex = work_y;

	get_hex (work_x, work_y, &world_hex);

	work_order.red_lizards = world_hex.red_lizards;
	work_order.green_lizards = world_hex.green_lizards;;
	work_order.grey_lizards = world_hex.grey_lizards;;
	work_order.black_lizards = world_hex.black_lizards;;
	work_order.yellow_lizards = world_hex.yellow_lizards;;

	work_order.moves [0] = 0;
	work_order.moves [1] = 0;

        strtoken (temp_string2, temp_string, " ");

	work_order.moves [0] = string_to_direction (temp_string2);

	work_order.moves [1] = string_to_direction (temp_string);

	if (work_order.player > 0 && work_order.moves [0] > 0 &&
	    work_order.x_hex >= 0 && work_order.y_hex >= 0 &&
	    work_order.x_hex < world_x && work_order.y_hex < world_y)
	{
	  add_order (&work_order);
	} else {
	  send_player_mesg (present_player, " - 'SA %s' order is incorrectly written, or incomplete. Please refer to your Rulebook.\n",
	    work_string);
	}

	break;
      case 36:		/* Kill Player Order */
      case 37:
	if (present_player == 0)
        {
          printf ("FATAL ERROR: Order issued when player number unknown. Press SPACE.\n");

	  getch ();

	  exit (EXIT_FAILURE);
	}

	work_order.player = present_player;
	work_order.order_type = KILL_PLAYER;

	work_order.x_hex = 0;
	work_order.y_hex = 0;

	work_order.red_lizards = 0;
	work_order.green_lizards = 0;
	work_order.grey_lizards = 0;
	work_order.black_lizards = 0;
	work_order.yellow_lizards = 0;

	work_order.moves [0] = 0;
	work_order.moves [1] = 0;

	add_order (&work_order);

	break;
      case 38:		/* Sneak Spy Order */
      case 39:
	if (present_player == 0)
	{
          printf ("FATAL ERROR: Order issued when player number unknown. Press SPACE.\n");

	  getch ();

	  exit (EXIT_FAILURE);
	}

        work_order.player = present_player;
	work_order.order_type = SNEAK;

        strtoken (temp_string2, temp_string, " ");

	string_to_loc (temp_string2, &work_x, &work_y);

	work_order.x_hex = work_x;
	work_order.y_hex = work_y;

	get_hex (work_x, work_y, &world_hex);

	if (world_hex.hex_owner == present_player)
	{
	  work_order.red_lizards = world_hex.red_lizards;
	  work_order.green_lizards = world_hex.green_lizards;
	  work_order.grey_lizards = world_hex.grey_lizards;
	  work_order.black_lizards = world_hex.black_lizards;
	  work_order.yellow_lizards = world_hex.yellow_lizards;
	} else {
	  work_order.red_lizards = 0;
	  work_order.green_lizards = 0;
	  work_order.grey_lizards = 0;
	  work_order.black_lizards = 0;
	  work_order.yellow_lizards = 0;
	}

	work_order.moves [0] = 0;
	work_order.moves [1] = 0;

        strtoken (temp_string2, temp_string, " ");

	work_order.moves [0] = string_to_direction (temp_string2);

	work_order.moves [1] = string_to_direction (temp_string);

	if (work_order.player > 0 && work_order.moves [0] > 0 &&
	    work_order.x_hex >= 0 && work_order.y_hex >= 0 &&
	    work_order.x_hex < world_x && work_order.y_hex < world_y)
	{
	  add_order (&work_order);
	} else {
	  send_player_mesg (present_player, " - 'SN %s' order is incorrectly written, or incomplete. Please refer to your Rulebook.\n",
	    work_string);
	}

	break;
      case 40:		/* Erupt Volcano Order */
      case 41:
	work_order.player = 0;
	work_order.order_type = ERUPT_VOLCANO;

	strtoken (temp_string2, temp_string, " ");

	string_to_loc (temp_string2, &work_x, &work_y);

	work_order.x_hex = work_x;
	work_order.y_hex = work_y;

	get_hex (work_x, work_y, &world_hex);

	if (work_order.x_hex >= 0 && work_order.y_hex >= 0 &&
	    work_order.x_hex < world_x && work_order.y_hex < world_y &&
	    world_hex.terrain == VOLCANO)
	{
	  add_order (&work_order);
	}

	break;
      case 42:
      case 43:  	/* Spawn Turn Order */
	sprintf (filename, "%s%s%s", data_path, WORLD_SWITCH_F, game_code);

	if ((temp_fptr = fopen (filename, "wt")) == '\0')
	{
	  printf ("FATAL ERROR: Unable to write to %s file. Press SPACE.\n", filename);

	  getch ();

	  exit (EXIT_FAILURE);
	}

	fprintf (temp_fptr, "SPAWN\n");

	fclose (temp_fptr);

	break;
      default:
	/* screwed order parse! */

	token_table = '\0';
	screwed [0] = '\0';

	token_table = make_token_table (token_table, temp_string, " ");

	if (strcmp (token_table [0], "?") != 0) /* order */
	{
	  strcat (screwed, token_table [0]);
	  strcat (screwed, " ");
	}

	if (strcmp (token_table [1], "?") != 0) /* hex */
	{
	  strcat (screwed, token_table [1]);
	  strcat (screwed, " ");
	}

	if (strcmp (token_table [4], "?") != 0) /* red liz */
	{
	  strcat (screwed, token_table [4]);
	  strcat (screwed, "RED ");
	}

	if (strcmp (token_table [5], "?") != 0) /* grn liz */
	{
	  strcat (screwed, token_table [5]);
	  strcat (screwed, "GRN ");
	}

	if (strcmp (token_table [6], "?") != 0) /* gry liz */
	{
	  strcat (screwed, token_table [6]);
	  strcat (screwed, "GRY ");
	}

	if (strcmp (token_table [7], "?") != 0) /* yel liz */
	{
	  strcat (screwed, token_table [7]);
	  strcat (screwed, "YEL ");
	}

	if (strcmp (token_table [8], "?") != 0) /* blk liz */
	{
	  strcat (screwed, token_table [8]);
	  strcat (screwed, "BLK ");
	}

	if (strcmp (token_table [9], "?") != 0) /* extra */
	{
	  strcat (screwed, token_table [0]);
	  strcat (screwed, " ");
	}

	if (strcmp (token_table [2], "?") != 0) /* m1 */
	{
	  strcat (screwed, token_table [0]);
	  strcat (screwed, " ");
	}

	if (strcmp (token_table [3], "?") != 0) /* m2 */
	{
	  strcat (screwed, token_table [0]);
	  strcat (screwed, " ");
	}

	send_player_mesg (present_player, " - '%s' order is incorrectly written, or incomplete. Please refer to your Rulebook.\n",
	  screwed);

	free_malloced_table (token_table);

	break;
    }
  }

  fclose (in_fptr);

  printf ("Saving Processed Orders...\n\n");

  sprintf (filename, "%s%s%03d.%s", data_path, ORDS_PROC_F, turn_number,
    game_code);

  if ((bin_fptr = fopen (filename, "wb")) == '\0')
  {
    printf ("FATAL ERROR: Unable to write to %s file. Press SPACE.\n", filename);

    getch ();

    exit (EXIT_FAILURE);
  }

  for (work = 0; work < number_of_orders; work ++)
  {
    put_rec (bin_fptr, sizeof (order_t), work, order_list [work]);

    free (order_list [work]);
  }

  fclose (bin_fptr);

  for (loop = 1; loop < players_in_game; loop ++)
    if (no_move_returned_flags [loop] && turn_number > 0)
      send_player_mesg (loop, " - SORRY, BUT THE MODERATOR DID NOT RECEIVE ORDERS FROM YOU FOR THIS TURN REPORT.\n"
	" - PLEASE POST YOUR ORDERS FOR THIS TURN EARLY, TO ENSURE THIS DOESN'T HAPPEN AGAIN.\n");

  sprintf (filename, "%s%s%03d.%s", data_path, TEXT_ORDS_F, turn_number,
    game_code);

  remove (filename); /* remove order file now we've finished with it */
  printf ("Finished!\n");
  return (EXIT_SUCCESS);
}

void add_order (order_t *order)
{
  char hex_loc [20],
       string [256];

  int work,
      split_total,
      sneak_total;

  hex_t hex;

  switch (order->order_type)
  {
    case SPLIT:
      for (work = 0, split_total = 0; work < number_of_orders; work ++)
	if (order_list [work]->order_type == SPLIT &&
	    order->player == order_list [work]->player &&
	    order->x_hex == order_list [work]->x_hex &&
	    order->y_hex == order_list [work]->y_hex)
	{
	  split_total += order_list [work]->red_lizards +
			 order_list [work]->green_lizards +
			 order_list [work]->grey_lizards +
			 order_list [work]->black_lizards +
			 order_list [work]->yellow_lizards;
	}

	get_hex (order->x_hex, order->y_hex, &hex);

	if (split_total >= lizards_in_hex (&hex))
	{
	  /* Redundant order ignored! */

	  xy_to_string (order->x_hex, order->y_hex, hex_loc);

	  if (order->moves [1] != 0)
	    send_player_mesg (order->player,
	      " - SPLIT %s %s %s %s ignored, not enough lizards starting in hex.\n",
	      hex_loc, dir [order->moves [0]], dir [order->moves [1]],
	      show_lizards (order->red_lizards, order->green_lizards,
	      order->grey_lizards, order->black_lizards,
	      order->yellow_lizards, string));
	  else
	    send_player_mesg (order->player,
	      " - SPLIT %s %s %s ignored, not enough lizards starting in hex.\n",
	      hex_loc, dir [order->moves [0]], show_lizards (order->red_lizards,
	      order->green_lizards, order->grey_lizards, order->black_lizards,
	      order->yellow_lizards, string));

	  return;
	}

      break;
    case MOVE:
      for (work = 0; work < number_of_orders; work ++)
	if (order_list [work]->order_type == MOVE &&
	    order->player == order_list [work]->player &&
	    order->x_hex == order_list [work]->x_hex &&
	    order->y_hex == order_list [work]->y_hex &&
	    order->red_lizards == order_list [work]->red_lizards &&
	    order->green_lizards == order_list [work]->green_lizards &&
	    order->grey_lizards == order_list [work]->grey_lizards &&
	    order->black_lizards == order_list [work]->black_lizards &&
	    order->yellow_lizards == order_list [work]->yellow_lizards)
	{
	  /* Redundant order ignored! */

	  xy_to_string (order->x_hex, order->y_hex, hex_loc);

	  if (order->moves [1] != 0)
	    send_player_mesg (order->player,
	      " - MOVE %s %s %s %s ignored, not enough lizards starting in hex.\n",
	      hex_loc, dir [order->moves [0]], dir [order->moves [1]],
	      show_lizards (order->red_lizards, order->green_lizards,
	      order->grey_lizards, order->black_lizards,
	      order->yellow_lizards, string));
	  else
	    send_player_mesg (order->player,
	      " - MOVE %s %s %s ignored, not enough lizards starting in hex.\n",
	      hex_loc, dir [order->moves [0]], show_lizards (order->red_lizards,
	      order->green_lizards, order->grey_lizards, order->black_lizards,
	      order->yellow_lizards, string));

	  return;
	}

      break;
    case SNEAK:
      for (work = 0, sneak_total = 0; work < number_of_orders; work ++)
	if (order_list [work]->order_type == SNEAK &&
	    order->player == order_list [work]->player &&
	    order->x_hex == order_list [work]->x_hex &&
	    order->y_hex == order_list [work]->y_hex)
	{
	  sneak_total ++;
	}

	get_hex (order->x_hex, order->y_hex, &hex);

	if (sneak_total >= spy_lizards_in_hex (order->x_hex, order->y_hex, order->player))
	{
	  /* Redundant order ignored! */

	  xy_to_string (order->x_hex, order->y_hex, hex_loc);

	  if (order->moves [1] != 0)
	    send_player_mesg (order->player,
	      " - SNEAK %s %s %s ignored, not enough lizards starting in hex.\n",
	      hex_loc, dir [order->moves [0]], dir [order->moves [1]]);
	  else
	    send_player_mesg (order->player,
	      " - SNEAK %s %s ignored, not enough lizards starting in hex.\n",
	      hex_loc, dir [order->moves [0]]);

	  return;
	}

      break;
    case CHANT:
      for (work = 0; work < number_of_orders; work ++)
	if (order_list [work]->order_type == CHANT &&
	    order->player == order_list [work]->player &&
	    order->x_hex == order_list [work]->x_hex &&
	    order->y_hex == order_list [work]->y_hex)
      {
	/* Redundant order ignored! */

	xy_to_string (order->x_hex, order->y_hex, hex_loc);

	if (order->moves [1] != 0)
	  send_player_mesg (order->player,
	    " - CHANT %s %s %s %s ignored, only one chant allowed from hex.\n",
	    hex_loc, dir [order->moves [0]], dir [order->moves [1]],
	    chant_code [order->yellow_lizards]);
	else
	  send_player_mesg (order->player,
	    " - CHANT %s %s %s ignored, only one chant allowed from hex.\n",
	    hex_loc, dir [order->moves [0]],
	    chant_code [order->yellow_lizards]);

	return;
      }

      break;
    case LOB:
      for (work = 0; work < number_of_orders; work ++)
	if (order_list [work]->order_type == LOB &&
	    order->player == order_list [work]->player &&
	    order->x_hex == order_list [work]->x_hex &&
	    order->y_hex == order_list [work]->y_hex)
      {
	/* Redundant order ignored! */

	xy_to_string (order->x_hex, order->y_hex, hex_loc);

	if (order->moves [1] != 0)
	  send_player_mesg (order->player,
	    " - LOB %s %s %s ignored, only one lob allowed from hex.\n",
	    hex_loc, dir [order->moves [0]], dir [order->moves [1]]);
	else
	  send_player_mesg (order->player,
	    " - LOB %s %s ignored, only one lob allowed from hex.\n",
	    hex_loc, dir [order->moves [0]]);

	return;
      }

      break;
    case BUILD_FERTILE:
      for (work = 0; work < number_of_orders; work ++)
	if (order_list [work]->order_type == BUILD_FERTILE &&
	    order->player == order_list [work]->player &&
	    order->x_hex == order_list [work]->x_hex &&
	    order->y_hex == order_list [work]->y_hex)
      {
	/* Redundant order ignored! */

	xy_to_string (order->x_hex, order->y_hex, hex_loc);

	send_player_mesg (order->player,
	  " - BUILD FERTILE %s ignored, only one fertile build allowed from hex.\n",
	  hex_loc);

	return;
      }

      break;
    case BUILD_DEN:
      for (work = 0; work < number_of_orders; work ++)
	if (order_list [work]->order_type == BUILD_DEN &&
	    order->player == order_list [work]->player &&
	    order->x_hex == order_list [work]->x_hex &&
	    order->y_hex == order_list [work]->y_hex)
      {
	/* Redundant order ignored! */

	xy_to_string (order->x_hex, order->y_hex, hex_loc);

	send_player_mesg (order->player,
	  " - BUILD DEN %s ignored, only one den build allowed from hex.\n",
	  hex_loc);

	return;
      }

      break;
    case BUILD_BRIDGE:
      for (work = 0; work < number_of_orders; work ++)
	if (order_list [work]->order_type == BUILD_BRIDGE &&
	    order->player == order_list [work]->player &&
	    order->x_hex == order_list [work]->x_hex &&
	    order->y_hex == order_list [work]->y_hex)
      {
	/* Redundant order ignored! */

	xy_to_string (order->x_hex, order->y_hex, hex_loc);

	send_player_mesg (order->player,
	  " - BUILD BRIDGE %s %s ignored, only one bridge build allowed from hex.\n",
	  hex_loc, order->moves [0]);

	return;
      }

      break;
    case DESTROY_FERTILE:
      for (work = 0; work < number_of_orders; work ++)
	if (order_list [work]->order_type == DESTROY_FERTILE &&
	    order->player == order_list [work]->player &&
	    order->x_hex == order_list [work]->x_hex &&
	    order->y_hex == order_list [work]->y_hex)
      {
	/* Redundant order ignored! */

	xy_to_string (order->x_hex, order->y_hex, hex_loc);

	send_player_mesg (order->player,
	  " - DESTROY FERTILE %s ignored, only one fertile destroy allowed from hex.\n",
	  hex_loc);

	return;
      }

      break;
    case DESTROY_BRIDGE:
      for (work = 0; work < number_of_orders; work ++)
	if (order_list [work]->order_type == DESTROY_BRIDGE &&
	    order->player == order_list [work]->player &&
	    order->x_hex == order_list [work]->x_hex &&
	    order->y_hex == order_list [work]->y_hex)
      {
	/* Redundant order ignored! */

	xy_to_string (order->x_hex, order->y_hex, hex_loc);

	send_player_mesg (order->player,
	  " - DESTROY BRIDGE %s %s ignored, only one bridge destroy allowed from hex.\n",
	  hex_loc, order->moves [0]);

	return;
      }

      break;
  }

  if ((order_list [number_of_orders] = malloc (sizeof (order_t))) == '\0')
  {
    printf ("FATAL ERROR: Out of memory. Press SPACE.\n");

    getch ();

    exit (EXIT_FAILURE);
  }

  *(order_list [number_of_orders]) = *order;

  number_of_orders ++;

  if (number_of_orders >= MAX_ORDERS_PER_TURN)
  {
    printf ("FATAL ERROR: Exceeded Maximum of %d orders per turn. Press SPACE.\n",
	     MAX_ORDERS_PER_TURN);

    getch ();

    exit (EXIT_FAILURE);
  }
}
