#define CUSTSTAR_LOG_FILE "STARTLOG"

#include <conio.h>
#include <ctype.h>
#include <dos.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "lizard.h"
#include "map.h"
#include "custom.h"

#include "strgfunc.h"

#include "lizgame.h"
#include "lizhex.h"
#include "lizmisc.h"

#define PLAYER_CODE "PLAYER"
#define STARTUP_CODE "STARTUP"
#define TEMPLATE_CODE "TEMPLATE"
#define HOME_HEX_CODE "HOME"

#define CODE_LENGTH 4

#define RUIN_CODE "RUI"
#define PLAIN_CODE "PLA"
#define SWAMP_CODE "SWA"
#define SCRUB_CODE "SCR"
#define FERTILE_CODE "FER"
#define PEAK_CODE "PEA"
#define VOLCANO_CODE "VOL"
#define TEMPLE_CODE "TEM"
#define CURSED_CODE "CUR"
#define WHIRLPOOL_CODE "WHI"
#define WATER_CODE "WAT"

#define TERRAIN_TYPES 16

#define RED_DEN_CODE "RED"
#define YELLOW_DEN_CODE "YEL"
#define GREEN_DEN_CODE "GRN"
#define GREY_DEN_CODE "GRY"
#define BLACK_DEN_CODE "BLK"

#define RED_LIZARD_CODE "RED"
#define YELLOW_LIZARD_CODE "YEL"
#define GREEN_LIZARD_CODE "GRN"
#define GREY_LIZARD_CODE "GRY"
#define BLACK_LIZARD_CODE "BLK"

const unsigned char template_hexes [TEMPLATES + 1] =
    { 0, TEMPLATE1_HEX, TEMPLATE2_HEX, TEMPLATE3_HEX, TEMPLATE4_HEX,
      TEMPLATE5_HEX, TEMPLATE6_HEX, TEMPLATE7_HEX, TEMPLATE8_HEX };

const unsigned char template_cost [TEMPLATES + 1] =
    { 0, TEMPLATE1_COST, TEMPLATE2_COST, TEMPLATE3_COST, TEMPLATE4_COST,
      TEMPLATE5_COST, TEMPLATE6_COST, TEMPLATE7_COST, TEMPLATE8_COST };

const char template_hex_x_offset [TEMPLATES + 1][MAX_TEMPLATE_HEX] =
  {{0},
   {-1,-1,0,0,0,1,1},
   {-1,-1,-1,0,0,0,1,1,1},
   {-2,-1,-1,-1,0,0,0,1,1,1,2},
   {-2,-2,-1,-1,-1,0,0,0,0,1,1,1,2,2},
   {-1,-1,-1,-1,-1,0,0,0,0,0,0,1,1,1,1,1},
   {-3,-3,-2,-2,-2,-1,-1,0,0,0,1,1,2,2,2,3,3},
   {-2,-2,-2,-1,-1,-1,-1,0,0,0,0,0,1,1,1,1,2,2,2},
   {-3,-3,-3,-2,-2,-2,-2,-1,-1,-1,-1,0,0,0,0,1,1,1,1,2,2,2}};

const char template_hex_y_offset [TEMPLATES + 1][MAX_TEMPLATE_HEX] =
  {{0},
   {-1,0,-1,0,1,-1,0},
   {0,1,2,0,1,2,-1,0,1},
   {1,-1,0,1,-1,0,1,-2,-1,0,-1},
   {-1,0,-2,-1,0,-2,-1,0,1,-2,-1,0,-1,0},
   {-2,-1,0,1,2,-2,-1,0,1,2,3,-2,-1,0,1,2},
   {-1,0,-1,0,1,-1,0,-1,0,1,-1,0,-1,0,1,-1,0},
   {-1,0,1,-2,-1,0,1,-2,-1,0,1,2,-2,-1,0,1,-1,0,1},
   {-2,-1,0,-2,-1,0,1,-2,-1,0,1,-1,0,1,2,-1,0,1,2,0,1,2}};

char data_path [80],
     game_code [5];

char player_code [MAX_PLAYERS + 1][6];
char player_name [MAX_PLAYERS + 1][37];
char player_clan_name [MAX_PLAYERS + 1][37];
char player_home_den [MAX_PLAYERS + 1];
char player_grid [MAX_PLAYERS + 1];
char players_in_game;

char player_road [MAX_PLAYERS + 1][37];
char player_burb [MAX_PLAYERS + 1][37];
char player_city [MAX_PLAYERS + 1][37];
char player_country [MAX_PLAYERS + 1][37];
char player_cred [MAX_PLAYERS + 1][10];
char player_output_mode [MAX_PLAYERS + 1];
char game_due_date [20];

int world_x,
    world_y,
    grid_x,
    grid_y,
    grid_size,
    turn_number;

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

int main (int argc, char **argv);
void build_startup (unsigned int player, FILE *template_fptr, FILE *log_fptr);
void lay_template (unsigned int player, unsigned int template, hex_t *hexes,
                   FILE *log_fptr);
