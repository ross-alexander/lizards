#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <math.h>
#include <alloca.h>
#include <assert.h>
#include <libxml/tree.h>
#include <libxml++/libxml++.h>

#include "lizards.h"


#define TEMPLATES 8
#define MAX_TEMPLATE_HEX 22

#define DEFAULT_STARTUP 3 /* which one the players get stuck with if they fail to allocate valid home den */
#define VALID_STARTUP 4
#define CUSTOM_STARTUP 0

#define TEMPLATE1_HEX 7
#define TEMPLATE2_HEX 9
#define TEMPLATE3_HEX 11
#define TEMPLATE4_HEX 14
#define TEMPLATE5_HEX 16
#define TEMPLATE6_HEX 17
#define TEMPLATE7_HEX 19
#define TEMPLATE8_HEX 22

#define TEMPLATE1_COST 100
#define TEMPLATE2_COST 110
#define TEMPLATE3_COST 120
#define TEMPLATE4_COST 140
#define TEMPLATE5_COST 150
#define TEMPLATE6_COST 160
#define TEMPLATE7_COST 180
#define TEMPLATE8_COST 200

#define CUSTSTAR_LOG_FILE "STARTLOG"

#define PLAYER_CODE "PLAYER"
#define STARTUP_CODE "STARTUP"
#define TEMPLATE_CODE "TEMPLATE"
#define HOME_HEX_CODE "HOME"

#define TERRAIN_TYPES 16
#define CODE_LENGTH 3

#define RUIN_CODE	"RUI"
#define PLAIN_CODE	"PLA"
#define SWAMP_CODE	"SWA"
#define SCRUB_CODE	"SCR"
#define FERTILE_CODE	"FER"
#define PEAK_CODE	"PEA"
#define VOLCANO_CODE	"VOL"
#define TEMPLE_CODE	"TEM"
#define CURSED_CODE	"CUR"
#define WHIRLPOOL_CODE	"WHI"
#define WATER_CODE	"WAT"

#define RED_DEN_CODE	"RED"
#define YELLOW_DEN_CODE	"YEL"
#define GREEN_DEN_CODE	"GRN"
#define GREY_DEN_CODE	"GRY"
#define BLACK_DEN_CODE	"BLK"

#define RED_LIZARD_CODE		"RED"
#define YELLOW_LIZARD_CODE	"YEL"
#define GREEN_LIZARD_CODE	"GRN"
#define GREY_LIZARD_CODE	"GRY"
#define BLACK_LIZARD_CODE	"BLK"

#define WATER_COST 0
#define PLAIN_COST 15
#define RUIN_COST 40
#define FERTILE_COST 25
#define SWAMP_COST 10
#define PEAK_COST 10
#define VOLCANO_COST 10
#define WHIRLPOOL_COST 5
#define SCRUB_COST 10
#define TEMPLE_COST 40
#define CURSED_COST 15

#define GROUP_SIZE 10		/* lizards are dealt with in 10 groups */

#define RED_WARRIOR_COST 10	/* all per 10 pop lizards */
#define YELLOW_WARRIOR_COST 15
#define GREEN_WARRIOR_COST 20
#define GREY_WARRIOR_COST 20
#define BLACK_WARRIOR_COST 30

#define RED_POP_COST 15 	/* all per 10 pop lizards */
#define YELLOW_POP_COST 25
#define GREEN_POP_COST 25
#define GREY_POP_COST 30
#define BLACK_POP_COST 50

#define MINIMUM_POP_PER_DEN 3	/* 3 groups of 10 pop lizards! */

#define RED_DEN_COST 65 + (MINIMUM_POP_PER_DEN * RED_POP_COST)
#define YELLOW_DEN_COST 100 + (MINIMUM_POP_PER_DEN * YELLOW_POP_COST)
#define GREEN_DEN_COST 110 + (MINIMUM_POP_PER_DEN * GREEN_POP_COST)
#define GREY_DEN_COST 95 + (MINIMUM_POP_PER_DEN * GREY_POP_COST)
#define BLACK_DEN_COST 150 + (MINIMUM_POP_PER_DEN * BLACK_POP_COST)

#define PLAYER_SETUP_POINTS 1200

struct tmpl_xy_t {
  int x;
  int y;
};

struct template_t {
  int size;
  int cost;
  tmpl_xy_t offsets[MAX_TEMPLATE_HEX];
};

struct warrior_map_t {
  int id;
  const char *text;
  int cost;
};

static warrior_map_t warrior_map[] = {
  {RED,    "RED", RED_WARRIOR_COST},
  {GREEN,  "GRN", GREEN_WARRIOR_COST},
  {GREY,   "GRY", GREY_WARRIOR_COST},
  {BLACK,  "BLK", BLACK_WARRIOR_COST},
  {YELLOW, "YEL", YELLOW_WARRIOR_COST},
};

template_t templates[] = {
  { 0, 0, {}},
  { TEMPLATE1_HEX, TEMPLATE1_COST, 
    {
      {-1, 0}, {-1, 1},
      { 0,-1}, { 0, 0}, { 0, 1},
      { 1, 0}, { 1, 1}
    },
  },
  { TEMPLATE2_HEX, TEMPLATE2_COST, 
    {
      {-1, 0}, {-1, 1}, {-1, 2},
      { 0,-1}, { 0, 0}, { 0, 1},
      { 1,-1}, { 1, 0}, { 1, 1}
    }
  },
  { TEMPLATE3_HEX, TEMPLATE3_COST, 
    {
      {-2, 1},
      {-1, 0}, {-1, 1}, {-1, 2},
      { 0,-1}, { 0, 0}, { 0, 1},
      { 1,-1}, { 1, 0}, { 1, 1},
      { 2,-1}
    }
  },
  { TEMPLATE4_HEX, TEMPLATE4_COST, 
    {
      {-2,-1}, {-2, 0},
      {-1,-1}, {-1, 0}, {-1, 1},
      { 0,-2}, { 0,-1}, { 0, 0}, { 0, 1},
      { 1,-1}, { 1, 0}, { 1, 1},
      { 2,-1}, { 2, 0}
    }
  },
  { TEMPLATE5_HEX, TEMPLATE5_COST, 
    {
      {-1,-2}, {-1,-1}, {-1, 0}, {-1, 1}, {-1, 2},
      { 0,-3}, { 0,-2}, { 0,-1}, { 0, 0}, { 0, 1}, { 0, 2},  
      { 1,-2}, { 1,-1}, { 1, 0}, { 1, 1}, { 1, 2}
    }
  },
  { TEMPLATE6_HEX, TEMPLATE6_COST, 
    {
      {-3, 0}, {-3, 1},
      {-2,-1}, {-2, 0}, {-2, 1},
      {-1, 0}, {-1, 1},
      { 0,-1}, { 0, 0}, { 0, 1},
      { 1, 0}, { 2, 1},
      { 2,-1}, { 2, 0}, { 2, 1},
      { 3, 0}, { 3, 1}
    }
  },
  { TEMPLATE7_HEX, TEMPLATE7_COST, 
    {
      {-2,-1}, {-2, 0}, {-2, 1},
      {-1,-1}, {-1, 0}, {-1, 1}, {-1, 2},
      { 0,-2}, { 0,-1}, { 0, 0}, { 0, 1}, { 0, 2},
      { 1,-1}, { 1, 0}, { 1, 1}, { 1, 2},
      { 2,-1}, { 2, 0}, { 2, 1}
    }
  },
  { TEMPLATE8_HEX, TEMPLATE8_COST, 
    {
      {-3,-1}, {-3, 0}, {-3, 1},
      {-2,-2}, {-2,-1}, {-2, 0}, {-2, 1},
      {-1,-1}, {-1, 0}, {-1, 1}, {-1, 2},
      { 0,-1}, { 0, 0}, { 0, 1}, { 0, 2},
      { 1, 0}, { 1, 1}, { 1, 2}, { 1, 3},
      { 2, 0}, { 2, 1}, { 2, 2}
    }
  },
};

