#define ORDER_ORIGIN_FIELD 2
#define PLAYER_NUMBER_FIELD 3
#define ORDER_FORM_START_FIELD 9
#define MESSAGE_HEADER_FIELD 309
#define MESSAGE_LINE1_FIELD 310
#define MESSAGE_LINE2_FIELD 311

#define NO_ORDERS 0
#define PAPER_ORDERS 1  /* orders entered from paper */
#define BBS_ORDERS 2    /* orders entered from bbs */
#define DISK_ORDERS 3	/* orders entered from disk */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lizard.h"
#include "order.h"
#include "template.h"

#include "strgfunc.h"
#include "tempfunc.h"

#include "lizgame.h"
#include "lizhex.h"
#include "lizmisc.h"

char player_orders_origin [MAX_PLAYERS + 1];

char chant_code [NUMBER_CHANTS][5] = { "EYES", "REVE", "SHUN", "MOUL",
				       "COER", "TRAN", "DARK", "FREE",
				       "SUMM", "GATE" };

int main (int argc, char **argv);
void input_order_form (char *filename, int input_player,
		       field_t **order_template,
		       char **order_data [MAX_PLAYERS + 1],
		       field_t **other_bit_template,
		       char **other_bit);
void order_string_into_template (char *order, char *string, int *order_num,
                                 int player, FILE *fptr,
				 field_t **order_template,
				 char **order_data [MAX_PLAYERS + 1],
                                 field_t **other_bit_template,
				 char **other_bit);
void output_order_form (field_t **template, char **data, int *kill_players, FILE *fptr);
int order_num_from_string (char *string);
int order_ditto (unsigned int *field, field_t **template, char **data);
int check_order_format (unsigned int *field, field_t **template, char **data);
int order_skip_to_field (unsigned int *field, field_t **template, char **data);
void show_order_form (void);
void show_other_bit_form (void);

int main (int argc, char *argv[])
{
  FILE *fptr;

  char string [256];

  int kill_players [MAX_PLAYERS + 1];

  unsigned int player = 1,
	       loop,
	       option,
	       field_in,
	       start_field,
	       order_template_fields = 0,
	       other_bit_template_fields = 0;

  field_t **order_template,
	  **other_bit_template;

  char **other_bit,
       **player_orders [MAX_PLAYERS + 1];

  printf ("LIZARDS! Order Entry Utility. %s Last compiled %s.\n\n",
	  VER_STRING, __DATE__);

  if (argc == 6)
  {
    data_path = LzCheckDir(argv[1], 0);
    game_code = (char*)malloc(strlen(argv[2] + 1));
    strcpy (game_code, argv [2]);
    strupr (game_code);
  } else {
    printf ("FATAL ERROR: Turn and Game Number not specified on command line!\n"
	     "             Command Line Format: EXEFILE data_path disk_data_path email_data_path turn game.\n"
	     "                             e.g. ORDERENT C:\\ A:\\ E:\\ 1 A\n");

    exit (EXIT_FAILURE);
  }

  world = (World*)malloc(sizeof(World));
  get_world(world);
  if (world->num_players == 0)
  {
    printf ("FATAL ERROR: Game %s has no players.\n", game_code);
    exit (EXIT_FAILURE);
  }

  order_template = load_template ("ORDFORM.TEM", &order_template_fields);
  other_bit_template = load_template ("OTHFORM.TEM", &other_bit_template_fields);

  for (player = 0; player < MAX_PLAYERS + 1; player ++)
    player_orders_origin [player] = NO_ORDERS;
  
  for (player = 1; player < world->num_players + 1; player ++)
    {
    if ((player_orders [player] = calloc (order_template_fields + 1,
					  sizeof (char *))) == '\0')
      {
      printf ("FATAL ERROR: Out of memory for calloc.\n");
      exit (EXIT_FAILURE);
    }

    set_field (order_template, player_orders [player], 0, game_code);
    set_field (order_template, player_orders [player], 1, argv [4]);
    set_field (order_template, player_orders [player], 3, itoa (player, string, 10));
    set_field (order_template, player_orders [player], 4, world->player[player].code);
    set_field (order_template, player_orders [player], 6, world->player[player].clan_name);
    set_field (order_template, player_orders [player], 7, world->player[player].code);
  }

  if ((other_bit = calloc (other_bit_template_fields + 1,
    sizeof (char *))) == '\0')
  {
    printf ("FATAL ERROR: Out of memory for calloc.\n");

    

    exit (EXIT_FAILURE);
  }

  /* check if there's an already existing (partial?) set of orders. If so,
    load into existing tables to allow editing */

  sprintf (string, "%s%s%03d.%s", data_path, TEXT_ORDS_F, world->turn,
    game_code);

  input_order_form (string, 0, order_template, player_orders,
    other_bit_template, other_bit);

  player = 1;

  start_field = ORDER_FORM_START_FIELD;

  while (player != 0)
  {
    for (loop = player; loop < world->num_players + 1 &&
      strcmp (world->player[loop].code, "****") == 0; loop ++);

    if (loop != world->num_players + 1)
    {
      player = loop;
    }

    show_order_form ();

    show_template (order_template, player_orders [player]);

    if (player_orders [player][ORDER_ORIGIN_FIELD] != '\0')
    switch (enter_template (order_template, player_orders [player], start_field,
      &field_in, check_order_format, order_ditto, order_skip_to_field))
    {
      case ERASE_KEY:
	clear_error ();

	show_error ("SELECT: B=LOAD BBS, D=LOAD DISK, E=ERASE? ");

	option = toupper (getch ());

	switch (option)
	{
	  case 'B':
	    /* look for bbs order files in email directory */

	    clear_error ();

	    /* look for BBS order files in directory */

	    sprintf (string, "%sDOR%02d%03d.%s", email_data_path, player,
	      world.turn, game_code);

	    if ((fptr = fopen (string, "rt")) != '\0')
	    {
	      fclose (fptr);

	      show_error ("SURE YOU WANT TO LOAD BBS ORDERS (Y/N)?");

	      option = toupper (getch ());

	      if (option == 'Y')
	      {
		/* loading bbs orders for player */

		input_order_form (string, player, order_template,
		  player_orders, other_bit_template, other_bit);

		player_orders_origin [player] = BBS_ORDERS;
		set_field (order_template, player_orders [player], ORDER_ORIGIN_FIELD, "BBS");
	      } else {
		show_error ("NO BBS ORDERS LOADED FOR PLAYER.");
	      }
	    } else {
	      sprintf (string, "NO BBS ORDERS FOUND FOR PLAYER IN %s, PRESS SPACE.",
		email_data_path);

	      show_error (string);

	      
	    }

	    break;
	  case 'D':
	    clear_error ();

	    sprintf (string, "Insert disk for %s, Player %02d, Game %s into %s.",
	      player_name [player], player, game_code, disk_data_path);

	    show_error (string);

	    

	    /* look for disk order files in drive */

	    sprintf (string, "%sDOR%02d%03d.%s", disk_data_path, player,
	      world.turn, game_code);

	    if ((fptr = fopen (string, "rt")) != '\0')
	    {
	      fclose (fptr);

	      show_error ("SURE YOU WANT TO LOAD DISK ORDERS (Y/N)?");

	      option = toupper (getch ());

	      if (option == 'Y')
	      {
		/* loading disk orders for player */

		input_order_form (string, player, order_template,
		  player_orders, other_bit_template, other_bit);

		player_orders_origin [player] = DISK_ORDERS;
		set_field (order_template, player_orders [player], ORDER_ORIGIN_FIELD, "DISK");
	      } else {
		show_error ("NO DISK ORDERS LOADED FOR PLAYER.");
	      }
	    } else {
	      sprintf (string, "NO DISK ORDERS FOUND FOR PLAYER ON DISK IN %s, PRESS SPACE.",
		disk_data_path);

	      show_error (string);

	      
	    }

	    break;
	  case 'E':
	    clear_error ();

	    show_error ("SURE YOU WANT TO ERASE FORM (Y/N)? ");

	    option = toupper (getch ());

	    if (option == 'Y')
	    {
	      clear_error ();

	      free_data (order_template, player_orders [player]);

	      if ((player_orders [player] = calloc (order_template_fields + 1,
		sizeof (char *))) == '\0')
	      {
		printf ("FATAL ERROR: Out of memory for calloc.\n");

		

		exit (EXIT_FAILURE);
	      }

	      set_field (order_template, player_orders [player], 0, game_code);
	      set_field (order_template, player_orders [player], 1, argv [4]);
	      set_field (order_template, player_orders [player], 3, itoa (player, string, 10));
	      set_field (order_template, player_orders [player], 4, player_name [player]);

	      switch (player_output_mode [player])
	      {
		case 'D':
		  set_field (order_template, player_orders [player], 5, "DISK");
		  break;
		case 'B':
		  set_field (order_template, player_orders [player], 5, "BBS");
		  break;
		default:
		  set_field (order_template, player_orders [player], 5, "PAPER");
		  break;
	      }

	      set_field (order_template, player_orders [player], 6, player[player].clan_name);
	      set_field (order_template, player_orders [player], 7, player[player].code);
	      set_field (order_template, player_orders [player], 8, player_cred [player]);
	    }

	    break;
	}

	break;
      case FINISH_KEY:
	clear_error ();

	show_error ("SURE YOU HAVE COMPLETED ORDER ENTRY (Y/N)? ");

	option = toupper (getch ());

        if (option == 'Y')
        {
          clear_error ();
	  player = 0;
        }

	break;
      case ESCAPE_KEY:
	clear_error ();

	show_error ("SURE YOU WANT TO EXIT ORDER ENTRY UTILITY (Y/N)? ");

        option = toupper (getch ());

        if (option == 'Y')
	  {
	    NcursesExit();
	    exit(EXIT_FAILURE);
	  }
	break;
      case UP_FORM_KEY:
        start_field = ORDER_FORM_START_FIELD;

        for (loop = player - 1; loop > 0 &&
          strcmp (player_name [loop], "****") == 0; loop --);

	if (loop != 0)
          player = loop;

        break;
      case DOWN_FORM_KEY:
	if (field_in < MESSAGE_HEADER_FIELD)
	{
	  start_field = MESSAGE_HEADER_FIELD;
          break;
        } else
          start_field = ORDER_FORM_START_FIELD;
      default:
        for (loop = player + 1; loop < players_in_game + 1 &&
	  strcmp (player_name [loop], "****") == 0; loop ++);

	if (loop != players_in_game + 1)
	{
	  player = loop;
	}

	break;
    }
  }

  show_other_bit_form ();

  show_template (other_bit_template, other_bit);

  loop = LZ_TRUE;

  while (loop)
  {
    switch (enter_template (other_bit_template, other_bit, 0, &field_in, '\0',
      '\0', '\0'))
    {
      case FINISH_KEY:
        loop = LZ_FALSE;
        break;
      case ERASE_KEY:
	free_data (other_bit_template, other_bit);

	if ((other_bit = calloc (other_bit_template_fields + 1,
	  sizeof (char *))) == '\0')
	{
	  printf ("FATAL ERROR: Out of memory for calloc.\n");

	  

	  exit (EXIT_FAILURE);
	}

	break;
    }
  }

  for (loop = 0; loop < MAX_PLAYERS + 1; loop ++)
    kill_players [loop] = 0;

  for (loop = 0; loop < 10; loop ++)
    if (other_bit [loop] != '\0')
      kill_players [loop] = atoi (other_bit [loop]);

  sprintf (string, "%s%s%03d.%s", data_path, TEXT_ORDS_F, world.turn,
    game_code);

  if ((fptr = fopen (string, "wt")) == '\0')
  {
    printf ("FATAL ERROR: Unable to write to %s file.\n", string);

    

    exit (EXIT_FAILURE);
  }

  for (player = 1; player < players_in_game + 1; player ++)
    output_order_form (order_template, player_orders [player], kill_players, fptr);

  for (loop = 10; loop < 16; loop ++)
    if (other_bit [loop] != '\0')
    {
      fprintf (fptr, "MMS\n");

      for (;loop < 16; loop ++)
	if (other_bit [loop] != '\0')
	  fprintf (fptr, "%s\n", other_bit [loop]);

      fprintf (fptr, "ENDMESG\n");

      break;
    }

  fclose (fptr);

  free_template (order_template);
  free_template (other_bit_template);

  clrscr ();

  printf ("Finished!\n");
  NcursesExit();
  return (EXIT_SUCCESS);
}

