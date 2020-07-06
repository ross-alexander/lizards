#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lizards.h"
#include "strgfunc.h"
#include "lizgame.h"
#include "lizmisc.h"

#define PLAIN_PC "pla "
#define EMPTY_DEN_PC "eden "
#define RED_DEN_PC "rden "
#define GRN_DEN_PC "gden "
#define GRY_DEN_PC "grden "
#define BLK_DEN_PC "bden "
#define YEL_DEN_PC "yden "
#define HOME_DEN_PC "hden "
#define RUIN_PC "rui "
#define FERTILE_PC "fer "
#define SWAMP_PC "swp "
#define PEAK_PC "mou "
#define VOLCANO_PC "vol "
#define WATER_PC "wat "
#define WHIRLPOOL_PC "whi "
#define BRIDGE_PC "bri "
#define SCRUB_PC "bru "
#define TEMPLE_PC "tem "
#define CURSED_PC "cur "
#define RAFT_PC "raf "
#define HALF_DEN_PC "uden "

world_t* world;
int generic_on;
int number_units [MAX_PLAYERS + 1];
unit_t *player_units [MAX_PLAYERS + 1][500];

typedef coord element_t;

typedef struct
{
  int num_elements;
  element_t *listptr;
} anchor_t;

anchor_t base;

int *weights,
  centre_x,
  centre_y;

static BandList BuildBandList (world_t *w, int player)
{
  int work3, work4, x, y;
  int i, cnt;
  BandList ulist;
  hex_t *hex;
  unit_t *work_unit;

  ulist.cnt = cnt = 0;
  for (x = 0; x < w->x; x++)
    for (y = 0; y < w->y; y++)
      {
	hex = get_hex (world, coord_cons(x, y));
	if ((hex->owner == player) && (lizards_in_hex(hex) > 0))
	  cnt++;
      }
  for (i = 0; i < world->num_spies; i++)
    if (world->spylist[i]->player == player)
      cnt++;

  /* cnt is upper limit on total number of possible units */

  if (!(ulist.list = (unit_t*)calloc(1, sizeof(unit_t) * cnt)))
    {
      fprintf(stderr, "calloc failed in line __LINE__ of file __FILE__\n");
      exit(EXIT_FAILURE);
    }
  for (x = 0; x < world->x; x++)
    for (y = 0; y < world->y; y++)
      {
	hex = get_hex (world, coord_cons(x, y));
	if ((hex->owner == player) && (lizards_in_hex (hex) > 0))
	  {
	    work_unit = &ulist.list[ulist.cnt];
	    work_unit->xy.x = x;
	    work_unit->xy.y = y;
	    work_unit->spy = 0;
	    if (hex->terrain == PEAK)
	      work_unit->peak = 1;
	    else
	      work_unit->peak = 0;
	    ulist.cnt++;
	  }
      }

  for (i = 0; i < world->num_spies; i++)
    {
      if (world->spylist[i]->player == player)
	{
	  work4 = 0;
	  for (work3 = 0; work3 < ulist.cnt; work3++)
	    if ((ulist.list[work3].xy.x == world->spylist[i]->x_hex) &&
		(ulist.list[work3].xy.y == world->spylist[i]->y_hex))
	      {
		work4 = 1;
		break;
	      }
	  if (work4 == 0)
	    {
	      work_unit = &ulist.list[ulist.cnt];
	      work_unit->xy.x = world->spylist[i]->x_hex;
	      work_unit->xy.y = world->spylist[i]->y_hex;
	      work_unit->spy = 1;
	      hex = get_hex (world, work_unit->xy);
	      if (hex->terrain == PEAK)
		work_unit->peak = 1;
	      else
		work_unit->peak = 0;
	      ulist.cnt++;
	    }
	}
    }
  return ulist;
}

static void mark_hex(world_t *w, int *seen, coord xy, int depth)
{
  int i;
  hex_t *hex = get_hex(w, xy);
  seen[w->x * hex->xy.y + hex->xy.x] = 1;
  if (depth > 1)
    for (i = 0; i < 6; i++)
      if (!hex->edge[i].invalid)
	mark_hex(w, seen, hex->edge[i], depth - 1);
}

