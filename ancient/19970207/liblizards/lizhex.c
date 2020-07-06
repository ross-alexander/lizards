#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "lizard.h"
#include "map.h"
#include "order.h"

#include "filefunc.h"
#include "lizhex.h"
#include "lizmisc.h"

extern int world_x,
	   world_y,
	   number_orders,
	   turn_number;

extern spy_t *spy_list [MAX_SPYS];
extern int number_spys;

extern unsigned int spawn_turn_switch,
		    capture_switch,
		    volcano_dormant_switch,
		    peak_active_switch,
		    world_wrap_switch;

FILE *lizards_world_fptr = NULL;

void move_lizards (int old_x, int old_y, int new_x, int new_y, int red,
		   int green, int grey, int black, int yellow)
{
  hex_t new_hex,
	old_hex;

  get_hex (old_x, old_y, &old_hex);
  get_hex (new_x, new_y, &new_hex);

  if (lizards_in_hex (&new_hex) > 0)
  {
    new_hex.hex_owner = old_hex.hex_owner;

    new_hex.combat_points =
      average_combat_points (red + green + grey + yellow + black,
        old_hex.combat_points, lizards_in_hex (&new_hex),
        new_hex.combat_points);

    new_hex.turns_hungry =
      average_hunger_points (red + green + grey + yellow + black,
        old_hex.turns_hungry, lizards_in_hex (&new_hex),
        new_hex.turns_hungry);

    new_hex.red_lizards += red;
    old_hex.red_lizards -= red;

    new_hex.green_lizards += green;
    old_hex.green_lizards -= green;

    new_hex.grey_lizards += grey;
    old_hex.grey_lizards -= grey;

    new_hex.black_lizards += black;
    old_hex.black_lizards -= black;

    new_hex.yellow_lizards += yellow;
    old_hex.yellow_lizards -= yellow;
  } else {
    new_hex.hex_owner = old_hex.hex_owner;

    new_hex.combat_points = old_hex.combat_points;
    new_hex.turns_hungry = old_hex.turns_hungry;

    new_hex.red_lizards += red;
    old_hex.red_lizards -= red;

    new_hex.green_lizards += green;
    old_hex.green_lizards -= green;

    new_hex.grey_lizards += grey;
    old_hex.grey_lizards -= grey;

    new_hex.black_lizards += black;
    old_hex.black_lizards -= black;

    new_hex.yellow_lizards += yellow;
    old_hex.yellow_lizards -= yellow;
  }

  put_hex (old_x, old_y, &old_hex);
  put_hex (new_x, new_y, &new_hex);
}