void input_order_form (char *filename, int input_player,
		       field_t **order_template,
		       char **order_data [MAX_PLAYERS + 1],
		       field_t **other_bit_template,
		       char **other_bit)
{
  FILE *fptr;

  char string [256],
       token [256],
       number [10];

  int player = 0,
      player_order_num = 0;

  if ((fptr = fopen (filename, "rt")) == '\0')
    return;

  while (fgets (string, 255, fptr) != '\0')
  {
    /* find the order */

    if (string [strlen (string) - 1] == '\n')
      string [strlen (string) - 1] = '\0';

    strupr (string);

    strtoken (token, string, " ");

    if (strcmp (token, "PLAYER") == 0)
    {
      /* new player number */

      player = atoi (string);
      player_order_num = 0;

      /* see if we can find player orders origin */

      strtoken (token, string, " ");

      switch (toupper (string [0]))
      {
	case 'B':
	  player_orders_origin [player] = BBS_ORDERS;
	  set_field (order_template, order_data [player], ORDER_ORIGIN_FIELD, "BBS");
	  break;
	case 'D':
	  player_orders_origin [player] = DISK_ORDERS;
	  set_field (order_template, order_data [player], ORDER_ORIGIN_FIELD, "DISK");
	  break;
	default:
	  player_orders_origin [player] = PAPER_ORDERS;
	  set_field (order_template, order_data [player], ORDER_ORIGIN_FIELD, "PAPER");
	  break;
      }
    } else
      if (input_player == 0 || (input_player != 0 && player == input_player))
	order_string_into_template (token, string, &player_order_num, player,
				    fptr, order_template, order_data,
				    other_bit_template, other_bit);
  }

  fclose (fptr);
}

void order_string_into_template (char *order, char *string, int *order_num,
				 int player, FILE *fptr,
				 field_t **order_template,
				 char **order_data [MAX_PLAYERS + 1],
				 field_t **other_bit_template,
				 char **other_bit)
{
  char token [256],
       number [10];

  int field_counter = 0;

  if (strcmp (order, "MS") == 0)
  {
    int line = 0;

    /* internal player message */

    set_field (order_template, order_data [player], MESSAGE_HEADER_FIELD, string); /* to players */

    if (fgets (string, 255, fptr) != '\0')
    {
      if (string [strlen (string) - 1] == '\n')
	string [strlen (string) - 1] = '\0';

      strupr (string);

      if (strcmp (string, "ENDMESG") == 0)
        return;

      if (strlen (string) >= 47)
      {
        set_field (order_template, order_data [player], MESSAGE_LINE1_FIELD, string);
	set_field (order_template, order_data [player], MESSAGE_LINE2_FIELD, &string [47]);
      } else
        set_field (order_template, order_data [player], MESSAGE_LINE1_FIELD, string);

      fgets (string, 255, fptr); /* to snatch 'ENDMESG' */
    }
  } else
    if (strcmp (order, "MMS") == 0)
    {
      int line = 0;

      /* other bits */

      while (fgets (string, 255, fptr) != '\0' && line < ORDER_FORM_START_FIELD)
      {
        if (string [strlen (string) - 1] == '\n')
          string [strlen (string) - 1] = '\0';

        strupr (string);

	if (strcmp (string, "ENDMESG") == 0) break;

	set_field (other_bit_template, other_bit, line + 10, string);

	line ++;
      }
    } else {
      /* normal order */

      if (player != 0 && (*order_num) < 30)
      {
	switch (order_num_from_string (order))
	{
	  case MOVE:
	    set_field (order_template, order_data [player],     /* move code */
	      ORDER_FORM_START_FIELD + ((*order_num) * 10), order);

	    set_field (order_template, order_data [player],     /* hex */
	      ORDER_FORM_START_FIELD + ((*order_num) * 10) + 1,
	      strtoken (order, string, " "));

            set_field (order_template, order_data [player],     /* move 1 */
              ORDER_FORM_START_FIELD + ((*order_num) * 10) + 2,
	      strtoken (order, string, " "));

            set_field (order_template, order_data [player],     /* move 2 */
	      ORDER_FORM_START_FIELD + ((*order_num) * 10) + 3,
	      strtoken (order, string, " "));

            break;
	  case SPLIT:
            set_field (order_template, order_data [player],     /* split code */
              ORDER_FORM_START_FIELD + ((*order_num) * 10), order);

            set_field (order_template, order_data [player],     /* hex */
              ORDER_FORM_START_FIELD + ((*order_num) * 10) + 1,
	      strtoken (order, string, " "));

            while (atoi (string) > 0)
            {
	      strtoken (order, string, " ");

              itoa (atoi (order), number, 10);

	      switch (order [strlen (order) - 1])
              {
		case 'D': /* red */
		  set_field (order_template, order_data [player],
                    ORDER_FORM_START_FIELD + ((*order_num) * 10) + 4, number);

                  break;
                case 'N': /* green */
                  set_field (order_template, order_data [player],
		    ORDER_FORM_START_FIELD + ((*order_num) * 10) + 5, number);

                  break;
                case 'Y': /* grey */
		  set_field (order_template, order_data [player],
                    ORDER_FORM_START_FIELD + ((*order_num) * 10) + 6, number);

		  break;
                case 'L': /* yellow */
                  set_field (order_template, order_data [player],
                    ORDER_FORM_START_FIELD + ((*order_num) * 10) + 7, number);

                  break;
		case 'K': /* black */
		  set_field (order_template, order_data [player],
                    ORDER_FORM_START_FIELD + ((*order_num) * 10) + 8, number);

                  break;
              }
            }

            set_field (order_template, order_data [player],     /* move 1 */
              ORDER_FORM_START_FIELD + ((*order_num) * 10) + 2,
	      strtoken (order, string, " "));

            set_field (order_template, order_data [player],     /* move 2 */
              ORDER_FORM_START_FIELD + ((*order_num) * 10) + 3,
	      strtoken (order, string, " "));

            break;
          case BUILD_FERTILE:
	    set_field (order_template, order_data [player],     /* build code */
              ORDER_FORM_START_FIELD + ((*order_num) * 10), order);

	    set_field (order_template, order_data [player],     /* hex */
              ORDER_FORM_START_FIELD + ((*order_num) * 10) + 1,
	      strtoken (order, string, " "));

            break;
          case BUILD_DEN:
	    set_field (order_template, order_data [player],     /* build code */
              ORDER_FORM_START_FIELD + ((*order_num) * 10), order);

            set_field (order_template, order_data [player],     /* hex */
	      ORDER_FORM_START_FIELD + ((*order_num) * 10) + 1,
	      strtoken (order, string, " "));

	    break;
          case BUILD_BRIDGE:
            set_field (order_template, order_data [player],     /* build code */
              ORDER_FORM_START_FIELD + ((*order_num) * 10), order);

            set_field (order_template, order_data [player],     /* hex */
	      ORDER_FORM_START_FIELD + ((*order_num) * 10) + 1,
	      strtoken (order, string, " "));

            set_field (order_template, order_data [player],     /* move 1 */
              ORDER_FORM_START_FIELD + ((*order_num) * 10) + 2,
	      strtoken (order, string, " "));

	    break;
	  case DESTROY_FERTILE:
            set_field (order_template, order_data [player],     /* destroy code */
              ORDER_FORM_START_FIELD + ((*order_num) * 10), order);

            set_field (order_template, order_data [player],     /* hex */
              ORDER_FORM_START_FIELD + ((*order_num) * 10) + 1,
	      strtoken (order, string, " "));

            break;
	  case DESTROY_BRIDGE:
	    set_field (order_template, order_data [player],     /* destroy code */
              ORDER_FORM_START_FIELD + ((*order_num) * 10), order);

	    set_field (order_template, order_data [player],     /* hex */
              ORDER_FORM_START_FIELD + ((*order_num) * 10) + 1,
	      strtoken (order, string, " "));

            set_field (order_template, order_data [player],     /* move 1 */
              ORDER_FORM_START_FIELD + ((*order_num) * 10) + 2,
	      strtoken (order, string, " "));

            break;
          case RECRUIT:
	    set_field (order_template, order_data [player],     /* recruit code */
              ORDER_FORM_START_FIELD + ((*order_num) * 10), order);

	    set_field (order_template, order_data [player],     /* hex */
              ORDER_FORM_START_FIELD + ((*order_num) * 10) + 1,
	      strtoken (order, string, " "));

	    break;
	  case CHANT:
	    set_field (order_template, order_data [player],     /* chant code */
	      ORDER_FORM_START_FIELD + ((*order_num) * 10), order);

            set_field (order_template, order_data [player],     /* hex */
              ORDER_FORM_START_FIELD + ((*order_num) * 10) + 1,
	      strtoken (order, string, " "));

	    if (atoi (string) > 0)
            {
	      strtoken (order, string, " ");

	      itoa (atoi (order), number, 10);

              set_field (order_template, order_data [player],
		ORDER_FORM_START_FIELD + ((*order_num) * 10) + 8, number);
            }

            set_field (order_template, order_data [player],     /* chant */
	      ORDER_FORM_START_FIELD + ((*order_num) * 10) + 9,
	      strtoken (order, string, " "));

	    set_field (order_template, order_data [player],     /* move 1 */
              ORDER_FORM_START_FIELD + ((*order_num) * 10) + 2,
	      strtoken (order, string, " "));

            set_field (order_template, order_data [player],     /* move 2 */
              ORDER_FORM_START_FIELD + ((*order_num) * 10) + 3,
	      strtoken (order, string, " "));

            break;
          case LOB:
	    set_field (order_template, order_data [player],     /* lob code */
              ORDER_FORM_START_FIELD + ((*order_num) * 10), order);

	    set_field (order_template, order_data [player],     /* hex */
              ORDER_FORM_START_FIELD + ((*order_num) * 10) + 1,
	      strtoken (order, string, " "));

	    set_field (order_template, order_data [player],     /* move 1 */
              ORDER_FORM_START_FIELD + ((*order_num) * 10) + 2,
	      strtoken (order, string, " "));

            set_field (order_template, order_data [player],     /* move 2 */
              ORDER_FORM_START_FIELD + ((*order_num) * 10) + 3,
	      strtoken (order, string, " "));

            break;
	  case GIVE:
            set_field (order_template, order_data [player],     /* give code */
              ORDER_FORM_START_FIELD + ((*order_num) * 10), order);

	    set_field (order_template, order_data [player],     /* hex */
              ORDER_FORM_START_FIELD + ((*order_num) * 10) + 1,
	      strtoken (order, string, " "));

            set_field (order_template, order_data [player],     /* extra field */
              ORDER_FORM_START_FIELD + ((*order_num) * 10) + 9,
	      strtoken (order, string, " "));

            break;
	  case FREE:
	    set_field (order_template, order_data [player],     /* free code */
              ORDER_FORM_START_FIELD + ((*order_num) * 10), order);

            set_field (order_template, order_data [player],     /* hex */
              ORDER_FORM_START_FIELD + ((*order_num) * 10) + 1,
	      strtoken (order, string, " "));

            while (atoi (string) > 0)
            {
	      strtoken (order, string, " ");

              itoa (atoi (order), number, 10);

	      switch (order [strlen (order) - 1])
              {
                case 'D': /* red */
                  set_field (order_template, order_data [player],
		    ORDER_FORM_START_FIELD + ((*order_num) * 10) + 4, number);

		  break;
		case 'N': /* green */
                  set_field (order_template, order_data [player],
                    ORDER_FORM_START_FIELD + ((*order_num) * 10) + 5, number);

                  break;
                case 'Y': /* grey */
		  set_field (order_template, order_data [player],
                    ORDER_FORM_START_FIELD + ((*order_num) * 10) + 6, number);

                  break;
		case 'L': /* yellow */
                  set_field (order_template, order_data [player],
                    ORDER_FORM_START_FIELD + ((*order_num) * 10) + 7, number);

                  break;
                case 'K': /* black */
                  set_field (order_template, order_data [player],
		    ORDER_FORM_START_FIELD + ((*order_num) * 10) + 8, number);

		  break;
	      }
            }

            break;
          case BUILD_RAFT:
            set_field (order_template, order_data [player],     /* build code */
	      ORDER_FORM_START_FIELD + ((*order_num) * 10), order);

            set_field (order_template, order_data [player],     /* hex */
              ORDER_FORM_START_FIELD + ((*order_num) * 10) + 1,
	      strtoken (order, string, " "));

            set_field (order_template, order_data [player],     /* move 1 */
	      ORDER_FORM_START_FIELD + ((*order_num) * 10) + 2,
	      strtoken (order, string, " "));

	    break;
	  case SAIL:
            set_field (order_template, order_data [player],     /* sail code */
	      ORDER_FORM_START_FIELD + ((*order_num) * 10), order);

            set_field (order_template, order_data [player],     /* hex */
              ORDER_FORM_START_FIELD + ((*order_num) * 10) + 1,
	      strtoken (order, string, " "));

            set_field (order_template, order_data [player],     /* move 1 */
	      ORDER_FORM_START_FIELD + ((*order_num) * 10) + 2,
	      strtoken (order, string, " "));

            set_field (order_template, order_data [player],     /* move 2 */
	      ORDER_FORM_START_FIELD + ((*order_num) * 10) + 3,
	      strtoken (order, string, " "));

	    break;
          case KILL_PLAYER:
            set_field (order_template, order_data [player],     /* kill code */
	      ORDER_FORM_START_FIELD + ((*order_num) * 10), order);

	    break;
	  case SNEAK:
	    set_field (order_template, order_data [player],     /* sneak code */
	      ORDER_FORM_START_FIELD + ((*order_num) * 10), order);

	    set_field (order_template, order_data [player],     /* hex */
	      ORDER_FORM_START_FIELD + ((*order_num) * 10) + 1,
	      strtoken (order, string, " "));

	    set_field (order_template, order_data [player],     /* move 1 */
	      ORDER_FORM_START_FIELD + ((*order_num) * 10) + 2,
	      strtoken (order, string, " "));

	    set_field (order_template, order_data [player],     /* move 2 */
	      ORDER_FORM_START_FIELD + ((*order_num) * 10) + 3,
	      strtoken (order, string, " "));

	    break;
	  case 99:
	    /* screwed order parse! */

	    field_counter = 0;

	    strtoken (order, string, " ");

	    while (order [0] != '\0')
	    {
	      if (strcmp (order, "?") != 0)
		set_field (order_template, order_data [player],
		  ORDER_FORM_START_FIELD + ((*order_num) * 10) +
		  field_counter, order);

	      field_counter ++;

	      strtoken (order, string, " ");
	    }

	    break;
	  default:
	    break;
	}

	(*order_num) ++;
      }
    }
}