static HexList *BuildHexList(world_t *world, BandList ulist)
{
  int work, i, j, *seen;
  HexList *hexlist;
  see_t *see_ptr;
  int see_hex;
  
  if ((seen = (int*)calloc (world->x * world->y, sizeof(int))) == NULL)
    {
      printf ("FATAL ERROR: Out of memory line __LINE__ of __FILE__.\n");
      exit (EXIT_FAILURE);
    }
  for (work = 0; work < ulist.cnt; work++)
    mark_hex(world, seen, ulist.list[work].xy, ulist.list[work].peak ? 4 : 3);
  see_hex = 0;
  for (j = 0; j < world->y; j++)
    for (i = 0; i < world->x; i++)
      if (seen[j * world->x + i])
	see_hex++;

  if ((see_ptr = (see_t*)calloc (see_hex, sizeof (see_t))) == NULL)
    {
      printf ("FATAL ERROR: Out of memory line __LINE__ of __FILE__.\n");
      exit (EXIT_FAILURE);
    }
  see_hex = 0;
  for (j = 0; j < world->y; j++)
    for (i = 0; i < world->x; i++)
      if (seen[j * world->x + i])
	{
	  see_ptr[see_hex].x = i;
	  see_ptr[see_hex].y = j;
	  see_hex++;
	}
  if (!(hexlist = (HexList*)calloc(1, sizeof(HexList))))
    {
      fprintf(stderr, "calloc failed in line __LINE__ of file __FILE__\n");
      exit(EXIT_FAILURE);
    }
  hexlist->cnt = see_hex;
  hexlist->list = see_ptr;
  
  free(seen);
  return hexlist;
}
  
void ps_show_world (world_t *,int player, FILE *fptr);
see_t *build_see_list (int player, int *list_len);
see_t *centre_see_list (see_t *see_list, int player, int list_len);
void centre_points (anchor_t *base, int centre_x, int centre_y);
int sum_distances (anchor_t *base, int x, int y);
void weight_points (anchor_t *base, int *weights);
void find_weighted_centre (anchor_t *base, int *weights, int *centre_x, int *centre_y);
int see_list_centred_y_sort (const void *elem1, const void *elem2);
int see_list_x_sort (const void *elem1, const void *elem2);
int see_list_y_sort (const void *elem1, const void *elem2);
void see_wrap_hex (int *x, int *y);
void ps_show_hex (world_t *, int x, int y, int player, FILE *fptr);
int spy_at (world_t*, int x, int y, int player);
char *brace_string (char *string);
char *ps_hack_braces_string (char *string);
void break_str (char *string, char *string2, int length);
void build_unit_list (void);

/* ----------------------------------------------------------------------
--
-- min
--
---------------------------------------------------------------------- */

int min(int,int);
int min(int a, int b)
{
  return a > b ? b : a;
}


/* ----------------------------------------------------------------------
--
-- main
--
---------------------------------------------------------------------- */

extern int optind;
extern char *optarg;

