#define ORDER_ORIGIN_FIELD 2
#define PLAYER_NUMBER_FIELD 3
#define ORDER_FORM_START_FIELD 7
#define MESSAGE_HEADER_FIELD 307
#define MESSAGE_LINE1_FIELD 308
#define MESSAGE_LINE2_FIELD 309

#define NO_ORDERS 0
#define PAPER_ORDERS 1  /* orders entered from paper */
#define BBS_ORDERS 2    /* orders entered from bbs */
#define DISK_ORDERS 3

#include <conio.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lizard.h"
#include "order.h"
#include "map.h"
#include "template.h"

#include "strgfunc.h"
#include "tempfunc.h"

#include "lizgame.h"
#include "lizhex.h"
#include "lizmisc.h"

char data_path [80],
     email_data_path [80],
     game_code [5];

int world.x,
    world.y,
    number_orders,
    world.turn;

char player[MAX_PLAYERS + 1].code[6];
char player_name [MAX_PLAYERS + 1][37];
char player[MAX_PLAYERS + 1].clan_name[37];
char player[MAX_PLAYERS + 1].home_den;
char player_orders_origin [MAX_PLAYERS + 1];
char players_in_game;

char player_road [MAX_PLAYERS + 1][37];
char player_burb [MAX_PLAYERS + 1][37];
char player_city [MAX_PLAYERS + 1][37];
char player_country [MAX_PLAYERS + 1][37];
char player_output_mode [MAX_PLAYERS + 1];
char player_cred [MAX_PLAYERS + 1][10];

char game_due_date [20];

spy_t *spy_list [MAX_SPYS];
int number_spys;

unsigned int spawn_turn_switch = 0,
	     capture_switch = 0,
	     volcano_dormant_switch = 0,
	     peak_active_switch = 0,
	     world_wrap_switch = 0,
	     victory_points_switch = 0,
	     home_owned_worth = 0,
	     home_victory_points = 0,
	     give_switch = 0;

char chant_code [NUMBER_CHANTS][5] = { "EYES", "REVE", "SHUN", "MOUL",
				       "COER", "TRAN", "DARK", "FREE",
				       "SUMM", "GATE" };

unsigned int selected_player = 0;

int main (int argc, char **argv);
void input_order_form (char *filename, field_t **order_template,
		       char **order_data [MAX_PLAYERS + 1],
		       field_t **other_bit_template,
		       char **other_bit);
void order_string_into_template (char *order, char *string, int *order_num,
				 int player, FILE *fptr,
				 field_t **order_template,
				 char **order_data [MAX_PLAYERS + 1],
				 field_t **other_bit_template,
				 char **other_bit);
void output_order_form (field_t **template, char **data, FILE *fptr);
int order_num_from_string (char *string);
int order_ditto (unsigned int *field, field_t **template, char **data);
int check_order_format (unsigned int *field, field_t **template, char **data);
int order_skip_to_field (unsigned int *field, field_t **template, char **data);
void show_order_form (void);

int main (int argc, char **argv)
{
  FILE *fptr;

  char string [256];

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

  printf ("LIZARDS! Order Entry Door. %s Last compiled %s.\n\n",
          VER_STRING, __DATE__);

  if (argc == 6)
  {
    strcpy (data_path, argv [1]);
    strcpy (email_data_path, argv [2]);
    world.turn = atoi (argv [3]);
    strncpy (game_code, argv [4], 3);
    game_code [4] = NULL;
    strupr (game_code);
    selected_player = atoi (argv [5]);
  } else {
    printf ("FATAL ERROR: Turn and Game Number not specified on command line!\n"
	     "             Command Line Format: EXEFILE data_path email_data_path turn game player.\n"
	     "                             e.g. ORDERDOR C:\\ D:\\ 1 A 10\n");

    exit (EXIT_FAILURE);
  }

  get_switch_list ();

  get_player_list ();

  if (players_in_game == 0)
  {
    printf ("FATAL ERROR: Game %s has no players.\n", game_code);

    

    exit (EXIT_FAILURE);
  }

  get_spy_list ();

  get_world(&world);

  order_template = load_template ("DORFORM.TEM", &order_template_fields);
  other_bit_template = load_template ("OTHFORM.TEM", &other_bit_template_fields);

  for (player = 0; player < MAX_PLAYERS + 1; player ++)
    player_orders_origin [player] = NO_ORDERS;

  for (player = 1; player < players_in_game + 1; player ++)
  {
    if ((player_orders [player] = calloc (order_template_fields + 1,
      sizeof (char *))) == NULL)
    {
      printf ("FATAL ERROR: Out of memory for calloc.\n");

      

      exit (EXIT_FAILURE);
    }

    set_field (order_template, player_orders [player], 0, game_code);
    set_field (order_template, player_orders [player], 1, argv [2]);
    set_field (order_template, player_orders [player], 3, itoa (player, string, 10));
    set_field (order_template, player_orders [player], 4, player_name [player]);
    set_field (order_template, player_orders [player], 5, player[player].clan_name);
    set_field (order_template, player_orders [player], 6, player[player].code);
  }

  if ((other_bit = calloc (other_bit_template_fields + 1,
    sizeof (char *))) == NULL)
  {
    printf ("FATAL ERROR: Out of memory for calloc.\n");

    

    exit (EXIT_FAILURE);
  }

  /* check if there's an already existing (partial?) set of orders. If so,
    load into existing tables to allow editing */

  sprintf (string, "%sDOR%02d%03d.%s", email_data_path, selected_player,
    world.turn + 1, game_code);

  input_order_form (string, order_template, player_orders, other_bit_template,
    other_bit);

  player = 1;

  start_field = ORDER_FORM_START_FIELD;

  if (selected_player > players_in_game)
  {
    printf ("FATAL ERROR: Invalid player number for game.\n");

    

    exit (EXIT_FAILURE);
  }

  while (selected_player != 0)
  {
    show_order_form ();

    show_template (order_template, player_orders [selected_player]);

    switch (enter_template (order_template, player_orders [selected_player], start_field,
      &field_in, check_order_format, order_ditto, order_skip_to_field))
    {
      case ERASE_KEY:
        clear_error ();

        show_error ("SURE YOU WANT TO ERASE FORM (Y/N)? ");

        option = toupper (getch ());

        if (option == 'Y')
        {
          clear_error ();

          free_data (order_template, player_orders [selected_player]);

          if ((player_orders [selected_player] = calloc (order_template_fields + 1,
            sizeof (char *))) == NULL)
          {
            printf ("FATAL ERROR: Out of memory for calloc.\n");

            

            exit (EXIT_FAILURE);
          }

          set_field (order_template, player_orders [selected_player], 0, game_code);
          set_field (order_template, player_orders [selected_player], 1, argv [2]);
          set_field (order_template, player_orders [selected_player], 3, itoa (selected_player, string, 10));
          set_field (order_template, player_orders [selected_player], 4, player_name [selected_player]);
          set_field (order_template, player_orders [selected_player], 5, player[selected_player].clan_name);
          set_field (order_template, player_orders [selected_player], 6, player[selected_player].code);
        }

        break;
      case FINISH_KEY:
        clear_error ();

        show_error ("SURE YOU HAVE COMPLETED ORDER ENTRY (Y/N)? ");

	option = toupper (getch ());

	if (option == 'Y')
	{
	  clear_error ();
	  selected_player = 0;
	}

	break;
      case ESCAPE_KEY:
	clear_error ();

	show_error ("SURE YOU WANT TO EXIT ORDER ENTRY UTILITY (Y/N)? ");

	option = toupper (getch ());

	if (option == 'Y')
	  exit (EXIT_FAILURE);

	break;
    }
  }

  sprintf (string, "%sDOR%02d%03d.%s", email_data_path, selected_player,
    world.turn + 1, game_code);

  if ((fptr = fopen (string, "wt")) == NULL)
  {
    printf ("FATAL ERROR: Unable to write to %s file.\n", string);

    

    exit (EXIT_FAILURE);
  }

  for (player = 1; player < players_in_game + 1; player ++)
    output_order_form (order_template, player_orders [player], fptr);

  fclose (fptr);

  free_template (order_template);
  free_template (other_bit_template);

  printf ("\x1B[2J");

  printf ("Finished!\n");

  return (EXIT_SUCCESS);
}

