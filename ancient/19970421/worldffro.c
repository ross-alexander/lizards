#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "compat/cio.h"

#include "lizard.h"
#include "map.h"

#include "lizgame.h"

#define TITLE_WINDOW 1, 1, 78, 4, COLOR_RED, COLOR_BLACK
#define SHOW_WINDOW 1, 5, 78, 19, COLOR_RED, COLOR_BLACK
#define MENU_WINDOW 1, 19, 78, 20, COLOR_RED, COLOR_BLACK
#define OPTION_WINDOW 0, 21, 78, 22, COLOR_RED, COLOR_BLACK
#define ERROR_WINDOW 0, 23, 78, 24, COLOR_RED, COLOR_BLACK

double den_p = 0.22,
      red_den_p = 0.20,
      green_den_p = 0.20,
      grey_den_p = 0.20,
      black_den_p = 0.20,
      yellow_den_p = 0.20,
      ruin_p = 0.02,
      swamp_p = 0.27,
      fertile_p = 0.04,
      peak_p = 0.02,
      volcano_p = 0.015,
      whirlpool_p = 0.05,
      scrub_p = 0.22,
      temple_p = 0.10,
      cursed_p = 0.0,
      plain_p = 0.45;

int islands_on_f = 1;

int noninteractive = 0;

int main (int argc, char **argv);
void show_menu (void);
void show_percents (void);
void show_err (char *string);

WINDOW *show_win, *title_win, *menu_win, *option_win, *error_win;

#define open_text_window(a,b,c) *(a) = NcursesOpenWindow(b,c); cWindow = *(a)
#define close_text_window(a) NcursesCloseWindow(*(a))
#define reopen_text_window(a) NcursesReopenWindow(*(a)); cWindow = *(a)
#define cscanf scanf

/* ---------------------------------------------------------------------- */

