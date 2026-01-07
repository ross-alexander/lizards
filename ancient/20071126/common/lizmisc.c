#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "lizards.h"
#include "lizmisc.h"
#include "lizhex.h"

void open_player_mesg (world_t *w, int player, FILE **fptr)
{
  fprintf(stderr, "open_player_mesg called.\n");
  exit(1);
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
      string [0] = ('A' + x);
      string [1] = '\0';
    }
  else
    {
      work = (x / 26);
      string [0] = ('A' + (work - 1));
      string [1] = ('A' + (x - (work * 26)));
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
  if (!dir_string)
    return 0;
  for (work = strlen (dir_string) - 1; work >= 0; work--)
    if (dir_string [work] == ' ')
      dir_string [work] = '\0';
    else
      break;

  for (work = 0; work < 6; work ++)
    if (strcmp (directions [work], dir_string) == 0)
      return (work + 1);
  return 0;
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