int combat (int red, int green, int grey, int black, int yellow,
	    int *new_red, int *new_green, int *new_grey, int *new_black,
	    int *new_yellow, int attack_x, int attack_y, int defend_x,
	    int defend_y, int flee_flag)
{
  FILE *fptr1,
       *fptr2;

  float attack_percent,
	defend_percent;

  int attacking_deaths,
      defending_deaths,
      attack_hit,
      defend_hit,
      round,
      work,
      lizards;

  hex_t attack_hex,
	defend_hex;

  round = 0;

  *new_red = red;
  *new_green = green;
  *new_grey = grey;
  *new_black = black;
  *new_yellow = yellow;

  get_hex (attack_x, attack_y, &attack_hex);
  get_hex (defend_x, defend_y, &defend_hex);

  open_player_mesg (attack_hex.hex_owner, &fptr1);

  if (defend_hex.hex_owner > 0)
    open_player_mesg (defend_hex.hex_owner, &fptr2);
  else
    fptr2 = NULL;

  if (defend_hex.terrain == DEN || defend_hex.terrain == HOME_DEN)
    attack_hit = DEN_ATTACK;
  else
    attack_hit = BASE_ATTACK;

  defend_hit = BASE_ATTACK;

  attack_hit += (attack_hex.combat_points * CP_MOD_LEVEL) -
		(defend_hex.combat_points * CP_MOD_LEVEL);

  do
  {
    defending_deaths = 0;
    attacking_deaths = 0;

    lizards = (*new_red + *new_green + *new_grey + *new_black + *new_yellow) / 2;

    if (lizards == 0)
      lizards = 1;

    for (work = 0; work < lizards; work ++)
      if (random (100) <= attack_hit &&
	  defending_deaths < lizards_in_hex (&defend_hex))
	defending_deaths ++;

    lizards = (lizards_in_hex (&defend_hex)) / 2;

    if (lizards == 0)
      lizards = 1;

    for (work = 0; work < lizards; work ++)
      if (random (100) <= defend_hit &&
	  attacking_deaths < (*new_red + *new_green + *new_grey + *new_black + *new_yellow))
	attacking_deaths ++;

    round ++;

    kill_lizards (*new_red, *new_green, *new_grey, *new_black, *new_yellow,
		  new_red, new_green, new_grey, new_black, new_yellow,
		  attacking_deaths);

    kill_lizards_xy (&defend_hex, defending_deaths);

    if ((*new_red + *new_green + *new_grey + *new_black + *new_yellow) > 0 &&
	lizards_in_hex (&defend_hex) > 0)
    {
      attack_percent = 100 * ((float) lizards_in_hex (&defend_hex) /
			  (float) (*new_red + *new_green + *new_grey +
			  *new_black + *new_yellow));
      defend_percent = 100 * ((float) (*new_red + *new_green + *new_grey + *new_black + *new_yellow)
		   / (float) lizards_in_hex (&defend_hex));

      if (attack_percent > 150 && flee_flag != -1)
      {
	attack_percent -= 125;

	if (random (100) <= attack_percent)
	{
	  fprintf (fptr1, " - Combat round %d! ", round);

	  fprintf (fptr1, "Standing Lizards: %d. Standing enemy Lizards: %d.\n",
	     (*new_red + *new_green + *new_grey + *new_black + *new_yellow),
	     lizards_in_hex (&defend_hex));

	  if (fptr2 != NULL)
	  {
	    fprintf (fptr2, " - Combat round %d! ", round);

	    fprintf (fptr2, "Standing Lizards: %d. Standing enemy Lizards: %d.\n",
		     lizards_in_hex (&defend_hex), (*new_red + *new_green + *new_grey +
		     *new_black + *new_yellow));

	    fprintf (fptr2, " - Attacking Lizards flee!\n");

	    fclose (fptr2);
	  }

	  fclose (fptr1);

	  put_hex (attack_x, attack_y, &attack_hex);
	  put_hex (defend_x, defend_y, &defend_hex);

	  return (-1);
	}
      }

      if (defend_hex.terrain != DEN && defend_hex.terrain != HOME_DEN &&
	  defend_percent > 150 &&
	  flee_flag == 1)
      {
	defend_percent -= 125;

	if (random (100) <= defend_percent)
	{
	  fprintf (fptr1, " - Combat round %d! ", round);

	  fprintf (fptr1, "Standing Lizards: %d. Standing enemy Lizards: %d.\n",
	     (*new_red + *new_green + *new_grey + *new_black + *new_yellow),
	     lizards_in_hex (&defend_hex));

	  fprintf (fptr1, " - Enemy Lizards attempt to flee!\n");

	  if (fptr2 != NULL)
	  {
	    fprintf (fptr2, " - Combat round %d! ", round);

	    fprintf (fptr2, "Standing Lizards: %d. Standing enemy Lizards: %d.\n",
		     lizards_in_hex (&defend_hex),
		     (*new_red + *new_green + *new_grey + *new_black + *new_yellow));

	    fclose (fptr2);
	  }

	  fclose (fptr1);

	  put_hex (attack_x, attack_y, &attack_hex);
	  put_hex (defend_x, defend_y, &defend_hex);

	  return (1);
	}
      }
    }
  } while ((*new_red + *new_green + *new_grey + *new_black + *new_yellow) > 0 &&
	   lizards_in_hex (&defend_hex) > 0);

  fprintf (fptr1, " - Combat round %d! ", round);

   fprintf (fptr1, "Standing Lizards: %d. Standing enemy Lizards: %d.\n",
	    (*new_red + *new_green + *new_grey + *new_black + *new_yellow),
	    lizards_in_hex (&defend_hex));

  fclose (fptr1);

  if (fptr2 != NULL)
  {
    fprintf (fptr2, " - Combat round %d! ", round);

    fprintf (fptr2, "Standing Lizards: %d. Standing enemy Lizards: %d.\n",
	     lizards_in_hex (&defend_hex),
	     (*new_red + *new_green + *new_grey + *new_black + *new_yellow));

    fclose (fptr2);
  }

  put_hex (attack_x, attack_y, &attack_hex);
  put_hex (defend_x, defend_y, &defend_hex);

  return (0);
}