void build_hex (unsigned int *points, hex_t *hex, char *string);

int main (int argc, char **argv)
{
  FILE *order_fptr,
       *template_fptr,
       *log_fptr;

  char filename [80],
       text_string [256],
       tok_string [256];

  unsigned int player,
	       startup,
	       loop;

  randomize ();

  show_title ();

  printf ("LIZARDS! Custom Startup Program. %s Last compiled %s.\n\n",
          VER_STRING, __DATE__);

  if (argc == 3)
  {
    strcpy (data_path, argv [1]);
    strncpy (game_code, argv [2], 3);
    game_code [3] = NULL;
    strupr (game_code);
  } else {
    printf ("FATAL ERROR: Turn and Game Number not specified on command line!\r\n"
	     "             Command Line Format: EXEFILE data_path game.\r\n"
	     "             e.g. CUSTSTAR C:\\ A\r\n");

    exit (EXIT_FAILURE);
  }

  get_world ();

  get_player_list ();

  if (players_in_game == 0)
  {
    printf ("FATAL ERROR: Game %s has no players. Press SPACE.\n", game_code);

    getch ();

    exit (EXIT_FAILURE);
  }

  grid_size = sqrt (players_in_game);

  if (grid_size * grid_size < players_in_game) grid_size ++;

  grid_x = world_x / grid_size;
  grid_y = world_y / grid_size;

  for (loop = 0; loop < players_in_game + 1; loop ++)
  {
    player_home_den [loop] = 0;
    player_grid [loop] = -1;
  }

  sprintf (filename, "%s%s.%s", data_path, CUSTSTAR_LOG_FILE, game_code);

  if ((log_fptr = fopen (filename, "wt")) == NULL)
  {
    printf ("FATAL ERROR: Unable to write to %s file. Press SPACE.\n", filename);

    getch ();

    exit (EXIT_FAILURE);
  }

  fprintf (log_fptr, "\nCustom Startup Log File for LIZARDS! Game %s.\n\n", game_code);

  sprintf (filename, "%s%s.%s", data_path, TEXT_START_ORDS_F, game_code);

  if ((order_fptr = fopen (filename, "rt")) == NULL)
  {
    printf ("FATAL ERROR: Unable to read %s file. Press SPACE.\n", filename);

    getch ();

    exit (EXIT_FAILURE);
  }

  /* read file until we get to a new 'PLAYER' header */

  while (fgets (text_string, 255, order_fptr) != NULL)
  {
    if (text_string [strlen (text_string) - 1] == '\n')
      text_string [strlen (text_string) - 1] = NULL;

    strupr (text_string);

    strtoken (tok_string, text_string, " ");

    if (strcmp (tok_string, PLAYER_CODE) == 0)
    {
      /* found player header, now find and validate player code */

      player = atoi (text_string);

      if (player == 0 || player > players_in_game)
      {
        fprintf (log_fptr, "FATAL ERROR: Invalid player number in %s file.\n",
          filename);

        printf ("FATAL ERROR: Invalid player number in %s file. Press SPACE.\n",
          filename);

        getch ();

	exit (EXIT_FAILURE);
      }

      /* check player hasn't already started */

      if (player_home_den [player] != 0)
      {
        fprintf (log_fptr, "FATAL ERROR: Attempted to re-start player %d.\n",
          player);

        printf ("FATAL ERROR: Attempted to re-start player %d. Press SPACE.\n",
          player);

        getch ();

	exit (EXIT_FAILURE);
      }

      /* next line should be STARTUP */

      if (fgets (text_string, 255, order_fptr) == NULL)
      {
        fprintf (log_fptr, "FATAL ERROR: Unexpected end of file %s.\n", filename);

        printf ("FATAL ERROR: Unexpected end of file %s. Press SPACE.\n", filename);

        getch ();

	exit (EXIT_FAILURE);
      }

      if (text_string [strlen (text_string) - 1] == '\n')
	text_string [strlen (text_string) - 1] = NULL;

      strupr (text_string);

      strtoken (tok_string, text_string, " ");

      if (strcmp (tok_string, STARTUP_CODE) == 0)
      {
	/* find startup */

	if (!isdigit (text_string [0]) && text_string [0] != 'C')
	{
          fprintf (log_fptr, "FATAL ERROR: Player %d selected invalid STARTUP.\n",
            player);

          printf ("FATAL ERROR: Player %d selected invalid STARTUP. Press SPACE.\n",
            player);

          getch ();

	  exit (EXIT_FAILURE);
	}

	startup = atoi (text_string);

	if (startup > VALID_STARTUP)
        {
          fprintf (log_fptr, "FATAL ERROR: Player %d selected invalid STARTUP.\n",
            player);

          printf ("FATAL ERROR: Player %d selected invalid STARTUP. Press SPACE.\n",
            player);

          getch ();

	  exit (EXIT_FAILURE);
	}

	switch (startup)
	{
	  case CUSTOM_STARTUP:
	    build_startup (player, order_fptr, log_fptr);

	    break;
	  case 1:
	  case 2:
	  case 3:
	  case 4:
	    sprintf (filename, "CUST%d.MAP", startup);

	    if ((template_fptr = fopen (filename, "rt")) == NULL)
            {
              printf ("FATAL ERROR: Unable to read %s file. Press SPACE.\n", filename);

              getch ();

	      exit (EXIT_FAILURE);
	    }

	    build_startup (player, template_fptr, log_fptr);

	    fclose (template_fptr);

	    break;
	  default:
	    if (startup > VALID_STARTUP)
            {
              fprintf (log_fptr, "FATAL ERROR: Player %d selected invalid STARTUP.\n",
                player);

              printf ("FATAL ERROR: Player %d selected invalid STARTUP. Press SPACE.\n",
                player);

              getch ();

	      exit (EXIT_FAILURE);
	    }
	}
      } else {
        fprintf (log_fptr, "FATAL ERROR: Player %d selected invalid STARTUP.\n",
          player);

        printf ("FATAL ERROR: Player %d unspecified STARTUP. Press SPACE.\n",
          player);

        getch ();

	exit (EXIT_FAILURE);
      }
    }
  }

  fclose (order_fptr);

  fprintf (log_fptr, "\nEND OF CUSTOM STARTUP LOG FILE\n");

  fclose (log_fptr);

  put_home_den_status ();

  printf ("Finished!\n");

  return (EXIT_SUCCESS);
}