int main (int argc, char *argv [])
{
  FILE *out_fptr;
  char out_filename [20];
  int work, ch;
  generic_on = 1;
  show_title ();

  printf ("LIZARDS! POSTSCRIPT Centred Print Utility. %s Last compiled %s.\n\n",
	  VER_STRING, __DATE__);

  while((ch = getopt(argc, argv, "g")) != EOF)
    switch(ch)
      {
      case 'g':
	generic_on = 1 - generic_on;
	break;
      }

  if ((argc - optind) != 2)
    {
      printf ("FATAL ERROR: Turn, Game Number not specified on command line!\n"
	      "             Command Line Format: EXEFILE data_path disk_data_path world.turn game generic.\n"
	      "                             e.g. PSCENPRN C:\\ D:\\ 1 A N\n");

      exit (EXIT_FAILURE);
    }
  world = get_world(LzCheckDir(argv[optind], 0), argv[optind + 1]);

  if (world->num_players == 0)
    {
      fprintf (stderr, "FATAL ERROR: Game %s has no players.\n", world->game);
      exit (EXIT_FAILURE);
    }

  strcpy (world->player[0]->code, "C-G");

  sprintf (out_filename, "%s%s.%s%03d", world->path, world->game,
	   POSTSCRIPT_F, world->turn);

  if ((out_fptr = fopen (out_filename, "w")) == NULL)
    {
      fprintf (stderr, "FATAL ERROR: Unable to write to %s file.\n", out_filename);
      exit (EXIT_FAILURE);
    }

  fprintf (out_fptr, "/Gameletter {(%s)} def\n", world->game);
  fprintf (out_fptr, "/Turnnumber {(%d)} def\n", world->turn);
  fprintf (out_fptr, "/NTN {(%d)} def\n", world->turn + 1);
  fprintf (out_fptr, "/Date {(%ld)} def\n", world->due);

  printf ("Building player maps...");

  printf (" \n");

  for (work = 1; work < world->num_players; work ++)
    if (world->player[work])
      {
	printf ("Postscripting Player %d...\n", work);

	/*
	sprintf (filename, "%s%s.%s%02d_%03d", data_path, game_code,
		 PLAYER_MESG_F, work, world->turn);

	if ((in_fptr = fopen (filename, "r")) == NULL)
	  {
	    fprintf (stderr, "FATAL ERROR: Unable to read %s file.\n", filename);
	    exit (EXIT_FAILURE);
	  }
	
	  */
	fprintf (out_fptr, "/saveobj save def\n");
	fprintf (out_fptr, "/Shadow_Code {(%s)} def\n",
		 world->player[work]->code);
	
	/*
	while (fgets (work_string, 255, in_fptr) != NULL &&
	       strcmp (work_string, "Write\n") != 0)
	  fprintf (out_fptr, "%s", work_string);

	fprintf (out_fptr, "%s[\n", work_string);

	while (fgets (work_string, 255, in_fptr) != NULL)
	  {
	    work_string [strlen (work_string) - 1] = '\0';
	    
	    if (strncmp (work_string, " * ", 3) == 0 && work > 0)
	      {
		fprintf (out_fptr, "Startf (%s) Endf\n",
			 ps_hack_braces_string (&work_string [2]));
	      } else {
		fprintf (out_fptr, "%s Linef\n", brace_string (work_string));
	      }
	  }
	fprintf (out_fptr, "] format\n");
	fprintf (out_fptr, "pr\n");
	fclose (in_fptr);
	remove (filename);
	*/
	ps_show_world (world, work, out_fptr);
	fprintf (out_fptr, "saveobj restore\n");
      }
    fclose (out_fptr);
  printf ("\nFinished!\n");
  return (EXIT_SUCCESS);
}

/* ----------------------------------------------------------------------
--
-- ps_show_world
--
---------------------------------------------------------------------- */