void capture_lizards (hex_t *hex, int no_captured, int *red, int *green, int *grey,
		      int *black, int *yellow, int *red_captured, int *green_captured,
		      int *grey_captured, int *black_captured, int *yellow_captured)
{
  int work,
      work2,
      work3,
      work4;

  work = no_captured;

  *red_captured = 0;
  *green_captured = 0;
  *grey_captured = 0;
  *black_captured = 0;
  *yellow_captured = 0;

  if (capture_switch == 0)
    return;

  while (work > 0 && lizards_in_hex (hex) < MAX_LIZARDS_IN_HEX)
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
	  work4 = *red;

	  break;
	case GREEN_LIZARD:
	  work4 = *green;

	  break;
	case GREY_LIZARD:
	  work4 = *grey;

	  break;
	case BLACK_LIZARD:
	  work4 = *black;

	  break;
	case YELLOW_LIZARD:
	  work4 = *yellow;

	  break;
      }
    } while (work4 == 0);

    if (work2 > work4)
      work2 = work4;

    if (lizards_in_hex (hex) + work2 > MAX_LIZARDS_IN_HEX)
      work2 = MAX_LIZARDS_IN_HEX - lizards_in_hex (hex);

    switch (work3)
    {
      case RED_LIZARD:
	*red -= work2;
	*red_captured += work2;
	hex->red_lizards += work2;

	break;
      case GREEN_LIZARD:
	*green -= work2;
	*green_captured += work2;
	hex->green_lizards += work2;

	break;
      case GREY_LIZARD:
	*grey -= work2;
	*grey_captured += work2;
	hex->grey_lizards += work2;

	break;
      case BLACK_LIZARD:
	*black -= work2;
	*black_captured += work2;
	hex->black_lizards += work2;

	break;
      case YELLOW_LIZARD:
	*yellow -= work2;
	*yellow_captured += work2;
	hex->yellow_lizards += work2;
    }

    work -= work2;
  }
}

void combat_sea_monster (int red, int green, int grey, int black, int yellow,
			int *new_red, int *new_green, int *new_grey,
			int *new_black, int *new_yellow, int attack_x,
			int attack_y, int *sea_monster)
{
  FILE *fptr;

  int attacking_deaths,
      defending_deaths,
      attack_hit,
      defend_hit,
      round,
      work,
      lizards;

  hex_t world_hex;

  round = 0;

  *new_red = red;
  *new_green = green;
  *new_grey = grey;
  *new_black = black;
  *new_yellow = yellow;

  get_hex (attack_x, attack_y, &world_hex);

  open_player_mesg (world_hex.hex_owner, &fptr);

  attack_hit = BASE_ATTACK + (world_hex.combat_points * CP_MOD_LEVEL);

  defend_hit = MONSTER_DEFEND;

  do
  {
    defending_deaths = 0;
    attacking_deaths = 0;

    lizards = (*new_red + *new_green + *new_grey + *new_black + *new_yellow) / 2;

    if (lizards == 0)
      lizards = 1;

    for (work = 0; work < lizards; work ++)
      if (random (100) <= attack_hit &&
	  defending_deaths < *sea_monster)
	defending_deaths ++;

    for (work = 0; work < (*sea_monster / 2); work ++)
      if (random (100) >= defend_hit &&
	  attacking_deaths < (*new_red + *new_green + *new_grey + *new_black + *new_yellow))
	attacking_deaths ++;

    round ++;

    kill_lizards (*new_red, *new_green, *new_grey, *new_black, *new_yellow,
		  new_red, new_green, new_grey, new_black, new_yellow,
		  attacking_deaths);

    *sea_monster -= defending_deaths;
  } while ((*new_red + *new_green + *new_grey + *new_black + *new_yellow) > 0 &&
	   *sea_monster > 0);

  fprintf (fptr, " - Combat round %d! ", round);

  fprintf (fptr, "Surviving Lizards: %d.\n",
	   (*new_red + *new_green + *new_grey + *new_black + *new_yellow));

  fclose (fptr);
}

