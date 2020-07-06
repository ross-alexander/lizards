#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "lizard.h"

#include "lizmisc.h"
#include "lizgame.h"
#include "lizhex.h"

char *show_lizards (int red, int green, int grey, int black, int yellow,
		    char *string)
{
  char number [20];

  int first;

  string [0] = '\0';

  first = 1;

  if (red > 0)
  {
    strcat (string, itoa (red, number, 10));
    strcat (string, " Red");

    first = 0;
  }

  if (green > 0)
  {
    if (!first)
      strcat (string, ", ");
    else
      first = 0;

    strcat (string, itoa (green, number, 10));
    strcat (string, " Green");
  }

  if (grey > 0)
  {
    if (!first)
      strcat (string, ", ");
    else
      first = 0;

    strcat (string, itoa (grey, number, 10));
    strcat (string, " Grey");
  }

  if (yellow > 0)
  {
    if (!first)
      strcat (string, ", ");
    else
      first = 0;

    strcat (string, itoa (yellow, number, 10));
    strcat (string, " Yellow");
  }

  if (black > 0)
  {
    if (!first)
      strcat (string, ", ");
    else
      first = 0;

    strcat (string, itoa (black, number, 10));
    strcat (string, " Black");
  }

  return (string);
}

void kill_lizards (int red, int green, int grey, int black, int yellow,
		   int *new_red, int *new_green, int *new_grey,
		   int *new_black, int *new_yellow, int deaths)
{
  int work,
      work2,
      work3,
      work4;

  work = deaths;

  *new_red = red;
  *new_green = green;
  *new_grey = grey;
  *new_black = black;
  *new_yellow = yellow;

  while (work > 0)
  {
    if (work > 10)
      work2 = random (work / 10) + 1;
    else
      work2 = 1;

    do
    {
      work3 = random (LIZARD_TYPES) + 1;

      switch (work3)
      {
	case RED_LIZARD:
	  work4 = *new_red;

	  break;
	case GREEN_LIZARD:
	  work4 = *new_green;

	  break;
	case GREY_LIZARD:
	  work4 = *new_grey;

	  break;
	case BLACK_LIZARD:
	  work4 = *new_black;

	  break;
	case YELLOW_LIZARD:
	  work4 = *new_yellow;

	  break;
      }
    } while (work4 == 0);

    if (work2 > work4)
      work2 = work4;

    switch (work3)
    {
      case RED_LIZARD:
	*new_red -= work2;

	break;
      case GREEN_LIZARD:
	*new_green -= work2;

	break;
      case GREY_LIZARD:
	*new_grey -= work2;

	break;
      case BLACK_LIZARD:
	*new_black -= work2;

	break;
      case YELLOW_LIZARD:
	*new_yellow -= work2;
    }

    work -= work2;
  }
}

void kill_lizards_xy (hex_t *hex, int deaths)
{
  kill_lizards (hex->red_lizards, hex->green_lizards, hex->grey_lizards,
    hex->black_lizards, hex->yellow_lizards, &hex->red_lizards,
    &hex->green_lizards, &hex->grey_lizards,
    &hex->black_lizards, &hex->yellow_lizards, deaths);
}

int lizards_in_hex (hex_t *hex)
{
  return (hex->red_lizards + hex->green_lizards +
	  hex->grey_lizards + hex->black_lizards +
	  hex->yellow_lizards);
}

void open_player_mesg (int player, FILE **fptr)
{
  char filename [20];

  sprintf (filename, "%s%s.%s%02d_%03d", data_path, game_code,
	   PLAYER_MESG_F, player, world->turn);

  if ((*fptr = fopen (filename, "a")) == NULL)
    *fptr = fopen (filename, "w");
}

void send_player_mesg (int player, char *format, ...)
{
  FILE *player_fptr;

  char filename [80];

  sprintf (filename, "%s%s.%s%02d_%03d", data_path, game_code,
	   PLYERR_MESG_F, player, world->turn);

  if ((player_fptr = fopen (filename, "a")) == NULL)
    if ((player_fptr = fopen (filename, "w")) == NULL)
    {
      printf ("FATAL ERROR: Unable to write to %s file. Press SPACE.\n",
	filename);

      exit (EXIT_FAILURE);
    }

#ifdef BROKEN
  vfprintf (player_fptr, format, ...);
#endif

  fclose (player_fptr);
}

char *xy_to_string (int x, int y, char *string)
{
  char temp_string [80];

  strcpy (string, itoa (y + 1, temp_string, 10));
  strcat (string, loc_to_string (x, temp_string));

  return (string);
}

char *loc_to_string (int x, char *string)
{
  int work;

  if (x < 26)
  {
    string [0] = (65 + x);

    string [1] = '\0';
  } else {
    work = (x / 26);

    string [0] = (65 + (work - 1));
    string [1] = (65 + (x - (work * 26)));
    string [2] = '\0';
  }

  return (string);
}