void ps_show_world (world_t *world, int player, FILE *fptr)
{
  char text_string [255];

  int page_one_offset,
    page_x,
    page_y,
    range,
    work,
    high_end,
    hex_per_page;

  BandList bandlist;
  HexList *hexlist;
  see_t *player_hex_see_list;

  bandlist = BuildBandList(world, player);
  hexlist = BuildHexList(world, bandlist);

  if (bandlist.cnt == 0)
    return;

  if (world->y / 2 > 16)
    hex_per_page = 18;
  else
    hex_per_page = 16;

  player_hex_see_list = hexlist->list;
  high_end = hexlist->cnt;
  
  player_hex_see_list = centre_see_list (player_hex_see_list,player,high_end);
  if ((range = player_hex_see_list [high_end - 1].centred_y -
       player_hex_see_list [0].centred_y + 1) > hex_per_page)
    {
      if (hex_per_page > 16)
	{
	  page_one_offset = 18;
	  fprintf (fptr, "Map_Page_R\n");
	  fprintf (fptr, "-1190 140 Location_Full_R\n");
	}
      else
	{
	  page_one_offset = 16;
	  fprintf (fptr, "Map_Page_R\n");
	  fprintf (fptr, "-1190 140 Location_R\n");
	}

      fprintf (fptr, "(%s Clan World View for Lizards! Game %s Turn %d Page 1 of 2. World size 1A to %s) Map_Page_Num\n",
	       world->player[player]->clan,
	       world->game, world->turn,
	       xy_to_string (world->x - 1, world->y - 1, text_string));

      for (work = 0; player_hex_see_list [work].centred_y < page_one_offset; work ++)
	{
	  page_x = (player_hex_see_list [work].centred_x * 35) + 25;

	  if (player_hex_see_list [work].centred_x % 2 == 1)
	    page_y = 775 - (player_hex_see_list [work].centred_y * 50);
	  else
	    page_y = 750 - (player_hex_see_list [work].centred_y * 50);

	  fprintf (fptr, "%d %d ", page_x, page_y);

	  ps_show_hex (world,
		       player_hex_see_list [work].x,
		       player_hex_see_list [work].y,
		       player, fptr);
	}

      fprintf (fptr, "printing\n");

      if ((player_hex_see_list [high_end - 1].centred_y -
	   hex_per_page + 1) > 16)
	{
	  fprintf (fptr, "Map_Page_R\n");
	  fprintf (fptr, "-1190 140 Location_Full_R\n");
	}
      else
	{
	  fprintf (fptr, "Map_Page_R\n");
	  fprintf (fptr, "-1190 140 Location_R\n");
	}

      fprintf (fptr, "(%s Clan World View for Lizards! Game %s Turn %d Page 2 of 2. World size 1A to %s) Map_Page_Num\n",
	       world->player[player]->clan,
	       world->game, world->turn,
	       xy_to_string (world->x - 1, world->y - 1, text_string));

      for (; work < high_end; work ++)
	if (player_hex_see_list [work].centred_y >= page_one_offset)
	  {
	    page_x = (player_hex_see_list [work].centred_x * 35) + 25;

	    if (player_hex_see_list [work].centred_x % 2 == 1)
	      page_y = 775 - ((player_hex_see_list [work].centred_y - page_one_offset) * 50);
	    else
	      page_y = 750 - ((player_hex_see_list [work].centred_y - page_one_offset) * 50);

	    fprintf (fptr, "%d %d ", page_x, page_y);
	    ps_show_hex (world,
			 player_hex_see_list [work].x,
			 player_hex_see_list [work].y,
			 player, fptr);
	  }

      fprintf (fptr, "printing\n");
    }
  else
    {
      page_one_offset = player_hex_see_list [0].centred_y;
      
      if (player_hex_see_list [0].centred_y != 0 ||
	  player_hex_see_list [0].centred_y != hex_per_page)
	{
	  if (hex_per_page > 16)
	    {
	      if (range > 16)
		{
		  fprintf (fptr, "Map_Page_R\n");
		  fprintf (fptr, "-1190 140 Location_Full_R\n");
		}
	      else
		{
		  fprintf (fptr, "Map_Page_R\n");
		  fprintf (fptr, "-1190 140 Location_R\n");
		}
	    }
	  else
	    {
	      fprintf (fptr, "Map_Page_R\n");
	      fprintf (fptr, "-1190 140 Location_R\n");
	    }

	  fprintf (fptr, "(%s Clan World View for Lizards! Game %s Turn %d Page 1 of 1. World size 1A to %s) Map_Page_Num\n",
		   world->player[player]->clan, world->game, world->turn,
		   xy_to_string (world->x - 1, world->y - 1, text_string));

	  for (work = 0; work < high_end; work ++)
	    {
	      page_x = (player_hex_see_list [work].centred_x * 35) + 25;

	      if (player_hex_see_list [work].centred_x % 2 == 1)
		page_y = 775 - ((player_hex_see_list [work].centred_y - page_one_offset) * 50);
	      else
		page_y = 750 - ((player_hex_see_list [work].centred_y - page_one_offset) * 50);

	      fprintf (fptr, "%d %d ", page_x, page_y);

	      ps_show_hex (world,
			   player_hex_see_list [work].x,
			   player_hex_see_list [work].y,
			   player, fptr);
	    }

	  fprintf (fptr, "printing\n");
	}
      else
	{
	  if (hex_per_page > 16)
	    {
	      fprintf (fptr, "Map_Page_R\n");
	      fprintf (fptr, "-1190 140 Location_Full_R\n");
	    }
	  else
	    {
	      fprintf (fptr, "Map_Page_R\n");
	      fprintf (fptr, "-1190 140 Location_R\n");
	    }

	  fprintf (fptr, "(%s Clan World View for Lizards! Game %s Turn %d Page 1 of 1. World size 1A to %s) Map_Page_Num\n",
		   world->player[player]->clan, world->game, world->turn,
		   xy_to_string (world->x - 1, world->y - 1, text_string));

	  for (work = 0; work < high_end; work ++)
	    {
	      page_x = (player_hex_see_list [work].centred_x * 35) + 25;

	      if (player_hex_see_list [work].centred_x % 2 == 1)
		page_y = 775 - ((player_hex_see_list [work].centred_y - page_one_offset) * 50);
	      else
		page_y = 750 - ((player_hex_see_list [work].centred_y - page_one_offset) * 50);

	      fprintf (fptr, "%d %d ", page_x, page_y);

	      ps_show_hex (world,
			   player_hex_see_list [work].x,
			   player_hex_see_list [work].y,
			   player, fptr);
	    }

	  fprintf (fptr, "printing\n");
	}
    }

  free (player_hex_see_list);
}