class cgrid_t {
public:
  int size;
  int *pgrid;

  cgrid_t(int n) {
    pgrid = new int[n];
    size = (int)(sqrt (n - 1));
    if (size * size < n-1) size++;

    for (int i = 0; i < n; i++)
    {
      pgrid[i] = -1;
    }
  };
  ~cgrid_t() { delete [] pgrid; };
};

/* ----------------------------------------------------------------------
--
-- lay_template
--
---------------------------------------------------------------------- */
void world_t::lay_template (int p, int tmpl, hex_t *hexes, FILE *log_fptr, cgrid_t &cg)
{
  hex_t *hex;
  int grid_loc = -1,
      loop,
      x,
      y,
      hex_x,
      hex_y;
  feature_t *f;
  /* find free grid square */

  int grid_x = (*map).width / cg.size;
  int grid_y = (*map).height / cg.size;

  do
    {
      grid_loc = misc_t::uniform(num_players - 1);
      for (loop = 1; loop < num_players && grid_loc != -1; loop ++)
	if (cg.pgrid[loop] == grid_loc)
	  grid_loc = -1;
    }
  while (grid_loc == -1);
  cg.pgrid[p] = grid_loc;
  
  misc_t::dbg.log("Player %d has grid location %d.", p, grid_loc);

  /* lay tmpl down in grid square */

  x = ((grid_loc % cg.size) * grid_x) + grid_x / 2;
  y = ((grid_loc / cg.size) * grid_y) + grid_y / 2;
  
  if (x % 2) x ++; /* force to even location! */
  if (y % 2) y ++;

  /* now, start forcing hex types to tmpl type! */

  //  x --; /* reduce to 0 based map ref! */
  //  y --;

  fprintf (log_fptr, "Player %d tmpl placed at %s.\n", p, (*map)(x, y)->title);
  printf ("Player %d tmpl placed at %s.\n", p, (*map)(x, y)->title);

  for (loop = 0; loop < templates[tmpl].size; loop ++)
    {
      hex_x = x + templates[tmpl].offsets[loop].x;
      hex_y = y + templates[tmpl].offsets[loop].y;

      hex = (*map)(hex_x, hex_y);
      hex->clear_features();
      hex->owner = p;
      hex->terrain = hexes[loop].terrain;
      hex->tmpl = LZ_TRUE;
      while ((f = hexes[loop].pop_feature()))
	hex->add_feature(f);
      misc_t::dbg.log("Put hex %d into %s (owner %d).", loop, hex->title, hex->owner);
    }
}

/* ----------------------------------------------------------------------
--
-- getline
--
---------------------------------------------------------------------- */
char **world_t::getline(FILE *f)
{
  char buf[1024];
  char *s;
  unsigned int i, j;
  if ((s = fgets(buf, 1024, f)) == NULL)
    {
      return NULL;
    }

  for (i = 0; i < strlen(s); i++)
    s[i] = islower(s[i]) ? toupper(s[i]) : s[i];

  char *ctmp = buf;
  for (i = 0; (j = strcspn(ctmp, " \n")); i++)
    ctmp += j + strspn(ctmp + j, " \n");

  char **tokens = new char*[i+1];

  tokens[i] = 0;
  ctmp = buf;

  for (i = 0; (s = strtok(ctmp, " \n")); i++)
    {
      tokens[i] = strdup(s);
      ctmp = NULL;
    }
  return tokens;
}

/* ----------------------------------------------------------------------
--
-- build_hex
--
---------------------------------------------------------------------- */