void output_order_form (field_t **template, char **data, int *kill_players, FILE *fptr)
{
  unsigned int present_field = ORDER_FORM_START_FIELD,
	       loop,
	       loop_two,
	       player_orders_f = LZ_FALSE,
	       kill_player_f = LZ_FALSE;

  /* check to see if player actually has any orders */

  for (loop = 0, present_field = ORDER_FORM_START_FIELD; loop < 30; loop ++, present_field += 10)
    if (data [present_field] != '\0')
      player_orders_f = LZ_TRUE;

  if (!player_orders_f)
    if (data [MESSAGE_LINE1_FIELD] != '\0' || data [MESSAGE_LINE2_FIELD] != '\0')
      player_orders_f = LZ_TRUE;

  for (loop = 0; loop < MAX_PLAYERS + 1; loop ++)
    if (kill_players [loop] == atoi (data [PLAYER_NUMBER_FIELD]))
      player_orders_f = LZ_TRUE;

  if (!player_orders_f) return; /* no orders, no output! */

  fprintf (fptr, "PLAYER %s", data [PLAYER_NUMBER_FIELD]);

  if (data [ORDER_ORIGIN_FIELD] == '\0')
    fprintf (fptr, "\n");
  else
    fprintf (fptr, " %c\n", data [ORDER_ORIGIN_FIELD][0]);

  for (loop = 0, present_field = ORDER_FORM_START_FIELD; loop < 30; loop ++, present_field += 10)
    if (data [present_field] != '\0')
    {
      switch (order_num_from_string (data [present_field]))
      {
	case MOVE:
	  if (data [present_field + 1] != '\0' &&
	    (data [present_field + 2] != '\0' ||
	     data [present_field + 3] != '\0'))
	  {
	    fprintf (fptr, "MO %s ", data [present_field + 1]);

	    if (data [present_field + 2] != '\0')
	      fprintf (fptr, "%s ", (data [present_field + 2]));

	    if (data [present_field + 3] != '\0')
	      fprintf (fptr, "%s ", (data [present_field + 3]));

	    fprintf (fptr, "\n");
	  } else {
	    /* screwed order output */

	    fprintf (fptr, "?? ");

	    for (loop_two = 0; loop_two < 10; loop_two ++)
	      if (data [present_field + loop_two] != '\0')
		fprintf (fptr, "%s ", data [present_field + loop_two]);
	      else
		fprintf (fptr, "? ");

	    fprintf (fptr, "\n");
	  }

	  break;
	case SPLIT:
	  if (data [present_field + 1] != '\0' &&
	    (data [present_field + 2] != '\0' ||
	     data [present_field + 3] != '\0') &&
	    (data [present_field + 4] != '\0' ||
	     data [present_field + 5] != '\0' ||
	     data [present_field + 6] != '\0' ||
	     data [present_field + 7] != '\0' ||
	     data [present_field + 8] != '\0'))
	  {
	    fprintf (fptr, "SP %s ", data [present_field + 1]);

	    if (data [present_field + 4] != '\0')
	      fprintf (fptr, "%sRED ", (data [present_field + 4]));

	    if (data [present_field + 5] != '\0')
	      fprintf (fptr, "%sGRN ", (data [present_field + 5]));

	    if (data [present_field + 6] != '\0')
	      fprintf (fptr, "%sGRY ", (data [present_field + 6]));

	    if (data [present_field + 7] != '\0')
	      fprintf (fptr, "%sYEL ", (data [present_field + 7]));

	    if (data [present_field + 8] != '\0')
	      fprintf (fptr, "%sBLK ", (data [present_field + 8]));

	    if (data [present_field + 2] != '\0')
	      fprintf (fptr, "%s ", (data [present_field + 2]));

	    if (data [present_field + 3] != '\0')
	      fprintf (fptr, "%s ", (data [present_field + 3]));

	    fprintf (fptr, "\n");
	  } else {
	    /* screwed order output */

	    fprintf (fptr, "?? ");

	    for (loop_two = 0; loop_two < 10; loop_two ++)
	      if (data [present_field + loop_two] != '\0')
		fprintf (fptr, "%s ", data [present_field + loop_two]);
	      else
		fprintf (fptr, "? ");

	    fprintf (fptr, "\n");
	  }

	  break;
	case BUILD_FERTILE:
	  if (data [present_field + 1] != '\0')
	    fprintf (fptr, "BF %s\n", data [present_field + 1]);
	  else {
	    /* screwed order output */

	    fprintf (fptr, "?? ");

	    for (loop_two = 0; loop_two < 10; loop_two ++)
	      if (data [present_field + loop_two] != '\0')
		fprintf (fptr, "%s ", data [present_field + loop_two]);
	      else
		fprintf (fptr, "? ");

	    fprintf (fptr, "\n");
	  }

	  break;
	case BUILD_DEN:
	  if (data [present_field + 1] != '\0')
	    fprintf (fptr, "BD %s\n", data [present_field + 1]);
	  else {
	    /* screwed order output */

	    fprintf (fptr, "?? ");

	    for (loop_two = 0; loop_two < 10; loop_two ++)
	      if (data [present_field + loop_two] != '\0')
		fprintf (fptr, "%s ", data [present_field + loop_two]);
	      else
		fprintf (fptr, "? ");

	    fprintf (fptr, "\n");
	  }

	  break;
	case BUILD_BRIDGE:
	  if (data [present_field + 1] != '\0' &&
	    data [present_field + 2] != '\0')
	    fprintf (fptr, "BB %s %s\n", data [present_field + 1],
	      data [present_field + 2]);
	  else {
	    /* screwed order output */

	    fprintf (fptr, "?? ");

	    for (loop_two = 0; loop_two < 10; loop_two ++)
	      if (data [present_field + loop_two] != '\0')
		fprintf (fptr, "%s ", data [present_field + loop_two]);
	      else
		fprintf (fptr, "? ");

	    fprintf (fptr, "\n");
	  }

	  break;
	case DESTROY_FERTILE:
	  if (data [present_field + 1] != '\0')
	    fprintf (fptr, "DF %s\n", data [present_field + 1]);
	  else {
	    /* screwed order output */

	    fprintf (fptr, "?? ");

	    for (loop_two = 0; loop_two < 10; loop_two ++)
	      if (data [present_field + loop_two] != '\0')
		fprintf (fptr, "%s ", data [present_field + loop_two]);
	      else
		fprintf (fptr, "? ");

	    fprintf (fptr, "\n");
	  }

	  break;
	case DESTROY_BRIDGE:
	  if (data [present_field + 1] != '\0' &&
	    data [present_field + 2] != '\0')
	    fprintf (fptr, "DB %s %s\n", data [present_field + 1],
	      data [present_field + 2]);
	  else {
	    /* screwed order output */

	    fprintf (fptr, "?? ");

	    for (loop_two = 0; loop_two < 10; loop_two ++)
	      if (data [present_field + loop_two] != '\0')
		fprintf (fptr, "%s ", data [present_field + loop_two]);
	      else
		fprintf (fptr, "? ");

	    fprintf (fptr, "\n");
	  }

	  break;
	case RECRUIT:
	  if (data [present_field + 1] != '\0')
	    fprintf (fptr, "RE %s\n", data [present_field + 1]);
	  else {
	    /* screwed order output */

	    fprintf (fptr, "?? ");

	    for (loop_two = 0; loop_two < 10; loop_two ++)
	      if (data [present_field + loop_two] != '\0')
		fprintf (fptr, "%s ", data [present_field + loop_two]);
	      else
		fprintf (fptr, "? ");

	    fprintf (fptr, "\n");
	  }

	  break;
	case CHANT:
	  if (data [present_field + 1] != '\0' &&
	    (data [present_field + 2] != '\0' ||
	     data [present_field + 3] != '\0') &&
	     data [present_field + 9] != '\0')
	  {
	    fprintf (fptr, "CH %s ", data [present_field + 1]);

	    if (data [present_field + 8] != '\0')
	      fprintf (fptr, "%sBLK ", (data [present_field + 8]));

	    if (data [present_field + 9] != '\0')
	      fprintf (fptr, "%s ", (data [present_field + 9]));

	    if (data [present_field + 2] != '\0')
	      fprintf (fptr, "%s ", (data [present_field + 2]));

	    if (data [present_field + 3] != '\0')
	      fprintf (fptr, "%s ", (data [present_field + 3]));

	    fprintf (fptr, "\n");
	  } else {
	    /* screwed order output */

	    fprintf (fptr, "?? ");

	    for (loop_two = 0; loop_two < 10; loop_two ++)
	      if (data [present_field + loop_two] != '\0')
		fprintf (fptr, "%s ", data [present_field + loop_two]);
	      else
		fprintf (fptr, "? ");

	    fprintf (fptr, "\n");
	  }

	  break;
	case LOB:
	  if (data [present_field + 1] != '\0' &&
	    (data [present_field + 2] != '\0' ||
	     data [present_field + 3] != '\0'))
	  {
	    fprintf (fptr, "LO %s ", data [present_field + 1]);

	    if (data [present_field + 2] != '\0')
	      fprintf (fptr, "%s ", (data [present_field + 2]));

	    if (data [present_field + 3] != '\0')
	      fprintf (fptr, "%s ", (data [present_field + 3]));

	    fprintf (fptr, "\n");
	  } else {
	    /* screwed order output */

	    fprintf (fptr, "?? ");

	    for (loop_two = 0; loop_two < 10; loop_two ++)
	      if (data [present_field + loop_two] != '\0')
		fprintf (fptr, "%s ", data [present_field + loop_two]);
	      else
		fprintf (fptr, "? ");

	    fprintf (fptr, "\n");
	  }

	  break;
	case GIVE:
	  if (data [present_field + 1] != '\0' &&
	    data [present_field + 9] != '\0')
	    fprintf (fptr, "GI %s %s\n", data [present_field + 1],
	      data [present_field + 9]);
	  else {
	    /* screwed order output */

	    fprintf (fptr, "?? ");

	    for (loop_two = 0; loop_two < 10; loop_two ++)
	      if (data [present_field + loop_two] != '\0')
		fprintf (fptr, "%s ", data [present_field + loop_two]);
	      else
		fprintf (fptr, "? ");

	    fprintf (fptr, "\n");
	  }

	  break;
	case FREE:
	  if (data [present_field + 1] != '\0')
	  {
	    fprintf (fptr, "FR %s ", data [present_field + 1]);

	    if (data [present_field + 4] != '\0')
	      fprintf (fptr, "%sRED ", (data [present_field + 4]));

	    if (data [present_field + 5] != '\0')
	      fprintf (fptr, "%sGRN ", (data [present_field + 5]));

	    if (data [present_field + 6] != '\0')
	      fprintf (fptr, "%sGRY ", (data [present_field + 6]));

	    if (data [present_field + 7] != '\0')
	      fprintf (fptr, "%sYEL ", (data [present_field + 7]));

	    if (data [present_field + 8] != '\0')
	      fprintf (fptr, "%sBLK ", (data [present_field + 8]));

	    fprintf (fptr, "\n");
	  } else {
	    /* screwed order output */

	    fprintf (fptr, "?? ");

	    for (loop_two = 0; loop_two < 10; loop_two ++)
	      if (data [present_field + loop_two] != '\0')
		fprintf (fptr, "%s ", data [present_field + loop_two]);
	      else
		fprintf (fptr, "? ");

	    fprintf (fptr, "\n");
	  }

	  break;
	case BUILD_RAFT:
	  if (data [present_field + 1] != '\0' &&
	    data [present_field + 2] != '\0')
	    fprintf (fptr, "BR %s %s\n", data [present_field + 1],
	      data [present_field + 2]);
	  else {
	    /* screwed order output */

	    fprintf (fptr, "?? ");

	    for (loop_two = 0; loop_two < 10; loop_two ++)
	      if (data [present_field + loop_two] != '\0')
		fprintf (fptr, "%s ", data [present_field + loop_two]);
	      else
		fprintf (fptr, "? ");

	    fprintf (fptr, "\n");
	  }

	  break;
	case SAIL:
	  if (data [present_field + 1] != '\0' &&
	    (data [present_field + 2] != '\0' ||
	     data [present_field + 3] != '\0'))
	  {
	    fprintf (fptr, "SA %s ", data [present_field + 1]);

	    if (data [present_field + 2] != '\0')
	      fprintf (fptr, "%s ", (data [present_field + 2]));

	    if (data [present_field + 3] != '\0')
	      fprintf (fptr, "%s ", (data [present_field + 3]));

	    fprintf (fptr, "\n");
	  } else {
	    /* screwed order output */

	    fprintf (fptr, "?? ");

	    for (loop_two = 0; loop_two < 10; loop_two ++)
	      if (data [present_field + loop_two] != '\0')
		fprintf (fptr, "%s ", data [present_field + loop_two]);
	      else
		fprintf (fptr, "? ");

	    fprintf (fptr, "\n");
	  }

	  break;
	case SNEAK:
	  if (data [present_field + 1] != '\0' &&
	    (data [present_field + 2] != '\0' ||
	     data [present_field + 3] != '\0'))
	  {
	    fprintf (fptr, "SN %s ", data [present_field + 1]);

	    if (data [present_field + 2] != '\0')
	      fprintf (fptr, "%s ", (data [present_field + 2]));

	    if (data [present_field + 3] != '\0')
	      fprintf (fptr, "%s ", (data [present_field + 3]));

	    fprintf (fptr, "\n");
	  } else {
	    /* screwed order output */

	    fprintf (fptr, "?? ");

	    for (loop_two = 0; loop_two < 10; loop_two ++)
	      if (data [present_field + loop_two] != '\0')
		fprintf (fptr, "%s ", data [present_field + loop_two]);
	      else
		fprintf (fptr, "? ");

	    fprintf (fptr, "\n");
	  }

	  break;
	case KILL_PLAYER:
	  if (!kill_player_f)
	  {
	    fprintf (fptr, "KI\n");
	    kill_player_f = LZ_TRUE;
	  } else {
	    /* screwed order output */

	    fprintf (fptr, "?? ");

	    for (loop_two = 0; loop_two < 10; loop_two ++)
	      if (data [present_field + loop_two] != '\0')
		fprintf (fptr, "%s ", data [present_field + loop_two]);
	      else
		fprintf (fptr, "? ");

	    fprintf (fptr, "\n");
	  }

	  break;
	default:
	  /* screwed order output */

	  fprintf (fptr, "?? ");

	  for (loop_two = 0; loop_two < 10; loop_two ++)
	    if (data [present_field + loop_two] != '\0')
	      fprintf (fptr, "%s ", data [present_field + loop_two]);
	    else
	      fprintf (fptr, "? ");

	  fprintf (fptr, "\n");

	  break;
      }
    }

  if (data [MESSAGE_LINE1_FIELD] != '\0' || data [MESSAGE_LINE2_FIELD] != '\0')
  {
    fprintf (fptr, "MS ");

    if (data [MESSAGE_HEADER_FIELD] != '\0')
      fprintf (fptr, "%s\n", data [MESSAGE_HEADER_FIELD]);
    else
      fprintf (fptr, "\n");

    if (data [MESSAGE_LINE1_FIELD] != '\0')
      fprintf (fptr, "%s", data [MESSAGE_LINE1_FIELD]);

    if (data [MESSAGE_LINE2_FIELD] != '\0')
      fprintf (fptr, "%s", data [MESSAGE_LINE2_FIELD]);

    fprintf (fptr, "\nENDMESG\n");
  }

  for (loop = 0; loop < MAX_PLAYERS + 1; loop ++)
    if (kill_players [loop] == atoi (data [PLAYER_NUMBER_FIELD]))
      if (!kill_player_f)
      {
	fprintf (fptr, "KI\n");
	kill_player_f = LZ_TRUE;
      }
}