int terrain_adjacent (int x, int y, int terrain)
{
  int work,
      work2,
      work_x,
      work_y;

  hex_t hex;

  for (work = 1, work2 = 0; work < 7; work ++)
  {
    move_coords (work, x, y, &work_x, &work_y);

    if (x != work_x || y != work_y)
    {
      get_hex (work_x, work_y, &hex);

      if (hex.terrain == terrain)
	work2 ++;
    }
  }

  return (work2);
}

void move_coords (int direction, int x, int y, int *new_x, int *new_y)
{
  *new_x = x;
  *new_y = y;

  switch (direction)
  {
    case NORTH:
      *new_y -= 1;

      break;
    case NORTH_EAST:
      *new_x += 1;

      if ((x % 2) == 1)
	*new_y -= 1;

      break;
    case SOUTH_EAST:
      *new_x += 1;

      if ((x % 2) != 1)
	*new_y += 1;

      break;
    case SOUTH:
      *new_y += 1;

      break;
    case SOUTH_WEST:
      *new_x -= 1;

      if ((x % 2) != 1)
	*new_y += 1;

      break;
    case NORTH_WEST:
      *new_x -= 1;

      if ((x % 2) == 1)
	*new_y -= 1;

      break;
  }

  if (world_wrap_switch)
  {
    if (*new_x < 0)
      *new_x += world_x;
    else
      if (*new_x >= world_x)
	*new_x -= world_x;

    if (*new_y < 0)
      *new_y += world_y;
    else
      if (*new_y >= world_y)
	*new_y -= world_y;
  } else {
    if (*new_x < 0 || *new_y < 0 ||
	*new_x >= world_x || *new_y >= world_y)
    {
      *new_x = x;
      *new_y = y;
    }
  }
}

/* ----------------------------------------------------------------------
--
-- in_sight
--
---------------------------------------------------------------------- */

void in_sight (int x, int y, int *players)
{
  /* In y,x order! */

  int one_hex_odd  [6][2] = {{-1,0},{-1,-1},{-1,1},{0,-1},{0,1},{1,0}};
  int one_hex_even [6][2] = {{-1,0},{0,-1},{0,1},{1,-1},{1,1},{1,0}};
  int two_hex_odd  [12][2] = {{-2,0},{-2,-1},{-2,1},{-1,-2},{-1,2},{0,-2},
			      {0,2},{1,-2},{1,2},{1,-1},{1,1},{2,0}};
  int two_hex_even [12][2] = {{-2,0},{-1,-1},{-1,1},{-1,-2},{-1,2},{0,-2},
			      {0,2},{1,-2},{1,2},{2,-1},{2,1},{2,0}};

  int three_hex_odd [18][2] = {{-3,0},{-3,-1},{-3,1},{-2,-2},{-2,2},{-2,-3},
			       {-2,3},{-1,-3},{-1,3},{0,-3},{0,3},{1,-3},
			       {1,3},{2,-2},{2,2},{2,-1},{2,1},{3,0}};

  int three_hex_even [18][2] = {{-3,0},{-2,-1},{-2,1},{-2,-2},{-2,2},{-1,-3},
				{-1,3},{0,-3},{0,3},{1,-3},{1,3},{2,-3},
				{2,3},{2,-2},{2,2},{3,-1},{3,1},{3,0}};

  int work;
  hex_t hex;
  for (work = 0; work < MAX_PLAYERS + 1; work ++)
    players [work] = 0;

  if ((x % 2) == 1)
  {
    players_here (x, y, players);

    for (work = 0; work < 6; work ++)
      players_here ((x + one_hex_odd [work][1]), (y + one_hex_odd [work][0]),
		    players);

    for (work = 0; work < 12; work ++)
      players_here ((x + two_hex_odd [work][1]), (y + two_hex_odd [work][0]),
		    players);

    for (work = 0; work < 18; work ++)
    {
      get_hex ((x + three_hex_odd [work][1]), (y + three_hex_odd [work][0]), &hex);

      if (hex.terrain == PEAK)
	players_here ((x + three_hex_odd [work][1]), (y + three_hex_odd [work][0]),
		      players);
    }
  } else {
    players_here (x, y, players);

    for (work = 0; work < 6; work ++)
      players_here ((x + one_hex_even [work][1]), (y + one_hex_even [work][0]),
		    players);

    for (work = 0; work < 12; work ++)
      players_here ((x + two_hex_even [work][1]), (y + two_hex_even [work][0]),
		    players);

    for (work = 0; work < 18; work ++)
    {
      get_hex ((x + three_hex_even [work][1]), (y + three_hex_even [work][0]), &hex);

      if (hex.terrain == PEAK)
	players_here ((x + three_hex_even [work][1]), (y + three_hex_even [work][0]),
		      players);
    }
  }
}

