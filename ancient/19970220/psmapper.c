#include <conio.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lizard.h"
#include "map.h"

#include "strgfunc.h"

#include "lizgame.h"
#include "lizhex.h"
#include "lizmisc.h"

int number_units [MAX_PLAYERS + 1];
unit_t *player_units [MAX_PLAYERS + 1][500];

int main (int argc, char *argv []);
void ps_show_world (FILE *fptr);
void ps_show_hex (int x, int y, FILE *fptr);
int spy_at (int x, int y, int player);
char *brace_string (char *string);
char *ps_hack_braces_string (char *string);
void break_str (char *string, char *string2, int length);

int main (int argc, char *argv [])
{
  FILE *out_fptr;

  char out_filename [20];

  show_title ();

  printf ("LIZARDS! POSTSCRIPT Map Print Utility. %s Last compiled %s.\n\n",
          VER_STRING, __DATE__);

  if (argc != 3)
  {
    printf ("FATAL ERROR: Turn and Game Number not specified on command line!\n"
	     "             Command Line Format: EXEFILE data_path game.\n"
	     "                             e.g. PSMAPPER C:\\ A\n");

    exit (EXIT_FAILURE);
  }
  data_path = LzCheckDir(argv[1], 0);
  game_code = (char*)malloc(strlen(argv[2] + 1));
  strcpy (game_code, argv [2]);
  strupr (game_code);

  get_world(&world);

  get_switch_list ();

  get_player_list ();

  get_spy_list ();

  sprintf (out_filename, "%sLZPSMAP.%s", data_path, game_code);

  if ((out_fptr = fopen (out_filename, "at")) == NULL)
  {
    printf ("FATAL ERROR: Unable to write to %s file.\n", out_filename);

    

    exit (EXIT_FAILURE);
  }

  fprintf (out_fptr, "/Gameletter {(%s)} def\n", game_code);

  printf ("Generating POSTSCRIPT world map...");

  ps_show_world (out_fptr);

  fprintf (out_fptr, "\x04");

  fclose (out_fptr);

  printf (" \nFinished!\n");

  return (EXIT_SUCCESS);
}

void ps_show_world (FILE *fptr)
{
  char text_string [255];

  int page_one_offset,
      page_x,
      page_y,
      x,
      y,
      range,
      hex_per_page;

  hex_per_page = (world.y / 2) + 1;

  range = world.y;

  if (range >= hex_per_page)
  {
    page_one_offset = hex_per_page;

    if (hex_per_page > 16)
    {
      if (world_wrap_switch)
      {
	fprintf (fptr, "Map_Page_R\n");
	fprintf (fptr, "-1190 140 Location_Full_R\n");
      } else {
	fprintf (fptr, "Map_Page\n");
	fprintf (fptr, "-1169 140 Location_Full\n");
      }
    } else {
      if (world_wrap_switch)
      {
	fprintf (fptr, "Map_Page_R\n");
	fprintf (fptr, "-1190 140 Location_R\n");
      } else {
	fprintf (fptr, "Map_Page\n");
	fprintf (fptr, "-1169 140 Location\n");
      }
    }

    fprintf (fptr, "(COMPLETE World Map for Circle Games Lizards! Game %s Page 1 of 2. World Size 1A to %s) Map_Page_Num\n",
      game_code, xy_to_string (world.x - 1, world.y - 1, text_string));

    for (y = 0; y < page_one_offset; y ++)
    {
      for (x = 0; x < world.x; x ++)
      {
	page_x = (x * 35) + 25;

	if (x % 2 == 1)
	  page_y = 775 - (y * 50);
	else
	  page_y = 750 - (y * 50);

	fprintf (fptr, "%d %d ", page_x, page_y);

	ps_show_hex (x, y, fptr);
      }
    }

    fprintf (fptr, "\nprinting\n");

    if (hex_per_page > 16)
    {
      if (world_wrap_switch)
      {
	fprintf (fptr, "Map_Page_R\n");
	fprintf (fptr, "-1190 140 Location_Full_R\n");
      } else {
	fprintf (fptr, "Map_Page\n");
	fprintf (fptr, "-1169 140 Location_Full\n");
      }
    } else {
      if (world_wrap_switch)
      {
	fprintf (fptr, "Map_Page_R\n");
	fprintf (fptr, "-1190 140 Location_R\n");
      } else {
	fprintf (fptr, "Map_Page\n");
	fprintf (fptr, "-1169 140 Location\n");
      }
    }

    fprintf (fptr, "(COMPLETE World Map for Circle Games Lizards! Game %s Page 2 of 2. World size 1A to %s) Map_Page_Num\n",
      game_code, xy_to_string (world.x - 1, world.y - 1, text_string));

    for (y = page_one_offset; y < world.y; y ++)
    {
      for (x = 0; x < world.x; x ++)
      {
	page_x = (x * 35) + 25;

	if (x % 2 == 1)
	  page_y = 775 - ((y - page_one_offset) * 50);
	else
	  page_y = 750 - ((y - page_one_offset) * 50);

	fprintf (fptr, "%d %d ", page_x, page_y);

	ps_show_hex (x, y, fptr);
      }
    }

      fprintf (fptr, "\nprinting\n");
  } else {
    page_one_offset = 0;

    if (hex_per_page > 16)
    {
      if (world_wrap_switch)
      {
	fprintf (fptr, "Map_Page_R\n");
	fprintf (fptr, "-1190 140 Location_Full_R\n");
      } else {
	fprintf (fptr, "Map_Page\n");
	fprintf (fptr, "-1169 140 Location_Full\n");
      }
    } else {
      if (world_wrap_switch)
      {
	fprintf (fptr, "Map_Page_R\n");
	fprintf (fptr, "-1190 140 Location_R\n");
      } else {
	fprintf (fptr, "Map_Page\n");
	fprintf (fptr, "-1169 140 Location\n");
      }
    }

    fprintf (fptr, "(COMPLETE World Map for Circle Games Lizards! Game %s Page 1 of 1. World Size 1A to %s) Map_Page_Num\n",
      game_code, xy_to_string (world.x - 1, world.y - 1, text_string));

    for (y = 0; y < world.y; y ++)
    {
      for (x = 0; x < world.x; x ++)
      {
	page_x = (x * 35) + 25;

	if (x % 2 == 1)
	  page_y = 775 - (y * 50);
	else
	  page_y = 750 - (y * 50);

	fprintf (fptr, "%d %d ", page_x, page_y);

	ps_show_hex (x, y, fptr);
      }
    }

    fprintf (fptr, "\nprinting\n");
  }
}