int order_num_from_string (char *string)
{
  char orders [17][3] = { "??", "MO", "SP", "BF", "BD", "BB", "DF", "DB", "RE", "CH",
			  "GI", "LO", "FR", "BR", "SA", "SN", "KI" };

  unsigned int order_num [17] = { 99, MOVE, SPLIT, BUILD_FERTILE, BUILD_DEN,
				  BUILD_BRIDGE, DESTROY_FERTILE,
				  DESTROY_BRIDGE, RECRUIT, CHANT, GIVE,
				  LOB, FREE, BUILD_RAFT, SAIL, SNEAK, KILL_PLAYER };

  unsigned int loop;

  for (loop = 0; loop < 16; loop ++)
    if (strcmp (orders [loop], strupr (string)) == 0)
      return (order_num [loop]);

  return (-1);
}

int order_ditto (unsigned int *field, field_t **template, char **data)
{
  if (*field >= ORDER_FORM_START_FIELD && *field < MESSAGE_HEADER_FIELD &&
    template [*field]->jump_up != 0 &&
    abs (template [*field]->jump_up) > 1)
  {
    if (data [*field] != '\0')
    {
      free (data [*field]);
      data [*field] = '\0';
    }

    if (data [*field + template [*field]->jump_up] != '\0')
      set_field (template, data, *field,
        data [*field + template [*field]->jump_up]);
    else
      data [*field] = '\0';

    show_field (template [*field], data [*field]);

    (*field) ++;

    return (LZ_TRUE);
  }
  return (LZ_FALSE);
}

