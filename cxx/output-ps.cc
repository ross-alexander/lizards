#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libxml/tree.h>
#include <libxml++/libxml++.h>
#include <gtk/gtk.h>
#include <librsvg/rsvg.h>

#include "lizards.h"
#include "hexlist.h"
#include "output.h"

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



#define MAX_PLAYERS 16

int generic_on;
int number_units [MAX_PLAYERS + 1];
unit_t *player_units [MAX_PLAYERS + 1][500];


// ----------------------------------------------------------------------

char *strsplt (char *dest, char *source, int start)
{
  unsigned int work,
      work2;

  for (work = start, work2 = 0; work < strlen (source); work ++, work2 ++)
    dest [work2] = source [work];

  dest [work2] = '\0';

  return (dest);
}

int spy_at (world_t*, int x, int y, int player);
char *brace_string (char *string);
char *ps_hack_braces_string (char *string);
void break_str (char *string, char *string2, int length);

/* ----------------------------------------------------------------------
--
-- ps_show_world
--
---------------------------------------------------------------------- */

void output_ps_t::show_world (int player, FILE *fptr)
{
  int page_one_offset,
    page_x,
    page_y,
    range,
    work,
    high_end,
    hex_per_page;

  bandlist_t bandlist(world, player);
  hexlist_t *hexlist;
  see_t *player_hex_see_list;

  hexlist = new hexlist_t(world, &bandlist);

  if (bandlist.cnt == 0)
    return;

  if (world->map->height / 2 > 16)
    hex_per_page = 18;
  else
    hex_per_page = 16;

  hexlist->center();

  player_hex_see_list = hexlist->list;
  high_end = hexlist->cnt;

  range = player_hex_see_list [high_end - 1].cy -
    player_hex_see_list [0].cy + 1;

  if (range > hex_per_page)
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
	       (*world->map)(world->map->width - 1, world->map->height - 1)->title);

      for (work = 0; player_hex_see_list [work].cy < page_one_offset; work ++)
	{
	  page_x = (player_hex_see_list [work].cx * 35) + 25;

	  if (player_hex_see_list [work].cx % 2 == 1)
	    page_y = 775 - (player_hex_see_list [work].cy * 50);
	  else
	    page_y = 750 - (player_hex_see_list [work].cy * 50);

	  fprintf (fptr, "%d %d ", page_x, page_y);

	  show_hex (player_hex_see_list [work].x,
		    player_hex_see_list [work].y,
		    player, fptr);
	}

      fprintf (fptr, "printing\n");

      if ((player_hex_see_list [high_end - 1].cy -
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
	       (*world->map)(world->map->width - 1, world->map->height - 1)->title);

      for (; work < high_end; work ++)
	if (player_hex_see_list [work].cy >= page_one_offset)
	  {
	    page_x = (player_hex_see_list [work].cx * 35) + 25;

	    if (player_hex_see_list [work].cx % 2 == 1)
	      page_y = 775 - ((player_hex_see_list [work].cy - page_one_offset) * 50);
	    else
	      page_y = 750 - ((player_hex_see_list [work].cy - page_one_offset) * 50);

	    fprintf (fptr, "%d %d ", page_x, page_y);
	    show_hex (player_hex_see_list [work].x,
		      player_hex_see_list [work].y,
		      player, fptr);
	  }

      fprintf (fptr, "printing\n");
    }
  else
    {
      printf("Single page only.\n");

      page_one_offset = player_hex_see_list [0].cy;
      
      if (player_hex_see_list [0].cy != 0 ||
	  player_hex_see_list [0].cy != hex_per_page)
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
		   (*world->map)(world->map->width - 1, world->map->height - 1)->title);

	  for (work = 0; work < high_end; work ++)
	    {
	      page_x = (player_hex_see_list [work].cx * 35) + 25;

	      if (player_hex_see_list [work].cx % 2 == 1)
		page_y = 775 - ((player_hex_see_list [work].cy - page_one_offset) * 50);
	      else
		page_y = 750 - ((player_hex_see_list [work].cy - page_one_offset) * 50);

	      fprintf (fptr, "%d %d ", page_x, page_y);

	      show_hex (player_hex_see_list [work].x,
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
		   (*world->map)(world->map->width - 1, world->map->height - 1)->title);

	  for (work = 0; work < high_end; work ++)
	    {
	      page_x = (player_hex_see_list [work].cx * 35) + 25;

	      if (player_hex_see_list [work].cx % 2 == 1)
		page_y = 775 - ((player_hex_see_list [work].cy - page_one_offset) * 50);
	      else
		page_y = 750 - ((player_hex_see_list [work].cy - page_one_offset) * 50);

	      fprintf (fptr, "%d %d ", page_x, page_y);

	      show_hex (player_hex_see_list [work].x,
			player_hex_see_list [work].y,
			player, fptr);
	    }

	  fprintf (fptr, "printing\n");
	}
    }
}