int average_combat_points (int lizards1, int cp1, int lizards2, int cp2)
{
  int work,
      work2,
      newvalue;

  work = lizards1 + lizards2;
  work2 = (lizards1 * cp1) + (lizards2 * cp2);
  newvalue = work2 / work;

  return (newvalue);
}

int average_hunger_points (int lizards1, int hp1, int lizards2, int hp2)
{
  int work,
      work2,
      newvalue;

  work = lizards1 + lizards2;
  work2 = (lizards1 * hp1) + (lizards2 * hp2);
  newvalue = work2 / work;

  return (newvalue);
}

int string_to_direction (char *dir_string)
{
  char directions [6][3] = { "N", "NE", "SE", "S", "SW", "NW" };

  int work;

  for (work = strlen (dir_string) - 1; work > -1; work --)
    if (dir_string [work] == ' ')
      dir_string [work] = '\0';
    else
      break;

  for (work = 0; work < 6; work ++)
    if (strcmp (directions [work], dir_string) == 0)
      return (work + 1);

  return (0);
}

void string_to_loc (char *string, int *x, int *y)
{
  char temp_string [255],
       temp_string2 [255];

  strcpy (temp_string2, string);

  *y = atoi (temp_string2);

  if (*y < 10)
    strcpy (temp_string, &(temp_string2 [1]));
  else
    strcpy (temp_string, &(temp_string2 [2]));

  strupr (temp_string);

  if (strlen (temp_string) > 1)
  {
    *x = temp_string [1] - 65;
    *x += ((temp_string [0] - 64) * 26);
  } else
    *x = temp_string [0] - 65;

  *y -= 1;
}


char *ShowLizards(char *ptr, hex_t *hex)
{
  sprintf(ptr, "%d %d %d %d %d %d %d",
	  hex->red_lizards, hex->green_lizards, hex->grey_lizards,
	  hex->black_lizards, hex->yellow_lizards,
	  hex->turns_hungry, hex->combat_points);
  return ptr;
}

/* ----------------------------------------------------------------------
--
-- BuildUnitList
--
---------------------------------------------------------------------- */

BandList BuildBandList (World *world, int player)
{
  int work2, work3, work4, x, y;
  int i, cnt;
  BandList ulist;
  hex_t world_hex;
  unit_t *work_unit;

  ulist.cnt = cnt = 0;
  for (x = 0; x < world->x; x++)
    for (y = 0; y < world->y; y++)
      {
	get_hex (world, x, y, &world_hex);
	if ((world_hex.hex_owner == player)&&(lizards_in_hex(&world_hex) > 0))
	  cnt++;
      }
  for (i = 0; i < world->num_spies; i++)
    if (world->spylist[i]->player == player)
      cnt++;

  /* cnt is upper limit on total number of possible units */

  if (!(ulist.list = (unit_t*)malloc(sizeof(unit_t) * cnt)))
    {
      fprintf(stderr, "malloc failed in line __LINE__ of file __FILE__\n");
      exit(EXIT_FAILURE);
    }


  for (x = 0; x < world->x; x++)
    for (y = 0; y < world->y; y++)
      {
	get_hex (world, x, y, &world_hex);
	if ((world_hex.hex_owner == player)&&(lizards_in_hex (&world_hex) > 0))
	  {
	    work_unit = &ulist.list[ulist.cnt];
	    work_unit->x = x;
	    work_unit->y = y;
	    work_unit->spy = 0;

	    if (world_hex.terrain == PEAK)
	      work_unit->peak = 1;
	    else
	      work_unit->peak = 0;
	    ulist.cnt++;
	  }
      }

  for (i = 0; i < world->num_spies; i++)
    {
      if (world->spylist[i]->player > 0)
	{
	  work2 = world->spylist[i]->player;
	  work4 = 0;
	  for (work3 = 0; work3 < ulist.cnt; work3++)
	    if ((ulist.list[work3].x == world->spylist[i]->x_hex) &&
		(ulist.list[work3].y == world->spylist[i]->y_hex))
	      {
		work4 = 1;
		break;
	      }

	  if (work4 == 0)
	    {
	      work_unit = &ulist.list[ulist.cnt];
	      work_unit->x = world->spylist[i]->x_hex;
	      work_unit->y = world->spylist[i]->y_hex;
	      work_unit->spy = 1;

	      get_hex (world, work_unit->x, work_unit->y, &world_hex);

	      if (world_hex.terrain == PEAK)
		work_unit->peak = 1;
	      else
		work_unit->peak = 0;
	      ulist.cnt++;
	    }
	}
    }
  return ulist;
}

/* ----------------------------------------------------------------------
--
-- BuildUnitList
--
---------------------------------------------------------------------- */

void see_wrap_hex (World *world, int *x, int *y);
void see_wrap_hex (World *world, int *x, int *y)
{
  if (world->flags & LZ_WRAP)
  {
    if (*x < 0)
      *x += world->x;
    else
      if (*x >= world->x)
	*x -= world->x;

                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         