void ps_show_hex (int x, int y, FILE *fptr)
{
  char string [80];

  hex_t hex;

  get_hex (x, y, &hex);

  if (hex.bridge_here == 1 || hex.raft_here > 0 ||
      (hex.turns_undisturbed > 0 && hex.terrain != HOME_DEN))
  {
    if (hex.bridge_here == 1)
      fprintf (fptr, BRIDGE_PC);
    else
      if (hex.turns_undisturbed > 0)
	fprintf (fptr, HALF_DEN_PC);
      else
	fprintf (fptr, RAFT_PC);
  } else {
    switch (hex.terrain)
    {
      case PLAIN:
	fprintf (fptr, PLAIN_PC);
	break;
      case DEN:
	switch (hex.den_lizard_type)
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
	break;
      case HOME_DEN:
	fprintf (fptr, HOME_DEN_PC);
	break;
      case RUIN:
	fprintf (fptr, RUIN_PC);
	break;
      case FERTILE:
	fprintf (fptr, FERTILE_PC);
	break;
      case SWAMP:
	fprintf (fptr, SWAMP_PC);
	break;
      case PEAK:
	fprintf (fptr, PEAK_PC);
	break;
      case VOLCANO:
	fprintf (fptr, VOLCANO_PC);
	break;
      case WATER:
	fprintf (fptr, WATER_PC);
	break;
      case WHIRLPOOL:
	fprintf (fptr, WHIRLPOOL_PC);
	break;
      case SCRUB:
	fprintf (fptr, SCRUB_PC);
	break;
      case TEMPLE:
	fprintf (fptr, TEMPLE_PC);
	break;
      case CURSED:
	fprintf (fptr, CURSED_PC);
	break;
    }
  }

  xy_to_string (x, y, string);

  fprintf (fptr, "(%s) ", string);

  if (hex.terrain == DEN || hex.terrain == HOME_DEN)
  {
    if (hex.hex_owner == 0)
    {
      fprintf (fptr, "Small (Free) Owner\n");
    } else {
      fprintf (fptr, "Small (%s) Owner (%d) Troop_Number\n",
	       player[hex.hex_owner].code, lizards_in_hex (&hex) + hex.lizard_pop);
    }
  } else {
    if (lizards_in_hex (&hex) > 0)
    {
      fprintf (fptr, "Small (%s) Owner (%d) Troop_Number\n",
	       player[hex.hex_owner].code, lizards_in_hex (&hex));
    } else {
      fprintf (fptr, "Small\n");
    }
  }
}

int spy_at (int x, int y, int player)
{
  int work;

  for (work = 0; work < number_spys; work ++)
    if (spy_list [work]->x_hex == x && spy_list [work]->y_hex == y &&
	spy_list [work]->player == player)
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

  while (ptr = strtok (NULL, " "))
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

  work_string [0] = NULL;

  for (work = 0, work2 = 0; work < strlen (string); work ++, work2 ++)
  {
    if (string [work] == '(' || string [work] == ')')
      work_string [work2 ++] = '\\';

    work_string [work2] = string [work];
  }

  work_string [work2] = NULL;

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

      string [work + 1] = NULL;

      return;
    }

  string2 [0] = NULL;
}
