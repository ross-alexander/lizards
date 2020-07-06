#include <conio.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lizard.h"
#include "order.h"
#include "map.h"
#include "template.h"
#include "custom.h"

#include "strgfunc.h"
#include "tempfunc.h"

#include "lizgame.h"
#include "lizmisc.h"

char data_path [80],
     game_code [5];

int world_x,
    world_y,
    number_orders,
    turn_number;

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

int main (int argc, char **argv);
void input_order_forms (field_t **order_template,
                        char **order_data [MAX_PLAYERS + 1]);
void output_order_form (field_t **template, char **data, FILE *fptr);
int order_ditto (unsigned int *field, field_t **template, char **data);
int check_order_form (field_t **template, char **data);
int check_order_format (unsigned int *field, field_t **template, char **data);
void show_order_form (void);

int main (int argc, char **argv)
{
  FILE *fptr;

  char string [256];

  unsigned int player = 1,
               loop,
               option,
               field_in,
               order_template_fields = 0;

  field_t **order_template;

  char **player_orders [MAX_PLAYERS + 1];

  printf ("LIZARDS! Custom Startup Entry Utility. %s Last compiled %s.\n\n",
          VER_STRING, __DATE__);

  if (argc == 3)
  {
    strcpy (data_path, argv [1]);
    strncpy (game_code, argv [2], 3);
    game_code [3] = NULL;
    strupr (game_code);
  } else {
    printf ("FATAL ERROR: Turn and Game Number not specified on command line!\n"
	     "             Command Line Format: EXEFILE data_path game.\n"
	     "                             e.g. CUSTENT C:\\ A\n");

    exit (EXIT_FAILURE);
  }

  get_switch_list ();

  get_player_list ();

  if (players_in_game == 0)
  {
    printf ("FATAL ERROR: Game %s has no players. Press SPACE.\n", game_code);

    getch ();

    exit (EXIT_FAILURE);
  }

  get_spy_list ();

  get_world ();

  order_template = load_template ("CUSTFORM.TEM", &order_template_fields);

  for (player = 1; player < players_in_game + 1; player ++)
  {
    if ((player_orders [player] = calloc (order_template_fields + 1,
      sizeof (char *))) == NULL)
    {
      printf ("FATAL ERROR: Out of memory for calloc. Press SPACE.\n");

      getch ();

      exit (EXIT_FAILURE);
    }

    set_field (order_template, player_orders [player], 0, game_code);
    set_field (order_template, player_orders [player], 1, itoa (player, string, 10));
    set_field (order_template, player_orders [player], 2, player_name [player]);
    set_field (order_template, player_orders [player], 3, player_clan_name [player]);
    set_field (order_template, player_orders [player], 4, player_code [player]);
  }

  /* check if there's an already existing (partial?) set of orders. If so,
    load into existing tables to allow editing */

  input_order_forms (order_template, player_orders);

  player = 1;

  while (player != 0)
  {
    unsigned int exit_template = FALSE;

    for (loop = player; loop < players_in_game + 1 &&
      strcmp (player_name [loop], "****") == 0; loop ++);

    if (loop != players_in_game + 1)
      player = loop;

    show_order_form ();

    show_template (order_template, player_orders [player]);

    while (!exit_template)
      switch (enter_template (order_template, player_orders [player], 5,
        &field_in, check_order_format, order_ditto, NULL))
      {
        case ERASE_KEY:
          clear_error ();

          show_error ("SURE YOU WANT TO ERASE FORM (Y/N)? ");

          option = toupper (getch ());

          if (option == 'Y')
          {
            clear_error ();

            free_data (order_template, player_orders [player]);

            if ((player_orders [player] = calloc (order_template_fields + 1,
              sizeof (char *))) == NULL)
            {
              printf ("FATAL ERROR: Out of memory for calloc. Press SPACE.\n");

              getch ();

              exit (EXIT_FAILURE);
            }

            set_field (order_template, player_orders [player], 0, game_code);
            set_field (order_template, player_orders [player], 1, itoa (player, string, 10));
            set_field (order_template, player_orders [player], 2, player_name [player]);
            set_field (order_template, player_orders [player], 3, player_clan_name [player]);
            set_field (order_template, player_orders [player], 4, player_code [player]);

            exit_template = TRUE;
          }

          break;
        case FINISH_KEY:
          clear_error ();

          show_error ("SURE YOU HAVE COMPLETED ORDER ENTRY (Y/N)? ");

          option = toupper (getch ());

          if (option == 'Y')
          {
            if (check_order_form (order_template, player_orders [player]))
            {
              clear_error ();
              player = 0;
              exit_template = TRUE;
            }
          }

          break;
        case ESCAPE_KEY:
          clear_error ();

          show_error ("SURE YOU WANT TO EXIT CUSTOM STARTUP ENTRY UTILITY (Y/N)? ");

          option = toupper (getch ());

          if (option == 'Y')
            exit (EXIT_FAILURE);

          break;
        case UP_FORM_KEY:
          for (loop = player - 1; loop > 0 &&
            strcmp (player_name [loop], "****") == 0; loop --);

          if (loop != 0)
          {
            player = loop;
            exit_template = TRUE;
          }

          break;
        case DOWN_FORM_KEY:
          if (!check_order_form (order_template, player_orders [player]))
            break;

          exit_template = TRUE;
        default:
          for (loop = player + 1; loop < players_in_game + 1 &&
            strcmp (player_name [loop], "****") == 0; loop ++);

          if (loop != players_in_game + 1)
            player = loop;

          exit_template = TRUE;

          break;
      }
  }

  sprintf (string, "%s%s.%s", data_path, TEXT_START_ORDS_F, game_code);

  if ((fptr = fopen (string, "wt")) == NULL)
  {
    printf ("FATAL ERROR: Unable to write to %s file. Press SPACE.\n", string);

    getch ();

    exit (EXIT_FAILURE);
  }

  for (player = 1; player < players_in_game + 1; player ++)
    output_order_form (order_template, player_orders [player], fptr);

  fclose (fptr);

  free_template (order_template);

  clrscr ();

  printf ("Finished!\n");

  return (EXIT_SUCCESS);
}