extern char* optarg;
extern int optind;
int main (int argc, char *argv[])
{
  FILE *fptr;
  
  char option,
    filename [255];
  
  double work_double;

  int ch;
  while ((ch = getopt(argc, argv, "d:g:nv")) != EOF)
    switch(ch)
      {
      case 'd':
	data_path = LzCheckDir(optarg, 0);
	break;
      case 'g':
	game_code = (char*)malloc(strlen(optarg) + 1);
	strcpy (game_code, optarg);
	strupr (game_code);
	break;
      case 'v':
	printf ("\n\rLIZARDS! World Fill Menu. %s Last compiled %s.\r\n\n\r",
		VER_STRING, __DATE__);
	printf ("FATAL ERROR: Game Number not specified on command line!\r\n"
		"             Command Line Format: EXEFILE data_path game.\r\n"
		"                             e.g. WORLFFRO C:\\ A\r\n");
	exit (EXIT_FAILURE);
	break;
      case 'n':
	noninteractive = 1;
	break;
      }
  if (!data_path && !game_code)
    {
      fprintf(stderr, "Datapath and/or gamecode was not given.\n");
      exit(EXIT_FAILURE);
    }
  printf("Datapath: %s\n", data_path);
  printf("Gamecode: %s\n", game_code);
  get_world (world);
  printf("World: %d x %d\n", world->x, world->y);

  NcursesInit(1);
  open_text_window (&title_win, TITLE_WINDOW, SINGLE_BORDER);

  cprintf ("\n\rLIZARDS! World Fill Menu. %s Last compiled %s.\r\n",
    VER_STRING, __DATE__);

  open_text_window (&show_win, SHOW_WINDOW, NO_BORDER);
  open_text_window (&menu_win, MENU_WINDOW, NO_BORDER);
  open_text_window (&option_win, OPTION_WINDOW, NO_BORDER);
  open_text_window (&error_win, ERROR_WINDOW, NO_BORDER);

  do {
    show_percents ();
    show_menu ();
/*
    clrscr ();
*/
   option = toupper (getche ());

    switch (option)
    {
      case 'P':
	clrscr ();

	cprintf ("NEW PLAIN PERCENTAGE: ");

	cscanf ("%lf", &work_double);

	if (work_double < 0.0 || work_double > 1.0)
	{
	  show_err ("Invalid Percentage Entered.");

	  break;
	} else
	  plain_p = work_double;

        break;
      case 'D':
        clrscr ();

	cprintf ("NEW DEN PERCENTAGE: ");

	cscanf ("%lf", &work_double);

	if (work_double < 0.0 || work_double > 1.0)
	{
	  show_err ("Invalid Percentage Entered.");

	  break;
	} else
	  den_p = work_double;

        break;
      case '1':
        clrscr ();

	cprintf ("NEW RED DEN PERCENTAGE (OF DEN PERCENTAGE): ");

	cscanf ("%lf", &work_double);

	if (work_double < 0.0 || work_double > 1.0)
	{
	  show_err ("Invalid Percentage Entered.");

	  break;
	} else
	  red_den_p = work_double;

        break;
      case '2':
        clrscr ();

	cprintf ("NEW GREEN DEN PERCENTAGE (OF DEN PERCENTAGE): ");

	cscanf ("%lf", &work_double);

	if (work_double < 0.0 || work_double > 1.0)
	{
	  show_err ("Invalid Percentage Entered.");

	  break;
	} else
	  green_den_p = work_double;

        break;
      case '3':
        clrscr ();

	cprintf ("NEW GREY DEN PERCENTAGE (OF DEN PERCENTAGE): ");

	cscanf ("%lf", &work_double);

	if (work_double < 0.0 || work_double > 1.0)
	{
	  show_err ("Invalid Percentage Entered.");

	  break;
	} else
	  grey_den_p = work_double;

	break;
      case '4':
        clrscr ();

	cprintf ("NEW BLACK DEN PERCENTAGE (OF DEN PERCENTAGE): ");

	cscanf ("%lf", &work_double);

	if (work_double < 0.0 || work_double > 1.0)
	{
	  show_err ("Invalid Percentage Entered.");

	  break;
	} else
	  black_den_p = work_double;

        break;
      case '5':
        clrscr ();

	cprintf ("NEW YELLOW DEN PERCENTAGE (OF DEN PERCENTAGE): ");

	cscanf ("%lf", &work_double);

	if (work_double < 0.0 || work_double > 1.0)
	{
	  show_err ("Invalid Percentage Entered.");

	  break;
	} else
	  yellow_den_p = work_double;

        break;
      case 'R':
        clrscr ();

	cprintf ("NEW RUIN PERCENTAGE: ");

	cscanf ("%lf", &work_double);

	if (work_double < 0.0 || work_double > 1.0)
	{
	  show_err ("Invalid Percentage Entered.");

	  break;
	} else
	  ruin_p = work_double;

        break;
      case 'T':
        clrscr ();

	cprintf ("NEW TEMPLE PERCENTAGE: ");

	cscanf ("%lf", &work_double);

	if (work_double < 0.0 || work_double > 1.0)
	{
	  show_err ("Invalid Percentage Entered.");

	  break;
	} else
	  temple_p = work_double;

        break;
      case 'F':
        clrscr ();

	cprintf ("NEW FERTILE PERCENTAGE: ");

	cscanf ("%lf", &work_double);

	if (work_double < 0.0 || work_double > 1.0)
	{
	  show_err ("Invalid Percentage Entered.");

	  break;
	} else
	  fertile_p = work_double;

        break;
      case 'S':
        clrscr ();

	cprintf ("NEW SWAMP PERCENTAGE: ");

	cscanf ("%lf", &work_double);

	if (work_double < 0.0 || work_double > 1.0)
	{
	  show_err ("Invalid Percentage Entered.");

	  break;
	} else
	  swamp_p = work_double;

        break;
      case 'C':
        clrscr ();

	cprintf ("NEW SCRUB PERCENTAGE: ");

	cscanf ("%lf", &work_double);

	if (work_double < 0.0 || work_double > 1.0)
	{
	  show_err ("Invalid Percentage Entered.");

	  break;
	} else
	  scrub_p = work_double;

        break;
      case 'U':
        clrscr ();

	cprintf ("NEW CURSED PERCENTAGE: ");

	cscanf ("%lf", &work_double);

	if (work_double < 0.0 || work_double > 1.0)
	{
	  show_err ("Invalid Percentage Entered.");

	  break;
	} else
	  cursed_p = work_double;

        break;
      case 'K':
        clrscr ();

	cprintf ("NEW PEAK: ");

	cscanf ("%lf", &work_double);

	if (work_double < 0.0 || work_double > 1.0)
	{
	  show_err ("Invalid Percentage Entered.");

	  break;
	} else
	  peak_p = work_double;

        break;
      case 'V':
        clrscr ();

	cprintf ("NEW VOLCANO PERCENTAGE: ");

	cscanf ("%lf", &work_double);

	if (work_double < 0.0 || work_double > 1.0)
	{
	  show_err ("Invalid Percentage Entered.");

	  break;
	} else
	  volcano_p = work_double;

        break;
      case 'W':
        clrscr ();

	cprintf ("NEW WHIRLPOOL PERCENTAGE: ");

	cscanf ("%lf", &work_double);

	if (work_double < 0.0 || work_double > 1.0)
	{
	  show_err ("Invalid Percentage Entered.");

	  break;
	} else
	  whirlpool_p = work_double;

	break;
      /* Not implemented for the released version as the chances of a
        'novice' moderater screwing things up with this option are large
      case 'I':
	if (islands_on_f)
	  islands_on_f = 0;
	else
	  islands_on_f = 1;

	break;
      */
      case 'I':
	clrscr ();

	cprintf ("Quit Menu and run WORLFILL program to initialise world (y/N)? ");

        option = toupper (getche ());

	if (option == 'Y')
	  option = 'I';
	else
	  option = ' ';

	break;
      default:
	break;
    }
  } while (option != 'I');

  /* dump parameter file */

  sprintf (filename, "%sWORLDFILL.PAR", data_path);

  if ((fptr = fopen (filename, "wt")) == NULL)
  {
    printf ("FATAL ERROR: Unable to write to WORLFILL.PAR file.\n");
    exit (EXIT_FAILURE);
  }

  fprintf (fptr, "%f\n", den_p);
  fprintf (fptr, "%f\n", red_den_p);
  fprintf (fptr, "%f\n", green_den_p);
  fprintf (fptr, "%f\n", grey_den_p);
  fprintf (fptr, "%f\n", black_den_p);
  fprintf (fptr, "%f\n", yellow_den_p);
  fprintf (fptr, "%f\n", ruin_p);
  fprintf (fptr, "%f\n", swamp_p);
  fprintf (fptr, "%f\n", fertile_p);
  fprintf (fptr, "%f\n", peak_p);
  fprintf (fptr, "%f\n", volcano_p);
  fprintf (fptr, "%f\n", whirlpool_p);
  fprintf (fptr, "%f\n", scrub_p);
  fprintf (fptr, "%f\n", temple_p);
  fprintf (fptr, "%f\n", cursed_p);
  fprintf (fptr, "%f\n", plain_p);
  fprintf (fptr, "%d\n", islands_on_f);

  fclose (fptr);

  /* build command line list */

  close_text_window (&title_win);
  close_text_window (&show_win);
  close_text_window (&menu_win);
  close_text_window (&option_win);
  close_text_window (&error_win);
  return (EXIT_SUCCESS);
}