see_t *centre_see_list (see_t *see_list, int player, int list_len)
{
  unsigned int loop;

  base.num_elements = list_len;

  if ((base.listptr = malloc (sizeof (element_t) * list_len)) == NULL)
    {
      printf ("FATAL ERROR: Out of memory.\n");
      exit (EXIT_FAILURE);
    }

  if ((weights = malloc (sizeof (int) * list_len)) == NULL)
    {
      printf ("FATAL ERROR: Out of memory.\n");
      exit (EXIT_FAILURE);
    }

  for (loop = 0; loop < list_len; loop ++)
    {
      base.listptr [loop].x = see_list [loop].x;
      base.listptr [loop].y = see_list [loop].y;
    }

  weight_points (&base, weights);
  find_weighted_centre (&base, weights, &centre_x, &centre_y);
  centre_points (&base, centre_x, centre_y);

  for (loop = 0; loop < list_len; loop ++)
    {
      see_list [loop].centred_x = base.listptr [loop].x;
      see_list [loop].centred_y = base.listptr [loop].y;
    }
  free (base.listptr);
  free (weights);

  qsort (see_list, list_len, sizeof (see_t), see_list_centred_y_sort);
  return (see_list);
}

void centre_points (anchor_t *base, int centre_x, int centre_y)
{
  element_t *ptr;

  int loop,
    x,
    y;

  if (base->listptr == NULL) return;

  for ( loop = 0; loop < base->num_elements; loop ++ )
    {
      ptr = &base->listptr [loop];
      x = ptr->x - centre_x + (world->x / 2);

      if (x < 0) x += world->x;
      if (x >= world->x) x = x % world->x;
    
      if (ptr->x % 2 != x % 2)
	x += 1;

      y = ptr->y - centre_y + (world->y / 2);

      if (y < 0) y += world->y;
      if (y >= world->y) y = y % world->y;

      ptr->x = x;
      ptr->y = y;
    }
}

int sum_distances (anchor_t *base, int x, int y)
{
  element_t *ptr;

  int loop,
    i,
    j,
    total = 0;

  if (base->listptr == NULL) return (0);

  for (loop = 0; loop < base->num_elements; loop ++)
    {
      ptr = &base->listptr [loop];

      if (ptr->x < x)
	{
	  i = ptr->x;
	  j = x;
	} else {
	  i = x;
	  j = ptr->x;
	}

      total += min (j - i, world->x - j + i);

      if (ptr->y < y)
	{
	  i = ptr->y;
	  j = y;
	} else {
	  i = y;
	  j = ptr->y;
	}

      total += min (j - i, world->y - j + i);
    }

  return (total);
}

void weight_points (anchor_t *base, int *weights)
{
  element_t *ptr;

  int loop,
    i = 0;

  if (base->listptr == NULL) return;
  for (loop = 0; loop < base->num_elements; loop ++)
    {
      ptr = &base->listptr [loop];
      weights [i++] = sum_distances (base, ptr->x, ptr->y);
    }
}

void find_weighted_centre (anchor_t *base, int *weights, int *centre_x, int *centre_y)
{
  element_t *ptr;

  int loop,
    i = 0,
    lowest;

  if (base->listptr == NULL) return;

  lowest = weights [0];

  for (loop = 0; loop < base->num_elements; loop ++)
    {
      ptr = &base->listptr [loop];

      if (lowest >= weights [i])
	{
	  *centre_x = ptr->x;
	  *centre_y = ptr->y;
	  lowest = weights [i];
	}

      i ++;
    }
}

int see_list_centred_y_sort (const void *elem1, const void *elem2)
{
  if (((see_t *)elem1)->centred_y < ((see_t *)elem2)->centred_y)
    return (-1);
  else
    if (((see_t *)elem1)->centred_y > ((see_t *)elem2)->centred_y)
      return (1);
    else
      return (0);
}

int see_list_y_sort (const void *elem1, const void *elem2)
{
  if (((see_t *)elem1)->y < ((see_t *)elem2)->y)
    return (-1);
  else
    if (((see_t *)elem1)->y > ((see_t *)elem2)->y)
      return (1);
    else
      return (0);
}