void input_order_forms (field_t **order_template,
                        char **order_data [MAX_PLAYERS + 1])
{
  FILE *fptr;

  char filename [80],
       string [256],
       token [256],
       number [10];

  int player = 0,
      player_hex_line,
      lizards;

  sprintf (filename, "%s%s.%s", data_path, TEXT_START_ORDS_F, game_code);

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
    } else
      if (player != 0)
        if (strcmp (token, "STARTUP") == 0)
        {
          /* startup line */

          if (string [0] == 'C')
            set_field (order_template, order_data [player], 5, string);
          else {
            if (atoi (string) > 0)
              set_field (order_template, order_data [player], 5, string);
          }
        } else {
          if (strcmp (token, "TEMPLATE") == 0)
          {
            /* template line */

            if (atoi (string) > 0)
            {
              set_field (order_template, order_data [player], 6, string);
            }
          } else {
            if (strcmp (token, "HOME") == 0)
            {
              /* home hex line */

              if (atoi (string) > 0)
              {
                set_field (order_template, order_data [player], 7, string);
              }
            } else {
              /* normal hex line */

              player_hex_line = atoi (token);

              if (player_hex_line > 0 && player_hex_line < 23)
              {
                /* valid hex line */

                strtoken (token, string, " ");

                if (strlen (token) > 3)
                  if ((lizards = atoi (&token [3])) > 0)
                  {
                    /* must be pop type as first on line */

                    set_field (order_template, order_data [player],
                      2 + (8 * player_hex_line), &token [3]);
                  }

                /* terrain code first on line */

                set_field (order_template, order_data [player],
                  1 + (8 * player_hex_line), token);

                /* warrior codes */

                while (strtoken (token, string, " ") [0] != NULL)
                {
                  if (strlen (token) > 3)
                  {
                    lizards = atoi (&token [3]);

                    if (lizards > 0)
                    {
                      switch (token [2])
                      {
                        case 'D': /* red */
                          set_field (order_template, order_data [player],
                            3 + (8 * player_hex_line), &token [3]);

                          break;
                        case 'N': /* green */
                          set_field (order_template, order_data [player],
                            4 + (8 * player_hex_line), &token [3]);

                          break;
                        case 'Y': /* grey */
                          set_field (order_template, order_data [player],
                            5 + (8 * player_hex_line), &token [3]);

                          break;
                        case 'L': /* yellow */
                          set_field (order_template, order_data [player],
                            6 + (8 * player_hex_line), &token [3]);

                          break;
                        case 'K': /* black */
                          set_field (order_template, order_data [player],
                            7 + (8 * player_hex_line), &token [3]);

                          break;
                      }
                    }
                  }
                }
              }
            }
          }
        }
  }

  fclose (fptr);

}