void show_menu (void)
{
  reopen_text_window (&menu_win);
  clrscr ();
  cprintf ("Pla, Den (1-Red, 2-Grn, 3-Gry, 4-Blk, 5-Yel), Rui, Tem, Fer, Swa, sCr, cUr,\n\r"
	   "peaK, Vol, Whirl, Initialise World? ");

  reopen_text_window (&option_win);
}

void show_percents (void)
{
  int num_hexes;

  unsigned int den_a,
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
	       plain_a;

  reopen_text_window (&show_win);

  clrscr ();

  num_hexes = (world->x * world->y) * 0.40;

  den_a = num_hexes * den_p;
  red_den_a = den_a * red_den_p;
  green_den_a = den_a * green_den_p;
  grey_den_a = den_a * grey_den_p;
  black_den_a = den_a * black_den_p;
  yellow_den_a = den_a * yellow_den_p;
  ruin_a = num_hexes * ruin_p;
  swamp_a = num_hexes * swamp_p;
  fertile_a = num_hexes * fertile_p;
  peak_a = num_hexes * peak_p;
  volcano_a = num_hexes * volcano_p;
  whirlpool_a = num_hexes * whirlpool_p;
  scrub_a = num_hexes * scrub_p;
  temple_a = num_hexes * temple_p;
  cursed_a = num_hexes * cursed_p;
  plain_a = num_hexes * plain_p;

  if (!islands_on_f)
    cprintf ("\n\rRANDOM ISLAND GENERATION OFF - LAND HEXES MUST ALREADY BE GENERATED!\r\n");
  else
    cprintf ("\n\rRandom Island Generation ON.\n\r");
  cprintf ("  %0.3f Dens (%0.3f Red, %0.3f Green, %0.3f Grey, %0.3f Black, %0.3f Yellow).\r\n",
    den_p, red_den_p, green_den_p, grey_den_p, black_den_p, yellow_den_p);

  cprintf ("  %0.3f Plain, %0.3f Ruin, %0.3f Swamp, %0.3f Fertile, %0.3f Peak,\r\n",
    plain_p, ruin_p, swamp_p, fertile_p, peak_p);

  cprintf ("  %0.3f Volcano, %0.3f Whirlpool, %0.3f Scrub, %0.3f Temple, %0.3f Cursed.\r\n\r\n",
    volcano_p, whirlpool_p, scrub_p, temple_p, cursed_p);

  cprintf ("Initial land generated dictates %% of other terrain types:\r\n");
  cprintf ("World %s is %d x %d. Assuming 40%% Initial land = %d Plain hexes.\r\n\r\n",
     game_code, world->x, world->y, num_hexes);

  cprintf ("Estimated # hexes of each terrain type based on %d Plain hexes is:\n\r",
     num_hexes);

  cprintf ("  %u Dens (%u Red, %u Green, %u Grey, %u Black, %u Yellow).\r\n",
    den_a, red_den_a, green_den_a, grey_den_a, black_den_a, yellow_den_a);

  cprintf ("  %u Plain, %u Ruin, %u Swamp, %u Fertile, %u Peak, %u Volcano,\r\n",
    plain_a, ruin_a, swamp_a, fertile_a, peak_a, volcano_a);

  cprintf ("  %u Whirlpool, %u Scrub, %u Temple, %u Cursed.",
    whirlpool_a, scrub_a, temple_a, cursed_a);

  reopen_text_window (&option_win);
}

void show_err (char *string)
{
  reopen_text_window (&error_win);

  clrscr ();

  cprintf ("%s", string);

  reopen_text_window (&option_win);
}