int see_list_x_sort (const void *elem1, const void *elem2)
{
  if (((see_t *)elem1)->x < ((see_t *)elem2)->x)
    return (-1);
  else
    if (((see_t *)elem1)->x > ((see_t *)elem2)->x)
      return (1);
    else
      return (0);
}

void ps_show_hex (world_t *world, int x, int y, int player, FILE *fptr)
{
  char string [80];
  hex_t *hex;
  feature_t *f;

  hex = get_hex (world, coord_cons(x, y));
  if (has_feature(hex, BRIDGE))
    fprintf (fptr, BRIDGE_PC);
  else if (has_feature(hex, RAFT))
     fprintf (fptr, RAFT_PC);
  else if (has_feature(hex, BUILDING))
    fprintf (fptr, HALF_DEN_PC);
  else if ((f = has_feature(hex, DEN)))
    {
      if (f->f.den.home)
	fprintf (fptr, HOME_DEN_PC);
      else
	{
	  switch (f->f.den.type)
	    {
	    case 0:
	      fprintf (fptr, EMPTY_DEN_PC);
	      break;
	    case RED_LIZARD:
	      fprintf (fptr, RED_DEN_PC);
	      break;
	    case GREEN_LIZARD:
	      fprintf (fptr, GRN_DEN_PC);
	      break;
	    case GREY_LIZARD:
	      fprintf (fptr, GRY_DEN_PC);
	      break;
	    case BLACK_LIZARD:
	      fprintf (fptr, BLK_DEN_PC);
	      break;
	    case YELLOW_LIZARD:
	      fprintf (fptr, YEL_DEN_PC);
	      break;
	    }
	}
    }
  else if (has_feature(hex, RUIN))
    fprintf (fptr, RUIN_PC);
  else if (has_feature(hex, FERTILE))
    fprintf (fptr, FERTILE_PC);    
  else if (has_feature(hex, VOLCANO))
    fprintf (fptr, VOLCANO_PC);
  else if (has_feature(hex, WHIRLPOOL))
    fprintf (fptr, WHIRLPOOL_PC);
  else if (has_feature(hex, TEMPLE))
    fprintf (fptr, TEMPLE_PC);
  else if (has_feature(hex, CURSED))
    fprintf (fptr, CURSED_PC);
  else
    {
      switch (hex->terrain)
	{
	case PLAIN:
	  fprintf (fptr, PLAIN_PC);
	  break;
	case SWAMP:
	  fprintf (fptr, SWAMP_PC);
	  break;
	case PEAK:
	  fprintf (fptr, PEAK_PC);
	  break;
	case WATER:
	  fprintf (fptr, WATER_PC);
	  break;
	case SCRUB:
	  fprintf (fptr, SCRUB_PC);
	  break;
	}
    }
  
  if (generic_on)
    {
      xy_to_string (x, y, string);

      fprintf (fptr, "(%s) ", string);
    } else
      fprintf (fptr, "() ");

  if ((f= has_feature(hex, DEN)))
    {
      if (hex->owner == 0)
	{
	  fprintf (fptr, "Small (Free) Owner\n");
	}
      else
	{
	  if (hex->owner == player)
	    fprintf (fptr, "Small (%s) Owner (%d) Troop_Number\n",
		     world->player[hex->owner]->code,
		     lizards_in_hex (hex) + f->f.den.pop);
	  else
	    fprintf (fptr, "Small (%s) Owner\n", world->player[hex->owner]->code);
	}
    }
  else
    {
      if (lizards_in_hex (hex) > 0)
	{
	  if ((hex->terrain != SCRUB && !has_feature(hex, RUIN)) ||
	      hex->owner == player)
	    {
	      fprintf (fptr, "Small (%s) Owner (%d) Troop_Number\n",
		       world->player[hex->owner]->code, lizards_in_hex (hex));
	    }
	  else
	    {
	      if (spy_at (world, x, y, player))
		{
		  fprintf (fptr, "Small (%s) Owner (%d) Troop_Number\n",
			   world->player[hex->owner]->code,
			   lizards_in_hex (hex));
		}
	      else
		{
		  fprintf (fptr, "Small\n");
		}
	    }
	}
      else
	{
	  if (spy_at (world, x, y, player))
	    {
	      fprintf (fptr, "Small (%s) Owner (SPY) Troop_Number\n",
		       world->player[player]->code);
	    }
	  else
	    {
	      if ((has_feature(hex, BRIDGE) || has_feature(hex, TEMPLE)) &&
		  hex->owner > 0)
		fprintf (fptr, "Small (%s) Owner\n", world->player[hex->owner]->code);
	      else
		fprintf (fptr, "Small\n");
	    }
	}
    }
}