void output_order_form (field_t **template, char **data, FILE *fptr)
{
  unsigned int present_field = 9,
               loop,
               hexes;

  fprintf (fptr, "PLAYER %s\n", data [1]);
  fprintf (fptr, "STARTUP %s\n", data [5]);

  if (data [5][0] == 'C')
  {
    fprintf (fptr, "TEMPLATE %s\n", data [6]);
    fprintf (fptr, "HOME %s\n", data [7]);

    switch (atoi (data [6]))
    {
      case 1:
        hexes = TEMPLATE1_HEX;
        break;
      case 2:
        hexes = TEMPLATE2_HEX;
        break;
      case 3:
        hexes = TEMPLATE3_HEX;
        break;
      case 4:
        hexes = TEMPLATE4_HEX;
        break;
      case 5:
        hexes = TEMPLATE5_HEX;
        break;
      case 6:
        hexes = TEMPLATE6_HEX;
        break;
      case 7:
        hexes = TEMPLATE7_HEX;
        break;
      case 8:
        hexes = TEMPLATE8_HEX;
        break;
    }

    for (loop = 0; loop < hexes; loop ++, present_field += 8)
    {
      fprintf (fptr, "%02d ", loop + 1);

      if (data [present_field] != NULL)
      {
        fprintf (fptr, "%s", data [present_field]);

        if (data [present_field + 1] != NULL)
          fprintf (fptr, "%s ", data [present_field + 1]);
        else
          fprintf (fptr, " ");

        if (data [present_field + 2] != NULL)
          fprintf (fptr, "RED%s ", data [present_field + 2]);

        if (data [present_field + 3] != NULL)
          fprintf (fptr, "GRN%s ", data [present_field + 3]);

        if (data [present_field + 4] != NULL)
          fprintf (fptr, "GRY%s ", data [present_field + 4]);

        if (data [present_field + 5] != NULL)
          fprintf (fptr, "YEL%s ", data [present_field + 5]);

        if (data [present_field + 6] != NULL)
          fprintf (fptr, "BLK%s ", data [present_field + 6]);
      }

      fprintf (fptr, "\n");
    }

    fprintf (fptr, "\n");
  }
}