void input_order_form (char *filename, field_t **order_template,
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

  if ((fptr = fopen (filename, "rt")) == NULL)
    return;

  while (fgets (string, 255, fptr) != NULL)
  {
    /* find the order */

    if (string [strlen (string) - 1] == '\n')
      string [strlen (string) - 1] = NULL;

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
      if (selected_player == 0 || (selected_player != 0 && player == selected_player))
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

    if (fgets (string, 255, fptr) != NULL)
    {
      if (string [strlen (string) - 1] == '\n')
	string [strlen (string) - 1] = NULL;

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

      while (fgets (string, 255, fptr) != NULL && line < ORDER_FORM_START_FIELD)
      {
        if (string [strlen (string) - 1] == '\n')
          string [strlen (string) - 1] = NULL;

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

	    while (order [0] != NULL)
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

void output_order_form (field_t **template, char **data, FILE *fptr)
{
  unsigned int present_field = ORDER_FORM_START_FIELD,
	       loop,
	       loop_two,
	       player_orders_f = FALSE;

  /* check to see if player actually has any orders */

  for (loop = 0, present_field = ORDER_FORM_START_FIELD; loop < 30; loop ++, present_field += 10)
    if (data [present_field] != NULL)
      player_orders_f = TRUE;

  if (!player_orders_f)
    if (data [MESSAGE_LINE1_FIELD] != NULL || data [MESSAGE_LINE2_FIELD] != NULL)
      player_orders_f = TRUE;

  if (!player_orders_f) return; /* no orders, no output! */

  fprintf (fptr, "PLAYER %s", data [PLAYER_NUMBER_FIELD]);

  if (data [ORDER_ORIGIN_FIELD] == NULL)
    fprintf (fptr, "\n");
  else
    fprintf (fptr, " %c\n", data [ORDER_ORIGIN_FIELD][0]);

  for (loop = 0, present_field = ORDER_FORM_START_FIELD; loop < 30; loop ++, present_field += 10)
    if (data [present_field] != NULL)
    {
      switch (order_num_from_string (data [present_field]))
      {
	case MOVE:
	  if (data [present_field + 1] != NULL &&
	    (data [present_field + 2] != NULL ||
	     data [present_field + 3] != NULL))
	  {
	    fprintf (fptr, "MO %s ", data [present_field + 1]);

	    if (data [present_field + 2] != NULL)
	      fprintf (fptr, "%s ", (data [present_field + 2]));

	    if (data [present_field + 3] != NULL)
	      fprintf (fptr, "%s ", (data [present_field + 3]));

	    fprintf (fptr, "\n");
	  } else {
	    /* screwed order output */

	    fprintf (fptr, "?? ");

	    for (loop_two = 0; loop_two < 10; loop_two ++)
	      if (data [present_field + loop_two] != NULL)
		fprintf (fptr, "%s ", data [present_field + loop_two]);
	      else
		fprintf (fptr, "? ");

	    fprintf (fptr, "\n");
	  }

	  break;
	case SPLIT:
	  if (data [present_field + 1] != NULL &&
	    (data [present_field + 2] != NULL ||
	     data [present_field + 3] != NULL) &&
	    (data [present_field + 4] != NULL ||
	     data [present_field + 5] != NULL ||
	     data [present_field + 6] != NULL ||
	     data [present_field + 7] != NULL ||
	     data [present_field + 8] != NULL))
	  {
	    fprintf (fptr, "SP %s ", data [present_field + 1]);

	    if (data [present_field + 4] != NULL)
	      fprintf (fptr, "%sRED ", (data [present_field + 4]));

	    if (data [present_field + 5] != NULL)
	      fprintf (fptr, "%sGRN ", (data [present_field + 5]));

	    if (data [present_field + 6] != NULL)
	      fprintf (fptr, "%sGRY ", (data [present_field + 6]));

	    if (data [present_field + 7] != NULL)
	      fprintf (fptr, "%sYEL ", (data [present_field + 7]));

	    if (data [present_field + 8] != NULL)
	      fprintf (fptr, "%sBLK ", (data [present_field + 8]));

	    if (data [present_field + 2] != NULL)
	      fprintf (fptr, "%s ", (data [present_field + 2]));

	    if (data [present_field + 3] != NULL)
	      fprintf (fptr, "%s ", (data [present_field + 3]));

	    fprintf (fptr, "\n");
	  } else {
	    /* screwed order output */

	    fprintf (fptr, "?? ");

	    for (loop_two = 0; loop_two < 10; loop_two ++)
	      if (data [present_field + loop_two] != NULL)
		fprintf (fptr, "%s ", data [present_field + loop_two]);
	      else
		fprintf (fptr, "? ");

	    fprintf (fptr, "\n");
	  }

	  break;
	case BUILD_FERTILE:
	  if (data [present_field + 1] != NULL)
	    fprintf (fptr, "BF %s\n", data [present_field + 1]);
	  else {
	    /* screwed order output */

	    fprintf (fptr, "?? ");

	    for (loop_two = 0; loop_two < 10; loop_two ++)
	      if (data [present_field + loop_two] != NULL)
		fprintf (fptr, "%s ", data [present_field + loop_two]);
	      else
		fprintf (fptr, "? ");

	    fprintf (fptr, "\n");
	  }

	  break;
	case BUILD_DEN:
	  if (data [present_field + 1] != NULL)
	    fprintf (fptr, "BD %s\n", data [present_field + 1]);
	  else {
	    /* screwed order output */

	    fprintf (fptr, "?? ");

	    for (loop_two = 0; loop_two < 10; loop_two ++)
	      if (data [present_field + loop_two] != NULL)
		fprintf (fptr, "%s ", data [present_field + loop_two]);
	      else
		fprintf (fptr, "? ");

	    fprintf (fptr, "\n");
	  }

	  break;
	case BUILD_BRIDGE:
	  if (data [present_field + 1] != NULL &&
	    data [present_field + 2] != NULL)
	    fprintf (fptr, "BB %s %s\n", data [present_field + 1],
	      data [present_field + 2]);
	  else {
	    /* screwed order output */

	    fprintf (fptr, "?? ");

	    for (loop_two = 0; loop_two < 10; loop_two ++)
	      if (data [present_field + loop_two] != NULL)
		fprintf (fptr, "%s ", data [present_field + loop_two]);
	      else
		fprintf (fptr, "? ");

	    fprintf (fptr, "\n");
	  }

	  break;
	case DESTROY_FERTILE:
	  if (data [present_field + 1] != NULL)
	    fprintf (fptr, "DF %s\n", data [present_field + 1]);
	  else {
	    /* screwed order output */

	    fprintf (fptr, "?? ");

	    for (loop_two = 0; loop_two < 10; loop_two ++)
	      if (data [present_field + loop_two] != NULL)
		fprintf (fptr, "%s ", data [present_field + loop_two]);
	      else
		fprintf (fptr, "? ");

	    fprintf (fptr, "\n");
	  }

	  break;
	case DESTROY_BRIDGE:
	  if (data [present_field + 1] != NULL &&
	    data [present_field + 2] != NULL)
	    fprintf (fptr, "DB %s %s\n", data [present_field + 1],
	      data [present_field + 2]);
	  else {
	    /* screwed order output */

	    fprintf (fptr, "?? ");

	    for (loop_two = 0; loop_two < 10; loop_two ++)
	      if (data [present_field + loop_two] != NULL)
		fprintf (fptr, "%s ", data [present_field + loop_two]);
	      else
		fprintf (fptr, "? ");

	    fprintf (fptr, "\n");
	  }

	  break;
	case RECRUIT:
	  if (data [present_field + 1] != NULL)
	    fprintf (fptr, "RE %s\n", data [present_field + 1]);
	  else {
	    /* screwed order output */

	    fprintf (fptr, "?? ");

	    for (loop_two = 0; loop_two < 10; loop_two ++)
	      if (data [present_field + loop_two] != NULL)
		fprintf (fptr, "%s ", data [present_field + loop_two]);
	      else
		fprintf (fptr, "? ");

	    fprintf (fptr, "\n");
	  }

	  break;
	case CHANT:
	  if (data [present_field + 1] != NULL &&
	    (data [present_field + 2] != NULL ||
	     data [present_field + 3] != NULL) &&
	     data [present_field + 9] != NULL)
	  {
	    fprintf (fptr, "CH %s ", data [present_field + 1]);

	    if (data [present_field + 8] != NULL)
	      fprintf (fptr, "%sBLK ", (data [present_field + 8]));

	    if (data [present_field + 9] != NULL)
	      fprintf (fptr, "%s ", (data [present_field + 9]));

	    if (data [present_field + 2] != NULL)
	      fprintf (fptr, "%s ", (data [present_field + 2]));

	    if (data [present_field + 3] != NULL)
	      fprintf (fptr, "%s ", (data [present_field + 3]));

	    fprintf (fptr, "\n");
	  } else {
	    /* screwed order output */

	    fprintf (fptr, "?? ");

	    for (loop_two = 0; loop_two < 10; loop_two ++)
	      if (data [present_field + loop_two] != NULL)
		fprintf (fptr, "%s ", data [present_field + loop_two]);
	      else
		fprintf (fptr, "? ");

	    fprintf (fptr, "\n");
	  }

	  break;
	case LOB:
	  if (data [present_field + 1] != NULL &&
	    (data [present_field + 2] != NULL ||
	     data [present_field + 3] != NULL))
	  {
	    fprintf (fptr, "LO %s ", data [present_field + 1]);

	    if (data [present_field + 2] != NULL)
	      fprintf (fptr, "%s ", (data [present_field + 2]));

	    if (data [present_field + 3] != NULL)
	      fprintf (fptr, "%s ", (data [present_field + 3]));

	    fprintf (fptr, "\n");
	  } else {
	    /* screwed order output */

	    fprintf (fptr, "?? ");

	    for (loop_two = 0; loop_two < 10; loop_two ++)
	      if (data [present_field + loop_two] != NULL)
		fprintf (fptr, "%s ", data [present_field + loop_two]);
	      else
		fprintf (fptr, "? ");

	    fprintf (fptr, "\n");
	  }

	  break;
	case GIVE:
	  if (data [present_field + 1] != NULL &&
	    data [present_field + 9] != NULL)
	    fprintf (fptr, "GI %s %s\n", data [present_field + 1],
	      data [present_field + 9]);
	  else {
	    /* screwed order output */

	    fprintf (fptr, "?? ");

	    for (loop_two = 0; loop_two < 10; loop_two ++)
	      if (data [present_field + loop_two] != NULL)
		fprintf (fptr, "%s ", data [present_field + loop_two]);
	      else
		fprintf (fptr, "? ");

	    fprintf (fptr, "\n");
	  }

	  break;
	case FREE:
	  if (data [present_field + 1] != NULL)
	  {
	    fprintf (fptr, "FR %s ", data [present_field + 1]);

	    if (data [present_field + 4] != NULL)
	      fprintf (fptr, "%sRED ", (data [present_field + 4]));

	    if (data [present_field + 5] != NULL)
	      fprintf (fptr, "%sGRN ", (data [present_field + 5]));

	    if (data [present_field + 6] != NULL)
	      fprintf (fptr, "%sGRY ", (data [present_field + 6]));

	    if (data [present_field + 7] != NULL)
	      fprintf (fptr, "%sYEL ", (data [present_field + 7]));

	    if (data [present_field + 8] != NULL)
	      fprintf (fptr, "%sBLK ", (data [present_field + 8]));

	    fprintf (fptr, "\n");
	  } else {
	    /* screwed order output */

	    fprintf (fptr, "?? ");

	    for (loop_two = 0; loop_two < 10; loop_two ++)
	      if (data [present_field + loop_two] != NULL)
		fprintf (fptr, "%s ", data [present_field + loop_two]);
	      else
		fprintf (fptr, "? ");

	    fprintf (fptr, "\n");
	  }

	  break;
	case BUILD_RAFT:
	  if (data [present_field + 1] != NULL &&
	    data [present_field + 2] != NULL)
	    fprintf (fptr, "BR %s %s\n", data [present_field + 1],
	      data [present_field + 2]);
	  else {
	    /* screwed order output */

	    fprintf (fptr, "?? ");

	    for (loop_two = 0; loop_two < 10; loop_two ++)
	      if (data [present_field + loop_two] != NULL)
		fprintf (fptr, "%s ", data [present_field + loop_two]);
	      else
		fprintf (fptr, "? ");

	    fprintf (fptr, "\n");
	  }

	  break;
	case SAIL:
	  if (data [present_field + 1] != NULL &&
	    (data [present_field + 2] != NULL ||
	     data [present_field + 3] != NULL))
	  {
	    fprintf (fptr, "SA %s ", data [present_field + 1]);

	    if (data [present_field + 2] != NULL)
	      fprintf (fptr, "%s ", (data [present_field + 2]));

	    if (data [present_field + 3] != NULL)
	      fprintf (fptr, "%s ", (data [present_field + 3]));

	    fprintf (fptr, "\n");
	  } else {
	    /* screwed order output */

	    fprintf (fptr, "?? ");

	    for (loop_two = 0; loop_two < 10; loop_two ++)
	      if (data [present_field + loop_two] != NULL)
		fprintf (fptr, "%s ", data [present_field + loop_two]);
	      else
		fprintf (fptr, "? ");

	    fprintf (fptr, "\n");
	  }

	  break;
	case SNEAK:
	  if (data [present_field + 1] != NULL &&
	    (data [present_field + 2] != NULL ||
	     data [present_field + 3] != NULL))
	  {
	    fprintf (fptr, "SN %s ", data [present_field + 1]);

	    if (data [present_field + 2] != NULL)
	      fprintf (fptr, "%s ", (data [present_field + 2]));

	    if (data [present_field + 3] != NULL)
	      fprintf (fptr, "%s ", (data [present_field + 3]));

	    fprintf (fptr, "\n");
	  } else {
	    /* screwed order output */

	    fprintf (fptr, "?? ");

	    for (loop_two = 0; loop_two < 10; loop_two ++)
	      if (data [present_field + loop_two] != NULL)
		fprintf (fptr, "%s ", data [present_field + loop_two]);
	      else
		fprintf (fptr, "? ");

	    fprintf (fptr, "\n");
	  }

	  break;
	case KILL_PLAYER:
	  fprintf (fptr, "KI\n");

	  break;
	default:
	  /* screwed order output */

	  fprintf (fptr, "?? ");

	  for (loop_two = 0; loop_two < 10; loop_two ++)
	    if (data [present_field + loop_two] != NULL)
	      fprintf (fptr, "%s ", data [present_field + loop_two]);
	    else
	      fprintf (fptr, "? ");

	  fprintf (fptr, "\n");

	  break;
      }
    }

  if (data [MESSAGE_LINE1_FIELD] != NULL || data [MESSAGE_LINE2_FIELD] != NULL)
  {
    fprintf (fptr, "MS ");

    if (data [MESSAGE_HEADER_FIELD] != NULL)
      fprintf (fptr, "%s\n", data [MESSAGE_HEADER_FIELD]);
    else
      fprintf (fptr, "\n");

    if (data [MESSAGE_LINE1_FIELD] != NULL)
      fprintf (fptr, "%s", data [MESSAGE_LINE1_FIELD]);

    if (data [MESSAGE_LINE2_FIELD] != NULL)
      fprintf (fptr, "%s", data [MESSAGE_LINE2_FIELD]);

    fprintf (fptr, "\nENDMESG\n");
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
    if (data [*field] != NULL)
    {
      free (data [*field]);
      data [*field] = NULL;
    }

    if (data [*field + template [*field]->jump_up] != NULL)
      set_field (template, data, *field,
        data [*field + template [*field]->jump_up]);
    else
      data [*field] = NULL;

    show_field (template [*field], data [*field]);

    (*field) ++;

    return (TRUE);
  }
  return (FALSE);
}

int check_order_format (unsigned int *field, field_t **template, char **data)
{
  char error [255];

  int x, y, player, loop, lizards;

  hex_t hex;

  player = atoi (data [PLAYER_NUMBER_FIELD]);

  while (template [*field]->start_of_line_f != 1 &&
    template [*field]->start_of_line_f != -1)
    (*field) --;

  if (template [*field]->start_of_line_f == -1) return (FALSE);

  if (data [*field] != NULL)
  {
    switch (order_num_from_string (data [*field]))
    {
      case MOVE:
        if (data [*field + 1] == NULL)
        {
          show_error ("ERROR: MOVE ORDER WITH NO SPECIFIED HEX!");

          return (FALSE);
        }

        string_to_loc (data [*field + 1], &x, &y);

        if (x < 0 || y < 0 || x >= world.x || y >= world.y)
        {
          show_error ("ERROR: MOVE ORDER WITH INVALID HEX!");

          return (FALSE);
        }

        if (data [*field + 2] == NULL &&
          data [*field + 3] == NULL)
        {
          show_error ("ERROR: MOVE ORDER WITH NO DIRECTIONS!");

          return (FALSE);
        }

        if (string_to_direction (data [*field + 2]) == 0 &&
          data [*field + 2] != NULL)
        {
          show_error ("ERROR: MOVE ORDER WITH INVALID DIRECTION!");

          return (FALSE);
        }

        if (string_to_direction (data [*field + 3]) == 0 &&
          data [*field + 3] != NULL)
        {
          show_error ("ERROR: MOVE ORDER WITH INVALID DIRECTION!");

          return (FALSE);
        }

        if (data [*field + 4] != NULL || data [*field + 5] != NULL ||
          data [*field + 6] != NULL || data [*field + 7] != NULL ||
          data [*field + 8] != NULL || data [*field + 9] != NULL)
        {
          show_error ("ERROR: MOVE ORDER WITH EXTRA INFORMATION!");

          return (FALSE);
        }

        /* load hex and see if player has any lizards there */

        get_hex (x, y, &hex);

        if (lizards_in_hex (&hex) == 0)
        {
          show_error ("MOVE ORDER INVALID: HEX CONTAINS NO LIZARDS!");

          return (FALSE);
        } else
          if (hex.hex_owner != player)
          {
            show_error ("MOVE ORDER INVALID: HEX CONTAINS NO LIZARDS OWNED BY PLAYER!");

            return (FALSE);
          }

        break;
      case SPLIT:
        if (data [*field + 1] == NULL)
        {
          show_error ("ERROR: SPLIT ORDER WITH NO SPECIFIED HEX!");

          return (FALSE);
        }

        string_to_loc (data [*field + 1], &x, &y);

        if (x < 0 || y < 0 || x >= world.x || y >= world.y)
        {
          show_error ("ERROR: SPLIT ORDER WITH INVALID HEX!");

          return (FALSE);
        }

        if (data [*field + 2] == NULL &&
          data [*field + 3] == NULL)
        {
          show_error ("ERROR: SPLIT ORDER WITH NO DIRECTIONS!");

          return (FALSE);
        }

        if (string_to_direction (data [*field + 2]) == 0 &&
          data [*field + 2] != NULL)
        {
          show_error ("ERROR: SPLIT ORDER WITH INVALID DIRECTION!");

          return (FALSE);
        }

        if (string_to_direction (data [*field + 3]) == 0 &&
          data [*field + 3] != NULL)
        {
          show_error ("ERROR: SPLIT ORDER WITH INVALID DIRECTION!");

          return (FALSE);
        }

        if (data [*field + 4] == NULL &&
          data [*field + 5] == NULL &&
          data [*field + 6] == NULL &&
          data [*field + 7] == NULL &&
          data [*field + 8] == NULL)
        {
          show_error ("ERROR: SPLIT ORDER WITH NO LIZARDS!");

          return (FALSE);
        }

	for (lizards = -1, loop = 4; loop < 9; loop ++)
	  if (data [*field + loop] != NULL)
	    if (lizards == -1)
	      lizards = atoi (data [*field + loop]);
	    else
	      lizards += atoi (data [*field + loop]);

	if (lizards == 0)
	{
	  show_error ("ERROR: SPLIT ORDER WITH NO LIZARDS!");

	  return (FALSE);
	}

        if (data [*field + 9] != NULL)
        {
          show_error ("ERROR: SPLIT ORDER WITH EXTRA INFORMATION!");

          return (FALSE);
        }

        /* load hex and see if player has any lizards there */

        get_hex (x, y, &hex);

        if (lizards_in_hex (&hex) == 0)
        {
          show_error ("SPLIT ORDER INVALID: HEX CONTAINS NO LIZARDS!");

          return (FALSE);
        } else
          if (hex.hex_owner != player)
          {
            show_error ("SPLIT ORDER INVALID: HEX CONTAINS NO LIZARDS OWNED BY PLAYER!");

            return (FALSE);
          } else {
	    if (data [*field + 4] != NULL)
	    {
	      lizards = atoi (data [*field + 4]);

	      if (lizards > hex.red_lizards)
		if (hex.red_lizards == 0)
		{
		  show_error ("SPLIT ORDER INVALID: HEX CONTAINS NO RED LIZARDS!");

		  return (FALSE);
		} else {
		  sprintf (error, "SPLIT ORDER INVALID: HEX CONTAINS ONLY %d RED LIZARDS!",
		    hex.red_lizards);

		  show_error (error);

		  return (FALSE);
		}
	    }

	    if (data [*field + 5] != NULL)
	    {
	      lizards = atoi (data [*field + 5]);

	      if (lizards > hex.green_lizards)
		if (hex.green_lizards == 0)
		{
		  show_error ("SPLIT ORDER INVALID: HEX CONTAINS NO GREEN LIZARDS!");

		  return (FALSE);
		} else {
		  sprintf (error, "SPLIT ORDER INVALID: HEX CONTAINS ONLY %d GREEN LIZARDS!",
		    hex.green_lizards);

		  show_error (error);

		  return (FALSE);
		}
	    }

	    if (data [*field + 6] != NULL)
	    {
	      lizards = atoi (data [*field + 6]);

	      if (lizards > hex.grey_lizards)
		if (hex.grey_lizards == 0)
		{
		  show_error ("SPLIT ORDER INVALID: HEX CONTAINS NO GREY LIZARDS!");

		  return (FALSE);
		} else {
		  sprintf (error, "SPLIT ORDER INVALID: HEX CONTAINS ONLY %d GREY LIZARDS!",
		    hex.grey_lizards);

		  show_error (error);

		  return (FALSE);
		}
	    }

	    if (data [*field + 7] != NULL)
	    {
	      lizards = atoi (data [*field + 7]);

	      if (lizards > hex.yellow_lizards)
		if (hex.yellow_lizards == 0)
		{
		  show_error ("SPLIT ORDER INVALID: HEX CONTAINS NO YELLOW LIZARDS!");

		  return (FALSE);
		} else {
		  sprintf (error, "SPLIT ORDER INVALID: HEX CONTAINS ONLY %d YELLOW LIZARDS!",
		    hex.yellow_lizards);

		  show_error (error);

		  return (FALSE);
		}
	    }

	    if (data [*field + 8] != NULL)
	    {
	      lizards = atoi (data [*field + 8]);

	      if (lizards > hex.black_lizards)
		if (hex.black_lizards == 0)
		{
		  show_error ("SPLIT ORDER INVALID: HEX CONTAINS NO BLACK LIZARDS!");

		  return (FALSE);
		} else {
		  sprintf (error, "SPLIT ORDER INVALID: HEX CONTAINS ONLY %d BLACK LIZARDS!",
		    hex.black_lizards);

		  show_error (error);

		  return (FALSE);
		}
	    }
	  }

        break;
      case BUILD_FERTILE:
        if (data [*field + 1] == NULL)
        {
          show_error ("ERROR: BUILD FERTILE ORDER WITH NO SPECIFIED HEX!");

          return (FALSE);
        }

        string_to_loc (data [*field + 1], &x, &y);

        if (x < 0 || y < 0 || x >= world.x || y >= world.y)
        {
          show_error ("ERROR: BUILD FERTILE ORDER WITH INVALID HEX!");

          return (FALSE);
        }

        if (data [*field + 2] != NULL || data [*field + 3] != NULL ||
          data [*field + 4] != NULL || data [*field + 5] != NULL ||
          data [*field + 6] != NULL || data [*field + 7] != NULL ||
          data [*field + 8] != NULL || data [*field + 9] != NULL)
        {
          show_error ("ERROR: BUILD FERTILE ORDER WITH EXTRA INFORMATION!");

          return (FALSE);
        }

        /* load hex and see if player has any lizards there */

        get_hex (x, y, &hex);

        if (lizards_in_hex (&hex) == 0)
        {
          show_error ("BUILD FERTILE ORDER INVALID: HEX CONTAINS NO LIZARDS!");

          return (FALSE);
        } else
          if (hex.hex_owner != player)
          {
            show_error ("BUILD FERTILE ORDER INVALID: HEX CONTAINS NO LIZARDS OWNED BY PLAYER!");

            return (FALSE);
          } else {
            if (hex.green_lizards < BUILD_FERTILE_LEVEL)
            {
              show_error ("BUILD FERTILE ORDER INVALID: HEX CONTAINS INSUFFICIENT GREEN LIZARDS!");

              return (FALSE);
            }
          }

        break;
      case BUILD_DEN:
        if (data [*field + 1] == NULL)
        {
          show_error ("ERROR: BUILD DEN ORDER WITH NO SPECIFIED HEX!");

          return (FALSE);
        }

        string_to_loc (data [*field + 1], &x, &y);

        if (x < 0 || y < 0 || x >= world.x || y >= world.y)
        {
          show_error ("ERROR: BUILD DEN ORDER WITH INVALID HEX!");

          return (FALSE);
        }

        if (data [*field + 2] != NULL || data [*field + 3] != NULL ||
          data [*field + 4] != NULL || data [*field + 5] != NULL ||
          data [*field + 6] != NULL || data [*field + 7] != NULL ||
          data [*field + 8] != NULL || data [*field + 9] != NULL)
        {
          show_error ("ERROR: BUILD DEN ORDER WITH EXTRA INFORMATION!");

          return (FALSE);
        }

        /* load hex and see if player has any lizards there */

        get_hex (x, y, &hex);

        if (lizards_in_hex (&hex) == 0)
        {
          show_error ("BUILD DEN ORDER INVALID: HEX CONTAINS NO LIZARDS!");

          return (FALSE);
        } else
          if (hex.hex_owner != player)
          {
            show_error ("BUILD DEN ORDER INVALID: HEX CONTAINS NO LIZARDS OWNED BY PLAYER!");

            return (FALSE);
          } else {
            if (hex.grey_lizards < BUILD_DEN_LEVEL)
            {
              show_error ("BUILD DEN ORDER INVALID: HEX CONTAINS INSUFFICIENT GREY LIZARDS!");

              return (FALSE);
            }
          }

        break;
      case BUILD_BRIDGE:
        if (data [*field + 1] == NULL)
        {
          show_error ("ERROR: BUILD BRIDGE ORDER WITH NO SPECIFIED HEX!");

          return (FALSE);
        }

        string_to_loc (data [*field + 1], &x, &y);

        if (x < 0 || y < 0 || x >= world.x || y >= world.y)
        {
          show_error ("ERROR: BUILD BRIDGE ORDER WITH INVALID HEX!");

          return (FALSE);
        }

        if (data [*field + 2] == NULL)
        {
          show_error ("ERROR: BUILD BRIDGE ORDER WITH NO DIRECTION!");

          return (FALSE);
        }

        if (string_to_direction (data [*field + 2]) == 0  &&
          data [*field + 2] != NULL)
        {
          show_error ("ERROR: BUILD BRDIGE ORDER WITH INVALID DIRECTION!");

          return (FALSE);
        }

	if (data [*field + 3] != NULL || data [*field + 4] != NULL ||
	  data [*field + 5] != NULL || data [*field + 6] != NULL ||
	  data [*field + 7] != NULL || data [*field + 8] != NULL ||
	  data [*field + 9] != NULL)
	{
	  show_error ("ERROR: BUILD BRIDGE ORDER WITH EXTRA INFORMATION!");

	  return (FALSE);
	}

	/* load hex and see if player has any lizards there */

	get_hex (x, y, &hex);

	if (lizards_in_hex (&hex) == 0)
	{
	  show_error ("BUILD BRIDGE ORDER INVALID: HEX CONTAINS NO LIZARDS!");

	  return (FALSE);
	} else
	  if (hex.hex_owner != player)
	  {
	    show_error ("BUILD BRIDGE ORDER INVALID: HEX CONTAINS NO LIZARDS OWNED BY PLAYER!");

	    return (FALSE);
	  } else {
	    if (hex.grey_lizards < BUILD_BRIDGE_LEVEL)
	    {
	      show_error ("BUILD BRIDGE ORDER INVALID: HEX CONTAINS INSUFFICIENT GREY LIZARDS!");

	      return (FALSE);
	    }
	  }

	break;
      case DESTROY_FERTILE:
	if (data [*field + 1] == NULL)
	{
	  show_error ("ERROR: DESTROY FERTILE ORDER WITH NO SPECIFIED HEX!");

	  return (FALSE);
	}

        string_to_loc (data [*field + 1], &x, &y);

        if (x < 0 || y < 0 || x >= world.x || y >= world.y)
        {
          show_error ("ERROR: DESTROY FERTILE ORDER WITH INVALID HEX!");

          return (FALSE);
        }

        if (data [*field + 2] != NULL || data [*field + 3] != NULL ||
          data [*field + 4] != NULL || data [*field + 5] != NULL ||
          data [*field + 6] != NULL || data [*field + 7] != NULL ||
          data [*field + 8] != NULL || data [*field + 9] != NULL)
        {
          show_error ("ERROR: DESTROY FERTILE ORDER WITH EXTRA INFORMATION!");

          return (FALSE);
        }

        /* load hex and see if player has any lizards there */

        get_hex (x, y, &hex);

        if (lizards_in_hex (&hex) == 0)
        {
          show_error ("DESTROY FERTILE ORDER INVALID: HEX CONTAINS NO LIZARDS!");

          return (FALSE);
        } else
          if (hex.hex_owner != player)
          {
            show_error ("DESTROY FERTILE ORDER INVALID: HEX CONTAINS NO LIZARDS OWNED BY PLAYER!");

            return (FALSE);
          } else {
            if (hex.green_lizards < BUILD_FERTILE_LEVEL)
            {
              show_error ("DESTROY FERTILE ORDER INVALID: HEX CONTAINS INSUFFICIENT GREEN LIZARDS!");

              return (FALSE);
            }
          }

        break;
      case DESTROY_BRIDGE:
        if (data [*field + 1] == NULL)
        {
          show_error ("ERROR: DESTROY BRIDGE ORDER WITH NO SPECIFIED HEX!");

          return (FALSE);
        }

        string_to_loc (data [*field + 1], &x, &y);

        if (x < 0 || y < 0 || x >= world.x || y >= world.y)
        {
          show_error ("ERROR: DESTROY BRIDGE ORDER WITH INVALID HEX!");

          return (FALSE);
        }

        if (data [*field + 2] == NULL)
        {
          show_error ("ERROR: DESTROY BRIDGE ORDER WITH NO DIRECTION!");

          return (FALSE);
        }

        if (string_to_direction (data [*field + 2]) == 0  &&
          data [*field + 2] != NULL)
        {
          show_error ("ERROR: DESTROY BRDIGE ORDER WITH INVALID DIRECTION!");

          return (FALSE);
        }

        if (data [*field + 3] != NULL || data [*field + 4] != NULL ||
          data [*field + 5] != NULL || data [*field + 6] != NULL ||
          data [*field + 7] != NULL || data [*field + 8] != NULL ||
          data [*field + 9] != NULL)
        {
          show_error ("ERROR: DESTROY BRIDGE ORDER WITH EXTRA INFORMATION!");

          return (FALSE);
        }

        /* load hex and see if player has any lizards there */

        get_hex (x, y, &hex);

        if (lizards_in_hex (&hex) == 0)
        {
          show_error ("DESTROY BRIDGE ORDER INVALID: HEX CONTAINS NO LIZARDS!");

          return (FALSE);
        } else
          if (hex.hex_owner != player)
          {
            show_error ("DESTROY BRIDGE ORDER INVALID: HEX CONTAINS NO LIZARDS OWNED BY PLAYER!");

            return (FALSE);
          } else {
            if (hex.grey_lizards < BUILD_BRIDGE_LEVEL)
            {
              show_error ("DESTROY BRIDGE ORDER INVALID: HEX CONTAINS INSUFFICIENT GREY LIZARDS!");

              return (FALSE);
            }
          }

        break;
      case RECRUIT:
        if (data [*field + 1] == NULL)
        {
          show_error ("ERROR: RECRUIT ORDER WITH NO SPECIFIED HEX!");

          return (FALSE);
        }

        string_to_loc (data [*field + 1], &x, &y);

        if (x < 0 || y < 0 || x >= world.x || y >= world.y)
        {
          show_error ("ERROR: RECRUIT ORDER WITH INVALID HEX!");

          return (FALSE);
        }

        if (data [*field + 2] != NULL || data [*field + 3] != NULL ||
          data [*field + 4] != NULL || data [*field + 5] != NULL ||
          data [*field + 6] != NULL || data [*field + 7] != NULL ||
          data [*field + 8] != NULL || data [*field + 9] != NULL)
        {
          show_error ("ERROR: RECRUIT ORDER WITH EXTRA INFORMATION!");

          return (FALSE);
        }

        /* load hex and see if player has any lizards there */

        get_hex (x, y, &hex);

        if (hex.terrain != DEN && hex.terrain != HOME_DEN)
        {
          show_error ("RECRUIT ORDER INVALID: HEX DOESN'T CONTAIN DEN!");

          return (FALSE);
        } else
          if (hex.hex_owner != player)
          {
            show_error ("RECRUIT ORDER INVALID: DEN HEX CONTAINS NOT OWNED BY PLAYER!");

            return (FALSE);
          }

        break;
      case CHANT:
        if (data [*field + 1] == NULL)
        {
          show_error ("ERROR: CHANT ORDER WITH NO SPECIFIED HEX!");

          return (FALSE);
        }

        string_to_loc (data [*field + 1], &x, &y);

        if (x < 0 || y < 0 || x >= world.x || y >= world.y)
        {
          show_error ("ERROR: CHANT ORDER WITH INVALID HEX!");

          return (FALSE);
        }

        if (data [*field + 9] == NULL)
        {
          show_error ("ERROR: CHANT ORDER WITH NO SPECIFIED CHANT!");

          return (FALSE);
        }

	for (loop = 0; loop < NUMBER_CHANTS; loop ++)
	  if (strcmp (chant_code [loop], data [*field + 9]) == 0)
	    break;

	if (loop == NUMBER_CHANTS)
	{
	  show_error ("ERROR: CHANT ORDER WITH UNKNOWN CHANT SPECIFIED!");

	  return (FALSE);
	}

        if (data [*field + 4] != NULL || data [*field + 5] != NULL ||
          data [*field + 6] != NULL || data [*field + 7] != NULL)
        {
          show_error ("ERROR: CHANT ORDER WITH EXTRA INFORMATION!");

          return (FALSE);
        }

        /* load hex and see if player has any lizards there */

        get_hex (x, y, &hex);

        if (lizards_in_hex (&hex) == 0)
        {
          show_error ("CHANT ORDER INVALID: HEX CONTAINS NO LIZARDS!");

          return (FALSE);
        } else
          if (hex.hex_owner != player)
          {
            show_error ("CHANT ORDER INVALID: HEX CONTAINS NO LIZARDS OWNED BY PLAYER!");

            return (FALSE);
          } else {
            if (hex.black_lizards == 0)
            {
              show_error ("CHANT ORDER INVALID: HEX CONTAINS NO BLACK LIZARDS!");

              return (FALSE);
            }
          }

        break;
      case GIVE:
        if (data [*field + 1] == NULL)
        {
          show_error ("ERROR: GIVE ORDER WITH NO SPECIFIED HEX!");

          return (FALSE);
        }

        string_to_loc (data [*field + 1], &x, &y);

        if (x < 0 || y < 0 || x >= world.x || y >= world.y)
        {
          show_error ("ERROR: GIVE ORDER WITH INVALID HEX!");

          return (FALSE);
        }

        if (data [*field + 9] == NULL)
        {
          show_error ("ERROR: GIVE ORDER WITH NO SPECIFIED PLAYER!");

          return (FALSE);
        }

        if (data [*field + 2] != NULL || data [*field + 3] != NULL ||
          data [*field + 4] != NULL || data [*field + 5] != NULL ||
          data [*field + 6] != NULL || data [*field + 7] != NULL ||
          data [*field + 8] != NULL)
        {
          show_error ("ERROR: GIVE ORDER WITH EXTRA INFORMATION!");

          return (FALSE);
        }

        /* load hex and see if player has any lizards there */

        get_hex (x, y, &hex);

        if (hex.hex_owner != player)
        {
          show_error ("GIVE ORDER INVALID: HEX IS NO CONTROLLED BY PLAYER!");

          return (FALSE);
        }

        break;
      case LOB:
        if (data [*field + 1] == NULL)
        {
          show_error ("ERROR: LOB ORDER WITH NO SPECIFIED HEX!");

          return (FALSE);
        }

        string_to_loc (data [*field + 1], &x, &y);

        if (x < 0 || y < 0 || x >= world.x || y >= world.y)
        {
          show_error ("ERROR: LOB ORDER WITH INVALID HEX!");

          return (FALSE);
        }

        if (data [*field + 2] == NULL &&
          data [*field + 3] == NULL)
        {
          show_error ("ERROR: LOB ORDER WITH NO DIRECTIONS!");

          return (FALSE);
        }

        if (string_to_direction (data [*field + 2]) == 0 &&
          data [*field + 2] != NULL)
        {
          show_error ("ERROR: LOB ORDER WITH INVALID DIRECTION!");

          return (FALSE);
        }

        if (string_to_direction (data [*field + 3]) == 0 &&
          data [*field + 3] != NULL)
        {
          show_error ("ERROR: LOB ORDER WITH INVALID DIRECTION!");

          return (FALSE);
        }

        if (data [*field + 4] != NULL || data [*field + 5] != NULL ||
          data [*field + 6] != NULL || data [*field + 7] != NULL ||
          data [*field + 8] != NULL || data [*field + 9] != NULL)
        {
          show_error ("ERROR: LOB ORDER WITH EXTRA INFORMATION!");

          return (FALSE);
        }

        /* load hex and see if player has any lizards there */

        get_hex (x, y, &hex);

        if (lizards_in_hex (&hex) == 0)
        {
          show_error ("LOB ORDER INVALID: HEX CONTAINS NO LIZARDS!");

          return (FALSE);
        } else
          if (hex.hex_owner != player)
          {
            show_error ("LOB ORDER INVALID: HEX CONTAINS NO LIZARDS OWNED BY PLAYER!");

            return (FALSE);
          } else {
            if (hex.yellow_lizards == 0)
            {
              show_error ("LOB ORDER INVALID: HEX CONTAINS NO YELLOW LIZARDS!");

              return (FALSE);
            }
          }

        break;
      case FREE:
        if (data [*field + 1] == NULL)
        {
          show_error ("ERROR: FREE ORDER WITH NO SPECIFIED HEX!");

          return (FALSE);
        }

        string_to_loc (data [*field + 1], &x, &y);

        if (x < 0 || y < 0 || x >= world.x || y >= world.y)
        {
          show_error ("ERROR: FREE ORDER WITH INVALID HEX!");

          return (FALSE);
        }

        if (data [*field + 2] != NULL || data [*field + 3] != NULL ||
          data [*field + 9] != NULL)
        {
          show_error ("ERROR: FREE ORDER WITH EXTRA INFORMATION!");

          return (FALSE);
        }

        /* load hex and see if player has any lizards there */

        get_hex (x, y, &hex);

        if (lizards_in_hex (&hex) == 0)
        {
          show_error ("FREE ORDER INVALID: HEX CONTAINS NO LIZARDS!");

          return (FALSE);
        } else
          if (hex.hex_owner != player)
          {
            show_error ("FREE ORDER INVALID: HEX CONTAINS NO LIZARDS OWNED BY PLAYER!");

            return (FALSE);
          } else {
	    if (data [*field + 4] != NULL)
	    {
	      lizards = atoi (data [*field + 4]);

	      if (lizards > hex.red_lizards)
		if (hex.red_lizards == 0)
		{
		  show_error ("FREE ORDER INVALID: HEX CONTAINS NO RED LIZARDS!");

		  return (FALSE);
		} else {
		  sprintf (error, "FREE ORDER INVALID: HEX CONTAINS ONLY %d RED LIZARDS!",
		    hex.red_lizards);

		  show_error (error);

		  return (FALSE);
		}
	    }

	    if (data [*field + 5] != NULL)
	    {
	      lizards = atoi (data [*field + 5]);

	      if (lizards > hex.green_lizards)
		if (hex.green_lizards == 0)
		{
		  show_error ("FREE ORDER INVALID: HEX CONTAINS NO GREEN LIZARDS!");

		  return (FALSE);
		} else {
		  sprintf (error, "FREE ORDER INVALID: HEX CONTAINS ONLY %d GREEN LIZARDS!",
		    hex.green_lizards);

		  show_error (error);

		  return (FALSE);
		}
	    }

	    if (data [*field + 6] != NULL)
	    {
	      lizards = atoi (data [*field + 6]);

	      if (lizards > hex.grey_lizards)
		if (hex.grey_lizards == 0)
		{
		  show_error ("FREE ORDER INVALID: HEX CONTAINS NO GREY LIZARDS!");

		  return (FALSE);
		} else {
		  sprintf (error, "FREE ORDER INVALID: HEX CONTAINS ONLY %d GREY LIZARDS!",
		    hex.grey_lizards);

		  show_error (error);

		  return (FALSE);
		}
	    }

	    if (data [*field + 7] != NULL)
	    {
	      lizards = atoi (data [*field + 7]);

	      if (lizards > hex.yellow_lizards)
		if (hex.yellow_lizards == 0)
		{
		  show_error ("FREE ORDER INVALID: HEX CONTAINS NO YELLOW LIZARDS!");

		  return (FALSE);
		} else {
		  sprintf (error, "FREE ORDER INVALID: HEX CONTAINS ONLY %d YELLOW LIZARDS!",
		    hex.yellow_lizards);

		  show_error (error);

		  return (FALSE);
		}
	    }

	    if (data [*field + 8] != NULL)
	    {
	      lizards = atoi (data [*field + 8]);

	      if (lizards > hex.black_lizards)
		if (hex.black_lizards == 0)
		{
		  show_error ("FREE ORDER INVALID: HEX CONTAINS NO BLACK LIZARDS!");

		  return (FALSE);
		} else {
		  sprintf (error, "FREE ORDER INVALID: HEX CONTAINS ONLY %d BLACK LIZARDS!",
		    hex.black_lizards);

		  show_error (error);

		  return (FALSE);
		}
	    }
	  }

        break;
      case BUILD_RAFT:
        if (data [*field + 1] == NULL)
        {
          show_error ("ERROR: BUILD RAFT ORDER WITH NO SPECIFIED HEX!");

          return (FALSE);
        }

        string_to_loc (data [*field + 1], &x, &y);

        if (x < 0 || y < 0 || x >= world.x || y >= world.y)
        {
          show_error ("ERROR: BUILD RAFT ORDER WITH INVALID HEX!");

          return (FALSE);
        }

        if (data [*field + 3] != NULL || data [*field + 4] != NULL ||
          data [*field + 5] != NULL || data [*field + 6] != NULL ||
          data [*field + 7] != NULL || data [*field + 8] != NULL ||
          data [*field + 9] != NULL)
        {
          show_error ("ERROR: BUILD RAFT ORDER WITH EXTRA INFORMATION!");

          return (FALSE);
        }

        /* load hex and see if player has any lizards there */

        get_hex (x, y, &hex);

        if (lizards_in_hex (&hex) == 0)
        {
          show_error ("BUILD RAFT ORDER INVALID: HEX CONTAINS NO LIZARDS!");

          return (FALSE);
        } else
          if (hex.hex_owner != player)
          {
            show_error ("BUILD RAFT ORDER INVALID: HEX CONTAINS NO LIZARDS OWNED BY PLAYER!");

            return (FALSE);
          } else {
            if (hex.green_lizards < BUILD_RAFT_LEVEL)
            {
              show_error ("BUILD RAFT ORDER INVALID: HEX CONTAINS INSUFFICIENT GREEN LIZARDS!");

              return (FALSE);
            }
          }

        break;
      case SAIL:
        if (data [*field + 1] == NULL)
        {
          show_error ("ERROR: SAIL ORDER WITH NO SPECIFIED HEX!");

          return (FALSE);
        }

        string_to_loc (data [*field + 1], &x, &y);

        if (x < 0 || y < 0 || x >= world.x || y >= world.y)
        {
          show_error ("ERROR: SAIL ORDER WITH INVALID HEX!");

          return (FALSE);
        }

        if (data [*field + 2] == NULL &&
          data [*field + 3] == NULL)
        {
          show_error ("ERROR: SAIL ORDER WITH NO DIRECTIONS!");

          return (FALSE);
        }

        if (string_to_direction (data [*field + 2]) == 0 &&
          data [*field + 2] != NULL)
        {
          show_error ("ERROR: SAIL ORDER WITH INVALID DIRECTION!");

          return (FALSE);
        }

        if (string_to_direction (data [*field + 3]) == 0 &&
          data [*field + 3] != NULL)
        {
          show_error ("ERROR: SAIL ORDER WITH INVALID DIRECTION!");

          return (FALSE);
        }

        if (data [*field + 4] != NULL || data [*field + 5] != NULL ||
          data [*field + 6] != NULL || data [*field + 7] != NULL ||
          data [*field + 8] != NULL || data [*field + 9] != NULL)
        {
          show_error ("ERROR: SAIL ORDER WITH EXTRA INFORMATION!");

          return (FALSE);
        }

        /* load hex and see if player has any lizards there */

        get_hex (x, y, &hex);

        if (lizards_in_hex (&hex) == 0)
        {
          show_error ("SAIL ORDER INVALID: HEX CONTAINS NO LIZARDS!");

          return (FALSE);
        } else
          if (hex.hex_owner != player)
          {
            show_error ("SAIL ORDER INVALID: HEX CONTAINS NO LIZARDS OWNED BY PLAYER!");

            return (FALSE);
          }

        break;
      case SNEAK:
        if (data [*field + 1] == NULL)
        {
          show_error ("ERROR: SNEAK ORDER WITH NO SPECIFIED HEX!");

          return (FALSE);
        }

        string_to_loc (data [*field + 1], &x, &y);

        if (x < 0 || y < 0 || x >= world.x || y >= world.y)
        {
          show_error ("ERROR: SNEAK ORDER WITH INVALID HEX!");

          return (FALSE);
        }

        if (data [*field + 2] == NULL &&
          data [*field + 3] == NULL)
        {
          show_error ("ERROR: SNEAK ORDER WITH NO DIRECTIONS!");

          return (FALSE);
        }

        if (string_to_direction (data [*field + 2]) == 0 &&
          data [*field + 2] != NULL)
        {
          show_error ("ERROR: SNEAK ORDER WITH INVALID DIRECTION!");

          return (FALSE);
        }

        if (string_to_direction (data [*field + 3]) == 0  &&
          data [*field + 3] != NULL)
        {
          show_error ("ERROR: SNEAK ORDER WITH INVALID DIRECTION!");

          return (FALSE);
        }

        if (data [*field + 4] != NULL || data [*field + 5] != NULL ||
          data [*field + 6] != NULL || data [*field + 7] != NULL ||
          data [*field + 8] != NULL || data [*field + 9] != NULL)
        {
          show_error ("ERROR: SNEAK ORDER WITH EXTRA INFORMATION!");

          return (FALSE);
        }

	for (loop = 0; loop < number_spys; loop ++)
	  if (spy_list [loop]->x_hex == x && spy_list [loop]->y_hex == y &&
	    spy_list [loop]->player == player)
	    break;

	if (loop == number_spys)
	{
	  show_error ("ERROR: PLAYER HAS NO SPY IN SPECIFIED HEX!");

	  return (FALSE);
	}

        break;
      default:
        show_error ("ERROR: UNKNOWN ORDER!");

        return (FALSE);
    }
  }

  (*field) ++;

  while (template [*field]->start_of_line_f != 1 &&
    template [*field]->start_of_line_f != -1)
    (*field) ++;

  return (TRUE);
}

int order_skip_to_field (unsigned int *field, field_t **template, char **data)
{
  unsigned int work_field,
               present_field_on_line;

  work_field = *field;

  while (template [work_field]->start_of_line_f != 1 &&
    template [work_field]->start_of_line_f != -1)
    (work_field) --;

  if (template [work_field]->start_of_line_f == -1) return (FALSE);

  if (data [work_field] != NULL)
  {
    present_field_on_line = *field - work_field;

    switch (order_num_from_string (data [work_field]))
    {
      case CHANT:
        if (present_field_on_line > 3 && present_field_on_line < 8)
        {
          /* skip to black lizards */

          *field = work_field + 8;

          return (TRUE);
        }

        break;
      case GIVE:
        if (present_field_on_line > 1 && present_field_on_line < 9)
        {
          /* skip to extra field */

          *field = work_field + 9;

          return (TRUE);
        }

        break;
      case FREE:
        if (present_field_on_line > 1 && present_field_on_line < 4)
        {
          /* skip to first lizards field */

	  *field = work_field + 4;

          return (TRUE);
        }


        break;
    }
  }

  return (TRUE);
}

void show_order_form (void)
{
  printf ("\x1B[2J");

  printf ("Game Code:___           Turn Number:___          Order Origin Code:_\n"
	  "Player Number:__        Player Name:______________________________\n"
	  "Clan Name:____________________  Clan Code:___\n"
	  "OR HEX  M1 M2 RED GRN GRY YEL BLK EXTR | OR HEX  M1 M2 RED GRN GRY YEL BLK EXTR\n"
	  "\x1B[5;40H1\n"
	  "\x1B[6;40H|\n"
	  "\x1B[7;40H|\n"
	  "\x1B[8;40H|\n"
	  "\x1B[9;40H5\n"
	  "\x1B[10;40H|\n"
	  "\x1B[11;40H|\n"
	  "\x1B[12;40H|\n"
	  "\x1B[13;40H|\n"
	  "\x1B[14;40H0\n"
	  "\x1B[15;40H|\n"
	  "\x1B[16;40H|\n"
	  "\x1B[17;40H|\n"
	  "\x1B[18;40H|\n"
	  "\x1B[19;40H5\n"
	  "Speaking with the Enemy ______________________________           ^I            \n"
	  "Message  _______________________________________________    ^J <- + -> ^L      \n"
	  "         _______________________________________________         ^K            \n"
	  "^D=DITTO  ^V=VALIDATE ORDER  ^E=ERASE FORM  ^F=FINISHED!  ^A=ABORT!",
	  VER_STRING, __DATE__);
}