void world_t::build_hex (int *points, hex_t *hex, char **tokens)
{
  const char terrain_codes [TERRAIN_TYPES][CODE_LENGTH+1] =
    { WATER_CODE, PLAIN_CODE, RUIN_CODE, FERTILE_CODE, SWAMP_CODE, PEAK_CODE,
      VOLCANO_CODE, WHIRLPOOL_CODE, SCRUB_CODE, TEMPLE_CODE, CURSED_CODE,
      RED_DEN_CODE, YELLOW_DEN_CODE, GREEN_DEN_CODE, GREY_DEN_CODE,
      BLACK_DEN_CODE };

  const int terrain_cost [TERRAIN_TYPES] =
    { WATER_COST, PLAIN_COST, RUIN_COST, FERTILE_COST, SWAMP_COST, PEAK_COST,
      VOLCANO_COST, WHIRLPOOL_COST, SCRUB_COST, TEMPLE_COST, CURSED_COST,
      RED_DEN_COST, YELLOW_DEN_COST, GREEN_DEN_COST, GREY_DEN_COST,
      BLACK_DEN_COST };

  const int den_pop_cost[] = {
    RED_POP_COST,
    GREEN_POP_COST,
    GREY_POP_COST,
    BLACK_POP_COST,
    YELLOW_POP_COST,
  };

  char terrain_code [CODE_LENGTH+1] = { "\0" };
  char lizard_colour [CODE_LENGTH+1] = { "\0" };
  
  int terrain = -1,
      lizards,
      loop;

  int max_groups = 0;
  den_t *den;
  band_t *band;

  /* find terrain type */

  strncpy (terrain_code, tokens[1], CODE_LENGTH);
  terrain_code[CODE_LENGTH] = '\0';
  /* find terrain type */

  for (loop = 0; loop < TERRAIN_TYPES; loop ++)
    if (strcmp (terrain_codes[loop], terrain_code) == 0)
      {
	terrain = loop;
	break;
      }

  //  printf("Found terrain %s %d\n", tokens[1], terrain);

  if ((terrain >= 0) && (terrain_cost [terrain] + *points <= PLAYER_SETUP_POINTS))
    {
      den = 0;
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
	  hex->add_feature(new ruin_t);
	  break;
	case 3:
	  hex->terrain = PLAIN;
	  hex->add_feature(new fertile_t);
	  break;
	case 4:
	  hex->terrain = PLAIN;
	  hex->add_feature(new swamp_t);
	  break;
	case 5:
	  hex->terrain = PLAIN;
	  hex->add_feature(new peak_t);
	  break;
	case 6:
	  hex->terrain = PLAIN;
	  hex->add_feature(new volcano_t);
	  break;
	case 7:
	  hex->terrain = WATER;
	  hex->add_feature(new whirlpool_t);
	  break;
	case 8:
	  hex->terrain = PLAIN;
	  hex->add_feature(new scrub_t);
	  break;
	case 9:
	  hex->terrain = PLAIN;
	  hex->add_feature(new temple_t);
	  break;
	case 10:
	  hex->terrain = PLAIN;
	  hex->add_feature(new cursed_t);
	  break;
	case 11:
	  den = new den_t(RED);
	  break;
	case 12:
	  den = new den_t(YELLOW);
	  break;
	case 13:
	  den = new den_t(GREEN);
	  break;
	case 14:
	  den = new den_t(GREY);
	  break;
	case 15:
	  den = new den_t(BLACK);
	  break;
	default:
	  break;
	}
      if (den)
	{
	  hex->terrain = PLAIN;
	  hex->add_feature(den);
	  int den_cost = den_pop_cost[den->colour];
	  den->pop = MINIMUM_POP_PER_DEN * GROUP_SIZE;
	  lizards = (atoi(&tokens[1][CODE_LENGTH - 1]) / GROUP_SIZE);
	  if (lizards < 0) lizards = 0;
	  if ((lizards * GROUP_SIZE) + den->pop > MAX_LIZARDS_IN_HEX)
	    lizards -= ((lizards * GROUP_SIZE) + den->pop - MAX_LIZARDS_IN_HEX) / GROUP_SIZE;
	  
	  /* see if they can afford the extra pop */
	  if (lizards > 0)
	    {
	      if (*points + (lizards * den_cost) <= PLAYER_SETUP_POINTS)
		{
		  den->pop += lizards * GROUP_SIZE;
		  *points += (lizards * den_cost);
		}
	      else
		{
		  /* find max lizard pop they can purchase with remaining points */
		  
		  max_groups = (PLAYER_SETUP_POINTS - *points) / den_cost;
		  if (max_groups > 0 && max_groups <= lizards)
		    {
		      den->pop += max_groups * GROUP_SIZE;
		      *points += (max_groups * den_cost);
		    }
		}
	    }
	}
    }

  /* --------------------

  find any lizard types in hex and place them down if possible

  -------------------- */

  int token = 2;
  band = new band_t;
  while (tokens[token])
    {
      /* find colour code */
      strncpy (lizard_colour, tokens[token], CODE_LENGTH);
      lizard_colour[CODE_LENGTH] = '\0';
      for (loop = 0; loop < LIZARD_TYPES; loop ++)
	if (strcmp (lizard_colour, warrior_map[loop].text) == 0)
	  break;
      if (loop >= 0 && loop < LIZARD_TYPES)
	{
	  warrior_map_t &wmap = warrior_map[loop];

	  lizards = atoi(&tokens[token][CODE_LENGTH]);
	  if ((lizards + band->size()) > MAX_LIZARDS_IN_HEX)
	    lizards -= (lizards + hex->size() - MAX_LIZARDS_IN_HEX);

	  lizards -= lizards % GROUP_SIZE;
	  if (lizards > 0)
	    {
	      if (*points + ((lizards / GROUP_SIZE) * wmap.cost) <= PLAYER_SETUP_POINTS)
		{
		  band->colour[warrior_map[loop].id] += lizards;
		  *points += ((lizards / GROUP_SIZE) * wmap.cost);
		}
	      else
		{
		  max_groups = (PLAYER_SETUP_POINTS - *points) / wmap.cost;
		  band->colour[wmap.id] += max_groups * GROUP_SIZE;
		  *points += (max_groups * wmap.cost);
		}
	    }
	}
      token++;
    }
  if (band->size())
    hex->add_feature(band);
  else
    delete band;
}

/* ----------------------------------------------------------------------
--
-- BuildStartup
--
---------------------------------------------------------------------- */

