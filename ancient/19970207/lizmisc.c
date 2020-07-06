#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "lizard.h"
#include "map.h"

#include "lizmisc.h"

extern char game_code [5];

extern int world_x,
	   world_y,
	   number_orders,
	   turn_number;

extern char data_path [80];

char *show_lizards (int red, int green, int grey, int black, int yellow,
		    char *string)
{
  char number [20];

  int first;

  string [0] = NULL;

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
    hex->black_lizards, hex->yellow_lizards, &(int) hex->red_lizards,
    &(int) hex->green_lizards, &(int) hex->grey_lizards,
    &(int) hex->black_lizards, &(int) hex->yellow_lizards, deaths);
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

  sprintf (filename, "%s%s%02d_%03d.%s", data_path, PLAYER_MESG_F, player,
    turn_number, game_code);

  if ((*fptr = fopen (filename, "at")) == NULL)
    *fptr = fopen (filename, "wt");
}

void send_player_mesg (int player, char *format, ...)
{
  FILE *player_fptr;

  char filename [80];

  sprintf (filename, "%s%s%02d_%03d.%s", data_path, PLYERR_MESG_F, player,
    turn_number, game_code);

  if ((player_fptr = fopen (filename, "at")) == NULL)
    if ((player_fptr = fopen (filename, "wt")) == NULL)
    {
      printf ("FATAL ERROR: Unable to write to %s file. Press SPACE.\n",
	filename);

      getch ();

      exit (EXIT_FAILURE);
    }

  vfprintf (player_fptr, format, ...);

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

    string [1] = NULL;
  } else {
    work = (x / 26);

    string [0] = (65 + (work - 1));
    string [1] = (65 + (x - (work * 26)));
    string [2] = NULL;
  }

  return (string);
}

int average_combat_points (int lizards1, int cp1, int lizards2, int cp2)
{
  int work,
      work2,
      new;

  work = lizards1 + lizards2;
  work2 = (lizards1 * cp1) + (lizards2 * cp2);
  new = work2 / work;

  return (new);
}

int average_hunger_points (int lizards1, int hp1, int lizards2, int hp2)
{
  int work,
      work2,
      new;

  work = lizards1 + lizards2;
  work2 = (lizards1 * hp1) + (lizards2 * hp2);
  new = work2 / work;

  return (new);
}

int string_to_direction (char *dir_string)
{
  char directions [6][3] = { "N", "NE", "SE", "S", "SW", "NW" };

  int work;

  for (work = strlen (dir_string) - 1; work > -1; work --)
    if (dir_string [work] == ' ')
      dir_string [work] = NULL;
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
