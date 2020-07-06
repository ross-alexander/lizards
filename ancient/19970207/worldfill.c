#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "lizard.h"
#include "map.h"
#include "order.h"

#include "lizgame.h"
#include "lizhex.h"
#include "lizmisc.h"

int number_land_hexes;

unsigned int spawn_turn_switch = 0,
	     capture_switch = 0,
	     volcano_dormant_switch = 0,
	     peak_active_switch = 0,
	     world_wrap_switch = 0,
             victory_points_switch = 0,
             home_owned_worth = 0,
	     home_victory_points = 0,
	     give_switch = 0;

int main (int argc, char *argv []);
void lay_island (int x, int y);

int main (int argc, char *argv [])
{
  FILE *fptr;

  char filename [255];

  int x,
      y,
      work,
      work2,
      work3,
      work4,
      x2,
      y2,
      work_x,
      work_y,
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
      islands_on_f = 1;

  float den_p = 0.18,
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
	plain_p = 0.0;

  hex_t world_hex;

  randomize ();

  show_title ();

  printf ("LIZARDS! World Fill Utility. %s Last compiled %s.\n\n",
          VER_STRING, __DATE__);

  if (argc >= 3)
  {
    data_path = LzCheckDir(argv[1], 0);
    game_code = (char*)malloc(strlen(argv[2] + 1));
    strcpy (game_code, argv [2]);
    strupr (game_code);
  } else {
    printf ("FATAL ERROR: Game Number not specified on command line!\n"
	     "             Command Line Format: EXEFILE data_path game.\n"
	     "                             e.g. WORLFILL C:\\ A\n");

    exit (EXIT_FAILURE);
  }

  sprintf (filename, "%sWORLDFIL.PAR", data_path);

  if ((fptr = fopen (filename, "rt")) != NULL)
  {
    printf ("COMMAND LINE SWITCHES IGNORED.\nPARAMETER FILE LOADED.\n");

    fscanf (fptr, "%f", &den_p);
    fscanf (fptr, "%f", &red_den_p);
    fscanf (fptr, "%f", &green_den_p);
    fscanf (fptr, "%f", &grey_den_p);
    fscanf (fptr, "%f", &black_den_p);
    fscanf (fptr, "%f", &yellow_den_p);
    fscanf (fptr, "%f", &ruin_p);
    fscanf (fptr, "%f", &swamp_p);
    fscanf (fptr, "%f", &fertile_p);
    fscanf (fptr, "%f", &peak_p);
    fscanf (fptr, "%f", &volcano_p);
    fscanf (fptr, "%f", &whirlpool_p);
    fscanf (fptr, "%f", &scrub_p);
    fscanf (fptr, "%f", &temple_p);
    fscanf (fptr, "%f", &cursed_p);
    fscanf (fptr, "%f", &plain_p);
    fscanf (fptr, "%d", &islands_on_f);

    fclose (fptr);
    unlink(filename);
  }
  else
    {
    for (work = 4; work < argc + 1; work ++)
    {
      switch (toupper (argv [work - 1][0]))
      {
	case 'D':
	  den_p = atof (&argv [work - 1][1]);

	  printf ("DEN.........%s percent.\n", &argv [work - 1][1]);

	  break;
	case '1':
	  red_den_p = atof (&argv [work - 1][1]);

	  printf ("RED DEN.....%s percent.\n", &argv [work - 1][1]);

	  break;
	case '2':
	  green_den_p = atof (&argv [work - 1][1]);

	  printf ("GREEN DEN...%s percent.\n", &argv [work - 1][1]);

	  break;
	case '3':
	  grey_den_p = atof (&argv [work - 1][1]);

	  printf ("GREY DEN....%s percent.\n", &argv [work - 1][1]);

	  break;
	case '4':
	  black_den_p = atof (&argv [work - 1][1]);

	  printf ("BLACK DEN...%s percent.\n", &argv [work - 1][1]);

	  break;
	case '5':
	  yellow_den_p = atof (&argv [work - 1][1]);

	  printf ("YELLOW DEN..%s percent.\n", &argv [work - 1][1]);

	  break;
	case 'R':
	  ruin_p = atof (&argv [work - 1][1]);

	  printf ("RUIN........%s percent.\n", &argv [work - 1][1]);

	  break;
	case 'S':
	  swamp_p = atof (&argv [work - 1][1]);

	  printf ("SWAMP.......%s percent.\n", &argv [work - 1][1]);

	  break;
	case 'F':
	  fertile_p = atof (&argv [work - 1][1]);

	  printf ("FERTILE.......%s percent.\n", &argv [work - 1][1]);

	  break;
	case 'P':
	  plain_p = atof (&argv [work - 1][1]);

	  printf ("PLAIN.......%s percent.\n", &argv [work - 1][1]);

	  break;
	case 'K':
	  peak_p = atof (&argv [work - 1][1]);

	  printf ("PEAK........%s percent.\n", &argv [work - 1][1]);

	  break;
	case 'V':
	  volcano_p = atof (&argv [work - 1][1]);

	  printf ("VOLCANO.....%s percent.\n", &argv [work - 1][1]);

	  break;
	case 'W':
	  whirlpool_p = atof (&argv [work - 1][1]);

	  printf ("WHIRLPOOL...%s percent.\n", &argv [work - 1][1]);

	  break;
	case 'C':
	  scrub_p = atof (&argv [work - 1][1]);

	  printf ("SCRUB.......%s percent.\n", &argv [work - 1][1]);

	  break;
	case 'T':
	  temple_p = atof (&argv [work - 1][1]);

	  printf ("TEMPLE......%s percent.\n", &argv [work - 1][1]);

	  break;
	case 'U':
	  cursed_p = atof (&argv [work - 1][1]);

	  printf ("CURSED......%s percent.\n", &argv [work - 1][1]);

	  break;
	case 'I':
	  if (toupper (argv [work - 1][2]) == 'F')
	  {
	    islands_on_f = 0;

	    printf ("ISLAND GENERATION OFF.\n");
	  } else
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
          fprintf (stderr, "FATAL ERROR: Unknown switch in command line.\n");	  exit (EXIT_FAILURE);
      }
    }
  }

  get_switch_list ();

  printf ("Percents:\n");

  printf ("  %0.3f Dens (%0.3f Red, %0.3f Green, %0.3f Grey, %0.3f Black, %0.3f Yellow).\n",
    den_p, red_den_p, green_den_p, grey_den_p, black_den_p, yellow_den_p);

  printf ("  %0.3f Plain, %0.3f Ruin, %0.3f Swamp, %0.3f Fertile, %0.3f Peak,\n",
    plain_p, ruin_p, swamp_p, fertile_p, peak_p);

  printf ("  %0.3f Volcano, %0.3f Whirlpool, %0.3f Scrub, %0.3f Temple, %0.3f Cursed.\n",
    volcano_p, whirlpool_p, scrub_p, temple_p, cursed_p);

  printf("Datapath: %s\n", data_path);
  printf("Gamecode: %s\n", game_code);
  get_world ();
  printf("World: %d x %d\n", world_x, world_y);

  number_land_hexes = 0;

  if (islands_on_f)
  {
    printf ("Raising Islands I...");

    for (y = 1; y < world_y; y += 4)
      for (x = 1; x < world_x; x += 4)
      {
	lay_island (x, y);
      }

    printf (" \n");

    printf ("Raising Islands II...");

    for (y = 3; y < world_y; y += 4)
      for (x = 3; x < world_x; x += 4)
      {
	if (random (100) > 50)
	{
	  get_hex (x, y, &world_hex);

	  world_hex.terrain = PLAIN;

	  number_land_hexes ++;

	  put_hex (x, y, &world_hex);
	}
      }
  } else {
    printf ("Island Generation OFF - Counting Plains hexes on map...");

    for (y = 0; y < world_y; y ++)
    {
      for (x = 0; x < world_x; x ++)
      {
	get_hex (x, y, &world_hex);

	if (world_hex.terrain == PLAIN)
	  number_land_hexes ++;
      }
    }
  }

  printf (" \n");

  den_n = (float) number_land_hexes * den_p;
  red_den_n = (float) den_n * red_den_p;
  green_den_n = (float) den_n * green_den_p;
  grey_den_n = (float) den_n * grey_den_p;
  black_den_n = (float) den_n * black_den_p;
  yellow_den_n = (float) den_n * yellow_den_p;
  ruin_n = (float) number_land_hexes * ruin_p;
  swamp_n = (float) number_land_hexes * swamp_p;
  fertile_n = (float) number_land_hexes * fertile_p;
  peak_n = (float) number_land_hexes * peak_p;
  volcano_n = (float) number_land_hexes * volcano_p;
  whirlpool_n = (float) number_land_hexes * whirlpool_p;
  scrub_n = (float) number_land_hexes * scrub_p;
  temple_n = (float) number_land_hexes * temple_p;
  plain_n = (float) number_land_hexes * plain_p;
  cursed_n = (float) number_land_hexes * cursed_p;

  /* top up den colours if below required */

  while (red_den_n + green_den_n + grey_den_n + black_den_n + yellow_den_n <
	 den_n)
  {
    switch (random (LIZARD_TYPES) + 1)
    {
      case RED_LIZARD:
	if (red_den_n != 0.0)
	  red_den_n ++;

	break;
      case GREEN_LIZARD:
	if (green_den_n != 0.0)
	  green_den_n ++;

	break;
      case GREY_LIZARD:
	if (grey_den_n != 0.0)
	  grey_den_n ++;

	break;
      case BLACK_LIZARD:
	if (black_den_n != 0.0)
	  black_den_n ++;

	break;
      case YELLOW_LIZARD:
	if (yellow_den_n != 0.0)
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
	  (world_x * world_y) - number_land_hexes);
  printf ("World needs %d den (%d red, %d green, %d grey, %d black, %d yellow),\n"
	  "%d ruin, %d swamp, %d fertile, %d peak, %d volcano, %d whirlpool,\n"
	  "%d scrub, %d temple, %d plain, %d cursed hexes.\n", den_n, red_den_n,
	  green_den_n, grey_den_n, black_den_n, yellow_den_n, ruin_n, swamp_n,
	  fertile_n, peak_n, volcano_n, whirlpool_n, scrub_n, temple_n,
	  plain_n, cursed_n);

  printf ("Raising Plains...");

  for (work = 0; work < plain_n; work ++)
  {
    work2 = 0;

    do
    {
      do
      {
	x2 = random (world_x);
	y2 = random (world_y);

	get_hex (x2, y2, &world_hex);
      } while (world_hex.terrain != WATER);

      if (world_hex.terrain == WATER)
	break;

      work2 ++;
    } while (work2 != 10);

    if (work2 != 10)
    {
      plain_a ++;
      world_hex.terrain = PLAIN;

      put_hex (x2, y2, &world_hex);
    }
  }

  printf (" \n");

  printf ("Placing Dens...");

  for (work = 0; work < den_n; work ++)
  {
    work2 = 0;

    do
    {
      do
      {
	x = random (world_x);
	y = random (world_y);

	get_hex (x, y, &world_hex);
      } while (world_hex.terrain == WATER);

      if (world_hex.terrain == PLAIN &&
	  terrain_adjacent (x, y, PLAIN) > 0 &&
	  terrain_adjacent (x, y, FERTILE) == 0 &&
	  terrain_adjacent (x, y, DEN) == 0)
	break;

      work2 ++;
    } while (work2 != 10);

    if (work2 != 10)
    {
      work3 = 10;

      do {
	switch (random (LIZARD_TYPES) + 1)
	{
	  case RED_LIZARD:
	    if (red_den_a < red_den_n)
	    {
              den_a ++;

              world_hex.terrain = DEN;

	      world_hex.den_lizard_type = RED_LIZARD;

	      red_den_a ++;

	      work3 = 0;
	    }

	    break;
	  case GREEN_LIZARD:
	    if (green_den_a < green_den_n)
	    {
              den_a ++;

              world_hex.terrain = DEN;

	      world_hex.den_lizard_type = GREEN_LIZARD;

	      green_den_a ++;

	      work3 = 0;
	    }

	    break;
	  case GREY_LIZARD:
	    if (grey_den_a < grey_den_n)
	    {
              den_a ++;

              world_hex.terrain = DEN;

	      world_hex.den_lizard_type = GREY_LIZARD;

	      grey_den_a ++;

	      work3 = 0;
	    }

	    break;
	  case BLACK_LIZARD:
	    if (black_den_a < black_den_n)
	    {
              den_a ++;

              world_hex.terrain = DEN;

	      world_hex.den_lizard_type = BLACK_LIZARD;

	      black_den_a ++;

	      work3 = 0;
	    }

	    break;
	  case YELLOW_LIZARD:
	    if (yellow_den_a < yellow_den_n)
	    {
              den_a ++;

              world_hex.terrain = DEN;

	      world_hex.den_lizard_type = YELLOW_LIZARD;

	      yellow_den_a ++;

	      work3 = 0;
	    }

	    break;
	}

	if (work3 == 0)
	{
	  if (world_hex.den_lizard_type == 0)
	    work3 = 10;
	} else
	  work3 --;
      } while (work3 != 0);

      put_hex (x, y, &world_hex);

      work4 = 0;

      do
      {
	work2 = 0;

	for (work3 = 1; work3 < 7; work3 ++)
	{
	  move_coords (work3, x, y, &work_x, &work_y);

	  get_hex (work_x, work_y, &world_hex);

	  if (work_x != x || work_y != y)
	    if (random (100) > 50 && work2 < 4 &&
	      world_hex.terrain == PLAIN)
	    {
	      world_hex.terrain = FERTILE;

	      work2 ++;

	      put_hex (work_x, work_y, &world_hex);
	    }
	}

	work4 ++;
      } while (work2 == 0 && work4 < 20);

      if (work4 != 20)
      {
	get_hex (x, y, &world_hex);

	world_hex.lizard_pop = DEN_LIZARD_LEVEL * work2;

	work3 = ((float) world_hex.lizard_pop / 100.00) * 30;

	switch (world_hex.den_lizard_type)
	{
	  case RED_LIZARD:
	    world_hex.red_lizards = work3;

            break;
	  case GREEN_LIZARD:
	    world_hex.green_lizards = work3;

            break;
	  case GREY_LIZARD:
	    world_hex.grey_lizards = work3;

	    break;
	  case BLACK_LIZARD:
	    world_hex.black_lizards = work3;

	    break;
	  case YELLOW_LIZARD:
	    world_hex.yellow_lizards = work3;

	    break;
	}

	put_hex (x, y, &world_hex);
      }
    }
  }

  printf (" \n");

  printf ("Creating Temples...");

  for (work = 0; work < temple_n; work ++)
  {
    work2 = 0;

    do
    {
      do
      {
	x2 = random (world_x);
	y2 = random (world_y);

	get_hex (x2, y2, &world_hex);
      } while (world_hex.terrain == WATER);

      if (world_hex.terrain == PLAIN &&
	  terrain_adjacent (x2, y2, DEN) == 0)
	break;

      work2 ++;
    } while (work2 != 10);

    if (work2 != 10)
    {
      temple_a ++;
      world_hex.terrain = TEMPLE;

      put_hex (x2, y2, &world_hex);
    }
  }

  printf (" \n");

  printf ("Weathering Ruins..");

  for (work = 0; work < ruin_n; work ++)
  {
    work2 = 0;

    do
    {
      do
      {
	x2 = random (world_x);
	y2 = random (world_y);

	get_hex (x2, y2, &world_hex);
      } while (world_hex.terrain == WATER);

      if (world_hex.terrain == PLAIN &&
	  terrain_adjacent (x2, y2, DEN) == 0 &&
	  terrain_adjacent (x2, y2, TEMPLE) == 0)
	break;

      work2 ++;
    } while (work2 != 10);

    if (work2 != 10)
    {
      ruin_a ++;
      world_hex.terrain = RUIN;

      put_hex (x2, y2, &world_hex);
    }
  }

  printf (" \n");

  printf ("Cursing Cursed...");

  for (work = 0; work < cursed_n; work ++)
  {
    work2 = 0;

    do
    {
      do
      {
	x2 = random (world_x);
	y2 = random (world_y);

	get_hex (x2, y2, &world_hex);
      } while (world_hex.terrain != PLAIN);

      if (world_hex.terrain == PLAIN)
	break;

      work2 ++;
    } while (work2 != 10);

    if (work2 != 10)
    {
      plain_a ++;
      world_hex.terrain = CURSED;

      put_hex (x2, y2, &world_hex);
    }
  }

  printf (" \n");

  printf ("Sinking Swamps...");

  for (work = 0; work < swamp_n; work ++)
  {
    work2 = 0;

    do
    {
      do
      {
	x2 = random (world_x);
	y2 = random (world_y);

	get_hex (x2, y2, &world_hex);
      } while (world_hex.terrain == WATER);

      if (world_hex.terrain == PLAIN)
	break;

      work2 ++;
    } while (work2 != 10);

    if (work2 != 10)
    {
      swamp_a ++;
      world_hex.terrain = SWAMP;

      put_hex (x2, y2, &world_hex);
    }
  }

  printf (" \n");

  printf ("Growing Fertile...");

  for (work = 0; work < fertile_n; work ++)
  {
    work2 = 0;

    do
    {
      do
      {
	x2 = random (world_x);
	y2 = random (world_y);

	get_hex (x2, y2, &world_hex);
      } while (world_hex.terrain == WATER ||
	       terrain_adjacent (x2, y2, DEN) > 0);

      if (world_hex.terrain == PLAIN)
	break;

      work2 ++;
    } while (work2 != 10);

    if (work2 != 10)
    {
      fertile_a ++;
      world_hex.terrain = FERTILE;

      put_hex (x2, y2, &world_hex);
    }
  }

  printf (" \n");

  printf ("Raising Peaks...");

  for (work = 0; work < peak_n; work ++)
  {
    work2 = 0;

    do
    {
      do
      {
	x2 = random (world_x);
	y2 = random (world_y);

	get_hex (x2, y2, &world_hex);
      } while (world_hex.terrain == WATER);

      if (world_hex.terrain == PLAIN)
	break;

      work2 ++;
    } while (work2 != 10);

    if (work2 != 10)
    {
      peak_a ++;
      world_hex.terrain = PEAK;

      put_hex (x2, y2, &world_hex);
    }
  }

  printf (" \n");

  printf ("Raising Volcanos...");

  for (work = 0; work < volcano_n; work ++)
  {
    work2 = 0;

    do
    {
      do
      {
	x2 = random (world_x);
	y2 = random (world_y);

	get_hex (x2, y2, &world_hex);
      } while (world_hex.terrain == WATER);

      if (world_hex.terrain == PLAIN && terrain_adjacent (x2, y2, DEN) == 0
	  && terrain_adjacent (x2, y2, TEMPLE) == 0 &&
	  terrain_adjacent (x2, y2, FERTILE) == 0)
	break;

      work2 ++;
    } while (work2 != 10);

    if (work2 != 10)
    {
      volcano_a ++;
      world_hex.terrain = VOLCANO;

      put_hex (x2, y2, &world_hex);
    }
  }

  printf (" \n");

  printf ("Sinking Whirlpools...");

  for (work = 0; work < whirlpool_n; work ++)
  {
    work2 = 0;

    do
    {
      do
      {
	x2 = random (world_x);
	y2 = random (world_y);

	get_hex (x2, y2, &world_hex);
      } while (world_hex.terrain != WATER);

      if (world_hex.terrain == WATER &&
	  terrain_adjacent (x2, y2, WHIRLPOOL) == 0)
	break;

      work2 ++;
    } while (work2 != 10);

    if (work2 != 10)
    {
      whirlpool_a ++;
      world_hex.terrain = WHIRLPOOL;

      put_hex (x2, y2, &world_hex);
    }
  }

  printf (" \n");

  printf ("Planting Scrub...");

  for (work = 0; work < scrub_n; work ++)
  {
    work2 = 0;

    do
    {
      do
      {
	x2 = random (world_x);
	y2 = random (world_y);

	get_hex (x2, y2, &world_hex);
      } while (world_hex.terrain == WATER);

      if (world_hex.terrain == PLAIN)
	break;

      work2 ++;
    } while (work2 != 10);

    if (work2 != 10)
    {
      scrub_a ++;
      world_hex.terrain = SCRUB;

      put_hex (x2, y2, &world_hex);
    }
  }

  printf (" \n");

  printf ("World gets %d den (%d red, %d green, %d grey, %d black, %d yellow),\n"
	  "%d ruin, %d swamp, %d fertile, %d peak, %d volcano, %d whirlpool,\n"
	  "%d scrub, %d temple, %d plain, %d cursed hexes.\n", den_a, red_den_a,
	  green_den_a,grey_den_a, black_den_a, yellow_den_a, ruin_a, swamp_a,
	  fertile_a, peak_a, volcano_a, whirlpool_a, scrub_a, temple_a,
	  plain_a, cursed_a);

  printf ("Finished!\n");

  return (EXIT_SUCCESS);
}

void lay_island (int x, int y)
{
  int work,
      work2,
      work3,
      work4,
      x2,
      y2;

  hex_t hex;

  get_hex (x, y, &hex);

  if (random (100) > 50)
  {
    hex.terrain = PLAIN;

    number_land_hexes ++;

    put_hex (x, y, &hex);
  }

  for (work = 1; work < 7; work ++)
  {
    move_coords (work, x, y, &x2, &y2);

    get_hex (x2, y2, &hex);

    if (x != x2 || y != y2)
    {
      number_land_hexes ++;

      hex.terrain = PLAIN;

      put_hex (x2, y2, &hex);
    }
  }

  if ((x <= (world_x / 5) || x >= (world_x - (world_x / 5))) &&
      (y <= (world_y / 5) || y >= (world_y - (world_y / 5))))
    work4 = 30;
  else
    work4 = 70;

  if (random (100) > work4)
  {
    work = random (6) + 1;

    if (random (100) > 50)
      work3 = 1;
    else
      work3 = -1;

    for (work2 = 1; work2 < 7; work2 ++)
    {
      work += work3;

      if (work > 6)
	work = 1;
      else
	if (work < 1)
	  work = 6;

      move_coords (work, x, y, &x2, &y2);

      if (x2 != x || y2 != y)
      {
	get_hex (x2, y2, &hex);

	hex.terrain = WATER;

	number_land_hexes --;

	put_hex (x2, y2, &hex);

	if (random (100) > 65)
	  break;
      }
    }
  }
}