void players_here (int x, int y, int *players)
{
  int work;

  hex_t hex;

  if (x < 0 || y < 0 || x >= world_x || y >= world_y)
    return;

  get_hex (x, y, &hex);

  work = hex.red_lizards + hex.green_lizards +
	 hex.grey_lizards + hex.black_lizards +
	 hex.yellow_lizards;

  if (work > 0)
    players [hex.hex_owner] = 1;

  for (work = 0; work < number_spys; work ++)
    if (spy_list [work]->x_hex == x && spy_list [work]->y_hex == y &&
	spy_list [work]->player > 0)
      players [spy_list [work]->player] = 1;
}

/* ----------------------------------------------------------------------
--
-- get_hex
--
---------------------------------------------------------------------- */
void get_hex (int x, int y, hex_t *hex)
{
  char filename [255];

  if (lizards_world_fptr == NULL)
  {
    sprintf (filename, "%s%s%s", data_path, WORLD_DATA_F, game_code);

    if ((lizards_world_fptr = fopen (filename, "r+b")) == NULL)
    {
      printf ("FATAL ERROR: Unable to read/write %s file. Press SPACE.\n", filename);
      exit (EXIT_FAILURE);
    }
  }

  get_rec (lizards_world_fptr, sizeof (hex_t), (y * world_x) + x, hex);
}

void put_hex (int x, int y, hex_t *hex)
{
  char filename [255];

  if (lizards_world_fptr == NULL)
  {
    sprintf (filename, "%s%s%s", data_path, WORLD_DATA_F, game_code);

    if ((lizards_world_fptr = fopen (filename, "r+b")) == NULL)
    {
      printf ("FATAL ERROR: Unable to read/write %s file. Press SPACE.\n", filename);
      exit (EXIT_FAILURE);
    }
  }

  put_rec (lizards_world_fptr, sizeof (hex_t), (y * world_x) + x, hex);
}