void output_ps_t::show_hex (int x, int y, int player, FILE *fptr)
{
  hex_t *hex;
  feature_t *f;

  hex = (*world->map)(x, y);

  switch(order(hex))
    {
    case OUTPUT_BRIDGE:
      fprintf (fptr, BRIDGE_PC);
      break;
    case OUTPUT_RAFT:
      fprintf (fptr, RAFT_PC);
      break;
    case OUTPUT_BUILDING:
      fprintf (fptr, HALF_DEN_PC);
      break;
    case OUTPUT_HOME_DEN:
      fprintf (fptr, HOME_DEN_PC);
      break;
    case OUTPUT_EMPTY_DEN:
      fprintf (fptr, EMPTY_DEN_PC);
      break;
    case OUTPUT_RED_DEN:
      fprintf (fptr, RED_DEN_PC);
      break;
    case OUTPUT_GREEN_DEN:
      fprintf (fptr, GRN_DEN_PC);
      break;
    case OUTPUT_GREY_DEN:
      fprintf (fptr, GRY_DEN_PC);
      break;
    case OUTPUT_BLACK_DEN:
      fprintf (fptr, BLK_DEN_PC);
      break;
    case OUTPUT_YELLOW_DEN:
      fprintf (fptr, BLK_DEN_PC);
      break;
    case OUTPUT_RUIN:
      fprintf (fptr, RUIN_PC);
      break;
    case OUTPUT_FERTILE:
      fprintf (fptr, FERTILE_PC);
      break;
    case OUTPUT_VOLCANO:
      fprintf (fptr, VOLCANO_PC);
      break;
    case OUTPUT_WHIRLPOOL:
      fprintf (fptr, WHIRLPOOL_PC);
      break;
    case OUTPUT_TEMPLE:
      fprintf (fptr, TEMPLE_PC);
      break;
    case OUTPUT_CURSED:
      fprintf (fptr, CURSED_PC);
      break;
    case OUTPUT_PLAIN:
      fprintf (fptr, PLAIN_PC);
      break;
    case OUTPUT_SWAMP:
      fprintf (fptr, SWAMP_PC);
      break;
    case OUTPUT_PEAK:
      fprintf (fptr, PEAK_PC);
      break;
    case OUTPUT_WATER:
      fprintf (fptr, WATER_PC);
      break;
    case OUTPUT_SCRUB:
      fprintf (fptr, SCRUB_PC);
      break;
    default:
      break;
    }
  
  if (generic_on)
    fprintf (fptr, "() ");
  else
    fprintf (fptr, "(%s) ", hex->title);

  if ((f = hex->has_feature(DEN)))
    {
      den_t *den = (den_t*)den;
      if (hex->owner == 0)
	{
	  fprintf (fptr, "Small (Free) Owner\n");
	}
      else
	{
	  if (hex->owner == player)
	    fprintf (fptr, "Small (%s) Owner (%d) Troop_Number\n",
		     world->player[hex->owner]->code, hex->size());
	  else
	    fprintf (fptr, "Small (%s) Owner\n", world->player[hex->owner]->code);
	}
    }
  else
    {
      if (hex->size() > 0)
	{
	  if ((!hex->has_feature(SCRUB) && !hex->has_feature(RUIN)) ||
	      hex->owner == player)
	    {
	      fprintf (fptr, "Small (%s) Owner (%d) Troop_Number\n",
		       world->player[hex->owner]->code, hex->size());
	    }
	  else
	    {
	      if (spy_at (world, x, y, player))
		{
		  fprintf (fptr, "Small (%s) Owner (%d) Troop_Number\n",
			   world->player[hex->owner]->code, hex->size());
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
	      if ((hex->has_feature(BRIDGE) || hex->has_feature(TEMPLE)) &&
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
#ifdef X
  int work;
  for (work = 0; work < world->num_spies; work ++)
    if ((world->spylist [work]->x_hex == x) &&
	(world->spylist [work]->y_hex == y) &&
	(world->spylist [work]->player == player))
      return (1);
#endif
  return 0;
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

  for (work = 0, work2 = 0; work < (int)(strlen(string)); work ++, work2 ++)
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

/* ----------------------------------------------------------------------
--
-- PSPrint
--
---------------------------------------------------------------------- */

void output_ps_t::display()
{
  int work;
  FILE *out_fptr, *stream;
  char out_filename[200];

  if (world->num_players == 0)
    {
      fprintf (stderr, "FATAL ERROR: Game %s has no players.\n", world->game);
      exit (EXIT_FAILURE);
    }

  sprintf (out_filename, "%s%02d%s",
	   world->game, world->turn, POSTSCRIPT_F);

  if ((out_fptr = fopen (out_filename, "w")) == NULL)
    {
      fprintf (stderr, "FATAL ERROR: Unable to write to %s file.\n", out_filename);
      exit (EXIT_FAILURE);
    }

  if ((stream = fopen(PSHEADER_F, "r")))
    {
      while (!feof(stream))
	{
	  char buf[256];
	  int count = fread(buf, sizeof(char), 256, stream);
	  if (count) fwrite(buf, sizeof(char), count, out_fptr);
	}
      fclose(stream);
    }

  fprintf (out_fptr, "/Gameletter {(%s)} def\n", world->game);
  fprintf (out_fptr, "/Turnnumber {(%d)} def\n", world->turn);
  fprintf (out_fptr, "/NTN {(%d)} def\n", world->turn + 1);
  fprintf (out_fptr, "/Date {(%ld)} def\n", world->due);

  printf ("Building player maps...\n");
  for (work = 1; work < world->num_players; work ++)
    if (world->player[work])
      {
	printf ("Postscripting Player %d...\n", work);

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
	show_world (work, out_fptr);
	fprintf (out_fptr, "saveobj restore\n");
      }
    fclose (out_fptr);
  printf ("\nFinished!\n");
}

output_ps_t::output_ps_t(world_t *w) : output_t(w)
{
}
