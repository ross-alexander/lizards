#define CUSTSTAR_LOG_FILE "STARTLOG"

#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "lizards.h"
#include "custom.h"
#include "strgfunc.h"
#include "lizmisc.h"
#include "create.h"

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

int grid_x,
    grid_y,
    grid_size;
int* player_grid;

/* ----------------------------------------------------------------------
--
-- getline
--
---------------------------------------------------------------------- */
static char *getline(char *s, int l, FILE *f)
{
  unsigned int i;
  if ((s = fgets(s, l, f)))
    {
      for (i = 0; i < strlen(s); i++)
	s[i] = islower(s[i]) ? toupper(s[i]) : s[i];
      if (s[i-1] == '\n') s[i-1] = '\0';
    }
  return s;
}

/* ----------------------------------------------------------------------
--
-- build_hex
--
---------------------------------------------------------------------- */

static void build_hex (world_t *w, int *points, hex_t *hex, char *string)
{
  const char lizard_colours [LIZARD_TYPES][CODE_LENGTH] =
    { RED_LIZARD_CODE, YELLOW_LIZARD_CODE, GREEN_LIZARD_CODE,
      GREY_LIZARD_CODE, BLACK_LIZARD_CODE };

  const char terrain_codes [TERRAIN_TYPES][CODE_LENGTH] =
    { WATER_CODE, PLAIN_CODE, RUIN_CODE, FERTILE_CODE, SWAMP_CODE, PEAK_CODE,
      VOLCANO_CODE, WHIRLPOOL_CODE, SCRUB_CODE, TEMPLE_CODE, CURSED_CODE,
      RED_DEN_CODE, YELLOW_DEN_CODE, GREEN_DEN_CODE, GREY_DEN_CODE,
      BLACK_DEN_CODE };

  const int terrain_cost [TERRAIN_TYPES] =
    { WATER_COST, PLAIN_COST, RUIN_COST, FERTILE_COST, SWAMP_COST, PEAK_COST,
      VOLCANO_COST, WHIRLPOOL_COST, SCRUB_COST, TEMPLE_COST, CURSED_COST,
      RED_DEN_COST, YELLOW_DEN_COST, GREEN_DEN_COST, GREY_DEN_COST,
      BLACK_DEN_COST };

  char tok_string [256],
       terrain_code [CODE_LENGTH] = { "\0" },
       lizard_colour [CODE_LENGTH] = { "\0" };

  int terrain = -1,
      lizards,
      loop;
  int max_groups = 0;
  feature_t *f;

  /* find terrain type */
  
  strtoken (tok_string, string, " ");
  strncpy (terrain_code, tok_string, CODE_LENGTH - 1);
  terrain_code[CODE_LENGTH - 1] = '\0';
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
	      hex->terrain = PLAIN;
	      add_feature(w, create_feature(w, RUIN, coord_cons(0, 0)), hex);
	      break;
	    case 3:
	      hex->terrain = PLAIN;
	      add_feature(w, create_feature(w, FERTILE, coord_cons(0, 0)), hex);
	      break;
	    case 4:
	      hex->terrain = SWAMP;
	      break;
	    case 5:
	      hex->terrain = PEAK;
	      break;
	    case 6:
	      hex->terrain = PEAK;
	      add_feature(w, create_feature(w, VOLCANO, coord_cons(0, 0)), hex);
	      break;
	    case 7:
	      hex->terrain = WATER;
	      add_feature(w, create_feature(w, WHIRLPOOL, coord_cons(0, 0)), hex);
	  break;
	    case 8:
	      hex->terrain = SCRUB;
	      break;
	    case 9:
	      hex->terrain = PLAIN;
	      add_feature(w, create_feature(w, TEMPLE, coord_cons(0, 0)), hex);
	  break;
	    case 10:
	      hex->terrain = PLAIN;
	      add_feature(w, create_feature(w, CURSED, coord_cons(0, 0)), hex);
	      break;
	    case 11:
	      hex->terrain = PLAIN;
	      f = create_feature(w, DEN, coord_cons(0, 0));
	      add_feature(w, f, hex);
	      f->f.den.type = RED_LIZARD;
	      f->f.den.pop = MINIMUM_POP_PER_DEN * GROUP_SIZE;
	      lizards = (atoi (&tok_string [CODE_LENGTH - 1]) / GROUP_SIZE);
	      if (lizards < 0) lizards = 0;
	      /* see if extra pop can fit in hex! */
	      if ((lizards * GROUP_SIZE) + f->f.den.pop > MAX_LIZARDS_IN_HEX)
		lizards -= ((lizards * GROUP_SIZE) + f->f.den.pop -
			    MAX_LIZARDS_IN_HEX) / GROUP_SIZE;

	  /* see if they can afford the extra pop */
	      if (lizards > 0)
		{
		  if (*points + (lizards * RED_POP_COST) <= PLAYER_SETUP_POINTS)
		    {
		      f->f.den.pop += lizards * GROUP_SIZE;
		      *points += (lizards * RED_POP_COST);
		    }
		  else
		    {
		      /* find max lizard pop they can purchase with remaining points */

		      max_groups = (PLAYER_SETUP_POINTS - *points) / RED_POP_COST;
		      if (max_groups > 0 && max_groups <= lizards)
			{
			  f->f.den.pop += max_groups * GROUP_SIZE;
			  *points += (max_groups * RED_POP_COST);
			}
		    }
		}
	      break;
	case 12:
	      hex->terrain = PLAIN;
	      f = create_feature(w, DEN, coord_cons(0, 0));
	      add_feature(w, f, hex);
	      f->f.den.type = YELLOW_LIZARD;
	      f->f.den.pop = MINIMUM_POP_PER_DEN * GROUP_SIZE;
	      lizards = (atoi (&tok_string [CODE_LENGTH - 1]) / GROUP_SIZE);
	      if (lizards < 0) lizards = 0;
	      /* see if extra pop can fit in hex! */
	      if ((lizards * GROUP_SIZE) + f->f.den.pop > MAX_LIZARDS_IN_HEX)
		lizards -= ((lizards * GROUP_SIZE) + f->f.den.pop -
			    MAX_LIZARDS_IN_HEX) / GROUP_SIZE;

	  /* see if they can afford the extra pop */
	      if (lizards > 0)
		{
		  if (*points + (lizards * YELLOW_POP_COST) <= PLAYER_SETUP_POINTS)
		    {
		      f->f.den.pop += lizards * GROUP_SIZE;
		      *points += (lizards * YELLOW_POP_COST);
		    }
		  else
		    {
		      /* find max lizard pop they can purchase with remaining points */

		      max_groups = (PLAYER_SETUP_POINTS - *points) / YELLOW_POP_COST;
		      if (max_groups > 0 && max_groups <= lizards)
			{
			  f->f.den.pop += max_groups * GROUP_SIZE;
			  *points += (max_groups * YELLOW_POP_COST);
			}
		    }
		}
	      break;
	case 13:
	      hex->terrain = PLAIN;
	      f = create_feature(w, DEN, coord_cons(0, 0));
	      add_feature(w, f, hex);
	      f->f.den.type = GREEN_LIZARD;
	      f->f.den.pop = MINIMUM_POP_PER_DEN * GROUP_SIZE;
	      lizards = (atoi (&tok_string [CODE_LENGTH - 1]) / GROUP_SIZE);
	      if (lizards < 0) lizards = 0;
	      /* see if extra pop can fit in hex! */
	      if ((lizards * GROUP_SIZE) + f->f.den.pop > MAX_LIZARDS_IN_HEX)
		lizards -= ((lizards * GROUP_SIZE) + f->f.den.pop -
			    MAX_LIZARDS_IN_HEX) / GROUP_SIZE;

	  /* see if they can afford the extra pop */
	      if (lizards > 0)
		{
		  if (*points + (lizards * GREEN_POP_COST) <= PLAYER_SETUP_POINTS)
		    {
		      f->f.den.pop += lizards * GROUP_SIZE;
		      *points += (lizards * GREEN_POP_COST);
		    }
		  else
		    {
		      /* find max lizard pop they can purchase with remaining points */

		      max_groups = (PLAYER_SETUP_POINTS - *points) / GREEN_POP_COST;
		      if (max_groups > 0 && max_groups <= lizards)
			{
			  f->f.den.pop += max_groups * GROUP_SIZE;
			  *points += (max_groups * GREEN_POP_COST);
			}
		    }
		}
	      break;
	    case 14:
	      hex->terrain = PLAIN;
	      f = create_feature(w, DEN, coord_cons(0, 0));
	      add_feature(w, f, hex);
	      f->f.den.type = GREY_LIZARD;
	      f->f.den.pop = MINIMUM_POP_PER_DEN * GROUP_SIZE;
	      lizards = (atoi (&tok_string [CODE_LENGTH - 1]) / GROUP_SIZE);
	      if (lizards < 0) lizards = 0;
	      /* see if extra pop can fit in hex! */
	      if ((lizards * GROUP_SIZE) + f->f.den.pop > MAX_LIZARDS_IN_HEX)
		lizards -= ((lizards * GROUP_SIZE) + f->f.den.pop -
			    MAX_LIZARDS_IN_HEX) / GROUP_SIZE;

	  /* see if they can afford the extra pop */
	      if (lizards > 0)
		{
		  if (*points + (lizards * GREY_POP_COST) <= PLAYER_SETUP_POINTS)
		    {
		      f->f.den.pop += lizards * GROUP_SIZE;
		      *points += (lizards * GREY_POP_COST);
		    }
		  else
		    {
		      /* find max lizard pop they can purchase with remaining points */

		      max_groups = (PLAYER_SETUP_POINTS - *points) / GREY_POP_COST;
		      if (max_groups > 0 && max_groups <= lizards)
			{
			  f->f.den.pop += max_groups * GROUP_SIZE;
			  *points += (max_groups * GREY_POP_COST);
			}
		    }
		}
	      break;
	case 15:
	      hex->terrain = PLAIN;
	      f = create_feature(w, DEN, coord_cons(0, 0));
	      add_feature(w, f, hex);
	      f->f.den.type = BLACK_LIZARD;
	      f->f.den.pop = MINIMUM_POP_PER_DEN * GROUP_SIZE;
	      lizards = (atoi (&tok_string [CODE_LENGTH - 1]) / GROUP_SIZE);
	      if (lizards < 0) lizards = 0;
	      /* see if extra pop can fit in hex! */
	      if ((lizards * GROUP_SIZE) + f->f.den.pop > MAX_LIZARDS_IN_HEX)
		lizards -= ((lizards * GROUP_SIZE) + f->f.den.pop -
			    MAX_LIZARDS_IN_HEX) / GROUP_SIZE;

	  /* see if they can afford the extra pop */
	      if (lizards > 0)
		{
		  if (*points + (lizards * BLACK_POP_COST) <= PLAYER_SETUP_POINTS)
		    {
		      f->f.den.pop += lizards * GROUP_SIZE;
		      *points += (lizards * BLACK_POP_COST);
		    }
		  else
		    {
		      /* find max lizard pop they can purchase with remaining points */

		      max_groups = (PLAYER_SETUP_POINTS - *points) / BLACK_POP_COST;
		      if (max_groups > 0 && max_groups <= lizards)
			{
			  f->f.den.pop += max_groups * GROUP_SIZE;
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
      f = create_feature(w, BAND, coord_cons(0, 0));
      strncpy (lizard_colour, tok_string, CODE_LENGTH - 1);
      lizard_colour[CODE_LENGTH - 1] = '\0';
      for (loop = 0; loop < LIZARD_TYPES; loop ++)
	if (strcmp (lizard_colour, lizard_colours [loop]) == 0)
	  break;
      switch (loop)
	{
	case 0:
	  lizards = atoi (&tok_string [CODE_LENGTH - 1]) / GROUP_SIZE;
	  /* see if extra lizards can fit in hex! */
	  if ((lizards * GROUP_SIZE) + lizards_in_band(f) > MAX_LIZARDS_IN_HEX)
	    lizards -= ((lizards * GROUP_SIZE) + lizards_in_hex (hex) -
			MAX_LIZARDS_IN_HEX) / GROUP_SIZE;
	  if (lizards > 0)
	    if (*points + (lizards * RED_WARRIOR_COST) <= PLAYER_SETUP_POINTS)
	      {
		f->f.band.type[RED] += lizards * GROUP_SIZE;
		*points += (lizards * RED_WARRIOR_COST);
	      }
	    else
	      {
		/* find max lizards they can purchase with remaining points */
		max_groups = (PLAYER_SETUP_POINTS - *points) / RED_WARRIOR_COST;
		if (max_groups > 0 && max_groups <= lizards)
		  {
		    f->f.band.type[RED] += max_groups * GROUP_SIZE;
		    *points += (max_groups * RED_WARRIOR_COST);
		  }
	      }
	  break;
	case 1:
	  lizards = atoi (&tok_string [CODE_LENGTH - 1])  / GROUP_SIZE;
	/* see if extra lizards can fit in hex! */
	  if ((lizards * GROUP_SIZE) + lizards_in_band(f) > MAX_LIZARDS_IN_HEX)
	    lizards -= ((lizards * GROUP_SIZE) + lizards_in_hex (hex) -
			MAX_LIZARDS_IN_HEX) / GROUP_SIZE;
	  if (lizards > 0)
	    if (*points + (lizards * YELLOW_WARRIOR_COST) <= PLAYER_SETUP_POINTS)
	      {
		f->f.band.type[YELLOW] += lizards * GROUP_SIZE;
		*points += (lizards * YELLOW_WARRIOR_COST);
	      }
	    else
	      {
	    /* find max lizards they can purchase with remaining points */
		max_groups = (PLAYER_SETUP_POINTS - *points) / YELLOW_WARRIOR_COST;
		if (max_groups > 0 && max_groups <= lizards)
		  {
		    f->f.band.type[YELLOW] += max_groups * GROUP_SIZE;
		    *points += (max_groups * YELLOW_WARRIOR_COST);
		  }
	      }
	  break;
	case 2:
	  lizards = atoi (&tok_string [CODE_LENGTH - 1])  / GROUP_SIZE;
	  /* see if extra lizards can fit in hex! */
	  if ((lizards * GROUP_SIZE) + lizards_in_band(f) > MAX_LIZARDS_IN_HEX)
	    lizards -= ((lizards * GROUP_SIZE) + lizards_in_hex (hex) -
			MAX_LIZARDS_IN_HEX) / GROUP_SIZE;
	  if (lizards > 0)
	    if (*points + (lizards * GREEN_WARRIOR_COST) <= PLAYER_SETUP_POINTS)
	      {
		f->f.band.type[GREEN] += lizards * GROUP_SIZE;
		*points += (lizards * GREEN_WARRIOR_COST);
	      }
	    else
	      {
		/* find max lizards they can purchase with remaining points */
		max_groups = (PLAYER_SETUP_POINTS - *points) / GREEN_WARRIOR_COST;
		if (max_groups > 0 && max_groups <= lizards)
		  {
		    f->f.band.type[GREEN] += max_groups * GROUP_SIZE;
		    *points += (max_groups * GREEN_WARRIOR_COST);
		  }
	      }
	  break;
	case 3:
	  lizards = atoi (&tok_string [CODE_LENGTH - 1])  / GROUP_SIZE;
	  /* see if extra lizards can fit in hex! */
	  if ((lizards * GROUP_SIZE) + lizards_in_band(f) > MAX_LIZARDS_IN_HEX)
	    lizards -= ((lizards * GROUP_SIZE) + lizards_in_hex (hex) -
			MAX_LIZARDS_IN_HEX) / GROUP_SIZE;
	  if (lizards > 0)
	    if (*points + (lizards * GREY_WARRIOR_COST) <= PLAYER_SETUP_POINTS)
	      {
		f->f.band.type[GREY] += lizards * GROUP_SIZE;
		*points += (lizards * GREY_WARRIOR_COST);
	      }
	    else 
	      {
		/* find max lizards they can purchase with remaining points */
		max_groups = (PLAYER_SETUP_POINTS - *points) / GREY_WARRIOR_COST;
		if (max_groups > 0 && max_groups <= lizards)
		  {
		    f->f.band.type[GREY] += max_groups * GROUP_SIZE;
		    *points += (max_groups * GREY_WARRIOR_COST);
		  }
	      }
	  break;
	case 4:
	  lizards = atoi (&tok_string [CODE_LENGTH - 1])  / GROUP_SIZE;
	  /* see if extra lizards can fit in hex! */
	  if ((lizards * GROUP_SIZE) + lizards_in_band(f) > MAX_LIZARDS_IN_HEX)
	    lizards -= ((lizards * GROUP_SIZE) + lizards_in_hex (hex) -
			MAX_LIZARDS_IN_HEX) / GROUP_SIZE;
	  if (lizards > 0)
	    if (*points + (lizards * BLACK_WARRIOR_COST) <= PLAYER_SETUP_POINTS)
	      {
		f->f.band.type[BLACK] += lizards * GROUP_SIZE;
		*points += (lizards * BLACK_WARRIOR_COST);
	      }
	    else 
	      {
		/* find max lizards they can purchase with remaining points */
		max_groups = (PLAYER_SETUP_POINTS - *points) / BLACK_WARRIOR_COST;
		
		if (max_groups > 0 && max_groups <= lizards)
		  {
		    f->f.band.type[BLACK] += max_groups * GROUP_SIZE;
		    *points += (max_groups * BLACK_WARRIOR_COST);
		  }
	      }
	  break;
	default:
	  break;
	}
      if (lizards_in_band(f))
	add_feature(w, f, hex);
      else
	free(f);
 
      strtoken (tok_string, string, " \n");
    }
  while (tok_string [0] != '\0');
}

/* ----------------------------------------------------------------------
--
-- lay_template
--
---------------------------------------------------------------------- */
static void lay_template (world_t *world, int p, int tmpl,
		   hex_t *hexes, FILE *log_fptr)
{
  char string [256];
  hex_t *hex;
  int grid_loc = -1,
      loop,
      x,
      y,
      hex_x,
      hex_y;
  feature_t *f;
  /* find free grid square */

  do
    {
      grid_loc = random (world->num_players);
      for (loop = 1; loop < world->num_players && grid_loc != -1; loop ++)
	if (player_grid [loop] == grid_loc)
	  grid_loc = -1;
    }
  while (grid_loc == -1);
  player_grid [p] = grid_loc;

  /* lay tmpl down in grid square */

  x = ((grid_loc % grid_size) * grid_x) + grid_x / 2;
  y = ((grid_loc / grid_size) * grid_y) + grid_y / 2;
  
  if (x % 2) x ++; /* force to even location! */
  if (y % 2) y ++;

  /* now, start forcing hex types to tmpl type! */

  x --; /* reduce to 0 based map ref! */
  y --;

  fprintf (log_fptr, "Player %d tmpl placed at %s.\n", p,
	   xy_to_string (x, y, string));
  
  printf ("Player %d tmpl placed at %s.\n", p,
	  xy_to_string (x, y, string));

  for (loop = 0; loop < template_hexes [tmpl]; loop ++)
    {
      hex_x = x + template_hex_x_offset [tmpl][loop];
      hex_y = y + template_hex_y_offset [tmpl][loop];

      hexes[loop].xy.x = hex_x;
      hexes[loop].xy.y = hex_y;
      hex = get_hex(world, hexes[loop].xy);
      clear_features(world, hex);

      hex->xy = hexes[loop].xy;
      hex->owner = hexes[loop].owner;
      hex->terrain = hexes[loop].terrain;
      hex->tmpl = LIZ_TRUE;
      while ((f = pop_feature(world, &hexes[loop])))
	add_feature(world, f, hex);
    }
}

/* ----------------------------------------------------------------------
--
-- BuildStartup
--
---------------------------------------------------------------------- */

static void build_startup (world_t *world,int p, FILE *template_fptr, FILE *log_fptr)
{
  FILE *default_fptr;

  char text_string [256],
    tok_string [256],
    filename [256];
  
  int home_hex;

  int tmpl,
    player_points = 0,
    present_hex = 0,
    loop,
    loop_two,
    fertiles;

  hex_t empty_hex,
	new_hex;
  hex_t *hex_list;
  feature_t *f;

  /* find tmpl */

  if (getline(text_string, 255, template_fptr) == NULL)
    {
      fprintf (log_fptr, "FATAL ERROR: Player %d unspecified TMPL.\n", p);
      printf ("FATAL ERROR: Player %d unspecified TMPL. \n", p);
      exit (EXIT_FAILURE);
    }
  strtoken (tok_string, text_string, " ");

  if (strcmp (tok_string, TEMPLATE_CODE) != 0)
    {
      fprintf (log_fptr, "FATAL ERROR: Player %d unspecified TMPL.\n", p);
      printf ("FATAL ERROR: Player %d unspecified TMPL. \n", p);
      exit (EXIT_FAILURE);
    }
  tmpl = atoi (text_string);

  if (tmpl == 0 || tmpl > TEMPLATES)
    {
      fprintf (log_fptr, "FATAL ERROR: Player %d specified invalid TMPL.\n", p);

      printf ("FATAL ERROR: Player %d specified invalid TMPL. \n", p);
      exit (EXIT_FAILURE);
    }

  /* build tmpl list */

  if ((hex_list = (hex_t*)calloc(sizeof(hex_t), template_hexes [tmpl])) == NULL)
    {
      printf ("FATAL ERROR: Out of memory for malloc. \n");
      exit (EXIT_FAILURE);
    }

  /* cost tmpl list */
  player_points += template_cost [tmpl];

  /* wipe tmpl list */
  empty_hex.terrain = WATER;
  empty_hex.chain = NULL;
  empty_hex.owner = p;
  for (loop = 0; loop < template_hexes [tmpl]; loop ++)
    {
      hex_list[loop].terrain = WATER;
      empty_hex.chain = NULL;
      hex_list[loop].owner = 0;
    }
  /* find home_hex */

  if (getline(text_string, 255, template_fptr) == NULL)
    {
      fprintf (log_fptr, "FATAL ERROR: Player %d unspecified HOME HEX.\n", p);
      printf ("FATAL ERROR: Player %d unspecified HOME HEX. \n", p);
      exit (EXIT_FAILURE);
    }
  strtoken (tok_string, text_string, " ");
  if (strcmp (tok_string, HOME_HEX_CODE) != 0)
    {
      fprintf (log_fptr, "FATAL ERROR: Player %d unspecified HOME HEX.\n", p);
      printf ("FATAL ERROR: Player %d unspecified HOME HEX. \n", p);
      exit (EXIT_FAILURE);
    }
  home_hex = atoi (text_string);
  if (home_hex == 0 || home_hex > template_hexes [tmpl])
    {
      fprintf (log_fptr, "FATAL ERROR: Player %d specified invalid HOME HEX.\n", p);
      printf ("FATAL ERROR: Player %d specified invalid HOME HEX. \n", p);
      exit (EXIT_FAILURE);
    }
    home_hex --; /* adjust it down for 0 based hex list */

    /* build hex list, adding up hexes to 1200 points */
    /* read tmpl until eof or first line that begins with NON-DIGIT char */

    while (getline(text_string, 256, template_fptr) != NULL)
      {
	if (!isdigit (text_string [0])) /* non-digit char starts line! */
	  break;
	/* find hex list element */
	strtoken (tok_string, text_string, " ");
	present_hex = atoi (tok_string);
	if (present_hex == 0 || present_hex > template_hexes [tmpl])
	  {
      /* do nothing, just ignore invalid hexes */
	  }
	else
	  {
	    present_hex --; /* adjust down to 0 based list */
	    new_hex = empty_hex;
	    /* build hex, keeping track of points */
	    build_hex (world, &player_points, &new_hex, text_string);
	    hex_list [present_hex] = new_hex;
	  }
      }

  /* force volcanoes in invalid positions to peaks */

  /* note in list, any hexes whose x and y offsets are different by 1 or 0
     are ADJACENT! */

  for (loop = 0; loop < template_hexes [tmpl]; loop ++)
    if ((f = has_feature(&hex_list[loop], VOLCANO)))
      for (loop_two = 0; loop_two < template_hexes [tmpl]; loop_two ++)
	if (abs (template_hex_x_offset [tmpl][loop] -
		 template_hex_x_offset [tmpl][loop_two]) <= 1 &&
	    abs (template_hex_y_offset [tmpl][loop] -
		 template_hex_y_offset [tmpl][loop_two]) <= 1 &&
	    loop != loop_two)
	  {
	  /* they're adjacent, so check if volcano hex could scrag either den
	     or fertile */
	    if (has_feature(&hex_list[loop_two], DEN) ||
		has_feature(&hex_list[loop_two], FERTILE))
	      {
		/* convert volcano to peak as it's potentially destructive */
		free(release_feature(world, f));
		break;
	      }
	  }
  
  /* check home den position correct */

  /* first, count the fertile hexes adjacent to the selected home hex */

  for (loop = 0, fertiles = 0; loop < template_hexes [tmpl]; loop ++)
    if (abs (template_hex_x_offset [tmpl][home_hex] -
	     template_hex_x_offset [tmpl][loop]) <= 1 &&
	abs (template_hex_y_offset [tmpl][home_hex] -
	     template_hex_y_offset [tmpl][loop]) <= 1 &&
	loop != home_hex && has_feature(&hex_list[loop], FERTILE))
      fertiles ++; /* adjacent fertile hex */

  /* check their den choice is valid */

  if ((f = has_feature(&hex_list[home_hex], DEN)) && fertiles > 0)
    {
      /* valid home den! */
      f->f.den.home = 1;
      fprintf (log_fptr, "Player %d successfully built tmpl with %d points.\n",
	       p, player_points);
    
      printf ("Player %d successfully built tmpl with %d points.\n",
	      p, player_points);
      lay_template (world, p, tmpl, hex_list, log_fptr);
      free (hex_list);
      return;
    }
  else
    {
    /* invalid home den choice, see if we can pick another den */
    home_hex = -1; /* invalidate home den hex */

    /* go through the hexes, looking for dens with adjacent fertiles! */

    for (loop = 0, fertiles = 0; loop < template_hexes [tmpl]; loop ++)
      if ((f = has_feature(&hex_list [loop], DEN)))
	{
	/* found a den, see if it has 1 or more fertile hexes adjacent */

	  for (loop_two = 0, fertiles = 0; loop_two < template_hexes [tmpl]; loop_two ++)
	    if (abs (template_hex_x_offset [tmpl][loop] -
		     template_hex_x_offset [tmpl][loop_two]) <= 1 &&
		abs (template_hex_y_offset [tmpl][loop] -
		     template_hex_y_offset [tmpl][loop_two]) <= 1 &&
		loop_two != loop && has_feature(&hex_list[loop_two], FERTILE))
	      fertiles ++; /* adjacent fertile hex */
	  if (fertiles > 0)
	    {
	  /* found a valid home den! make it home den and built tmpl */
	      home_hex = loop;
	    }
	}
    if (home_hex > -1)
      {
      /* found a valid replacement home den */
	f->f.den.home = 1;
	fprintf (log_fptr, "Player %d selected invalid Home Den, but successfully relocated to hex %d.\n",
		 p, home_hex + 1);
	printf ("Player %d selected invalid Home Den, but successfully relocated to hex %d.\n",
		p, home_hex + 1);
	fprintf (log_fptr,
		 "Player %d successfully built tmpl with %d points.\n",
		 p, player_points);
	printf ("Player %d successfully built tmpl with %d points.\n",
		p, player_points);
	lay_template (world, p, tmpl, hex_list, log_fptr);
	free (hex_list);
	return;
      }
    else
      {
      /* didn't find a valid replacement home den, give player default start */

	fprintf (log_fptr, "Player %d selected invalid Home Den, NO VALID SUBSTITUTE FOUND!.\n",
		 p);
	
	printf ("Player %d selected invalid Home Den, NO VALID SUBSTITUTE FOUND!.\n",
		p);
	fprintf (log_fptr, "Player %d defaulted to startup %d.\n", p, DEFAULT_STARTUP);
	printf ("Player %d defaulted to startup %d.\n", p, DEFAULT_STARTUP);
	sprintf (filename, "CUST%d.MAP", DEFAULT_STARTUP);
	if ((default_fptr = fopen (filename, "rt")) == NULL)
	  {
	    printf ("FATAL ERROR: Unable to read %s file.\n", filename);
	    exit (EXIT_FAILURE);
	  }
	build_startup (world, p, default_fptr, log_fptr);
	fclose (default_fptr);
	return;
      }
    }
}

/* ----------------------------------------------------------------------
--
-- CustomStartup
--
---------------------------------------------------------------------- */
void CustomStartup(world_t *world)
{
  FILE *template_fptr,
       *log_fptr;

  char *filename,
    *text_string,
    *tok_string;

  int p,
    startup,
    loop;
  int *home_den;

  home_den = (int*)calloc(world->num_players, sizeof(int));
  player_grid = (int*)calloc(world->num_players, sizeof(int));
  grid_size = (int)(sqrt (world->num_players));

  if (grid_size * grid_size < world->num_players) grid_size ++;

  grid_x = world->x / grid_size;
  grid_y = world->y / grid_size;

  for (loop = 0; loop < world->num_players; loop ++)
    {
      home_den[loop] = 0;
      player_grid[loop] = -1;
    }
  filename = (char*)alloca(256);
  sprintf (filename, "%s%s.%s", world->path, world->game, CUSTSTAR_LOG_FILE);
  if ((log_fptr = fopen (filename, "w")) == NULL)
    {
      printf ("FATAL ERROR: Unable to write to %s file. \n", filename);
      exit (EXIT_FAILURE);
    }

  fprintf (log_fptr, "\nCustom Startup Log File for LIZARDS! Game %s.\n\n", world->game);

  for (p = 1; p < world->num_players; p++)
    {
      text_string = strdup(world->player[p]->startup);
      tok_string = strtok (text_string, " ");
      if (home_den[p] != 0)
	{
	  fprintf (log_fptr,
		   "FATAL ERROR: Attempted to re-start player %d.\n",
		   p);
	  printf ("FATAL ERROR: Attempted to re-start player %d. \n", p);
	  exit (EXIT_FAILURE);
	}
      if (strcmp (tok_string, STARTUP_CODE) == 0)
	{
	  tok_string = strtok(NULL, " \n");
	  if (!isdigit (tok_string[0]) && tok_string[0] != 'C')
	    {
	      fprintf (log_fptr,
		       "FATAL ERROR: Player %d selected invalid STARTUP.\n",
		       p);
	      printf ("FATAL ERROR: Player %d selected invalid STARTUP. \n",
		      p);
	      exit (EXIT_FAILURE);
	    }
	  startup = atoi (tok_string);
	  if (startup > VALID_STARTUP)
	    {
	      fprintf (log_fptr, "FATAL ERROR: Player %d selected invalid STARTUP.\n",
		       p);
	      printf ("FATAL ERROR: Player %d selected invalid STARTUP. \n",
		      p);
	      exit (EXIT_FAILURE);
	    }
	  switch (startup)
	    {
	    case CUSTOM_STARTUP:
	      filename = strtok (NULL, " \n");
	      if ((template_fptr = fopen (filename, "rt")) == NULL)
		{
		  printf ("FATAL ERROR: Unable to read %s file. \n", filename);
		  exit (EXIT_FAILURE);
		}
	      build_startup (world, p, template_fptr, log_fptr);
	      fclose(template_fptr);
	      break;
	    case 1:
	    case 2:
	    case 3:
	    case 4:
	      sprintf (filename, "CUST%d.MAP", startup);
	      if ((template_fptr = fopen (filename, "r")) == NULL)
		{
		  printf ("FATAL ERROR: Unable to read %s file. \n", filename);
		  exit (EXIT_FAILURE);
		}
	      build_startup (world, p, template_fptr, log_fptr);
	      fclose (template_fptr);
	      break;
	    default:
	      if (startup > VALID_STARTUP)
		{
		  fprintf (log_fptr, "FATAL ERROR: Player %d selected invalid STARTUP.\n",
			   p);
		  
		  printf ("FATAL ERROR: Player %d selected invalid STARTUP. \n",
			  p);
		  exit (EXIT_FAILURE);
		}
	    }
	}
      else
	{
	  fprintf (log_fptr, "FATAL ERROR: Player %d selected invalid STARTUP.\n",
		   p);
	  printf ("FATAL ERROR: Player %d unspecified STARTUP. \n",
		  p);
	  exit (EXIT_FAILURE);
	}
      free(text_string);
    }
  free(home_den);
  free(player_grid);
  fprintf (log_fptr, "\nEND OF CUSTOM STARTUP LOG FILE\n");
  fclose (log_fptr);
}