void build_startup (unsigned int player, FILE *template_fptr, FILE *log_fptr)
{
  FILE *default_fptr;

  char text_string [256],
       tok_string [256],
       filename [256];

  int home_hex;

  unsigned int template,
	       player_points = 0,
	       present_hex = 0,
	       loop,
	       loop_two,
	       fertiles;

  hex_t empty_hex,
	new_hex;

  hex_t *hex_list;

  /* find template */

  if (fgets (text_string, 255, template_fptr) == NULL)
  {
    fprintf (log_fptr, "FATAL ERROR: Player %d unspecified TEMPLATE.\n");

    printf ("FATAL ERROR: Player %d unspecified TEMPLATE. Press SPACE.\n", player);

    getch ();

    exit (EXIT_FAILURE);
  }

  if (text_string [strlen (text_string) - 1] == '\n')
    text_string [strlen (text_string) - 1] = NULL;

  strupr (text_string);

  strtoken (tok_string, text_string, " ");

  if (strcmp (tok_string, TEMPLATE_CODE) != 0)
  {
    fprintf (log_fptr, "FATAL ERROR: Player %d unspecified TEMPLATE.\n");

    printf ("FATAL ERROR: Player %d unspecified TEMPLATE. Press SPACE.\n", player);

    getch ();

    exit (EXIT_FAILURE);
  }

  template = atoi (text_string);

  if (template == 0 || template > TEMPLATES)
  {
    fprintf (log_fptr, "FATAL ERROR: Player %d specified invalid TEMPLATE.\n");

    printf ("FATAL ERROR: Player %d specified invalid TEMPLATE. Press SPACE.\n", player);

    getch ();

    exit (EXIT_FAILURE);
  }

  /* build template list */

  if ((hex_list = malloc (sizeof (hex_t) * template_hexes [template])) == NULL)
  {
    printf ("FATAL ERROR: Out of memory for malloc. Press SPACE.\n");

    getch ();

    exit (EXIT_FAILURE);
  }

  /* cost template list */

  player_points += template_cost [template];

  /* wipe template list */

  empty_hex.terrain = WATER;
  empty_hex.hex_owner = player;
  empty_hex.bridge_here = FALSE;
  empty_hex.den_lizard_type = 0;
  empty_hex.turns_hungry = 0;
  empty_hex.turns_den_hungry = 0;
  empty_hex.combat_points = 0;
  empty_hex.turns_undisturbed = 0;
  empty_hex.bridge_dir = 0;
  empty_hex.raft_here = 0;
  empty_hex.lizards_immobile = 0;
  empty_hex.red_lizards = 0;
  empty_hex.green_lizards = 0;
  empty_hex.grey_lizards = 0;
  empty_hex.black_lizards = 0;
  empty_hex.yellow_lizards = 0;
  empty_hex.lizard_pop = 0;

  for (loop = 0; loop < template_hexes [template]; loop ++)
    hex_list [loop] = empty_hex;

  /* find home_hex */

  if (fgets (text_string, 255, template_fptr) == NULL)
  {
    fprintf (log_fptr, "FATAL ERROR: Player %d unspecified HOME HEX.\n");

    printf ("FATAL ERROR: Player %d unspecified HOME HEX. Press SPACE.\n", player);

    getch ();

    exit (EXIT_FAILURE);
  }

  if (text_string [strlen (text_string) - 1] == '\n')
    text_string [strlen (text_string) - 1] = NULL;

  strupr (text_string);

  strtoken (tok_string, text_string, " ");

  if (strcmp (tok_string, HOME_HEX_CODE) != 0)
  {
    fprintf (log_fptr, "FATAL ERROR: Player %d unspecified HOME HEX.\n");

    printf ("FATAL ERROR: Player %d unspecified HOME HEX. Press SPACE.\n", player);

    getch ();

    exit (EXIT_FAILURE);
  }

  home_hex = atoi (text_string);

  if (home_hex == 0 || home_hex > template_hexes [template])
  {
    fprintf (log_fptr, "FATAL ERROR: Player %d specified invalid HOME HEX.\n");

    printf ("FATAL ERROR: Player %d specified invalid HOME HEX. Press SPACE.\n", player);

    getch ();

    exit (EXIT_FAILURE);
  }

  home_hex --; /* adjust it down for 0 based hex list */

  /* build hex list, adding up hexes to 1200 points */

  /* read template until eof or first line that begins with NON-DIGIT char */

  while (fgets (text_string, 256, template_fptr) != NULL)
  {
    if (text_string [strlen (text_string) - 1] == '\n')
      text_string [strlen (text_string) - 1] = NULL;

    strupr (text_string);

    if (!isdigit (text_string [0])) /* non-digit char starts line! */
      break;

    /* find hex list element */

    strtoken (tok_string, text_string, " ");

    present_hex = atoi (tok_string);

    if (present_hex == 0 || present_hex > template_hexes [template])
    {
      /* do nothing, just ignore invalid hexes */
    } else {
      present_hex --; /* adjust down to 0 based list */

      new_hex = empty_hex;

      /* build hex, keeping track of points */

      build_hex (&player_points, &new_hex, text_string);

      hex_list [present_hex] = new_hex;
    }
  }

  /* force volcanoes in invalid positions to peaks */

  /* note in list, any hexes whose x and y offsets are different by 1 or 0
     are ADJACENT! */

  for (loop = 0; loop < template_hexes [template]; loop ++)
    if (hex_list [loop].terrain == VOLCANO) /* volcano hex! */
      for (loop_two = 0; loop_two < template_hexes [template]; loop_two ++)
	if (abs (template_hex_x_offset [template][loop] -
	  template_hex_x_offset [template][loop_two]) <= 1 &&
	  abs (template_hex_y_offset [template][loop] -
	  template_hex_y_offset [template][loop_two]) <= 1 &&
	  loop != loop_two)
	{
	  /* they're adjacent, so check if volcano hex could scrag either den
	     or fertile */

	  if (hex_list [loop_two].terrain == DEN ||
	    hex_list [loop_two].terrain == FERTILE)
	  {
	    /* convert volcano to peak as it's potentially destructive */

	    hex_list [loop].terrain = PEAK;

	    break;
	  }
	}

  /* check home den position correct */

  /* first, count the fertile hexes adjacent to the selected home hex */

  for (loop = 0, fertiles = 0; loop < template_hexes [template]; loop ++)
    if (abs (template_hex_x_offset [template][home_hex] -
      template_hex_x_offset [template][loop]) <= 1 &&
      abs (template_hex_y_offset [template][home_hex] -
      template_hex_y_offset [template][loop]) <= 1 &&
      loop != home_hex && hex_list [loop].terrain == FERTILE)
      fertiles ++; /* adjacent fertile hex */

  /* check their den choice is valid */

  if (hex_list [home_hex].terrain == DEN && fertiles > 0)
  {
    /* valid home den! */

    hex_list [home_hex].terrain = HOME_DEN;

    fprintf (log_fptr, "Player %d successfully built template with %d points.\n",
      player, player_points);

    printf ("Player %d successfully built template with %d points.\n",
      player, player_points);

    lay_template (player, template, hex_list, log_fptr);

    player_home_den [player] = 1;

    free (hex_list);

    sleep (1);

    return;
  } else {
    /* invalid home den choice, see if we can pick another den */

    home_hex = -1; /* invalidate home den hex */

    /* go through the hexes, looking for dens with adjacent fertiles! */

    for (loop = 0, fertiles = 0; loop < template_hexes [template]; loop ++)
      if (hex_list [loop].terrain == DEN)
      {
	/* found a den, see if it has 1 or more fertile hexes adjacent */

	for (loop_two = 0, fertiles = 0; loop_two < template_hexes [template]; loop_two ++)
	  if (abs (template_hex_x_offset [template][loop] -
	    template_hex_x_offset [template][loop_two]) <= 1 &&
	    abs (template_hex_y_offset [template][loop] -
	    template_hex_y_offset [template][loop_two]) <= 1 &&
	    loop_two != loop && hex_list [loop_two].terrain == FERTILE)
	    fertiles ++; /* adjacent fertile hex */

	if (fertiles > 0)
	{
	  /* found a valid home den! make it home den and built template */

	  home_hex = loop;
	}
      }

    if (home_hex > -1)
    {
      /* found a valid replacement home den */

      hex_list [home_hex].terrain = HOME_DEN;

      fprintf (log_fptr, "Player %d selected invalid Home Den, but successfully relocated to hex %d.\n",
	player, home_hex + 1);

      printf ("Player %d selected invalid Home Den, but successfully relocated to hex %d.\n",
	player, home_hex + 1);

      fprintf (log_fptr, "Player %d successfully built template with %d points.\n",
	player, player_points);

      printf ("Player %d successfully built template with %d points.\n",
	player, player_points);

      lay_template (player, template, hex_list, log_fptr);

      player_home_den [player] = 1;

      free (hex_list);

      return;
    } else {
      /* didn't find a valid replacement home den, give player default start */

      fprintf (log_fptr, "Player %d selected invalid Home Den, NO VALID SUBSTITUTE FOUND!.\n",
	player);

      printf ("Player %d selected invalid Home Den, NO VALID SUBSTITUTE FOUND!.\n",
	player);

      fprintf (log_fptr, "Player %d defaulted to startup %d.\n", DEFAULT_STARTUP);

      printf ("Player %d defaulted to startup %d.\n", DEFAULT_STARTUP);

      sprintf (filename, "CUST%d.MAP", DEFAULT_STARTUP);

      if ((default_fptr = fopen (filename, "rt")) == NULL)
      {
        printf ("FATAL ERROR: Unable to read %s file. Press SPACE.\n", filename);

        getch ();

	exit (EXIT_FAILURE);
      }

      build_startup (player, default_fptr, log_fptr);

      fclose (default_fptr);

      return;
    }
  }
}

