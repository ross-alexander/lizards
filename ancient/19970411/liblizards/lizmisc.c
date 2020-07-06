#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "lizard.h"
#include "map.h"

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
	   PLAYER_MESG_F, player, world.turn);

  if ((*fptr = fopen (filename, "a")) == NULL)
    *fptr = fopen (filename, "w");
}

void send_player_mesg (int player, char *format, ...)
{
  FILE *player_fptr;

  char filename [80];

  sprintf (filename, "%s%s.%s%02d_%03d", data_path, game_code,
	   PLYERR_MESG_F, player, world.turn);

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

UnitList BuildUnitList (World *world, int player)
{
  int work2, work3, work4, x, y;
  int i, cnt;
  UnitList ulist;
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
  for (i = 0; i < number_spys; i++)
    if (spy_list[i]->player == player)
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

  for (i = 0; i < number_spys; i++)
    {
      if (spy_list[i]->player > 0)
	{
	  work2 = spy_list[i]->player;
	  work4 = 0;
	  for (work3 = 0; work3 < ulist.cnt; work3++)
	    if ((ulist.list[work3].x == spy_list[i]->x_hex) &&
		(ulist.list[work3].y == spy_list[i]->y_hex))
	      {
		work4 = 1;
		break;
	      }

	  if (work4 == 0)
	    {
	      work_unit = &ulist.list[ulist.cnt];
	      work_unit->x = spy_list[i]->x_hex;
	      work_unit->y = spy_list[i]->y_hex;
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

void see_wrap_hex (int wrap, World *world, int *x, int *y);
void see_wrap_hex (int wrap, World *world, int *x, int *y)
{
  if (wrap)
  {
    if (*x < 0)
      *x += world->x;
    else
      if (*x >= world->x)
	*x -= world->x;

    if (*y < 0)
      *y += world->y;
    else
      if (*y >= world->y)
	*y -= world->y;
  }
}

int build_see_list_sort (const void *elem1, const void *elem2);
int build_see_list_sort (const void *elem1, const void *elem2)
{
  if (((see_t *)elem1)->y < ((see_t *)elem2)->y)
    return (-1);
  else
    if (((see_t *)elem1)->y > ((see_t *)elem2)->y)
      return (1);
    else
      return (0);
}

HexList *BuildHexList(int player, UnitList ulist, int wrap, World *world)
{
  /* in y,x order! */

  int one_hex_even[6][2] = {{-1, 0}, {-1, -1},{-1, 1},{0, -1}, {0, 1},{ 1, 0}};
  int one_hex_odd[6][2] = {{-1, 0},{ 0, -1}, {0, 1}, {1, -1},{ 1, 1}, {1, 0}};
  int two_hex_even[12][2] = {{-2, 0},{-2,-1},{-2, 1},{-1,-2},{-1, 2}, {0,-2},
			     {0, 2}, {1, -2},{1, 2}, {1, -1},{ 1, 1},{ 2, 0}};
  int two_hex_odd[12][2] = {{-2, 0},{-1, -1},{-1, 1},{-1, -2},{ -1, 2},{0, -2},
			    { 0, 2},{1, -2},{  1, 2,},{2, -1},{ 2, 1},{ 2, 0}};
  int three_hex_even[18][2] = {{-3,0},{-3,-1},{-3, 1},{-2,-2},{-2,2},{-2,-3},
			       {-2,3},{-1,-3},{-1,3},{0,-3},{0,3},{1,-3},
			       {1,3},{2,-2},{2,2},{2,-1},{2,1},{3,0}};
  int three_hex_odd[18][2] = {{-3,0},{-2,-1},{-2,1},{-2,-2},{-2,2},{-1,-3},
			      {-1,3},{0,-3},{0,3},{1,-3},{1,3},{2,-3},
			      {2,3},{2,-2},{2,2},{3,-1},{3, 1},{3, 0}};

  int work, work2;

  HexList *hexlist;

  see_t *see_ptr, work_see;

  unsigned int total_see_hex, see_hex;

  total_see_hex = 0;
  see_hex = 0;

  for (work = 0; work < ulist.cnt; work++)
    if (ulist.list[work].peak)
      total_see_hex += 37;
    else
      total_see_hex += 19;

  if ((see_ptr = malloc (sizeof (see_t) * total_see_hex)) == NULL)
    {
      printf ("FATAL ERROR: Out of memory.\n");
      exit (EXIT_FAILURE);
    }

  for (work = 0; work < ulist.cnt; work++)
    {
      work_see.x = ulist.list[work].x;
      work_see.y = ulist.list[work].y;

      see_ptr[see_hex] = work_see;

      see_hex++;

      if (ulist.list[work].peak)
	{
	  if (ulist.list[work].x % 2 == 0)
	    {
	      for (work2 = 0; work2 < 6; work2++)
		{
		  work_see.x = ulist.list[work].x +
		    one_hex_odd[work2][1];
		  work_see.y = ulist.list[work].y +
		    one_hex_odd[work2][0];

		  see_wrap_hex (wrap, world, &work_see.x, &work_see.y);

		  if (work_see.x > -1 && work_see.x < world->x &&
		      work_see.y > -1 && work_see.y < world->y)
		    {
		      see_ptr[see_hex] = work_see;
		      see_hex++;
		    }
		}

	      for (work2 = 0; work2 < 12; work2++)
		{
		  work_see.x = ulist.list[work].x +
		    two_hex_odd[work2][1];
		  work_see.y = ulist.list[work].y +
		    two_hex_odd[work2][0];

		  see_wrap_hex (wrap, world, &work_see.x, &work_see.y);

		  if (work_see.x > -1 && work_see.x < world->x &&
		      work_see.y > -1 && work_see.y < world->y)
		    {
		      see_ptr[see_hex] = work_see;
		      see_hex++;
		    }
		}

	      for (work2 = 0; work2 < 18; work2++)
		{
		  work_see.x = ulist.list[work].x +
		    three_hex_odd[work2][1];
		  work_see.y = ulist.list[work].y +
		    three_hex_odd[work2][0];

		  see_wrap_hex (wrap, world, &work_see.x, &work_see.y);

		  if (work_see.x > -1 && work_see.x < world->x &&
		      work_see.y > -1 && work_see.y < world->y)
		    {
		      see_ptr[see_hex] = work_see;
		      see_hex++;
		    }
		}
	    }
	  else
	    {
	      for (work2 = 0; work2 < 6; work2++)
		{
		  work_see.x = ulist.list[work].x +
		    one_hex_even[work2][1];
		  work_see.y = ulist.list[work].y +
		    one_hex_even[work2][0];

		  see_wrap_hex (wrap, world, &work_see.x, &work_see.y);

		  if (work_see.x > -1 && work_see.x < world->x &&
		      work_see.y > -1 && work_see.y < world->y)
		    {
		      see_ptr[see_hex] = work_see;
		      see_hex++;
		    }
		}

	      for (work2 = 0; work2 < 12; work2++)
		{
		  work_see.x = ulist.list[work].x +
		    two_hex_even[work2][1];
		  work_see.y = ulist.list[work].y +
		    two_hex_even[work2][0];

		  see_wrap_hex (wrap, world, &work_see.x, &work_see.y);

		  if (work_see.x > -1 && work_see.x < world->x &&
		      work_see.y > -1 && work_see.y < world->y)
		    {
		      see_ptr[see_hex] = work_see;
		      see_hex++;
		    }
		}

	      for (work2 = 0; work2 < 18; work2++)
		{
		  work_see.x = ulist.list[work].x +
		    three_hex_even[work2][1];
		  work_see.y = ulist.list[work].y +
		    three_hex_even[work2][0];

		  see_wrap_hex (wrap, world, &work_see.x, &work_see.y);

		  if (work_see.x > -1 && work_see.x < world->x &&
		      work_see.y > -1 && work_see.y < world->y)
		    {
		      see_ptr[see_hex] = work_see;
		      see_hex++;
		    }
		}
	    }
	}
      else
	{
	  if (ulist.list[work].x % 2 == 0)
	    {
	      for (work2 = 0; work2 < 6; work2++)
		{
		  work_see.x = ulist.list[work].x +
		    one_hex_odd[work2][1];
		  work_see.y = ulist.list[work].y +
		    one_hex_odd[work2][0];

		  see_wrap_hex (wrap, world, &work_see.x, &work_see.y);

		  if (work_see.x > -1 && work_see.x < world->x &&
		      work_see.y > -1 && work_see.y < world->y)
		    {
		      see_ptr[see_hex] = work_see;
		      see_hex++;
		    }
		}

	      for (work2 = 0; work2 < 12; work2++)
		{
		  work_see.x = ulist.list[work].x +
		    two_hex_odd[work2][1];
		  work_see.y = ulist.list[work].y +
		    two_hex_odd[work2][0];

		  see_wrap_hex (wrap, world, &work_see.x, &work_see.y);

		  if (work_see.x > -1 && work_see.x < world->x &&
		      work_see.y > -1 && work_see.y < world->y)
		    {
		      see_ptr[see_hex] = work_see;
		      see_hex++;
		    }
		}
	    }
	  else
	    {
	      for (work2 = 0; work2 < 6; work2++)
		{
		  work_see.x = ulist.list[work].x +
		    one_hex_even[work2][1];
		  work_see.y = ulist.list[work].y +
		    one_hex_even[work2][0];

		  see_wrap_hex (wrap, world, &work_see.x, &work_see.y);

		  if (work_see.x > -1 && work_see.x < world->x &&
		      work_see.y > -1 && work_see.y < world->y)
		    {
		      see_ptr[see_hex] = work_see;
		      see_hex++;
		    }
		}

	      for (work2 = 0; work2 < 12; work2++)
		{
		  work_see.x = ulist.list[work].x +
		    two_hex_even[work2][1];
		  work_see.y = ulist.list[work].y +
		    two_hex_even[work2][0];

		  see_wrap_hex (wrap, world, &work_see.x, &work_see.y);

		  if (work_see.x > -1 && work_see.x < world->x &&
		      work_see.y > -1 && work_see.y < world->y)
		    {
		      see_ptr[see_hex] = work_see;
		      see_hex++;
		    }
		}
	    }
	}
    }

  if (!(hexlist = (HexList*)malloc(sizeof(HexList))))
    {
      fprintf(stderr, "malloc failed in line __LINE__ of file __FILE__\n");
      exit(EXIT_FAILURE);
    }
  hexlist->cnt = see_hex;

  for (work = 0; work < see_hex; work++)
    for (work2 = 0; work2 < see_hex; work2++)
      if (see_ptr[work].x == see_ptr[work2].x &&
	  see_ptr[work].y == see_ptr[work2].y &&
	  work != work2)
	{
	  see_ptr[work2].x = 127;
	  see_ptr[work2].y = 127;
	}

  qsort (see_ptr, see_hex, sizeof (see_t), build_see_list_sort);

  for (work = 0; work < see_hex; work++)
    if (see_ptr[work].y == 127)
      {
	hexlist->cnt = work;
	break;
      }
  hexlist->list = see_ptr;
  return hexlist;
}