int order_ditto (unsigned int *field, field_t **template, char **data)
{
  unsigned int loop;

  if (*field >= 9 && *field <= 184 &&
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
  char terrain_codes [16][4] = { "RUI", "PLA", "SWA", "SCR", "FER", "PEA",
                                 "VOL", "TEM", "CUR", "WHI", "WAT", "RED",
                                 "GRN", "GRY", "BLK", "YEL" };

  unsigned int terrains [16] = { RUIN, PLAIN, SWAMP, SCRUB, FERTILE, PEAK,
                                 VOLCANO, TEMPLE, CURSED, WHIRLPOOL, WATER,
                                 DEN, DEN, DEN, DEN, DEN };

  char number [10];

  int terrain = -1,
      den_colour,
      loop,
      cost,
      total_cost,
      lizards = 0;

  /* go back to the start of this line */

  while (template [*field]->start_of_line_f != 1 &&
    template [*field]->start_of_line_f != -1)
    (*field) --;

  if (template [*field]->start_of_line_f == -1) return (FALSE);

  if (data [*field] != NULL)
  {
    /* find terrain type */

    terrain = -1;
    den_colour = 0;

    for (loop = 0; loop < 16; loop ++)
      if (strcmp (terrain_codes [loop], data [*field]) == 0)
      {
        terrain = terrains [loop];

        if (terrain == DEN)
          den_colour = loop - 10;

        break;
      }

    if (terrain == -1)
    {
      show_error ("ERROR: UNKNOWN TERRAIN CODE.");

      return (FALSE);
    }
  }

  /* check if there's pop NOT in a den hex */

  if (data [*field + 1] != NULL && terrain != DEN)
  {
    show_error ("ERROR: POPULATION SPECIFIED IN NON-DEN HEX.");

    return (FALSE);
  } else
    if (data [*field + 1] != NULL)
      if ((lizards = atoi (data [*field + 1]) +
        (MINIMUM_POP_PER_DEN * GROUP_SIZE)) > MAX_LIZARDS_IN_HEX)
      {
        show_error ("ERROR: POPULATION LIZARDS EXCEED MAXIMUM ALLOWED IN HEX.");

        return (FALSE);
      }

  /* check warriors in hex don't exceed 300 */

  lizards = 0;

  for (loop = 2; loop < 7; loop ++)
    if (data [*field + loop] != NULL)
      lizards += atoi (data [*field + loop]);

  if (lizards > MAX_LIZARDS_IN_HEX)
  {
    show_error ("ERROR: WARRIOR LIZARDS EXCEED MAXIMUM ALLOWED IN HEX.");

    return (FALSE);
  }

  /* since line seems to be okay, calculate cost of line and place in
    appropriate places */

  cost = 0;
  total_cost = 0;

  switch (terrain)
  {
    case RUIN:
      cost += RUIN_COST;
      break;
    case PLAIN:
      cost += PLAIN_COST;
      break;
    case SWAMP:
      cost += SWAMP_COST;
      break;
    case SCRUB:
      cost += SCRUB_COST;
      break;
    case FERTILE:
      cost += FERTILE_COST;
      break;
    case PEAK:
      cost += PEAK_COST;
      break;
    case VOLCANO:
      cost += VOLCANO_COST;
      break;
    case TEMPLE:
      cost += TEMPLE_COST;
      break;
    case CURSED:
      cost += CURSED_COST;
      break;
    case WHIRLPOOL:
      cost += WHIRLPOOL_COST;
      break;
    case DEN:
      switch (den_colour)
      {
        case RED_LIZARD:
          cost += RED_DEN_COST;

          if (data [*field + 1] != NULL)
            lizards = atoi (data [*field + 1]);
          else
            lizards = 0;

          cost += (lizards / GROUP_SIZE) * RED_POP_COST;

          break;
        case GREEN_LIZARD:
          cost += GREEN_DEN_COST;

          if (data [*field + 1] != NULL)
            lizards = atoi (data [*field + 1]);
          else
            lizards = 0;

          cost += (lizards / GROUP_SIZE) * GREEN_POP_COST;

          break;
        case GREY_LIZARD:
          cost += GREY_DEN_COST;

          if (data [*field + 1] != NULL)
            lizards = atoi (data [*field + 1]);
          else
            lizards = 0;

          cost += (lizards / GROUP_SIZE) * GREY_POP_COST;

          break;
        case BLACK_LIZARD:
          cost += BLACK_DEN_COST;

          if (data [*field + 1] != NULL)
            lizards = atoi (data [*field + 1]);
          else
            lizards = 0;

          cost += (lizards / GROUP_SIZE) * BLACK_POP_COST;

          break;
        case YELLOW_LIZARD:
          cost += YELLOW_DEN_COST;

          if (data [*field + 1] != NULL)
            lizards = atoi (data [*field + 1]);
          else
            lizards = 0;

          cost += (lizards / GROUP_SIZE) * YELLOW_POP_COST;

          break;
      }

      break;
  }

  /* calculate warrior cost */

  for (loop = 2; loop < 7; loop ++)
    if (data [*field + loop] != NULL)
    {
      lizards = atoi (data [*field + loop]);

      switch (loop - 1)
      {
        case 1:
          cost += (lizards / GROUP_SIZE) * RED_WARRIOR_COST;

          break;
        case 2:
          cost += (lizards / GROUP_SIZE) * GREEN_WARRIOR_COST;

          break;
        case 3:
          cost += (lizards / GROUP_SIZE) * GREY_WARRIOR_COST;

          break;
        case 4:
          cost += (lizards / GROUP_SIZE) * YELLOW_WARRIOR_COST;

          break;
        case 5:
          cost += (lizards / GROUP_SIZE) * BLACK_WARRIOR_COST;

          break;
      }
    }

  /* put cost in line field and recalculate total! */

  set_field (template, data, *field + 7, itoa (cost, number, 10));

  show_field (template [*field + 7], data [*field + 7]);

  /* recalculate TOTAL cost! */

  for (loop = 16; loop < 192; loop += 8)
    if (data [loop] != NULL)
      total_cost += atoi (data [loop]);

  /* add in cost of template */

  if (data [6] != NULL)
  {
    switch (atoi (data [6]))
    {
      case 1:
        total_cost += TEMPLATE1_COST;
        break;
      case 2:
        total_cost += TEMPLATE2_COST;
        break;
      case 3:
        total_cost += TEMPLATE3_COST;
        break;
      case 4:
        total_cost += TEMPLATE4_COST;
        break;
      case 5:
        total_cost += TEMPLATE5_COST;
        break;
      case 6:
        total_cost += TEMPLATE6_COST;
        break;
      case 7:
        total_cost += TEMPLATE7_COST;
        break;
      case 8:
        total_cost += TEMPLATE8_COST;
        break;
    }
  }

  set_field (template, data, 8, itoa (total_cost, number, 10));

  show_field (template [8], data [8]);

  /* go to beginning of NEXT line */

  (*field) ++;

  while (template [*field]->start_of_line_f != 1 &&
    template [*field]->start_of_line_f != -1)
    (*field) ++;

  return (TRUE);
}

int check_order_form (field_t **template, char **data)
{
  int startup,
      template_num,
      home_hex;

  /* check player has a valid template specified */

  if (data [5] == NULL)
  {
    show_error ("ERROR: PLAYER STARTUP FORM WITH NO STARTUP SPECIFIED.");

    return (FALSE);
  }

  if (data [5][0] != 'C')
  {
    /* must be a default number */

    startup = atoi (data [5]);

    if (startup == 0 || startup > 4)
    {
      show_error ("ERROR: PLAYER STARTUP FORM WITH UNKNOWN STARTUP SPECIFIED.");

      return (FALSE);
    }
  } else {
    /* must be a custom startup */

    if (data [6] == NULL)
    {
      show_error ("ERROR: PLAYER STARTUP FORM WITH NO CUSTOM STARTUP TEMPLATE SPECIFIED.");

      return (FALSE);
    }

    template_num = atoi (data [6]);

    if (template_num == 0 || template_num > 8)
    {
      show_error ("ERROR: PLAYER STARTUP FORM WITH INVALID CUSTOM STARTUP TEMPLATE SPECIFIED.");

      return (FALSE);
    }

    if (data [7] == NULL)
    {
      show_error ("ERROR: PLAYER STARTUP FORM WITH NO HOME HEX SPECIFIED.");

      return (FALSE);
    }

    home_hex = atoi (data [7]);

    if (home_hex == 0 || home_hex > 22)
    {
      show_error ("ERROR: PLAYER STARTUP FORM WITH INVALID HOME HEX SPECIFIED.");

      return (FALSE);
    }
  }

  return (TRUE);
}

void show_order_form (void)
{
  clrscr ();

  cprintf ("LIZARDS! PLAYER CUSTOM STARTUP FORM. %s Last compiled %s.\n\r"
           "Game Code:___\n\r"
           "Player Number:__        Player Name:______________________________\n\r"
           "Clan Name:____________________  Clan Code:___  Default No. (or 'C'): _\n\r\n\r"
           "Template Number: _  Home Den Hex: __  Total Points: ____\n\r\n\r"
	   "HEX TER POP RED GRN GRY YEL BLK PNT   HEX TER POP RED GRN GRY YEL BLK PNT\n\r"
	   " 01 ___ ___ ___ ___ ___ ___ ___ ____   12 ___ ___ ___ ___ ___ ___ ___ ____\n\r"
	   " 02 ___ ___ ___ ___ ___ ___ ___ ____   13 ___ ___ ___ ___ ___ ___ ___ ____\n\r"
	   " 03 ___ ___ ___ ___ ___ ___ ___ ____   14 ___ ___ ___ ___ ___ ___ ___ ____\n\r"
	   " 04 ___ ___ ___ ___ ___ ___ ___ ____   15 ___ ___ ___ ___ ___ ___ ___ ____\n\r"
	   " 05 ___ ___ ___ ___ ___ ___ ___ ____   16 ___ ___ ___ ___ ___ ___ ___ ____\n\r"
	   " 06 ___ ___ ___ ___ ___ ___ ___ ____   17 ___ ___ ___ ___ ___ ___ ___ ____\n\r"
	   " 07 ___ ___ ___ ___ ___ ___ ___ ____   18 ___ ___ ___ ___ ___ ___ ___ ____\n\r"
	   " 08 ___ ___ ___ ___ ___ ___ ___ ____   19 ___ ___ ___ ___ ___ ___ ___ ____\n\r"
	   " 09 ___ ___ ___ ___ ___ ___ ___ ____   20 ___ ___ ___ ___ ___ ___ ___ ____\n\r"
	   " 10 ___ ___ ___ ___ ___ ___ ___ ____   21 ___ ___ ___ ___ ___ ___ ___ ____\n\r"
	   " 11 ___ ___ ___ ___ ___ ___ ___ ____   22 ___ ___ ___ ___ ___ ___ ___ ____\n\r\n\r"
           "F1=DITTO             F2=CHECK LINE         F3=ERASE FORM\n\r"
           "F4=ALL FINISHED!     F5=ABORT PROGRAM      PGUP=LAST PLAYER    PGDN=NEXT PLAYER\n\r",
           VER_STRING, __DATE__);
}