int check_order_format (unsigned int *field, field_t **template, char **data)
{
  char error [255];

  int x, y, player, lizards, loop;

  hex_t hex;

  player = atoi (data [PLAYER_NUMBER_FIELD]);

  while (template [*field]->start_of_line_f != 1 &&
    template [*field]->start_of_line_f != -1)
    (*field) --;

  if (template [*field]->start_of_line_f == -1) return (LZ_FALSE);

  if (data [*field] != '\0')
  {
    switch (order_num_from_string (data [*field]))
    {
      case MOVE:
	if (data [*field + 1] == '\0')
        {
	  show_error ("ERROR: MOVE ORDER WITH NO SPECIFIED HEX!");

          return (LZ_FALSE);
        }

        string_to_loc (data [*field + 1], &x, &y);

        if (x < 0 || y < 0 || x >= world.x || y >= world.y)
        {
	  show_error ("ERROR: MOVE ORDER WITH INVALID HEX!");

	  return (LZ_FALSE);
	}

        if (data [*field + 2] == '\0' &&
          data [*field + 3] == '\0')
        {
	  show_error ("ERROR: MOVE ORDER WITH NO DIRECTIONS!");

	  return (LZ_FALSE);
        }

        if (string_to_direction (data [*field + 2]) == 0 &&
          data [*field + 2] != '\0')
        {
	  show_error ("ERROR: MOVE ORDER WITH INVALID DIRECTION!");

          return (LZ_FALSE);
        }

        if (string_to_direction (data [*field + 3]) == 0 &&
          data [*field + 3] != '\0')
        {
	  show_error ("ERROR: MOVE ORDER WITH INVALID DIRECTION!");

          return (LZ_FALSE);
	}

	if (data [*field + 4] != '\0' || data [*field + 5] != '\0' ||
          data [*field + 6] != '\0' || data [*field + 7] != '\0' ||
          data [*field + 8] != '\0' || data [*field + 9] != '\0')
	{
	  show_error ("ERROR: MOVE ORDER WITH EXTRA INFORMATION!");

          return (LZ_FALSE);
        }

        /* load hex and see if player has any lizards there */

        get_hex (x, y, &hex);

        if (lizards_in_hex (&hex) == 0)
        {
	  show_error ("MOVE ORDER INVALID: HEX CONTAINS NO LIZARDS!");

	  return (LZ_FALSE);
        } else
	  if (hex.hex_owner != player)
          {
	    show_error ("MOVE ORDER INVALID: HEX CONTAINS NO LIZARDS OWNED BY PLAYER!");

	    return (LZ_FALSE);
          }

        break;
      case SPLIT:
	if (data [*field + 1] == '\0')
	{
	  show_error ("ERROR: SPLIT ORDER WITH NO SPECIFIED HEX!");

          return (LZ_FALSE);
        }

        string_to_loc (data [*field + 1], &x, &y);

        if (x < 0 || y < 0 || x >= world.x || y >= world.y)
	{
	  show_error ("ERROR: SPLIT ORDER WITH INVALID HEX!");

          return (LZ_FALSE);
	}

        if (data [*field + 2] == '\0' &&
          data [*field + 3] == '\0')
        {
	  show_error ("ERROR: SPLIT ORDER WITH NO DIRECTIONS!");

	  return (LZ_FALSE);
        }

        if (string_to_direction (data [*field + 2]) == 0 &&
          data [*field + 2] != '\0')
        {
	  show_error ("ERROR: SPLIT ORDER WITH INVALID DIRECTION!");

	  return (LZ_FALSE);
        }

        if (string_to_direction (data [*field + 3]) == 0 &&
          data [*field + 3] != '\0')
	{
	  show_error ("ERROR: SPLIT ORDER WITH INVALID DIRECTION!");

          return (LZ_FALSE);
	}

	if (data [*field + 4] == '\0' &&
	  data [*field + 5] == '\0' &&
	  data [*field + 6] == '\0' &&
	  data [*field + 7] == '\0' &&
	  data [*field + 8] == '\0')
	{
	  show_error ("ERROR: SPLIT ORDER WITH NO LIZARDS!");

	  return (LZ_FALSE);
	}

	for (lizards = -1, loop = 4; loop < 9; loop ++)
	  if (data [*field + loop] != '\0')
	    if (lizards == -1)
	      lizards = atoi (data [*field + loop]);
	    else
	      lizards += atoi (data [*field + loop]);

	if (lizards == 0)
	{
	  show_error ("ERROR: SPLIT ORDER WITH NO LIZARDS!");

	  return (LZ_FALSE);
	}

	if (data [*field + 9] != '\0')
	{
	  show_error ("ERROR: SPLIT ORDER WITH EXTRA INFORMATION!");

	  return (LZ_FALSE);
	}

	/* load hex and see if player has any lizards there */

	get_hex (x, y, &hex);

	if (lizards_in_hex (&hex) == 0)
	{
	  show_error ("SPLIT ORDER INVALID: HEX CONTAINS NO LIZARDS!");

	  return (LZ_FALSE);
	} else
	  if (hex.hex_owner != player)
	  {
	    show_error ("SPLIT ORDER INVALID: HEX CONTAINS NO LIZARDS OWNED BY PLAYER!");

	    return (LZ_FALSE);
	  } else {
	    if (data [*field + 4] != '\0')
	    {
	      lizards = atoi (data [*field + 4]);

	      if (lizards > hex.red_lizards)
		if (hex.red_lizards == 0)
		{
		  show_error ("SPLIT ORDER INVALID: HEX CONTAINS NO RED LIZARDS!");

		  return (LZ_FALSE);
		} else {
		  sprintf (error, "SPLIT ORDER INVALID: HEX CONTAINS ONLY %d RED LIZARDS!",
		    hex.red_lizards);

		  show_error (error);

		  return (LZ_FALSE);
		}
	    }

	    if (data [*field + 5] != '\0')
	    {
	      lizards = atoi (data [*field + 5]);

	      if (lizards > hex.green_lizards)
		if (hex.green_lizards == 0)
		{
		  show_error ("SPLIT ORDER INVALID: HEX CONTAINS NO GREEN LIZARDS!");

		  return (LZ_FALSE);
		} else {
		  sprintf (error, "SPLIT ORDER INVALID: HEX CONTAINS ONLY %d GREEN LIZARDS!",
		    hex.green_lizards);

		  show_error (error);

		  return (LZ_FALSE);
		}
	    }

	    if (data [*field + 6] != '\0')
	    {
	      lizards = atoi (data [*field + 6]);

	      if (lizards > hex.grey_lizards)
		if (hex.grey_lizards == 0)
		{
		  show_error ("SPLIT ORDER INVALID: HEX CONTAINS NO GREY LIZARDS!");

		  return (LZ_FALSE);
		} else {
		  sprintf (error, "SPLIT ORDER INVALID: HEX CONTAINS ONLY %d GREY LIZARDS!",
		    hex.grey_lizards);

		  show_error (error);

		  return (LZ_FALSE);
		}
	    }

	    if (data [*field + 7] != '\0')
	    {
	      lizards = atoi (data [*field + 7]);

	      if (lizards > hex.yellow_lizards)
		if (hex.yellow_lizards == 0)
		{
		  show_error ("SPLIT ORDER INVALID: HEX CONTAINS NO YELLOW LIZARDS!");

		  return (LZ_FALSE);
		} else {
		  sprintf (error, "SPLIT ORDER INVALID: HEX CONTAINS ONLY %d YELLOW LIZARDS!",
		    hex.yellow_lizards);

		  show_error (error);

		  return (LZ_FALSE);
		}
	    }

	    if (data [*field + 8] != '\0')
	    {
	      lizards = atoi (data [*field + 8]);

	      if (lizards > hex.black_lizards)
		if (hex.black_lizards == 0)
		{
		  show_error ("SPLIT ORDER INVALID: HEX CONTAINS NO BLACK LIZARDS!");

		  return (LZ_FALSE);
		} else {
		  sprintf (error, "SPLIT ORDER INVALID: HEX CONTAINS ONLY %d BLACK LIZARDS!",
		    hex.black_lizards);

		  show_error (error);

		  return (LZ_FALSE);
		}
	    }
	  }

	break;
      case BUILD_FERTILE:
	if (data [*field + 1] == '\0')
	{
	  show_error ("ERROR: BUILD FERTILE ORDER WITH NO SPECIFIED HEX!");

	  return (LZ_FALSE);
	}

	string_to_loc (data [*field + 1], &x, &y);

	if (x < 0 || y < 0 || x >= world.x || y >= world.y)
	{
	  show_error ("ERROR: BUILD FERTILE ORDER WITH INVALID HEX!");

	  return (LZ_FALSE);
	}

        if (data [*field + 2] != '\0' || data [*field + 3] != '\0' ||
	  data [*field + 4] != '\0' || data [*field + 5] != '\0' ||
          data [*field + 6] != '\0' || data [*field + 7] != '\0' ||
          data [*field + 8] != '\0' || data [*field + 9] != '\0')
        {
	  show_error ("ERROR: BUILD FERTILE ORDER WITH EXTRA INFORMATION!");

          return (LZ_FALSE);
	}

        /* load hex and see if player has any lizards there */

        get_hex (x, y, &hex);

        if (lizards_in_hex (&hex) == 0)
        {
	  show_error ("BUILD FERTILE ORDER INVALID: HEX CONTAINS NO LIZARDS!");

	  return (LZ_FALSE);
        } else
          if (hex.hex_owner != player)
	  {
	    show_error ("BUILD FERTILE ORDER INVALID: HEX CONTAINS NO LIZARDS OWNED BY PLAYER!");

            return (LZ_FALSE);
          } else {
            if (hex.green_lizards < BUILD_FERTILE_LEVEL)
            {
	      show_error ("BUILD FERTILE ORDER INVALID: HEX CONTAINS INSUFFICIENT GREEN LIZARDS!");

              return (LZ_FALSE);
            }
          }

        break;
      case BUILD_DEN:
        if (data [*field + 1] == '\0')
        {
	  show_error ("ERROR: BUILD DEN ORDER WITH NO SPECIFIED HEX!");

          return (LZ_FALSE);
        }

        string_to_loc (data [*field + 1], &x, &y);

        if (x < 0 || y < 0 || x >= world.x || y >= world.y)
        {
	  show_error ("ERROR: BUILD DEN ORDER WITH INVALID HEX!");

          return (LZ_FALSE);
        }

        if (data [*field + 2] != '\0' || data [*field + 3] != '\0' ||
          data [*field + 4] != '\0' || data [*field + 5] != '\0' ||
          data [*field + 6] != '\0' || data [*field + 7] != '\0' ||
	  data [*field + 8] != '\0' || data [*field + 9] != '\0')
        {
	  show_error ("ERROR: BUILD DEN ORDER WITH EXTRA INFORMATION!");

          return (LZ_FALSE);
        }

        /* load hex and see if player has any lizards there */

        get_hex (x, y, &hex);

        if (lizards_in_hex (&hex) == 0)
        {
	  show_error ("BUILD DEN ORDER INVALID: HEX CONTAINS NO LIZARDS!");

          return (LZ_FALSE);
        } else
          if (hex.hex_owner != player)
          {
	    show_error ("BUILD DEN ORDER INVALID: HEX CONTAINS NO LIZARDS OWNED BY PLAYER!");

            return (LZ_FALSE);
          } else {
	    if (hex.grey_lizards < BUILD_DEN_LEVEL)
            {
	      show_error ("BUILD DEN ORDER INVALID: HEX CONTAINS INSUFFICIENT GREY LIZARDS!");

              return (LZ_FALSE);
            }
          }

        break;
      case BUILD_BRIDGE:
	if (data [*field + 1] == '\0')
        {
	  show_error ("ERROR: BUILD BRIDGE ORDER WITH NO SPECIFIED HEX!");

          return (LZ_FALSE);
        }

	string_to_loc (data [*field + 1], &x, &y);

        if (x < 0 || y < 0 || x >= world.x || y >= world.y)
	{
	  show_error ("ERROR: BUILD BRIDGE ORDER WITH INVALID HEX!");

          return (LZ_FALSE);
        }

        if (data [*field + 2] == '\0')
        {
	  show_error ("ERROR: BUILD BRIDGE ORDER WITH NO DIRECTION!");

	  return (LZ_FALSE);
        }

	if (string_to_direction (data [*field + 2]) == 0  &&
	  data [*field + 2] != '\0')
	{
	  show_error ("ERROR: BUILD BRDIGE ORDER WITH INVALID DIRECTION!");

	  return (LZ_FALSE);
	}

	if (data [*field + 3] != '\0' || data [*field + 4] != '\0' ||
	  data [*field + 5] != '\0' || data [*field + 6] != '\0' ||
	  data [*field + 7] != '\0' || data [*field + 8] != '\0' ||
	  data [*field + 9] != '\0')
	{
	  show_error ("ERROR: BUILD BRIDGE ORDER WITH EXTRA INFORMATION!");

	  return (LZ_FALSE);
	}

	/* load hex and see if player has any lizards there */

	get_hex (x, y, &hex);

	if (lizards_in_hex (&hex) == 0)
	{
	  show_error ("BUILD BRIDGE ORDER INVALID: HEX CONTAINS NO LIZARDS!");

	  return (LZ_FALSE);
	} else
	  if (hex.hex_owner != player)
	  {
	    show_error ("BUILD BRIDGE ORDER INVALID: HEX CONTAINS NO LIZARDS OWNED BY PLAYER!");

	    return (LZ_FALSE);
	  } else {
	    if (hex.grey_lizards < BUILD_BRIDGE_LEVEL)
	    {
	      show_error ("BUILD BRIDGE ORDER INVALID: HEX CONTAINS INSUFFICIENT GREY LIZARDS!");

	      return (LZ_FALSE);
	    }
	  }

	break;
      case DESTROY_FERTILE:
	if (data [*field + 1] == '\0')
	{
	  show_error ("ERROR: DESTROY FERTILE ORDER WITH NO SPECIFIED HEX!");

	  return (LZ_FALSE);
	}

	string_to_loc (data [*field + 1], &x, &y);

	if (x < 0 || y < 0 || x >= world.x || y >= world.y)
	{
	  show_error ("ERROR: DESTROY FERTILE ORDER WITH INVALID HEX!");

	  return (LZ_FALSE);
	}

	if (data [*field + 2] != '\0' || data [*field + 3] != '\0' ||
	  data [*field + 4] != '\0' || data [*field + 5] != '\0' ||
	  data [*field + 6] != '\0' || data [*field + 7] != '\0' ||
	  data [*field + 8] != '\0' || data [*field + 9] != '\0')
        {
	  show_error ("ERROR: DESTROY FERTILE ORDER WITH EXTRA INFORMATION!");

          return (LZ_FALSE);
	}

        /* load hex and see if player has any lizards there */

        get_hex (x, y, &hex);

        if (lizards_in_hex (&hex) == 0)
        {
          show_error ("DESTROY FERTILE ORDER INVALID: HEX CONTAINS NO LIZARDS!");

	  return (LZ_FALSE);
        } else
          if (hex.hex_owner != player)
          {
            show_error ("DESTROY FERTILE ORDER INVALID: HEX CONTAINS NO LIZARDS OWNED BY PLAYER!");

            return (LZ_FALSE);
	  } else {
            if (hex.green_lizards < BUILD_FERTILE_LEVEL)
            {
	      show_error ("DESTROY FERTILE ORDER INVALID: HEX CONTAINS INSUFFICIENT GREEN LIZARDS!");

              return (LZ_FALSE);
            }
          }

        break;
      case DESTROY_BRIDGE:
        if (data [*field + 1] == '\0')
        {
	  show_error ("ERROR: DESTROY BRIDGE ORDER WITH NO SPECIFIED HEX!");

          return (LZ_FALSE);
        }

	string_to_loc (data [*field + 1], &x, &y);

	if (x < 0 || y < 0 || x >= world.x || y >= world.y)
        {
          show_error ("ERROR: DESTROY BRIDGE ORDER WITH INVALID HEX!");

          return (LZ_FALSE);
        }

        if (data [*field + 2] == '\0')
        {
          show_error ("ERROR: DESTROY BRIDGE ORDER WITH NO DIRECTION!");

          return (LZ_FALSE);
	}

        if (string_to_direction (data [*field + 2]) == 0  &&
          data [*field + 2] != '\0')
        {
          show_error ("ERROR: DESTROY BRDIGE ORDER WITH INVALID DIRECTION!");

	  return (LZ_FALSE);
	}

	if (data [*field + 3] != '\0' || data [*field + 4] != '\0' ||
	  data [*field + 5] != '\0' || data [*field + 6] != '\0' ||
	  data [*field + 7] != '\0' || data [*field + 8] != '\0' ||
	  data [*field + 9] != '\0')
	{
	  show_error ("ERROR: DESTROY BRIDGE ORDER WITH EXTRA INFORMATION!");

	  return (LZ_FALSE);
	}

	/* load hex and see if player has any lizards there */

	get_hex (x, y, &hex);

	if (lizards_in_hex (&hex) == 0)
	{
	  show_error ("DESTROY BRIDGE ORDER INVALID: HEX CONTAINS NO LIZARDS!");

	  return (LZ_FALSE);
	} else
	  if (hex.hex_owner != player)
	  {
	    show_error ("DESTROY BRIDGE ORDER INVALID: HEX CONTAINS NO LIZARDS OWNED BY PLAYER!");

	    return (LZ_FALSE);
	  } else {
	    if (hex.grey_lizards < BUILD_BRIDGE_LEVEL)
	    {
	      show_error ("DESTROY BRIDGE ORDER INVALID: HEX CONTAINS INSUFFICIENT GREY LIZARDS!");

	      return (LZ_FALSE);
	    }
	  }

	break;
      case RECRUIT:
	if (data [*field + 1] == '\0')
	{
	  show_error ("ERROR: RECRUIT ORDER WITH NO SPECIFIED HEX!");

          return (LZ_FALSE);
	}

        string_to_loc (data [*field + 1], &x, &y);

        if (x < 0 || y < 0 || x >= world.x || y >= world.y)
        {
          show_error ("ERROR: RECRUIT ORDER WITH INVALID HEX!");

          return (LZ_FALSE);
	}

        if (data [*field + 2] != '\0' || data [*field + 3] != '\0' ||
          data [*field + 4] != '\0' || data [*field + 5] != '\0' ||
          data [*field + 6] != '\0' || data [*field + 7] != '\0' ||
          data [*field + 8] != '\0' || data [*field + 9] != '\0')
        {
          show_error ("ERROR: RECRUIT ORDER WITH EXTRA INFORMATION!");

          return (LZ_FALSE);
        }

        /* load hex and see if player has any lizards there */

        get_hex (x, y, &hex);

        if (hex.terrain != DEN && hex.terrain != HOME_DEN)
        {
          show_error ("RECRUIT ORDER INVALID: HEX DOESN'T CONTAIN DEN!");

	  return (LZ_FALSE);
	} else
          if (hex.hex_owner != player)
          {
            show_error ("RECRUIT ORDER INVALID: DEN HEX CONTAINS NOT OWNED BY PLAYER!");

            return (LZ_FALSE);
          }

        break;
      case CHANT:
	if (data [*field + 1] == '\0')
        {
          show_error ("ERROR: CHANT ORDER WITH NO SPECIFIED HEX!");

          return (LZ_FALSE);
        }

        string_to_loc (data [*field + 1], &x, &y);

	if (x < 0 || y < 0 || x >= world.x || y >= world.y)
	{
          show_error ("ERROR: CHANT ORDER WITH INVALID HEX!");

          return (LZ_FALSE);
        }

	if (data [*field + 9] == '\0')
	{
	  show_error ("ERROR: CHANT ORDER WITH NO SPECIFIED CHANT!");

	  return (LZ_FALSE);
	}

	for (loop = 0; loop < NUMBER_CHANTS; loop ++)
	  if (strcmp (chant_code [loop], data [*field + 9]) == 0)
	    break;

	if (loop == NUMBER_CHANTS)
	{
	  show_error ("ERROR: CHANT ORDER WITH UNKNOWN CHANT SPECIFIED!");

	  return (LZ_FALSE);
	}

        if (data [*field + 4] != '\0' || data [*field + 5] != '\0' ||
          data [*field + 6] != '\0' || data [*field + 7] != '\0')
        {
	  show_error ("ERROR: CHANT ORDER WITH EXTRA INFORMATION!");

          return (LZ_FALSE);
        }

        /* load hex and see if player has any lizards there */

        get_hex (x, y, &hex);

        if (lizards_in_hex (&hex) == 0)
        {
          show_error ("CHANT ORDER INVALID: HEX CONTAINS NO LIZARDS!");

	  return (LZ_FALSE);
	} else
          if (hex.hex_owner != player)
          {
            show_error ("CHANT ORDER INVALID: HEX CONTAINS NO LIZARDS OWNED BY PLAYER!");

            return (LZ_FALSE);
	  } else {
            if (hex.black_lizards == 0)
            {
              show_error ("CHANT ORDER INVALID: HEX CONTAINS NO BLACK LIZARDS!");

              return (LZ_FALSE);
            }
          }

        break;
      case GIVE:
        if (data [*field + 1] == '\0')
	{
	  show_error ("ERROR: GIVE ORDER WITH NO SPECIFIED HEX!");

          return (LZ_FALSE);
        }

        string_to_loc (data [*field + 1], &x, &y);

	if (x < 0 || y < 0 || x >= world.x || y >= world.y)
        {
          show_error ("ERROR: GIVE ORDER WITH INVALID HEX!");

          return (LZ_FALSE);
        }

        if (data [*field + 9] == '\0')
        {
          show_error ("ERROR: GIVE ORDER WITH NO SPECIFIED PLAYER!");

          return (LZ_FALSE);
	}

	if (data [*field + 2] != '\0' || data [*field + 3] != '\0' ||
          data [*field + 4] != '\0' || data [*field + 5] != '\0' ||
          data [*field + 6] != '\0' || data [*field + 7] != '\0' ||
          data [*field + 8] != '\0')
        {
          show_error ("ERROR: GIVE ORDER WITH EXTRA INFORMATION!");

          return (LZ_FALSE);
        }

        /* load hex and see if player has any lizards there */

        get_hex (x, y, &hex);

        if (hex.hex_owner != player)
        {
          show_error ("GIVE ORDER INVALID: HEX IS NO CONTROLLED BY PLAYER!");

	  return (LZ_FALSE);
	}

        break;
      case LOB:
        if (data [*field + 1] == '\0')
        {
          show_error ("ERROR: LOB ORDER WITH NO SPECIFIED HEX!");

          return (LZ_FALSE);
        }

        string_to_loc (data [*field + 1], &x, &y);

        if (x < 0 || y < 0 || x >= world.x || y >= world.y)
        {
          show_error ("ERROR: LOB ORDER WITH INVALID HEX!");

          return (LZ_FALSE);
        }

	if (data [*field + 2] == '\0' &&
	  data [*field + 3] == '\0')
        {
          show_error ("ERROR: LOB ORDER WITH NO DIRECTIONS!");

          return (LZ_FALSE);
        }

        if (string_to_direction (data [*field + 2]) == 0 &&
          data [*field + 2] != '\0')
        {
          show_error ("ERROR: LOB ORDER WITH INVALID DIRECTION!");

          return (LZ_FALSE);
        }

        if (string_to_direction (data [*field + 3]) == 0 &&
          data [*field + 3] != '\0')
        {
	  show_error ("ERROR: LOB ORDER WITH INVALID DIRECTION!");

	  return (LZ_FALSE);
        }

        if (data [*field + 4] != '\0' || data [*field + 5] != '\0' ||
          data [*field + 6] != '\0' || data [*field + 7] != '\0' ||
          data [*field + 8] != '\0' || data [*field + 9] != '\0')
	{
          show_error ("ERROR: LOB ORDER WITH EXTRA INFORMATION!");

          return (LZ_FALSE);
        }

        /* load hex and see if player has any lizards there */

        get_hex (x, y, &hex);

        if (lizards_in_hex (&hex) == 0)
        {
	  show_error ("LOB ORDER INVALID: HEX CONTAINS NO LIZARDS!");

	  return (LZ_FALSE);
        } else
          if (hex.hex_owner != player)
          {
            show_error ("LOB ORDER INVALID: HEX CONTAINS NO LIZARDS OWNED BY PLAYER!");

	    return (LZ_FALSE);
          } else {
            if (hex.yellow_lizards == 0)
            {
              show_error ("LOB ORDER INVALID: HEX CONTAINS NO YELLOW LIZARDS!");

              return (LZ_FALSE);
            }
          }

	break;
      case FREE:
	if (data [*field + 1] == '\0')
	{
	  show_error ("ERROR: FREE ORDER WITH NO SPECIFIED HEX!");

	  return (LZ_FALSE);
	}

	string_to_loc (data [*field + 1], &x, &y);

	if (x < 0 || y < 0 || x >= world.x || y >= world.y)
	{
	  show_error ("ERROR: FREE ORDER WITH INVALID HEX!");

	  return (LZ_FALSE);
	}

	if (data [*field + 2] != '\0' || data [*field + 3] != '\0' ||
	  data [*field + 9] != '\0')
	{
	  show_error ("ERROR: FREE ORDER WITH EXTRA INFORMATION!");

	  return (LZ_FALSE);
	}

	/* load hex and see if player has any lizards there */

	get_hex (x, y, &hex);

	if (lizards_in_hex (&hex) == 0)
	{
	  show_error ("FREE ORDER INVALID: HEX CONTAINS NO LIZARDS!");

	  return (LZ_FALSE);
	} else
	  if (hex.hex_owner != player)
	  {
	    show_error ("FREE ORDER INVALID: HEX CONTAINS NO LIZARDS OWNED BY PLAYER!");

	    return (LZ_FALSE);
	  } else {
	    if (data [*field + 4] != '\0')
	    {
	      lizards = atoi (data [*field + 4]);

	      if (lizards > hex.red_lizards)
		if (hex.red_lizards == 0)
		{
		  show_error ("FREE ORDER INVALID: HEX CONTAINS NO RED LIZARDS!");

		  return (LZ_FALSE);
		} else {
		  sprintf (error, "FREE ORDER INVALID: HEX CONTAINS ONLY %d RED LIZARDS!",
		    hex.red_lizards);

		  show_error (error);

		  return (LZ_FALSE);
		}
	    }

	    if (data [*field + 5] != '\0')
	    {
	      lizards = atoi (data [*field + 5]);

	      if (lizards > hex.green_lizards)
		if (hex.green_lizards == 0)
		{
		  show_error ("FREE ORDER INVALID: HEX CONTAINS NO GREEN LIZARDS!");

		  return (LZ_FALSE);
		} else {
		  sprintf (error, "FREE ORDER INVALID: HEX CONTAINS ONLY %d GREEN LIZARDS!",
		    hex.green_lizards);

		  show_error (error);

		  return (LZ_FALSE);
		}
	    }

	    if (data [*field + 6] != '\0')
	    {
	      lizards = atoi (data [*field + 6]);

	      if (lizards > hex.grey_lizards)
		if (hex.grey_lizards == 0)
		{
		  show_error ("FREE ORDER INVALID: HEX CONTAINS NO GREY LIZARDS!");

		  return (LZ_FALSE);
		} else {
		  sprintf (error, "FREE ORDER INVALID: HEX CONTAINS ONLY %d GREY LIZARDS!",
		    hex.grey_lizards);

		  show_error (error);

		  return (LZ_FALSE);
		}
	    }

	    if (data [*field + 7] != '\0')
	    {
	      lizards = atoi (data [*field + 7]);

	      if (lizards > hex.yellow_lizards)
		if (hex.yellow_lizards == 0)
		{
		  show_error ("FREE ORDER INVALID: HEX CONTAINS NO YELLOW LIZARDS!");

		  return (LZ_FALSE);
		} else {
		  sprintf (error, "FREE ORDER INVALID: HEX CONTAINS ONLY %d YELLOW LIZARDS!",
		    hex.yellow_lizards);

		  show_error (error);

		  return (LZ_FALSE);
		}
	    }

	    if (data [*field + 8] != '\0')
	    {
	      lizards = atoi (data [*field + 8]);

	      if (lizards > hex.black_lizards)
		if (hex.black_lizards == 0)
		{
		  show_error ("FREE ORDER INVALID: HEX CONTAINS NO BLACK LIZARDS!");

		  return (LZ_FALSE);
		} else {
		  sprintf (error, "FREE ORDER INVALID: HEX CONTAINS ONLY %d BLACK LIZARDS!",
		    hex.black_lizards);

		  show_error (error);

		  return (LZ_FALSE);
		}
	    }
	  }

	break;
      case BUILD_RAFT:
	if (data [*field + 1] == '\0')
	{
	  show_error ("ERROR: BUILD RAFT ORDER WITH NO SPECIFIED HEX!");

          return (LZ_FALSE);
	}

        string_to_loc (data [*field + 1], &x, &y);

        if (x < 0 || y < 0 || x >= world.x || y >= world.y)
        {
          show_error ("ERROR: BUILD RAFT ORDER WITH INVALID HEX!");

          return (LZ_FALSE);
        }

        if (data [*field + 3] != '\0' || data [*field + 4] != '\0' ||
	  data [*field + 5] != '\0' || data [*field + 6] != '\0' ||
	  data [*field + 7] != '\0' || data [*field + 8] != '\0' ||
	  data [*field + 9] != '\0')
        {
          show_error ("ERROR: BUILD RAFT ORDER WITH EXTRA INFORMATION!");

          return (LZ_FALSE);
        }

        /* load hex and see if player has any lizards there */

        get_hex (x, y, &hex);

        if (lizards_in_hex (&hex) == 0)
        {
          show_error ("BUILD RAFT ORDER INVALID: HEX CONTAINS NO LIZARDS!");

          return (LZ_FALSE);
        } else
          if (hex.hex_owner != player)
	  {
	    show_error ("BUILD RAFT ORDER INVALID: HEX CONTAINS NO LIZARDS OWNED BY PLAYER!");

            return (LZ_FALSE);
          } else {
            if (hex.green_lizards < BUILD_RAFT_LEVEL)
            {
              show_error ("BUILD RAFT ORDER INVALID: HEX CONTAINS INSUFFICIENT GREEN LIZARDS!");

              return (LZ_FALSE);
            }
          }

        break;
      case SAIL:
        if (data [*field + 1] == '\0')
        {
          show_error ("ERROR: SAIL ORDER WITH NO SPECIFIED HEX!");

          return (LZ_FALSE);
	}

	string_to_loc (data [*field + 1], &x, &y);

        if (x < 0 || y < 0 || x >= world.x || y >= world.y)
        {
          show_error ("ERROR: SAIL ORDER WITH INVALID HEX!");

          return (LZ_FALSE);
        }

        if (data [*field + 2] == '\0' &&
          data [*field + 3] == '\0')
        {
          show_error ("ERROR: SAIL ORDER WITH NO DIRECTIONS!");

          return (LZ_FALSE);
        }

        if (string_to_direction (data [*field + 2]) == 0 &&
	  data [*field + 2] != '\0')
	{
	  show_error ("ERROR: SAIL ORDER WITH INVALID DIRECTION!");

          return (LZ_FALSE);
        }

        if (string_to_direction (data [*field + 3]) == 0 &&
          data [*field + 3] != '\0')
        {
          show_error ("ERROR: SAIL ORDER WITH INVALID DIRECTION!");

          return (LZ_FALSE);
        }

        if (data [*field + 4] != '\0' || data [*field + 5] != '\0' ||
          data [*field + 6] != '\0' || data [*field + 7] != '\0' ||
          data [*field + 8] != '\0' || data [*field + 9] != '\0')
        {
          show_error ("ERROR: SAIL ORDER WITH EXTRA INFORMATION!");

	  return (LZ_FALSE);
	}

        /* load hex and see if player has any lizards there */

        get_hex (x, y, &hex);

        if (lizards_in_hex (&hex) == 0)
        {
          show_error ("SAIL ORDER INVALID: HEX CONTAINS NO LIZARDS!");

          return (LZ_FALSE);
        } else
          if (hex.hex_owner != player)
          {
            show_error ("SAIL ORDER INVALID: HEX CONTAINS NO LIZARDS OWNED BY PLAYER!");

            return (LZ_FALSE);
	  }

        break;
      case SNEAK:
        if (data [*field + 1] == '\0')
        {
          show_error ("ERROR: SNEAK ORDER WITH NO SPECIFIED HEX!");

          return (LZ_FALSE);
        }

        string_to_loc (data [*field + 1], &x, &y);

        if (x < 0 || y < 0 || x >= world.x || y >= world.y)
	{
	  show_error ("ERROR: SNEAK ORDER WITH INVALID HEX!");

          return (LZ_FALSE);
        }

        if (data [*field + 2] == '\0' &&
          data [*field + 3] == '\0')
        {
	  show_error ("ERROR: SNEAK ORDER WITH NO DIRECTIONS!");

          return (LZ_FALSE);
        }

        if (string_to_direction (data [*field + 2]) == 0 &&
          data [*field + 2] != '\0')
        {
          show_error ("ERROR: SNEAK ORDER WITH INVALID DIRECTION!");

          return (LZ_FALSE);
	}

	if (string_to_direction (data [*field + 3]) == 0  &&
          data [*field + 3] != '\0')
        {
          show_error ("ERROR: SNEAK ORDER WITH INVALID DIRECTION!");

          return (LZ_FALSE);
        }

        if (data [*field + 4] != '\0' || data [*field + 5] != '\0' ||
          data [*field + 6] != '\0' || data [*field + 7] != '\0' ||
	  data [*field + 8] != '\0' || data [*field + 9] != '\0')
	{
	  show_error ("ERROR: SNEAK ORDER WITH EXTRA INFORMATION!");

	  return (LZ_FALSE);
	}

	for (loop = 0; loop < number_spys; loop ++)
	  if (spy_list [loop]->x_hex == x && spy_list [loop]->y_hex == y &&
	    spy_list [loop]->player == player)
	    break;

	if (loop == number_spys)
	{
	  show_error ("ERROR: PLAYER HAS NO SPY IN SPECIFIED HEX!");

	  return (LZ_FALSE);
	}

	break;
      case KILL_PLAYER:
	break;
      default:
	show_error ("ERROR: UNKNOWN ORDER!");

	return (LZ_FALSE);
    }
  }

  (*field) ++;

  while (template [*field]->start_of_line_f != 1 &&
    template [*field]->start_of_line_f != -1)
    (*field) ++;

  return (LZ_TRUE);
}