void whirlpool_kill (int x, int y)
{
  FILE *fptr;

  char work_string [80],
       temp_string [80];

  int work;

  hex_t hex;

  get_hex (x, y, &hex);

  if (lizards_in_hex (&hex) > 0)
  {
    xy_to_string (x, y, work_string);

    open_player_mesg (hex.hex_owner, &fptr);

    work = (float) lizards_in_hex (&hex) * WHIRLPOOL_KILL_LEVEL;

    if (work < 1)
      work = 1;

    fprintf (fptr, " - %d Lizards at %s are sucked down to their deaths in Whirlpool!\n",
		   work, work_string);

    kill_lizards_xy (&hex, work);

    put_hex (x, y, &hex);

    if (lizards_in_hex (&hex) == 0)
    {
      fprintf (fptr, " - NO Lizards resist the Whirlpool!\n");

      fclose (fptr);
    } else
      fprintf (fptr, " - %s Lizards at %s resist the Whirlpool!\n",
	       show_lizards (hex.red_lizards,
	       hex.green_lizards, hex.grey_lizards,
	       hex.black_lizards, hex.yellow_lizards,
	      temp_string), work_string);

    fclose (fptr);
  }

  for (work = 0; work < number_spys; work ++)
    if (spy_list [work]->x_hex == x && spy_list [work]->y_hex == y &&
	spy_list [work]->player > 0)
    {
      open_player_mesg (spy_list [work]->player, &fptr);

      xy_to_string (x, y, work_string);

      fprintf (fptr, " - Spy Lizard at %s sucked down it's death in Whirlpool!\n",
	       work_string);

      fclose (fptr);

      spy_list [work]->player = 0;
    }
}

void bridge_collapse_kill (int x, int y)
{
  FILE *fptr;

  char work_string [80],
       temp_string [80];

  int work;

  hex_t hex;

  get_hex (x, y, &hex);

  if (lizards_in_hex (&hex) > 0)
  {
    xy_to_string (x, y, work_string);

    open_player_mesg (hex.hex_owner, &fptr);

    work = (float) lizards_in_hex (&hex) * COLLAPSE_KILL_LEVEL;

    if (work > 1)
      work = random (work) + 1;
    else
      work = 1;

    fprintf (fptr, " - %d Lizards at %s are killed by falling wreckage!\n",
		   work, work_string);

    kill_lizards_xy (&hex, work);

    put_hex (x, y, &hex);

    if (lizards_in_hex (&hex) == 0)
    {
      fprintf (fptr, " - NO Lizards survive the fall!\n");

      fclose (fptr);
    } else
      fprintf (fptr, " - %s Lizards at %s survived the fall.\n",
	       show_lizards (hex.red_lizards,
	       hex.green_lizards, hex.grey_lizards,
	       hex.black_lizards, hex.yellow_lizards,
	      temp_string), work_string);

    fclose (fptr);
  }

  for (work = 0; work < number_spys; work ++)
    if (spy_list [work]->x_hex == x && spy_list [work]->y_hex == y &&
	spy_list [work]->player > 0)
    {
      open_player_mesg (spy_list [work]->player, &fptr);

      xy_to_string (x, y, work_string);

      fprintf (fptr, " - Spy Lizard at %s killed by falling wreckage!\n",
	       work_string);

      fclose (fptr);

      spy_list [work]->player = 0;
    }
}

int player_spy_here (int x, int y, int player)
{
  int work;

  for (work = 0; work < number_spys; work ++)
    if (spy_list [work]->x_hex == x && spy_list [work]->y_hex == y &&
	spy_list [work]->player == player)
      return (work);

  return (-1);
}

int moveable_player_spy_here (int x, int y, int player)
{
  int work,
      spys [20],
      num_spys;

  for (work = 0, num_spys = 0; work < number_spys; work ++)
    if (spy_list [work]->x_hex == x && spy_list [work]->y_hex == y &&
	spy_list [work]->player == player)
    {
      spys [num_spys] = work;

      num_spys ++;
    }

  for (work = 0; work < num_spys; work ++)
    if (spy_list [spys [work]]->mired == 0 &&
	spy_list [spys [work]]->moved_this_turn == 0)
	  return (spys [work]);

  if (num_spys > 0)
    return (-2);
  else
    return (-1);
}

void inc_combat_points (hex_t *hex)
{
  if (hex->combat_points < MAX_COMBAT_POINTS)
    hex->combat_points += 1;
}

int spy_lizards_in_hex (int x, int y, int player)
{
  int work,
      work2;

  for (work = 0, work2 = 0; work < number_spys; work ++)
    if (spy_list [work]->x_hex == x && spy_list [work]->y_hex == y &&
	spy_list [work]->player == player)
    {
      work2 ++;
    }

  return (work2);
}