void lay_template (unsigned int player, unsigned int template, hex_t *hexes,
                   FILE *log_fptr)
{
  char string [256];

  int grid_loc = -1,
      loop,
      x,
      y,
      hex_x,
      hex_y;

  /* find free grid square */

  do {
    grid_loc = random (players_in_game);

    for (loop = 1; loop < players_in_game + 1 && grid_loc != -1; loop ++)
      if (player_grid [loop] == grid_loc)
	grid_loc = -1;
  } while (grid_loc == -1);

  player_grid [player] = grid_loc;

  /* lay template down in grid square */

  x = ((grid_loc % grid_size) * grid_x) + grid_x / 2;
  y = ((grid_loc / grid_size) * grid_y) + grid_y / 2;

  if (x % 2) x ++; /* force to even location! */
  if (y % 2) y ++;

  /* now, start forcing hex types to template type! */

  x --; /* reduce to 0 based map ref! */
  y --;

  fprintf (log_fptr, "Player %d template placed at %s.\n", player,
    xy_to_string (x, y, string));

  printf ("Player %d template placed at %s.\n", player,
    xy_to_string (x, y, string));

  for (loop = 0; loop < template_hexes [template]; loop ++)
  {
    hexes [loop].template_hex = TRUE;

    hex_x = x + template_hex_x_offset [template][loop];
    hex_y = y + template_hex_y_offset [template][loop];

    put_hex (hex_x, hex_y, &hexes [loop]);
  }
}