int spy_at (world_t *world, int x, int y, int player)
{
  int work;

  for (work = 0; work < world->num_spies; work ++)
    if ((world->spylist [work]->x_hex == x) &&
	(world->spylist [work]->y_hex == y) &&
	(world->spylist [work]->player == player))
      return (1);

  return (0);
}

char *brace_string (char *string)
{
  char work_string [400],
    *ptr;

  ps_hack_braces_string (string);

  strcpy (work_string, "(");

  strtok (string, " ");

  strcat (work_string, string);
  strcat (work_string, ")");

  while ((ptr = strtok (NULL, " ")))
    {
      strcat (work_string, "(");
      strcat (work_string, ptr);
      strcat (work_string, ")");
    }

  strcpy (string, work_string);
  return (string);
}

char *ps_hack_braces_string (char *string)
{
  char work_string [400];

  int work,
    work2;

  work_string [0] = '\0';

  for (work = 0, work2 = 0; work < strlen (string); work ++, work2 ++)
    {
      if (string [work] == '(' || string [work] == ')')
	work_string [work2 ++] = '\\';

      work_string [work2] = string [work];
    }
  work_string [work2] = '\0';
  strcpy (string, work_string);
  return (string);
}

void break_str (char *string, char *string2, int length)
{
  int work;

  for (work = length; work > -1; work --)
    if (isspace (string [work]) || ispunct (string [work]))
      {
	strsplt (string2, string, work + 1);
	string [work] = '\n';
	string [work + 1] = '\0';
	return;
      }
  string2 [0] = '\0';
}

void build_unit_list (void)
{
  int work,
    work2,
    work3,
    work4,
    x,
    y;
  int i, j;
  hex_t* world_hex;
  unit_t work_unit;

  for (work = 1; work < world->num_players; work ++)
    number_units [work] = 0;

  for (x = 0; x < world->x; x ++)
    {
      for (y = 0; y < world->y; y ++)
	{
	  world_hex = get_hex (world, coord_cons(x, y));
	  if (world_hex->owner > 0 && lizards_in_hex (world_hex) > 0)
	    {
	      work = world_hex->owner;
	      for (work2 = 0, work3 = 0; work2 < number_units [work]; work2 ++)
		if (player_units [work][work2]->xy.x == x &&
		    player_units [work][work2]->xy.y == y)
		  {
		    work3 = 1;
		    break;
		  }

	      if (work3 == 0)
		{
		  work_unit.xy.x = x;
		  work_unit.xy.y = y;
		  work_unit.spy = 0;

		  if (world_hex->terrain == PEAK)
		    work_unit.peak = 1;
		  else
		    work_unit.peak = 0;

		  if ((player_units [work][number_units [work]] =
		       malloc (sizeof (unit_t))) == NULL)
		    {
		      printf ("FATAL ERROR: Out of memory.\n");
		      exit (EXIT_FAILURE);
		    }
		  *player_units [work][number_units [work]] = work_unit;
		  number_units [work] ++;
		}
	    }
	}
    }
  for (work = 0; work < world->num_spies; work ++)
    {
      if (world->spylist [work]->player > 0)
	{
	  i = world->spylist [work]->player;
	  for (j = 0, work4 = 0; work3 < number_units [work2]; j++)
	    if (player_units[i][j]->xy.x == world->spylist[work]->x_hex &&
		player_units[i][j]->xy.y == world->spylist[work]->y_hex)
	      {
		work4 = 1;
		break;
	      }
	  if (work4 == 0)
	    {
	      work_unit.xy.x = world->spylist [work]->x_hex;
	      work_unit.xy.y = world->spylist [work]->y_hex;
	      work_unit.spy = 1;

	      world_hex = get_hex (world, work_unit.xy);
	      if (world_hex->terrain == PEAK)
		work_unit.peak = 1;
	      else
		work_unit.peak = 0;
	      if ((player_units [i][number_units [i]] =
		   malloc (sizeof (unit_t))) == NULL)
		{
		  printf ("FATAL ERROR: Out of memory.\n");
		  exit (EXIT_FAILURE);
		}
	      *player_units [i][number_units [i]] = work_unit;
	      number_units [i] ++;
	    }
	}
    }
}
