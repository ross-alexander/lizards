#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "lizards.h"
#include "lizhex.h"
#include "lizmisc.h"
#include "create.h"

static int lay_island (world_t *world, int x, int y, double land_density)
{
  int work, work4;
  int i, start, dir;
  coord xy1, xy2;
  int number_land_hexes = 0;
  hex_t *hex;

  xy1.x = x; xy1.y = y;
  hex = get_hex (world, xy1);

  if (random (100) < (100 * land_density))
    {
      hex->terrain = PLAIN;
      number_land_hexes++;
    }
  for (work = 1; work < 7; work ++)
    {
      xy2 = move_coords (world, hex, work);
      hex = get_hex (world, xy2);
      if (!coord_eq(xy1, xy2))
	{
	  number_land_hexes++;
	  hex->terrain = PLAIN;
	}
    }
  if ((x <= (world->x / 5) || x >= (world->x - (world->x / 5))) &&
      (y <= (world->y / 5) || y >= (world->y - (world->y / 5))))
    work4 = (int)(140 * land_density);
  else
    work4 = (int)(60 * land_density);

  if (random (100) < work4)
    {
      start = random (6) + 1;
      if (random (100) < (100 * land_density))
	dir = 1;
      else
	dir = -1;
      for (i = 0; i < 6 && random(100) < 35; i++)
	{
	  start = (start + dir + 6) % 6;
	  xy2 = move_coords (world, hex, start + 1);
	  if (!coord_eq(xy1, xy2))
	    {
	      hex = get_hex (world, xy2);
	      if (hex->terrain == PLAIN)
		number_land_hexes--;
	      hex->terrain = WATER;
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

void WorldFill(world_t *w, char *filename, int argc, char *argv[])
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
    plain_p = 0.0,
    land_density = 0.50;

  feature_t *f, *band;
  hex_t *fea_hex, *hex, *adj_hex;
  coord xy2;

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
    for (work = 0; work < argc; work ++)
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
      for (y = 1; y < w->y; y += 4)
	for (x = 1; x < w->x; x += 4)
	  lay_island (w, x, y, land_density);

      number_land_hexes = 0;
      for (y = 0; y < w->y; y ++)
	for (x = 0; x < w->x; x ++)
	  {
	    hex = get_hex (w, coord_cons(x, y));
	    if (hex->terrain == PLAIN)
	      number_land_hexes ++;
	  }
      work = (int)(w->x * w->y * land_density - number_land_hexes);
      work2 = (int)((double)(work) / (w->x * w->y - number_land_hexes) * 100);

      printf ("Raising Islands II...\n");
      for (y = 0; y < w->y; y += 1)
	for (x = 0; x < w->x; x += 1)
	  {
	    hex = get_hex (w, coord_cons(x, y));
	    if (work > 0)
	      {
		if (hex->terrain == WATER && (random(100) < work2))
		  hex->terrain = PLAIN;
	      }
	    else
	      {
		if (hex->terrain == PLAIN && (random(100) < -work2))
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
  for (y = 0; y < w->y; y ++)
    for (x = 0; x < w->x; x ++)
      {
	hex = get_hex (w, coord_cons(x, y));
	if (hex->terrain == PLAIN)
	  number_land_hexes ++;
      }
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
	  (w->x * w->y) - number_land_hexes);
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
	  do hex = get_hex (w, coord_cons(random(w->x), random(w->y)));
	  while (hex->terrain != WATER);
	  work2 ++;
	}
      while (work2 != 10);
      if (work2 != 10)
	{
	  plain_a++;
	  hex->terrain = PLAIN;
	}
    }
  printf ("\n");
  printf ("Placing Dens...");
  work = 0;
  while (work < den_n)
    {
      do hex = get_hex (w, coord_cons(random(w->x), random(w->y)));
      while (hex->terrain == WATER);
      if (!(hex->terrain == PLAIN &&
	    !has_feature(hex, DEN) &&
	    terrain_adjacent (w, hex->xy, PLAIN) > 0 &&
	    feature_adjacent (w, hex->xy, FERTILE) == 0 &&
	    feature_adjacent (w, hex->xy, DEN) == 0))
	  continue;

      work3 = 1;
      do
	{
	  switch ((work2 = random (LIZARD_TYPES) + 1))
	    {
	    case RED_LIZARD:
	      if (red_den_a < red_den_n)
		{
		  work3 = 0;
		  red_den_a ++;
		}
	      break;
	    case GREEN_LIZARD:
	      if (green_den_a < green_den_n)
		{
		  work3 = 0;
		  green_den_a ++;
		}
	      break;
	    case GREY_LIZARD:
	      if (grey_den_a < grey_den_n)
		{
		  grey_den_a ++;
		  work3 = 0;
		}
	      break;
	    case BLACK_LIZARD:
	      if (black_den_a < black_den_n)
		{
		  black_den_a ++;
		  work3 = 0;
		}
	      break;
	    case YELLOW_LIZARD:
	      if (yellow_den_a < yellow_den_n)
		{
		  yellow_den_a ++;
		  work3 = 0;
		}
	      break;
	    }
	  if (work3 == 0)
	    {
	      den_a ++;
	      f = create_feature(w, DEN, hex->xy);
	      add_feature(w, f, hex);
	      f->f.den.pop = 0;
	      f->f.den.type = work2;
	      f->f.den.hunger = 0;
	    }
	}
      while (work3 != 0);

      fea_hex = hex;
      work4 = 0;
      do
	{
	  work2 = 0;
	  for (work3 = 0; work3 < 6; work3 ++)
	    {
	      xy2 = move_coords (w, hex, work3+1);
	      if (!xy2.invalid && (adj_hex = get_hex(w, xy2)))
		if (random (100) < 50 && work2 < 4 && adj_hex->terrain == PLAIN)
		  {
		    add_feature(w, create_feature(w, FERTILE, adj_hex->xy), adj_hex);
		    work2++;
		  }
	    }
	  work4 ++;
	}
      while (work2 == 0 && work4 < 20);

      if (work4 != 20)
	{
	  f->f.den.pop = DEN_LIZARD_LEVEL * work2;
	  work3 = (int)((double)(f->f.den.pop) * DEN_MILITIA_LEVEL);
	  band = create_feature(w, BAND, fea_hex->xy);
	  band->f.band.type[f->f.den.type - 1] = work3;
	  add_feature(w, band, fea_hex);
	}
      work++;
    }
  printf (" \n");
  printf ("Creating Temples...");
  for (work = 0; work < temple_n; work ++)
    {
      work2 = 0;
      do
	{
	  do
	    hex = get_hex (w, coord_cons(random(w->x), random(w->y)));
	  while (hex->terrain == WATER || num_features(hex));
	  work2 ++;
	} while (work2 != 10 && feature_adjacent (w, hex->xy, DEN) != 0);
      if (work2 != 10)
	{
	  temple_a++;
	  add_feature(w, create_feature(w, TEMPLE, hex->xy), hex);
	}
    }
  printf (" \n");
  printf ("Weathering Ruins..");
  for (work = 0; work < ruin_n; work ++)
    {
      work2 = 0;
      do
	{
	  do hex = get_hex (w, coord_cons(random(w->x), random(w->y)));
	  while (hex->terrain == WATER || num_features(hex));
	  if (feature_adjacent (w, hex->xy, DEN) == 0 &&
	      feature_adjacent (w, hex->xy, TEMPLE) == 0)
	    {
	      ruin_a ++;
	      add_feature(w, create_feature(w, TEMPLE, hex->xy), hex);
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
      do hex = get_hex (w, coord_cons(random(w->x), random(w->y)));
      while (hex->terrain == WATER || num_features(hex));
      plain_a++;
      cursed_a ++;
      add_feature(w, create_feature(w, CURSED, hex->xy), hex);
    }
  printf (" \n");

  printf ("Sinking Swamps...");
  for (work = 0; work < swamp_n; work ++)
    {
      do hex = get_hex (w, coord_cons(random(w->x), random(w->y)));
      while (hex->terrain == WATER || num_features(hex));
      swamp_a ++;
      hex->terrain = SWAMP;
    }
  printf (" \n");

  printf ("Growing Fertile...");
  for (work = 0; work < fertile_n; work ++)
    {
      do hex = get_hex (w, coord_cons(random(w->x), random(w->y)));
      while (hex->terrain == WATER || num_features(hex) ||
	     feature_adjacent(w, hex->xy, DEN));
      fertile_a ++;
      add_feature(w, create_feature(w, FERTILE, hex->xy), hex);
    }
  printf (" \n");

  printf ("Raising Peaks...");
  for (work = 0; work < peak_n; work ++)
    {
      do hex = get_hex (w, coord_cons(random(w->x), random(w->y)));
      while (hex->terrain == WATER || num_features(hex));
      peak_a ++;
      hex->terrain = PEAK;
    }
  printf (" \n");

  printf ("Raising Volcanos...");
  for (work = 0; work < volcano_n; work ++)
    {
      do hex = get_hex (w, coord_cons(random(w->x), random(w->y)));
      while (hex->terrain == WATER || num_features(hex) ||
	     feature_adjacent(w, hex->xy, DEN) ||
	     feature_adjacent(w, hex->xy, TEMPLE) ||
	     feature_adjacent(w, hex->xy, FERTILE));
      volcano_a ++;
      hex->terrain = PEAK;
      add_feature(w, create_feature(w, VOLCANO, hex->xy), hex);
    }
  printf (" \n");

  printf ("Sinking Whirlpools...");
  for (work = 0; work < whirlpool_n; work ++)
    {
      do hex = get_hex (w, coord_cons(random(w->x), random(w->y)));
      while (hex->terrain != WATER || num_features(hex) ||
	     feature_adjacent (w, hex->xy, WHIRLPOOL));
      whirlpool_a ++;
      hex->terrain = PEAK;
      add_feature(w, create_feature(w, WHIRLPOOL, hex->xy), hex);
    }
  printf (" \n");

  printf ("Planting Scrub...");
  for (work = 0; work < scrub_n; work ++)
    {
      do hex = get_hex (w, coord_cons(random(w->x), random(w->y)));
      while (hex->terrain != PLAIN || num_features(hex));
      scrub_a ++;
      hex->terrain = SCRUB;
    }
  printf (" \n");
  
  printf ("World gets %d den (%d red, %d green, %d grey, %d black, %d yellow),\n"
	  "%d ruin, %d swamp, %d fertile, %d peak, %d volcano, %d whirlpool,\n"
	  "%d scrub, %d temple, %d plain, %d cursed hexes.\n", den_a, red_den_a,
	  green_den_a,grey_den_a, black_den_a, yellow_den_a, ruin_a, swamp_a,
	  fertile_a, peak_a, volcano_a, whirlpool_a, scrub_a, temple_a,
	  plain_a, cursed_a);

}