void world_t::build_startup(int p, FILE *template_fptr, FILE *log_fptr, cgrid_t& cg)
{
  FILE *default_fptr;
  char filename[1024];
  char **tokens;
  
  int home_hex;

  int tmpl,
    player_points = 0,
    present_hex = 0,
    loop,
    loop_two,
    fertiles;

  hex_t *hex_list;
  feature_t *f;
  den_t *den;

  /* find tmpl */

  if ((tokens = getline(template_fptr)) == NULL)
    {
      fprintf (log_fptr, "FATAL ERROR: Player %d unspecified TMPL.\n", p);
      printf ("FATAL ERROR: Player %d unspecified TMPL. \n", p);
      exit (EXIT_FAILURE);
    }

  if (strcmp(tokens[0], TEMPLATE_CODE) != 0)
    {
      fprintf (log_fptr, "FATAL ERROR: Player %d unspecified TMPL.\n", p);
      printf ("FATAL ERROR: Player %d unspecified TMPL. \n", p);
      exit (EXIT_FAILURE);
    }

  tmpl = atoi(tokens[1]);

  if (tmpl < 1 || tmpl > TEMPLATES)
    {
      fprintf (log_fptr, "FATAL ERROR: Player %d specified invalid TMPL.\n", p);

      printf ("FATAL ERROR: Player %d specified invalid TMPL. \n", p);
      exit (EXIT_FAILURE);
    }

  misc_t::dbg.log("Got template %d for player %d", tmpl, p);
  /* build tmpl list */

  if ((hex_list = new hex_t[templates[tmpl].size]) == NULL)
    {
      printf ("FATAL ERROR: Out of memory for malloc. \n");
      exit (EXIT_FAILURE);
    }

  for (int k = 0; tokens[k]; k++)
    free(tokens[k]);
  free(tokens);

  /* cost tmpl list */

  player_points += templates[tmpl].cost;

  /* find home_hex */

  if ((tokens = getline(template_fptr)) == NULL)
    {
      fprintf (log_fptr, "FATAL ERROR: Player %d unspecified HOME HEX.\n", p);
      printf ("FATAL ERROR: Player %d unspecified HOME HEX. \n", p);
      exit (EXIT_FAILURE);
    }

  if (strcmp (tokens[0], HOME_HEX_CODE) != 0)
    {
      fprintf (log_fptr, "FATAL ERROR: Player %d unspecified HOME HEX.\n", p);
      printf ("FATAL ERROR: Player %d unspecified HOME HEX. \n", p);
      exit (EXIT_FAILURE);
    }
  home_hex = atoi(tokens[1]);
  if (home_hex == 0 || home_hex > templates[tmpl].size)
    {
      fprintf (log_fptr, "FATAL ERROR: Player %d specified invalid HOME HEX.\n", p);
      printf ("FATAL ERROR: Player %d specified invalid HOME HEX. \n", p);
      exit (EXIT_FAILURE);
    }
  home_hex --; /* adjust it down for 0 based hex list */

  for (int k = 0; tokens[k]; k++)
    free(tokens[k]);
  free(tokens);

  /* build hex list, adding up hexes to 1200 points */
  /* read tmpl until eof or first line that begins with NON-DIGIT char */

  while ((tokens = getline(template_fptr)) != NULL)
    {
      if (!isdigit(tokens[0][0])) /* non-digit char starts line! */
	break;
      /* find hex list element */
      present_hex = atoi(tokens[0]);
      if (present_hex > 0 && present_hex <= templates[tmpl].size)
	{
	  present_hex --; /* adjust down to 0 based list */
	  /* build hex, keeping track of points */
	  build_hex(&player_points, &hex_list[present_hex], tokens);
	}
      for (int j = 0; tokens[j]; j++)
	free(tokens[j]);
      free(tokens);
    }

  misc_t::dbg.log("Finished reading in hexes.");

  /* force volcanoes in invalid positions to peaks */

  /* note in list, any hexes whose x and y offsets are different by 1 or 0
     are ADJACENT! */

  for (loop = 0; loop < templates[tmpl].size; loop ++)
    if ((f = hex_list[loop].has_feature(VOLCANO)))
      for (loop_two = 0; loop_two < templates[tmpl].size; loop_two ++)
	if (abs (templates[tmpl].offsets[loop].x -
		 templates[tmpl].offsets[loop_two].x) <= 1 &&
	    abs (templates[tmpl].offsets[loop].y -
		 templates[tmpl].offsets[loop_two].y) <= 1 &&
	    loop != loop_two)
	  {
	    /* they're adjacent, so check if volcano hex could scrag either den or fertile */
	    if (hex_list[loop_two].has_feature(DEN) ||
		hex_list[loop_two].has_feature(FERTILE))
	      {
		/* convert volcano to peak as it's potentially destructive */
		delete hex_list[loop].del_feature(f);
		hex_list[loop].add_feature(new peak_t);
		break;
	      }
	  }
  
  /* check home den position correct */

  /* first, count the fertile hexes adjacent to the selected home hex */

  for (loop = 0, fertiles = 0; loop < templates[tmpl].size; loop ++)
    if (abs (templates[tmpl].offsets[home_hex].x -
	     templates[tmpl].offsets[loop].x) <= 1 &&
	abs (templates[tmpl].offsets[home_hex].y -
	     templates[tmpl].offsets[loop].y) <= 1 &&
	loop != home_hex && hex_list[loop].has_feature(FERTILE))
      fertiles ++; /* adjacent fertile hex */

  misc_t::dbg.log("Counted %d fertile hexes.", fertiles);

  /* check their den choice is valid */

  if ((den = (den_t*)hex_list[home_hex].has_feature(DEN)) && fertiles > 0)
    {
      /* valid home den! */
      den->home = 1;
      fprintf (log_fptr, "Player %d successfully built tmpl with %d points.\n", p, player_points);
      printf ("Player %d successfully built tmpl with %d points.\n", p, player_points);
      misc_t::dbg.log("calling lay_template()");

      lay_template (p, tmpl, hex_list, log_fptr, cg);
      misc_t::dbg.log("lay_template player = %d completed", p);
      get_player(p)->home_dens = 1;
      delete [] hex_list;
      return;
    }
  else
    {
      /* invalid home den choice, see if we can pick another den */
      home_hex = -1; /* invalidate home den hex */

      /* go through the hexes, looking for dens with adjacent fertiles! */

      for (loop = 0, fertiles = 0; loop < templates[tmpl].size; loop ++)
	if ((den = (den_t*)hex_list[loop].has_feature(DEN)))
	  {
	    /* found a den, see if it has 1 or more fertile hexes adjacent */

	    for (loop_two = 0, fertiles = 0; loop_two < templates[tmpl].size; loop_two ++)
	      if (abs (templates[tmpl].offsets[loop].x -
		       templates[tmpl].offsets[loop_two].y) <= 1 &&
		  abs (templates[tmpl].offsets[loop].x -
		       templates[tmpl].offsets[loop_two].y) <= 1 &&
		  loop_two != loop && hex_list[loop_two].has_feature(FERTILE))
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
	  den->home = 1;
	  fprintf (log_fptr, "Player %d selected invalid Home Den, but successfully relocated to hex %d.\n",
		   p, home_hex + 1);
	  printf ("Player %d selected invalid Home Den, but successfully relocated to hex %d.\n",
		  p, home_hex + 1);
	  fprintf (log_fptr,
		   "Player %d successfully built tmpl with %d points.\n",
		   p, player_points);
	  printf ("Player %d successfully built tmpl with %d points.\n",
		  p, player_points);
	  lay_template (p, tmpl, hex_list, log_fptr, cg);
	  delete [] hex_list;
	  return;
	}
      else
	{
	  /* didn't find a valid replacement home den, give player default start */
	  
	  fprintf (log_fptr, "Player %d selected invalid Home Den, NO VALID SUBSTITUTE FOUND!.\n", p);
	  printf ("Player %d selected invalid Home Den, NO VALID SUBSTITUTE FOUND!.\n", p);
	  fprintf (log_fptr, "Player %d defaulted to startup %d.\n", p, DEFAULT_STARTUP);
	  printf ("Player %d defaulted to startup %d.\n", p, DEFAULT_STARTUP);
	  sprintf (filename, "CUST%d.MAP", DEFAULT_STARTUP);

	  if ((default_fptr = fopen (filename, "rt")) == NULL)
	    {
	      printf ("FATAL ERROR: Unable to read %s file.\n", filename);
	      exit (EXIT_FAILURE);
	    }
	  build_startup (p, default_fptr, log_fptr, cg);
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
void world_t::custom_startup()
{
  FILE *template_fptr,
       *log_fptr;

  char *filename,
    *text_string,
    *tok_string;

  int p, startup;

  for (int i = 1; i < num_players; i++)
    {
      player_t *p = get_player(i);
      p->home_dens = 0;
    }

  cgrid_t cg(num_players);

  filename = (char*)alloca(256);
  sprintf (filename, "%s.%s", game, CUSTSTAR_LOG_FILE);
  if ((log_fptr = fopen (filename, "w")) == NULL)
    {
      printf ("FATAL ERROR: Unable to write to %s file. \n", filename);
      exit (EXIT_FAILURE);
    }

  fprintf (log_fptr, "\nCustom Startup Log File for LIZARDS! Game %s.\n\n", game);

  for (p = 1; p < num_players; p++)
    {
      text_string = strdup(player[p]->startup);
      misc_t::dbg.log("Got player startup %s.", text_string);
      tok_string = strtok(text_string, " ");
      if (get_player(p)->home_dens != 0)
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
	      build_startup (p, template_fptr, log_fptr, cg);
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
	      build_startup(p, template_fptr, log_fptr, cg);
	      fclose(template_fptr);
	      break;
	    default:
	      if (startup > VALID_STARTUP)
		{
		  fprintf (log_fptr, "FATAL ERROR: Player %d selected invalid STARTUP.\n", p);
		  printf ("FATAL ERROR: Player %d selected invalid STARTUP. \n", p);
		  exit (EXIT_FAILURE);
		}
	    }
	}
      else
	{
	  fprintf (log_fptr, "FATAL ERROR: Player %d selected invalid STARTUP.\n", p);
	  printf ("FATAL ERROR: Player %d unspecified STARTUP. \n", p);
	  exit (EXIT_FAILURE);
	}
      misc_t::dbg.log("Finished player %d", p);
      free(text_string);
    }
  fprintf (log_fptr, "\nEND OF CUSTOM STARTUP LOG FILE\n");
  fclose (log_fptr);
}

/* ----------------------------------------------------------------------
--
-- lay_island
--
---------------------------------------------------------------------- */

int world_t::lay_island(int x, int y, double land_density)
{
  int work4;
  int i, start, dir;
  int number_land_hexes = 0;
  hex_t *a_hex, *c_hex = (*map)(x, y);

  if (misc_t::uniform(100) < (100 * land_density))
    {
      c_hex->terrain = PLAIN;
      number_land_hexes++;
    }
  for (int dir = 0; dir < DIRECTIONS; dir++)
    {
      a_hex = c_hex->adjacent(dir);
      if (c_hex->xy != a_hex->xy)
	{
	  number_land_hexes++;
	  a_hex->terrain = PLAIN;
	}
    }
  if ((x <= ((*map).width / 5) || x >= ((*map).width - ((*map).width / 5))) &&
      (y <= ((*map).height / 5) || y >= ((*map).height - ((*map).height / 5))))
    work4 = (int)(140 * land_density);
  else
    work4 = (int)(60 * land_density);

  if (misc_t::uniform(100) < work4)
    {
      start = misc_t::uniform(6);
      if (misc_t::uniform(100) < (100 * land_density))
	dir = 1;
      else
	dir = -1;
      for (i = 0; i < 6 && misc_t::uniform(100) < 35; i++)
	{
	  start = (start + dir + 6) % 6;
	  a_hex = c_hex->adjacent(start);
	  if (c_hex->xy != a_hex->xy)
	    {
	      if (a_hex->terrain == PLAIN)
		number_land_hexes--;
	      a_hex->terrain = WATER;
	    }
	}
    }
  return number_land_hexes;
}

/* ----------------------------------------------------------------------
--
-- WorldFill
--
---------------------------------------------------------------------- */

void world_t::fill(char *filename, int optind, int argc, char *argv[])
{
  FILE *fptr;

  int x,
      y,
      work,
      work2,
      work3,
      work4,
      den_n,
      red_den_n,
      green_den_n,
      grey_den_n,
      black_den_n,
      yellow_den_n,
      ruin_n,
      swamp_n,
      fertile_n,
      peak_n,
      volcano_n,
      whirlpool_n,
      scrub_n,
      temple_n,
      cursed_n,
      plain_n;

  int den_a,
    red_den_a,
    green_den_a,
    grey_den_a,
    black_den_a,
    yellow_den_a,
    ruin_a,
    swamp_a,
    fertile_a,
    peak_a,
    volcano_a,
    whirlpool_a,
    scrub_a,
    temple_a,
    cursed_a,
    plain_a,
    number_land_hexes,
    islands_on_f = 1;
  
  double den_p = 0.18,
    red_den_p = 0.20,
    green_den_p = 0.20,
    grey_den_p = 0.20,
    black_den_p = 0.20,
    yellow_den_p = 0.20,
    ruin_p = 0.02,
    swamp_p = 0.18,
    fertile_p = 0.04,
    peak_p = 0.015,
    volcano_p = 0.015,
    whirlpool_p = 0.01,
    scrub_p = 0.15,
    temple_p = 0.02,
    cursed_p = 0.0,
    plain_p = 0.55,
    land_density = 0.50;

  band_t *band;
  den_t *den;
  hex_t *hex, *adj_hex;

  if (filename && ((fptr = fopen (filename, "r")) != NULL))
    {
      printf ("COMMAND LINE SWITCHES IGNORED.\nPARAMETER FILE LOADED.\n");
      fscanf (fptr, "%lf", &den_p);
      fscanf (fptr, "%lf", &red_den_p);
      fscanf (fptr, "%lf", &green_den_p);
      fscanf (fptr, "%lf", &grey_den_p);
      fscanf (fptr, "%lf", &black_den_p);
      fscanf (fptr, "%lf", &yellow_den_p);
      fscanf (fptr, "%lf", &ruin_p);
      fscanf (fptr, "%lf", &swamp_p);
      fscanf (fptr, "%lf", &fertile_p);
      fscanf (fptr, "%lf", &peak_p);
      fscanf (fptr, "%lf", &volcano_p);
      fscanf (fptr, "%lf", &whirlpool_p);
      fscanf (fptr, "%lf", &scrub_p);
      fscanf (fptr, "%lf", &temple_p);
      fscanf (fptr, "%lf", &cursed_p);
      fscanf (fptr, "%lf", &plain_p);
      fscanf (fptr, "%d", &islands_on_f);      
      fclose (fptr);
      unlink(filename);
    }
  else
    {
      for (work = optind; work < argc; work++)
	{
	  switch (toupper (argv [work][0]))
	    {
	    case 'D':
	      den_p = atof (&argv [work][1]);
	      printf ("DEN.........%s percent.\n", &argv [work - 1][1]);
	      break;
	    case '1':
	      red_den_p = atof (&argv [work][1]);
	      printf ("RED DEN.....%s percent.\n", &argv [work - 1][1]);
	      break;
	    case '2':
	      green_den_p = atof (&argv [work][1]);
	      printf ("GREEN DEN...%s percent.\n", &argv [work][1]);
	      break;
	    case '3':
	      grey_den_p = atof (&argv [work][1]);
	      printf ("GREY DEN....%s percent.\n", &argv [work][1]);
	      break;
	    case '4':
	      black_den_p = atof (&argv [work][1]);
	      printf ("BLACK DEN...%s percent.\n", &argv [work][1]);
	      break;
	    case '5':
	      yellow_den_p = atof (&argv [work][1]);
	      printf ("YELLOW DEN..%s percent.\n", &argv [work][1]);
	      break;
	    case 'R':
	      ruin_p = atof (&argv [work][1]);
	      printf ("RUIN........%s percent.\n", &argv [work][1]);
	      break;
	    case 'S':
	      swamp_p = atof (&argv [work][1]);
	      printf ("SWAMP.......%s percent.\n", &argv [work][1]);
	      break;
	    case 'F':
	      fertile_p = atof (&argv [work][1]);
	      printf ("FERTILE.......%s percent.\n", &argv [work][1]);
	      break;
	    case 'P':
	      plain_p = atof (&argv [work][1]);
	      printf ("PLAIN.......%s percent.\n", &argv [work][1]);
	      break;
	    case 'K':
	      peak_p = atof (&argv [work][1]);
	      printf ("PEAK........%s percent.\n", &argv [work][1]);
	      break;
	    case 'V':
	      volcano_p = atof (&argv [work][1]);
	      printf ("VOLCANO.....%s percent.\n", &argv [work][1]);
	      break;
	    case 'W':
	      whirlpool_p = atof (&argv [work][1]);
	      printf ("WHIRLPOOL...%s percent.\n", &argv [work][1]);
	      break;
	    case 'C':
	      scrub_p = atof (&argv [work][1]);
	      printf ("SCRUB.......%s percent.\n", &argv [work][1]);
	      break;	      
	    case 'T':
	      temple_p = atof (&argv [work][1]);
	      printf ("TEMPLE......%s percent.\n", &argv [work][1]);
	      break;
	    case 'U':
	      cursed_p = atof (&argv [work][1]);
	      printf ("CURSED......%s percent.\n", &argv [work][1]);
	      break;
	    case 'L':
	      land_density = atof(&argv[work][1]);
	      break;
	    case 'I':
	      if (toupper (argv [work][2]) == 'F')
		{
		  islands_on_f = 0;
		  printf ("ISLAND GENERATION OFF.\n");
		}
	      else
		islands_on_f = 1;
	      break;
	      
	    case '?':
	      printf ("Command Line Options: D%% = Den, 1%% = Red Den, 2%% = Green Den, 3%% = Grey Den,\n"
		      "                      4%% = Black Den, 5%% = Yellow Den, R%% = Ruin, S%% = Swamp,\n"
		      "                      F%% = Fertile, P%% = Plain, K%% = Peak, V%% = Volcano,\n"
		      "                      W%% = Whirlpool, C%% = Scrub, T%% = Temple, U%% = Cursed,\n"
		      "                      ION = Island Generation ON, IOF = Island Generation OFF.\n\n"
		      "                      eg. WORLFILL A R0.05 C0.20 V0.2\n\nAborted.\n");
	      
	      exit (EXIT_FAILURE);
	    default:
	      fprintf (stderr, "FATAL ERROR: Unknown switch in command line.\n");
	      exit (EXIT_FAILURE);
	    }
	}
    }
  printf ("Percents:\n");

  printf ("  %0.3f Dens (%0.3f Red, %0.3f Green, %0.3f Grey, %0.3f Black, %0.3f Yellow).\n",
    den_p, red_den_p, green_den_p, grey_den_p, black_den_p, yellow_den_p);

  printf ("  %0.3f Plain, %0.3f Ruin, %0.3f Swamp, %0.3f Fertile, %0.3f Peak,\n",
    plain_p, ruin_p, swamp_p, fertile_p, peak_p);

  printf ("  %0.3f Volcano, %0.3f Whirlpool, %0.3f Scrub, %0.3f Temple, %0.3f Cursed.\n",
    volcano_p, whirlpool_p, scrub_p, temple_p, cursed_p);

  number_land_hexes = 0;

  printf("Creating world -- land density %4.2f%%\n", land_density);

  if (islands_on_f)
    {
      printf ("Raising Islands I...\n");
      for (y = 1; y < map->height; y += 4)
	for (x = 1; x < map->width; x += 4)
	  lay_island (x, y, land_density);

      number_land_hexes = 0;
      for (y = 0; y < map->height; y ++)
	for (x = 0; x < map->width; x ++)
	  {
	    hex = (*map)(x, y);
	    if (hex->terrain == PLAIN)
	      number_land_hexes++;
	  }
      work = (int)(map->width * map->height * land_density - number_land_hexes);
      work2 = (int)((double)(work) / (map->width * map->height - number_land_hexes) * 100);

      printf ("Raising Islands II...\n");
      for (y = 0; y < map->height; y += 1)
	for (x = 0; x < map->width; x += 1)
	  {
	    hex = (*map)(x, y);
	    if (work > 0)
	      {
		if (hex->terrain == WATER && (misc_t::uniform(100) < work2))
		  hex->terrain = PLAIN;
	      }
	    else
	      {
		if (hex->terrain == PLAIN && (misc_t::uniform(100) < -work2))
		  hex->terrain = WATER;
	      }
	  }
    }
  else
    {
      printf ("Island Generation OFF - World is just water!\n");
    }
  
  printf("Counting Plains hexes on map...\n");
  number_land_hexes = 0;
  for (y = 0; y < map->height; y ++)
    for (x = 0; x < map->width; x ++)
      if ((*map)(x, y)->terrain == PLAIN)
	number_land_hexes++;

  den_n = (int)((double) number_land_hexes * den_p);
  red_den_n = (int)((double) den_n * red_den_p);
  green_den_n = (int)((double) den_n * green_den_p);
  grey_den_n = (int)((double) den_n * grey_den_p);
  black_den_n = (int)((double) den_n * black_den_p);
  yellow_den_n = (int)((double) den_n * yellow_den_p);
  ruin_n = (int)((double) number_land_hexes * ruin_p);
  swamp_n = (int)((double) number_land_hexes * swamp_p);
  fertile_n = (int)((double) number_land_hexes * fertile_p);
  peak_n = (int)((double) number_land_hexes * peak_p);
  volcano_n = (int)((double) number_land_hexes * volcano_p);
  whirlpool_n = (int)((double) number_land_hexes * whirlpool_p);
  scrub_n = (int)((double) number_land_hexes * scrub_p);
  temple_n = (int)((double) number_land_hexes * temple_p);
  plain_n = (int)((double) number_land_hexes * plain_p);
  cursed_n = (int)((double) number_land_hexes * cursed_p);

  /* top up den colours if below required */

  while (red_den_n + green_den_n + grey_den_n + black_den_n + yellow_den_n <
	 den_n)
    {
      switch (misc_t::uniform (LIZARD_TYPES))
	{
	case RED:
	  if (red_den_p != 0.0)
	    red_den_n ++;
	  break;
	case GREEN:
	  if (green_den_p != 0.0)
	    green_den_n ++;
	  break;
	case GREY:
	  if (grey_den_p != 0.0)
	    grey_den_n ++;
	  break;
	case BLACK:
	  if (black_den_p != 0.0)
	    black_den_n ++;
	  break;
	case YELLOW:
	  if (yellow_den_p != 0.0)
	    yellow_den_n ++;
	  break;
	}
    }
  den_a = 0;
  red_den_a = 0;
  green_den_a = 0;
  grey_den_a = 0;
  black_den_a = 0;
  yellow_den_a = 0;
  ruin_a = 0;
  swamp_a = 0;
  fertile_a = 0;
  peak_a = 0;
  volcano_a = 0;
  whirlpool_a = 0;
  scrub_a = 0;
  temple_a = 0;
  plain_a = 0;
  cursed_a = 0;

  printf ("World has %d land hexes and %d water hexes.\n", number_land_hexes,
	  (map->width * map->height) - number_land_hexes);
  printf ("World needs %d den (%d red, %d green, %d grey, %d black, %d yellow),\n"
	  "%d ruin, %d swamp, %d fertile, %d peak, %d volcano, %d whirlpool,\n"
	  "%d scrub, %d temple, %d plain, %d cursed hexes.\n", den_n, red_den_n,
	  green_den_n, grey_den_n, black_den_n, yellow_den_n, ruin_n, swamp_n,
	  fertile_n, peak_n, volcano_n, whirlpool_n, scrub_n, temple_n,
	  plain_n, cursed_n);

  printf ("Placing Dens...");
  work = 0;
  while (work < den_n)
    {
      do hex = (*map)(misc_t::uniform(map->width), misc_t::uniform(map->height));
      while (hex->terrain == WATER);
      if (!(hex->terrain == PLAIN &&
	    !hex->has_feature(DEN) &&
	    hex->terrain_adjacent(PLAIN) > 0 &&
	    hex->feature_adjacent(FERTILE) == 0 &&
	    hex->feature_adjacent(DEN) == 0))
	  continue;
      work3 = 1;
      do
	{
	  switch ((work2 = misc_t::uniform (LIZARD_TYPES)))
	    {
	    case RED:
	      if (red_den_a < red_den_n)
		{
		  work3 = 0;
		  red_den_a ++;
		}
	      break;
	    case GREEN:
	      if (green_den_a < green_den_n)
		{
		  work3 = 0;
		  green_den_a ++;
		}
	      break;
	    case GREY:
	      if (grey_den_a < grey_den_n)
		{
		  grey_den_a ++;
		  work3 = 0;
		}
	      break;
	    case BLACK:
	      if (black_den_a < black_den_n)
		{
		  black_den_a ++;
		  work3 = 0;
		}
	      break;
	    case YELLOW:
	      if (yellow_den_a < yellow_den_n)
		{
		  yellow_den_a ++;
		  work3 = 0;
		}
	      break;
	    }
	  if (work3 == 0)
	    {
	      den = new den_t(work2);
	      den_a ++;
	      hex->add_feature(den);
	    }
	}
      while (work3 != 0);

      work4 = 0;
      do
	{
	  work2 = 0;
	  for (work3 = 0; work3 < DIRECTIONS; work3 ++)
	    {
	      if ((adj_hex = hex->adjacent(work3)))
		if (misc_t::uniform (100) < 50 && work2 < 4 && adj_hex->terrain == PLAIN)
		  {
		    adj_hex->add_feature(new fertile_t());
		    work2++;
		  }
	    }
	  work4 ++;
	}
      while (work2 == 0 && work4 < 20);

      if (work4 != 20)
	{
	  den->pop = DEN_LIZARD_LEVEL * work2;
	  work3 = (int)((double)(den->pop) * DEN_MILITIA_LEVEL);
	  band = new band_t();
	  band->colour[den->colour] = work3;
	  hex->add_feature(band);
	}
      work++;
    }
  printf ("\n");
  printf ("Creating Temples...");
  for (work = 0; work < temple_n; work ++)
    {
      work2 = 0;
      do
	{
	  do hex = (*map)(misc_t::uniform(map->width), misc_t::uniform(map->height));
	  while (hex->terrain == WATER || hex->num_features());
	  work2 ++;
	} while (work2 != 10 && hex->feature_adjacent(DEN) != 0);
      if (work2 != 10)
	{
	  temple_a++;
	  hex->add_feature(new temple_t());
	}
    }
  printf (" \n");

  printf ("Weathering Ruins..");
  for (work = 0; work < ruin_n; work ++)
    {
      work2 = 0;
      do
	{
	  do hex = (*map)(misc_t::uniform(map->width), misc_t::uniform(map->height));
	  while (hex->terrain == WATER || hex->num_features());
	  if (hex->feature_adjacent(DEN) == 0 &&
	      hex->feature_adjacent(TEMPLE) == 0)
	    {
	      ruin_a ++;
	      hex->add_feature(new ruin_t());
	      break;
	    }
	  work2 ++;
	}
      while (work2 < 100);
    }
  printf (" \n");
  printf ("Cursing Cursed...");
  for (work = 0; work < cursed_n; work ++)
    {
      do hex = (*map)(misc_t::uniform(map->width), misc_t::uniform(map->height));
      while (hex->terrain == WATER || hex->num_features());
      cursed_a ++;
      hex->add_feature(new cursed_t());
    }
  printf (" \n");

  printf ("Sinking Swamps...");
  for (work = 0; work < swamp_n; work ++)
    {
      do hex = (*map)(misc_t::uniform(map->width), misc_t::uniform(map->height));
      while (hex->terrain == WATER || hex->num_features());
      swamp_a ++;
      hex->add_feature(new swamp_t);
    }
  printf (" \n");

  printf ("Growing Fertile...");
  for (work = 0; work < fertile_n; work ++)
    {
      do hex = (*map)(misc_t::uniform(map->width), misc_t::uniform(map->height));
      while (hex->terrain == WATER || hex->num_features() || hex->feature_adjacent(DEN));
      fertile_a ++;
      hex->add_feature(new fertile_t());
    }
  printf (" \n");

  printf ("Raising Peaks...");
  for (work = 0; work < peak_n; work ++)
    {
      do hex = (*map)(misc_t::uniform(map->width), misc_t::uniform(map->height));
      while ((hex->terrain == WATER) || hex->num_features());
      peak_a ++;
      hex->add_feature(new peak_t());
    }
  printf (" \n");

  printf ("Raising Volcanos...");
  for (work = 0; work < volcano_n; work ++)
    {
      do hex = (*map)(misc_t::uniform(map->width), misc_t::uniform(map->height));
      while (hex->terrain == WATER || hex->num_features() ||
	     hex->feature_adjacent(DEN) ||
	     hex->feature_adjacent(TEMPLE) ||
	     hex->feature_adjacent(FERTILE));
      volcano_a ++;
      hex->add_feature(new volcano_t());
    }
  printf (" \n");

  printf ("Sinking Whirlpools...");
  for (work = 0; work < whirlpool_n; work ++)
    {
      do hex = (*map)(misc_t::uniform(map->width), misc_t::uniform(map->height));
      while (hex->terrain != WATER || hex->num_features() ||
	     hex->feature_adjacent(WHIRLPOOL));
      whirlpool_a ++;
      hex->add_feature(new whirlpool_t());
    }
  printf ("\n");

  printf ("Planting Scrub...");
  for (work = 0; work < scrub_n; work ++)
    {
      do hex = (*map)(misc_t::uniform(map->width), misc_t::uniform(map->height));
      while (hex->terrain != PLAIN || hex->num_features());
      scrub_a ++;
      hex->add_feature(new scrub_t);
    }
  printf ("\n");
  
  for (work = 0; work < map->size; work++)
    if ((*map)(work)->terrain == PLAIN)
      plain_a++;

  printf ("World gets %d den (%d red, %d green, %d grey, %d black, %d yellow),\n"
	  "%d ruin, %d swamp, %d fertile, %d peak, %d volcano, %d whirlpool,\n"
	  "%d scrub, %d temple, %d plain, %d cursed hexes.\n", den_a, red_den_a,
	  green_den_a,grey_den_a, black_den_a, yellow_den_a, ruin_a, swamp_a,
	  fertile_a, peak_a, volcano_a, whirlpool_a, scrub_a, temple_a,
	  plain_a, cursed_a);

}

// ----------------------------------------------------------------------
//
// WorldPopulate
//
// ----------------------------------------------------------------------
void world_t::populate(int home_den_colour, int start_lizard_colour)
{
  int work, x, y;

  struct grid_t {
    int owner;
    int dens;
    point_t pt, den;
  } *grid;

  hex_t *hex, *adj;
  den_t *den;
  band_t *band;

  int np = num_players - 1;
  int npsqrt = (int)ceil(sqrt(np));
  int size_x = (*map).width / npsqrt;
  int size_y = (*map).height / npsqrt;

  while (((*map).width/size_x)*((*map).height/size_y) > np)
    {
      if (misc_t::uniform(2) == 0)
	if (((*map).width/(size_x+1))*((*map).height/size_y) > np)
	  size_x++;
	else
	  break;
      else
	if (((*map).width/size_x)*((*map).height/(size_y+1)) > np)
	  size_y++;
	else
	  break;
    }

  int grid_x = (*map).width / size_x;
  int grid_y = (*map).height / size_y;

  grid = new grid_t[grid_x * grid_y];

  for (int j = 0; j < grid_y; j++)
    for (int i = 0; i < grid_x; i++)
      {
	grid[j * grid_x + i].dens = 0;
	grid[j * grid_x + i].owner = 0;
	grid[j * grid_x + i].pt = point_t(i, j);
	for (int y = 0; y < size_y; y++)
	  for (int x = 0; x < size_x; x++)
	    {
	      hex = (*map)(i * size_x + x, j * size_y + y);
	      if (hex->has_feature(DEN))
		grid[j * grid_x + i].dens += 1;
	    }
      }
  for (work = 1; work < num_players; work++)
    {
      printf ("Player %d: ", work);

      int i = misc_t::uniform(grid_x * grid_y);
      int loop = 0;
      int j = i;
      while (!((j == i) && loop))
	{
	  if (grid[j].dens && !grid[j].owner)
	    break;
	  j++;
	  if (j == (grid_x * grid_y))
	    {
	      j = 0;
	      loop = 1;
	    }
	}
      if ((j == i) && loop)
	{
	  printf ("FATAL ERROR: No free locations for new player.\n");
	  exit (EXIT_FAILURE);
	}
      grid[j].owner = work;
      int k = misc_t::uniform(grid[j].dens) + 1;
      i = 0;
      for (y = 0; y < size_y && i != k; y++)
	for (x = 0; x < size_x && i != k; x++)
	  {
	    hex = (*map)(grid[j].pt.x * size_x + x, grid[j].pt.y * size_y + y);
	    if (hex->has_feature(DEN))
	      i++;
	  }
      grid[j].den = hex->xy;
      den = (den_t*)hex->has_feature(DEN);
      band = (band_t*)hex->has_feature(BAND);
      hex->owner = work;
      den->owner = work;
      band->owner = work;
      den->home = 1;
      hex->tmpl = 1;
      band->colour[den->colour] = 0;
      den->colour = home_den_colour;
      band->colour[den->colour] = HOME_SIZE;
      misc_t::dbg.log("den at %s has pop %d, band %d and fertile %d",
		hex->title, den->pop, band->size(), hex->feature_adjacent(FERTILE));
      if ((k = hex->feature_adjacent(FERTILE)) > 1)
	{
	  for (i = 0; i < 6 && k > 1; i++)
	    {
	      if ((adj = hex->adjacent(i)) != NULL)
		{
		  if (adj->has_feature(FERTILE))
		    {
		      delete adj->del_feature(adj->has_feature(FERTILE));
		      k--;
		    }
		}
	    }
	  den->pop = 50;
	}
      printf ("Home Den placed at %s.\n", hex->title);
      misc_t::dbg.log("Den at %s has pop %d", hex->title, den->pop);
    }
  delete grid;
}

/* ----------------------------------------------------------------------
--
-- world_fix
--
---------------------------------------------------------------------- */

void world_t::fix()
{
  int fertile,
      dir,
      loop,
      x,
      y;

  hex_t *hex,
        *adj_hex;
  feature_t *volcano;
  den_t *den;
  band_t *band;

  printf ("Fixing World...\n");
  for (y = 0; y < (*map).height; y++)
    for (x = 0; x < (*map).width; x++)
      {
	hex = (*map)(x, y);

	if ((den = (den_t*)hex->has_feature(DEN)) && !hex->tmpl)
	  {
	    band = (band_t*)hex->has_feature(BAND);
	    if ((fertile = hex->feature_adjacent (FERTILE)) == 0)
	      {
		dir = misc_t::uniform(6);
		for (loop = 0; loop < 6; loop++)
		  {
		    if ((adj_hex = hex->adjacent(dir)) != NULL)
		      {
			/* get and check hex not part of player's template */
			if (!adj_hex->tmpl && !adj_hex->has_feature(DEN))
			  {
			    /* convert to fertile and stop */
			    adj_hex->terrain = PLAIN;
			    adj_hex->clear_features();
			    adj_hex->add_feature(new fertile_t());
			    break;
			  }
		      }
		    dir = (dir + 1) % 6;
		  }
		if (loop != 6) /* broken on new fertile */
		  {
		    den->pop = DEN_LIZARD_LEVEL;
		    band->colour[den->colour] = (int)((double)den->pop * DEN_MILITIA_LEVEL);
		  }
		else
		  {
		    /* convert den to ruin */
		    delete hex->del_feature(band);
		    delete hex->del_feature(den);
		    hex->add_feature(new ruin_t);
		  }
	      }
	    else if (den->pop > DEN_LIZARD_LEVEL * fertile)
	      {
		den->pop = DEN_LIZARD_LEVEL * fertile;
		band->colour[den->colour] = (int)((double)den->pop *  DEN_MILITIA_LEVEL);
	      }
	  }
	if ((volcano = hex->has_feature(VOLCANO)))
	  {
	    if (((hex->feature_adjacent(FERTILE) > 0) ||
		 (hex->feature_adjacent (DEN) > 0) || !hex->tmpl))
	      {
		/* change it into a peak, UNLESS IT IS ON A PLAYER TEMPLATE
		   (in which case it will have already been checked for
		   template validity, so may effect the world OFF the
		   template. */
		delete hex->del_feature(volcano);
		hex->add_feature(new peak_t());
	      }
	  }
      }
  
  /* double check for any dens affected by changes */

  for (y = 0; y < (*map).height; y ++)
    for (x = 0; x < (*map).width; x ++)
      {
	hex = (*map)(x, y);
	if ((den = (den_t*)hex->has_feature(DEN)) && !hex->tmpl)
	  {
	    band = (band_t*)hex->has_feature(BAND);
	    fertile = hex->feature_adjacent (FERTILE);
	    if (den->pop != DEN_LIZARD_LEVEL * fertile)
	      {
		den->pop = DEN_LIZARD_LEVEL * fertile;
		band->colour[den->colour] = (int)((double)(den->pop) *  DEN_MILITIA_LEVEL);
	      }
	  }
      }
}