int order_skip_to_field (unsigned int *field, field_t **template, char **data)
{
  unsigned int work_field,
               present_field_on_line;

  work_field = *field;

  while (template [work_field]->start_of_line_f != 1 &&
    template [work_field]->start_of_line_f != -1)
    (work_field) --;

  if (template [work_field]->start_of_line_f == -1) return (LZ_FALSE);

  if (data [work_field] != '\0')
  {
    present_field_on_line = *field - work_field;

    switch (order_num_from_string (data [work_field]))
    {
      case CHANT:
        if (present_field_on_line > 3 && present_field_on_line < 8)
        {
          /* skip to black lizards */

	  *field = work_field + 8;

          return (LZ_TRUE);
        }

        break;
      case GIVE:
	if (present_field_on_line > 1 && present_field_on_line < 9)
        {
          /* skip to extra field */

          *field = work_field + 9;

          return (LZ_TRUE);
        }

        break;
      case FREE:
        if (present_field_on_line > 1 && present_field_on_line < 4)
        {
	  /* skip to first lizards field */

          *field = work_field + 4;

          return (LZ_TRUE);
        }


        break;
    }
  }

  return (LZ_TRUE);
}

void show_order_form (void)
{
  clrscr ();

  cprintf ("LIZARDS! Order Entry Utility. %s Last compiled %s.\n\r"
	   "Game Code:___     Turn Number:___                             Origin Code:_____\n\r"
	   "Player Number:__  Name:___________________________________    Player Code:_____\n\r"
	   "Clan Name:____________________  Clan Code:___    Credit:$__________\n\r"
	   "OR HEX  M1 M2 RED GRN GRY YEL BLK EXTR | OR HEX  M1 M2 RED GRN GRY YEL BLK EXTR\n\r"
	   "__ ____ __ __ ___ ___ ___ ___ ___ ____ 1 __ ____ __ __ ___ ___ ___ ___ ___ ____\n\r"
           "__ ____ __ __ ___ ___ ___ ___ ___ ____ | __ ____ __ __ ___ ___ ___ ___ ___ ____\n\r"
           "__ ____ __ __ ___ ___ ___ ___ ___ ____ | __ ____ __ __ ___ ___ ___ ___ ___ ____\n\r"
	   "__ ____ __ __ ___ ___ ___ ___ ___ ____ | __ ____ __ __ ___ ___ ___ ___ ___ ____\n\r"
	   "__ ____ __ __ ___ ___ ___ ___ ___ ____ 5 __ ____ __ __ ___ ___ ___ ___ ___ ____\n\r"
	   "__ ____ __ __ ___ ___ ___ ___ ___ ____ | __ ____ __ __ ___ ___ ___ ___ ___ ____\n\r"
	   "__ ____ __ __ ___ ___ ___ ___ ___ ____ | __ ____ __ __ ___ ___ ___ ___ ___ ____\n\r"
	   "__ ____ __ __ ___ ___ ___ ___ ___ ____ | __ ____ __ __ ___ ___ ___ ___ ___ ____\n\r"
	   "__ ____ __ __ ___ ___ ___ ___ ___ ____ | __ ____ __ __ ___ ___ ___ ___ ___ ____\n\r"
	   "__ ____ __ __ ___ ___ ___ ___ ___ ____ 0 __ ____ __ __ ___ ___ ___ ___ ___ ____\n\r"
	   "__ ____ __ __ ___ ___ ___ ___ ___ ____ | __ ____ __ __ ___ ___ ___ ___ ___ ____\n\r"
	   "__ ____ __ __ ___ ___ ___ ___ ___ ____ | __ ____ __ __ ___ ___ ___ ___ ___ ____\n\r"
	   "__ ____ __ __ ___ ___ ___ ___ ___ ____ | __ ____ __ __ ___ ___ ___ ___ ___ ____\n\r"
	   "__ ____ __ __ ___ ___ ___ ___ ___ ____ | __ ____ __ __ ___ ___ ___ ___ ___ ____\n\r"
	   "__ ____ __ __ ___ ___ ___ ___ ___ ____ 5 __ ____ __ __ ___ ___ ___ ___ ___ ____\n\r"
	   "Speaking with the Enemy ______________________________    PGUP=LAST PLAYER\n\r"
	   "Message  _______________________________________________  PGDN=NEXT PLAYER\n\r"
	   "         _______________________________________________   \n\r"
	   "F1=DITTO  F2=CHECK LINE  F3=LOAD/ERASE  F4=SAVE+EXIT  F5=EXIT",
	   VER_STRING, __DATE__);
}

void show_other_bit_form (void)
{
  clrscr ();

  cprintf ("LIZARDS! Order Entry Utility. %s Last compiled %s.\n\r"
	   "\n\rPlayers to Kill (remove entirely from game!):\n\r"
	   "\n\rKill Player Numbers: __ __ __ __ __ __ __ __ __ __\n\r"
	   "\n\rThe Other Bit (Message from the Moderator):\n\r\n\r"
	   " ____________________________________________________________ F3 = ERASE FORM\n\r"
	   " ____________________________________________________________ F4 = ALL FINISHED!\n\r"
	   " ____________________________________________________________\n\r"
	   " ____________________________________________________________\n\r"
	   " ____________________________________________________________\n\r",
	   VER_STRING, __DATE__);
}