void build_hex (unsigned int *points, hex_t *hex, char *string)
{
  const char lizard_colours [LIZARD_TYPES][CODE_LENGTH] =
    { RED_LIZARD_CODE, YELLOW_LIZARD_CODE, GREEN_LIZARD_CODE,
      GREY_LIZARD_CODE, BLACK_LIZARD_CODE };

  const char terrain_codes [TERRAIN_TYPES][CODE_LENGTH] =
    { WATER_CODE, PLAIN_CODE, RUIN_CODE, FERTILE_CODE, SWAMP_CODE, PEAK_CODE,
      VOLCANO_CODE, WHIRLPOOL_CODE, SCRUB_CODE, TEMPLE_CODE, CURSED_CODE,
      RED_DEN_CODE, YELLOW_DEN_CODE, GREEN_DEN_CODE, GREY_DEN_CODE,
      BLACK_DEN_CODE };

  const unsigned int terrain_cost [TERRAIN_TYPES] =
    { WATER_COST, PLAIN_COST, RUIN_COST, FERTILE_COST, SWAMP_COST, PEAK_COST,
      VOLCANO_COST, WHIRLPOOL_COST, SCRUB_COST, TEMPLE_COST, CURSED_COST,
      RED_DEN_COST, YELLOW_DEN_COST, GREEN_DEN_COST, GREY_DEN_COST,
      BLACK_DEN_COST };

  char tok_string [256],
       terrain_code [CODE_LENGTH] = { "\x0" },
       lizard_colour [CODE_LENGTH] = { "\x0" };

  int terrain = -1,
      lizards,
      loop;

  /* find terrain type */

  strtoken (tok_string, string, " ");

  strncpy (terrain_code, tok_string, CODE_LENGTH - 1);

  /* find terrain type */

  for (loop = 0; loop < TERRAIN_TYPES; loop ++)
    if (strcmp (terrain_codes [loop], terrain_code) == 0)
    {
      terrain = loop;
      break;
    }

  if (terrain > -1)
  {
    /* check they can afford terrain */

    if (terrain_cost [terrain] + *points <= PLAYER_SETUP_POINTS)
    {
      /* spend points and set to terrain type */

      *points += terrain_cost [terrain];

      switch (terrain)
      {
	case 0:
          hex->terrain = WATER;

          break;
        case 1:
	  hex->terrain = PLAIN;

	  break;
	case 2:
	  hex->terrain = RUIN;

	  break;
	case 3:
	  hex->terrain = FERTILE;

	  break;
	case 4:
	  hex->terrain = SWAMP;

	  break;
	case 5:
	  hex->terrain = PEAK;

	  break;
	case 6:
	  hex->terrain = VOLCANO;

	  break;
	case 7:
	  hex->terrain = WHIRLPOOL;

	  break;
	case 8:
	  hex->terrain = SCRUB;

	  break;
	case 9:
	  hex->terrain = TEMPLE;

	  break;
	case 10:
	  hex->terrain = CURSED;

	  break;
	case 11:
	  hex->terrain = DEN;
	  hex->den_lizard_type = RED_LIZARD;
	  hex->lizard_pop = MINIMUM_POP_PER_DEN * GROUP_SIZE;

	  lizards = (atoi (&tok_string [CODE_LENGTH - 1]) / GROUP_SIZE);

	  if (lizards < 0) lizards = 0;

	  /* see if extra pop can fit in hex! */

	  if ((lizards * GROUP_SIZE) + hex->lizard_pop > MAX_LIZARDS_IN_HEX)
	    lizards -= ((lizards * GROUP_SIZE) + hex->lizard_pop -
	      MAX_LIZARDS_IN_HEX) / GROUP_SIZE;

	  /* see if they can afford the extra pop */

	  if (lizards > 0)
	  {
	    if (*points + (lizards * RED_POP_COST) <= PLAYER_SETUP_POINTS)
	    {
	      hex->lizard_pop += lizards * GROUP_SIZE;

	      *points += (lizards * RED_POP_COST);
	    } else {
	      unsigned int max_groups = 0;

	      /* find max lizard pop they can purchase with remaining points */

	      max_groups = (PLAYER_SETUP_POINTS - *points) / RED_POP_COST;

	      if (max_groups > 0 && max_groups <= lizards)
	      {
		hex->lizard_pop += max_groups * GROUP_SIZE;

		*points += (max_groups * RED_POP_COST);
	      }
	    }
	  }

	  break;
	case 12:
	  hex->terrain = DEN;
	  hex->den_lizard_type = YELLOW_LIZARD;
	  hex->lizard_pop = MINIMUM_POP_PER_DEN * GROUP_SIZE;

          lizards = (atoi (&tok_string [CODE_LENGTH - 1]) / GROUP_SIZE);

	  if (lizards < 0) lizards = 0;

	  /* see if extra pop can fit in hex! */

	  if ((lizards * GROUP_SIZE) + hex->lizard_pop > MAX_LIZARDS_IN_HEX)
	    lizards -= ((lizards * GROUP_SIZE) + hex->lizard_pop -
	      MAX_LIZARDS_IN_HEX) / GROUP_SIZE;

	  /* see if they can afford the extra pop */

	  if (lizards > 0)
	  {
	    if (*points + (lizards * YELLOW_POP_COST) <= PLAYER_SETUP_POINTS)
	    {
	      hex->lizard_pop += lizards * GROUP_SIZE;

	      *points += (lizards * YELLOW_POP_COST);
	    } else {
	      unsigned int max_groups = 0;

	      /* find max lizard pop they can purchase with remaining points */

	      max_groups = (PLAYER_SETUP_POINTS - *points) / YELLOW_POP_COST;

	      if (max_groups > 0 && max_groups <= lizards)
	      {
		hex->lizard_pop += max_groups * GROUP_SIZE;

		*points += (max_groups * YELLOW_POP_COST);
	      }
	    }
	  }

	  break;
	case 13:
	  hex->terrain = DEN;
	  hex->den_lizard_type = GREEN_LIZARD;
	  hex->lizard_pop = MINIMUM_POP_PER_DEN * GROUP_SIZE;

	  lizards = (atoi (&tok_string [CODE_LENGTH - 1]) / GROUP_SIZE);

	  if (lizards < 0) lizards = 0;

	  /* see if extra pop can fit in hex! */

	  if ((lizards * GROUP_SIZE) + hex->lizard_pop > MAX_LIZARDS_IN_HEX)
	    lizards -= ((lizards * GROUP_SIZE) + hex->lizard_pop -
	      MAX_LIZARDS_IN_HEX) / GROUP_SIZE;

	  /* see if they can afford the extra pop */

	  if (lizards > 0)
	  {
	    if (*points + (lizards * GREEN_POP_COST) <= PLAYER_SETUP_POINTS)
	    {
	      hex->lizard_pop += lizards * GROUP_SIZE;

	      *points += (lizards * GREEN_POP_COST);
	    } else {
	      unsigned int max_groups = 0;

	      /* find max lizard pop they can purchase with remaining points */

	      max_groups = (PLAYER_SETUP_POINTS - *points) / GREEN_POP_COST;

	      if (max_groups > 0 && max_groups <= lizards)
	      {
		hex->lizard_pop += max_groups * GROUP_SIZE;

		*points += (max_groups * GREEN_POP_COST);
	      }
	    }
	  }

	  break;
	case 14:
	  hex->terrain = DEN;
	  hex->den_lizard_type = GREY_LIZARD;
	  hex->lizard_pop = MINIMUM_POP_PER_DEN * GROUP_SIZE;

	  lizards = (atoi (&tok_string [CODE_LENGTH - 1]) / GROUP_SIZE);

	  if (lizards < 0) lizards = 0;

	  /* see if extra pop can fit in hex! */

	  if ((lizards * GROUP_SIZE) + hex->lizard_pop > MAX_LIZARDS_IN_HEX)
	    lizards -= ((lizards * GROUP_SIZE) + hex->lizard_pop -
	      MAX_LIZARDS_IN_HEX) / GROUP_SIZE;

	  /* see if they can afford the extra pop */

	  if (lizards > 0)
	  {
	    if (*points + (lizards * GREY_POP_COST) <= PLAYER_SETUP_POINTS)
	    {
	      hex->lizard_pop += lizards * GROUP_SIZE;

	      *points += (lizards * GREY_POP_COST);
	    } else {
	      unsigned int max_groups = 0;

	      /* find max lizard pop they can purchase with remaining points */

	      max_groups = (PLAYER_SETUP_POINTS - *points) / GREY_POP_COST;

	      if (max_groups > 0 && max_groups <= lizards)
	      {
		hex->lizard_pop += max_groups * GROUP_SIZE;

		*points += (max_groups * GREY_POP_COST);
	      }
	    }
	  }

	  break;
	case 15:
	  hex->terrain = DEN;
	  hex->den_lizard_type = BLACK_LIZARD;
	  hex->lizard_pop = MINIMUM_POP_PER_DEN * GROUP_SIZE;

	  lizards = (atoi (&tok_string [CODE_LENGTH - 1]) / GROUP_SIZE);

	  if (lizards < 0) lizards = 0;

	  /* see if extra pop can fit in hex! */

	  if ((lizards * GROUP_SIZE) + hex->lizard_pop > MAX_LIZARDS_IN_HEX)
	    lizards -= ((lizards * GROUP_SIZE) + hex->lizard_pop -
	      MAX_LIZARDS_IN_HEX) / GROUP_SIZE;

	  /* see if they can afford the extra pop */

	  if (lizards > 0)
	  {
	    if (*points + (lizards * BLACK_POP_COST) <= PLAYER_SETUP_POINTS)
	    {
	      hex->lizard_pop += lizards * GROUP_SIZE;

	      *points += (lizards * BLACK_POP_COST);
	    } else {
	      unsigned int max_groups = 0;

	      /* find max lizard pop they can purchase with remaining points */

	      max_groups = (PLAYER_SETUP_POINTS - *points) / BLACK_POP_COST;

	      if (max_groups > 0 && max_groups <= lizards)
	      {
		hex->lizard_pop += max_groups * GROUP_SIZE;

		*points += (max_groups * BLACK_POP_COST);
	      }
	    }
	  }

	  break;
	default:
	  break;
      }
    }

    strtoken (tok_string, string, " ");
  }

  /* find any lizard types in hex and place them down if possible */

  do
  {
    /* find colour code */

    strncpy (lizard_colour, tok_string, CODE_LENGTH - 1);

    for (loop = 0; loop < LIZARD_TYPES; loop ++)
      if (strcmp (lizard_colour, lizard_colours [loop]) == 0)
	break;

    switch (loop)
    {
      case 0:
	lizards = atoi (&tok_string [CODE_LENGTH - 1]) / GROUP_SIZE;

	/* see if extra lizards can fit in hex! */

	if ((lizards * GROUP_SIZE) + lizards_in_hex (hex) > MAX_LIZARDS_IN_HEX)
	  lizards -= ((lizards * GROUP_SIZE) + lizards_in_hex (hex) -
	    MAX_LIZARDS_IN_HEX) / GROUP_SIZE;

	if (lizards > 0)
	  if (*points + (lizards * RED_WARRIOR_COST) <= PLAYER_SETUP_POINTS)
	  {
	    hex->red_lizards += lizards * GROUP_SIZE;

	    *points += (lizards * RED_WARRIOR_COST);
	  } else {
	    unsigned int max_groups = 0;

	    /* find max lizards they can purchase with remaining points */

	    max_groups = (PLAYER_SETUP_POINTS - *points) / RED_WARRIOR_COST;

	    if (max_groups > 0 && max_groups <= lizards)
	    {
	      hex->red_lizards += max_groups * GROUP_SIZE;

	      *points += (max_groups * RED_WARRIOR_COST);
	    }
	  }

	break;
      case 1:
	lizards = atoi (&tok_string [CODE_LENGTH - 1])  / GROUP_SIZE;

	/* see if extra lizards can fit in hex! */

	if ((lizards * GROUP_SIZE) + lizards_in_hex (hex) > MAX_LIZARDS_IN_HEX)
	  lizards -= ((lizards * GROUP_SIZE) + lizards_in_hex (hex) -
	    MAX_LIZARDS_IN_HEX) / GROUP_SIZE;

	if (lizards > 0)
	  if (*points + (lizards * YELLOW_WARRIOR_COST) <= PLAYER_SETUP_POINTS)
	  {
	    hex->yellow_lizards += lizards * GROUP_SIZE;

	    *points += (lizards * YELLOW_WARRIOR_COST);
	  } else {
	    unsigned int max_groups = 0;

	    /* find max lizards they can purchase with remaining points */

	    max_groups = (PLAYER_SETUP_POINTS - *points) / YELLOW_WARRIOR_COST;

	    if (max_groups > 0 && max_groups <= lizards)
	    {
	      hex->yellow_lizards += max_groups * GROUP_SIZE;

	      *points += (max_groups * YELLOW_WARRIOR_COST);
	    }
	  }

	break;
      case 2:
	lizards = atoi (&tok_string [CODE_LENGTH - 1])  / GROUP_SIZE;

	/* see if extra lizards can fit in hex! */

	if ((lizards * GROUP_SIZE) + lizards_in_hex (hex) > MAX_LIZARDS_IN_HEX)
	  lizards -= ((lizards * GROUP_SIZE) + lizards_in_hex (hex) -
	    MAX_LIZARDS_IN_HEX) / GROUP_SIZE;

	if (lizards > 0)
	  if (*points + (lizards * GREEN_WARRIOR_COST) <= PLAYER_SETUP_POINTS)
	  {
	    hex->green_lizards += lizards * GROUP_SIZE;

	    *points += (lizards * GREEN_WARRIOR_COST);
	  } else {
	    unsigned int max_groups = 0;

	    /* find max lizards they can purchase with remaining points */

	    max_groups = (PLAYER_SETUP_POINTS - *points) / GREEN_WARRIOR_COST;

	    if (max_groups > 0 && max_groups <= lizards)
	    {
	      hex->green_lizards += max_groups * GROUP_SIZE;

	      *points += (max_groups * GREEN_WARRIOR_COST);
	    }
	  }

	break;
      case 3:
	lizards = atoi (&tok_string [CODE_LENGTH - 1])  / GROUP_SIZE;

	/* see if extra lizards can fit in hex! */

	if ((lizards * GROUP_SIZE) + lizards_in_hex (hex) > MAX_LIZARDS_IN_HEX)
	  lizards -= ((lizards * GROUP_SIZE) + lizards_in_hex (hex) -
	    MAX_LIZARDS_IN_HEX) / GROUP_SIZE;

	if (lizards > 0)
	  if (*points + (lizards * GREY_WARRIOR_COST) <= PLAYER_SETUP_POINTS)
	  {
	    hex->grey_lizards += lizards * GROUP_SIZE;

	    *points += (lizards * GREY_WARRIOR_COST);
	  } else {
	    unsigned int max_groups = 0;

	    /* find max lizards they can purchase with remaining points */

	    max_groups = (PLAYER_SETUP_POINTS - *points) / GREY_WARRIOR_COST;

	    if (max_groups > 0 && max_groups <= lizards)
	    {
	      hex->grey_lizards += max_groups * GROUP_SIZE;

	      *points += (max_groups * GREY_WARRIOR_COST);
	    }
	  }

	break;
      case 4:
	lizards = atoi (&tok_string [CODE_LENGTH - 1])  / GROUP_SIZE;

	/* see if extra lizards can fit in hex! */

	if ((lizards * GROUP_SIZE) + lizards_in_hex (hex) > MAX_LIZARDS_IN_HEX)
	  lizards -= ((lizards * GROUP_SIZE) + lizards_in_hex (hex) -
	    MAX_LIZARDS_IN_HEX) / GROUP_SIZE;

	if (lizards > 0)
	  if (*points + (lizards * BLACK_WARRIOR_COST) <= PLAYER_SETUP_POINTS)
	  {
	    hex->black_lizards += lizards * GROUP_SIZE;

	    *points += (lizards * BLACK_WARRIOR_COST);
	  } else {
	    unsigned int max_groups = 0;

	    /* find max lizards they can purchase with remaining points */

	    max_groups = (PLAYER_SETUP_POINTS - *points) / BLACK_WARRIOR_COST;

	    if (max_groups > 0 && max_groups <= lizards)
	    {
	      hex->black_lizards += max_groups * GROUP_SIZE;

	      *points += (max_groups * BLACK_WARRIOR_COST);
	    }
	  }

	break;
      default:
	break;
    }

    strtoken (tok_string, string, " ");
  } while (tok_string [0] != NULL);
}
