#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lizard.h"

#include "strgfunc.h"
#include "lizgame.h"
#include "lizhex.h"
#include "lizmisc.h"

int generic_on;
int number_units [MAX_PLAYERS + 1];
unit_t *player_units [MAX_PLAYERS + 1][500];

typedef struct _element_t
{
  int x,
    y;
} element_t;

typedef struct
{
  int num_elements;
  element_t *listptr;
} anchor_t;

anchor_t base;

int *weights,
  centre_x,
  centre_y;

int main (int argc, char *argv []);
void ps_show_world (World *,int player, FILE *fptr);
see_t *build_see_list (int player, int *list_len);
see_t *centre_see_list (see_t *see_list, int player, int list_len);
void centre_points (anchor_t *base, int centre_x, int centre_y);
int sum_distances (anchor_t *base, int x, int y);
void weight_points (anchor_t *base, int *weights);
void find_weighted_centre (anchor_t *base, int *weights, int *centre_x, int *centre_y);
int see_list_centred_y_sort (const void *elem1, const void *elem2);
int see_list_x_sort (const void *elem1, const void *elem2);
int see_list_y_sort (const void *elem1, const void *elem2);
void see_wrap_hex (int *x, int *y);
void ps_show_hex (int x, int y, int player, FILE *fptr);
int spy_at (int x, int y, int player);
char *brace_string (char *string);
char *ps_hack_braces_string (char *string);
void break_str (char *string, char *string2, int length);
void build_unit_list (void);

/* ----------------------------------------------------------------------
--
-- min
--
---------------------------------------------------------------------- */

int min(int,int);
int min(int a, int b)
{
  return a > b ? b : a;
}


/* ----------------------------------------------------------------------
--
-- main
--
---------------------------------------------------------------------- */

int main (int argc, char *argv [])
{
  /*
  FILE *in_fptr;
  char work_string [255],
    filename [20];
    */
  FILE *out_fptr;
  char out_filename [20];
  int work;

  show_title ();

  printf ("LIZARDS! POSTSCRIPT Centred Print Utility. %s Last compiled %s.\n\n",
	  VER_STRING, __DATE__);

  if (argc != 4)
    {
      printf ("FATAL ERROR: Turn, Game Number not specified on command line!\n"
	      "             Command Line Format: EXEFILE data_path disk_data_path world.turn game generic.\n"
	      "                             e.g. PSCENPRN C:\\ D:\\ 1 A N\n");

      exit (EXIT_FAILURE);
    }
  data_path = LzCheckDir(argv[1], 0);
  game_code = (char*)malloc(strlen(argv[2]) + 1);
  strcpy (game_code, argv[2]);
  strupr (game_code);

  if (toupper (argv [3][0]) == 'N')
    generic_on = 0;
  else
    generic_on = 1;

  world = (World*)malloc(sizeof(World));
  get_world(world);

  if (world->num_players == 0)
    {
      fprintf (stderr, "FATAL ERROR: Game %s has no players.\n", game_code);
      exit (EXIT_FAILURE);
    }

  strcpy (world->player[0].code, "C-G");

  sprintf (out_filename, "%s%s.%s%03d", data_path, game_code, POSTSCRIPT_F, world->turn);

  if ((out_fptr = fopen (out_filename, "a")) == NULL)
    {
      fprintf (stderr, "FATAL ERROR: Unable to write to %s file.\n", out_filename);
      exit (EXIT_FAILURE);
    }

  fprintf (out_fptr, "/Gameletter {(%s)} def\n", game_code);
  fprintf (out_fptr, "/Turnnumber {(%d)} def\n", world->turn);
  fprintf (out_fptr, "/NTN {(%d)} def\n", world->turn + 1);
  fprintf (out_fptr, "/Date {(%s)} def\n", world->due_date);

  printf ("Building player maps...");

  /*
    build_unit_list ();
    */
    
  printf (" \n");

  for (work = 0; work <= world->num_players; work ++)
    if (strcmp (world->player[work].code, "****") != 0 &&
	strcmp (world->player[work].code, "DROP") != 0)
      {
	printf ("Postscripting Player %d...\n", work);

	/*
	sprintf (filename, "%s%s.%s%02d_%03d", data_path, game_code,
		 PLAYER_MESG_F, work, world->turn);

	if ((in_fptr = fopen (filename, "rt")) == NULL)
	  {
	    fprintf (stderr, "FATAL ERROR: Unable to read %s file.\n", filename);
	    exit (EXIT_FAILURE);
	  }
	
	  */
	fprintf (out_fptr, "/saveobj save def\n");
	fprintf (out_fptr, "/Shadow_Code {(%s)} def\n",
		 world->player[work].code);
	
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

	ps_show_world (world, work, out_fptr);
	fprintf (out_fptr, "saveobj restore\n");
      }
  
  fclose (out_fptr);
  printf ("\nFinished!\n");
  return (EXIT_SUCCESS);
}

/* ----------------------------------------------------------------------
--
-- ps_show_world
--
---------------------------------------------------------------------- */

void ps_show_world (World *world, int player, FILE *fptr)
{
  char text_string [255];

  int page_one_offset,
    page_x,
    page_y,
    range,
    work,
    high_end,
    hex_per_page;

  BandList bandlist;
  HexList *hexlist;
  see_t *player_hex_see_list;

  bandlist = BuildBandList(world, player);
  hexlist = BuildHexList(world, player, bandlist, 0);

  if (bandlist.cnt == 0)
    return;

  if (world->y / 2 > 16)
    hex_per_page = 18;
  else
    hex_per_page = 16;

  player_hex_see_list = hexlist->list;
  high_end = hexlist->cnt;

if (world->flags & LZ_WRAP)
   {
     player_hex_see_list = centre_see_list (player_hex_see_list, player,
					    high_end);
    }
  else
    {
      /* copy the uncentred values into the centred ones as although we use
	 centred values below, we don't actually want to centre the map as
	 it's not wrap-around */
      
      for (work = 0; work < high_end; work ++)
	{
	  player_hex_see_list [work].centred_x = player_hex_see_list [work].x;
	  player_hex_see_list [work].centred_y = player_hex_see_list [work].y;
	}
    }

  if ((range = player_hex_see_list [high_end - 1].centred_y -
       player_hex_see_list [0].centred_y + 1) > hex_per_page)
    {
      if (hex_per_page > 16)
	{
	  page_one_offset = 18;

	  if (world->flags & LZ_WRAP)
	    {
	      fprintf (fptr, "Map_Page_R\n");
	      fprintf (fptr, "-1190 140 Location_Full_R\n");
	    } else {
	      fprintf (fptr, "Map_Page\n");
	      fprintf (fptr, "-1169 140 Location_Full\n");
	    }
	}
      else
	{
	  page_one_offset = 16;

	  if (world->flags & LZ_WRAP)
	    {
	      fprintf (fptr, "Map_Page_R\n");
	      fprintf (fptr, "-1190 140 Location_R\n");
	  } else {
	      fprintf (fptr, "Map_Page\n");
	      fprintf (fptr, "-1169 140 Location\n");
	    }
	}

      fprintf (fptr, "(%s Clan World View for Lizards! Game %s Turn %d Page 1 of 2. World size 1A to %s) Map_Page_Num\n",
	       world->player[player].clan_name,
	       game_code, world->turn,
	       xy_to_string (world->x - 1, world->y - 1, text_string));

      for (work = 0; player_hex_see_list [work].centred_y < page_one_offset; work ++)
	{
	  page_x = (player_hex_see_list [work].centred_x * 35) + 25;

	  if (player_hex_see_list [work].centred_x % 2 == 1)
	    page_y = 775 - (player_hex_see_list [work].centred_y * 50);
	  else
	    page_y = 750 - (player_hex_see_list [work].centred_y * 50);

	  fprintf (fptr, "%d %d ", page_x, page_y);

	  ps_show_hex (player_hex_see_list [work].x, player_hex_see_list [work].y,
		       player, fptr);
	}

      fprintf (fptr, "printing\n");

      if ((player_hex_see_list [high_end - 1].centred_y -
	   hex_per_page + 1) > 16)
	{
	  if (world->flags & LZ_WRAP)
	    {
	      fprintf (fptr, "Map_Page_R\n");
	      fprintf (fptr, "-1190 140 Location_Full_R\n");
	    } else {
	      fprintf (fptr, "Map_Page\n");
	      fprintf (fptr, "-1169 140 Location_Full\n");
	    }
	} else {
	  if (world->flags & LZ_WRAP)
	    {
	      fprintf (fptr, "Map_Page_R\n");
	      fprintf (fptr, "-1190 140 Location_R\n");
	    } else {
	      fprintf (fptr, "Map_Page\n");
	      fprintf (fptr, "-1169 140 Location\n");
	    }
	}

      fprintf (fptr, "(%s Clan World View for Lizards! Game %s Turn %d Page 2 of 2. World size 1A to %s) Map_Page_Num\n",
	       world->player[player].clan_name,
	       game_code, world->turn,
	       xy_to_string (world->x - 1, world->y - 1, text_string));

      for (; work < high_end; work ++)
	if (player_hex_see_list [work].centred_y >= page_one_offset)
	  {
	    page_x = (player_hex_see_list [work].centred_x * 35) + 25;

	    if (player_hex_see_list [work].centred_x % 2 == 1)
	      page_y = 775 - ((player_hex_see_list [work].centred_y - page_one_offset) * 50);
	    else
	      page_y = 750 - ((player_hex_see_list [work].centred_y - page_one_offset) * 50);

	    fprintf (fptr, "%d %d ", page_x, page_y);

	    ps_show_hex (player_hex_see_list [work].x, player_hex_see_list [work].y,
			 player, fptr);
	  }

      fprintf (fptr, "printing\n");
    } else {
      page_one_offset = player_hex_see_list [0].centred_y;

      if (player_hex_see_list [0].centred_y != 0 ||
	  player_hex_see_list [0].centred_y != hex_per_page)
	{
	  if (hex_per_page > 16)
	    {
	      if (range > 16)
		{
		  if (world->flags & LZ_WRAP)
		    {
		      fprintf (fptr, "Map_Page_R\n");
		      fprintf (fptr, "-1190 140 Location_Full_R\n");
		    } else {
		      fprintf (fptr, "Map_Page\n");
		      fprintf (fptr, "-1169 140 Location_Full\n");
		    }
		} else {
		  if (world->flags & LZ_WRAP)
		    {
		      fprintf (fptr, "Map_Page_R\n");
		      fprintf (fptr, "-1190 140 Location_R\n");
		    } else {
		      fprintf (fptr, "Map_Page\n");
		      fprintf (fptr, "-1169 140 Location\n");
		    }
		}
	    } else {
	      if (world->flags & LZ_WRAP)
		{
		  fprintf (fptr, "Map_Page_R\n");
		  fprintf (fptr, "-1190 140 Location_R\n");
		} else {
		  fprintf (fptr, "Map_Page\n");
		  fprintf (fptr, "-1169 140 Location\n");
		}
	    }

	  fprintf (fptr, "(%s Clan World View for Lizards! Game %s Turn %d Page 1 of 1. World size 1A to %s) Map_Page_Num\n",
		   world->player[player].clan_name, game_code, world->turn,
		   xy_to_string (world->x - 1, world->y - 1, text_string));

	  for (work = 0; work < high_end; work ++)
	    {
	      page_x = (player_hex_see_list [work].centred_x * 35) + 25;

	      if (player_hex_see_list [work].centred_x % 2 == 1)
		page_y = 775 - ((player_hex_see_list [work].centred_y - page_one_offset) * 50);
	      else
		page_y = 750 - ((player_hex_see_list [work].centred_y - page_one_offset) * 50);

	      fprintf (fptr, "%d %d ", page_x, page_y);

	      ps_show_hex (player_hex_see_list [work].x, player_hex_see_list [work].y,
			   player, fptr);
	    }

	  fprintf (fptr, "printing\n");
	} else {
	  if (hex_per_page > 16)
	    {
	      if (world->flags & LZ_WRAP)
		{
		  fprintf (fptr, "Map_Page_R\n");
		  fprintf (fptr, "-1190 140 Location_Full_R\n");
		} else {
		  fprintf (fptr, "Map_Page\n");
		  fprintf (fptr, "-1169 140 Location_Full\n");
		}
	    } else {
	      if (world->flags & LZ_WRAP)
		{
		  fprintf (fptr, "Map_Page_R\n");
		  fprintf (fptr, "-1190 140 Location_R\n");
		} else {
		  fprintf (fptr, "Map_Page\n");
		  fprintf (fptr, "-1169 140 Location\n");
		}
	    }

	  fprintf (fptr, "(%s Clan World View for Lizards! Game %s Turn %d Page 1 of 1. World size 1A to %s) Map_Page_Num\n",
		   world->player[player].clan_name, game_code, world->turn,
		   xy_to_string (world->x - 1, world->y - 1, text_string));

	  for (work = 0; work < high_end; work ++)
	    {
	      page_x = (player_hex_see_list [work].centred_x * 35) + 25;

	      if (player_hex_see_list [work].centred_x % 2 == 1)
		page_y = 775 - ((player_hex_see_list [work].centred_y - page_one_offset) * 50);
	      else
		page_y = 750 - ((player_hex_see_list [work].centred_y - page_one_offset) * 50);

	      fprintf (fptr, "%d %d ", page_x, page_y);

	      ps_show_hex (player_hex_see_list [work].x, player_hex_see_list [work].y,
			   player, fptr);
	    }

	  fprintf (fptr, "printing\n");
	}
    }

  free (player_hex_see_list);
}

see_t *build_see_list (int player, int *list_len)
{
  /* in y,x order! */


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
				 
  
  
  int work,
    work2;
  
  see_t *see_ptr,
    work_see;
  
  unsigned int total_see_hex,
    see_hex;
  
  total_see_hex = 0;
  see_hex = 0;
  
  for (work = 0; work < number_units [player]; work ++)
    if (player_units [player][work]->peak)
      total_see_hex += 37;
    else
      total_see_hex += 19;
  
  if ((see_ptr = malloc (sizeof (see_t) * total_see_hex)) == NULL)
    {
      printf ("FATAL ERROR: Out of memory.\n");
      
      
      
      exit (EXIT_FAILURE);
    }
  
  for (work = 0; work < number_units [player]; work ++)
    {
      work_see.centred_x = work_see.x = player_units [player][work]->x;
      work_see.centred_y = work_see.y = player_units [player][work]->y;
      
      see_ptr [see_hex] = work_see;
      
      see_hex ++;
      
      if (player_units [player][work]->peak)
	{
	  if (player_units [player][work]->x % 2 == 0)
	    {
	      for (work2 = 0; work2 < 6; work2 ++)
		{
		  work_see.x = player_units [player][work]->x +
		    one_hex_odd [work2][1];
		  work_see.y = player_units [player][work]->y +
		    one_hex_odd [work2][0];
		  
		  see_wrap_hex (&work_see.x, &work_see.y);
		  
		  if (work_see.x > -1 && work_see.x < world->x &&
		      work_see.y > -1 && work_see.y < world->y)
		    {
		      see_ptr [see_hex] = work_see;
		      
		      see_hex ++;
		    }
		}
	      
	      for (work2 = 0; work2 < 12; work2 ++)
		{
		  work_see.x = player_units [player][work]->x +
		    two_hex_odd [work2][1];
		  work_see.y = player_units [player][work]->y +
		    two_hex_odd [work2][0];
		  
		  see_wrap_hex (&work_see.x, &work_see.y);
		  
		  if (work_see.x > -1 && work_see.x < world->x &&
		      work_see.y > -1 && work_see.y < world->y)
		    {
		      see_ptr [see_hex] = work_see;
		      
		      see_hex ++;
		    }
		}
	      
	      for (work2 = 0; work2 < 18; work2 ++)
		{
		  work_see.x = player_units [player][work]->x +
		    three_hex_odd [work2][1];
		  work_see.y = player_units [player][work]->y +
		    three_hex_odd [work2][0];
		  
		  see_wrap_hex (&work_see.x, &work_see.y);
		  
		  if (work_see.x > -1 && work_see.x < world->x &&
		      work_see.y > -1 && work_see.y < world->y)
		    {
		      see_ptr [see_hex] = work_see;
		      
		      see_hex ++;
		    }
		}
	    }
	  else
	    {
	      for (work2 = 0; work2 < 6; work2 ++)
		{
		  work_see.x = player_units [player][work]->x +
		    one_hex_even [work2][1];
		  work_see.y = player_units [player][work]->y +
		    one_hex_even [work2][0];
		  
		  see_wrap_hex (&work_see.x, &work_see.y);
		  
		  if (work_see.x > -1 && work_see.x < world->x &&
		      work_see.y > -1 && work_see.y < world->y)
		    {
		      see_ptr [see_hex] = work_see;
		      
		      see_hex ++;
		    }
		}
	      
	      for (work2 = 0; work2 < 12; work2 ++)
		{
		  work_see.x = player_units [player][work]->x +
		    two_hex_even [work2][1];
		  work_see.y = player_units [player][work]->y +
		    two_hex_even [work2][0];
		  
		  see_wrap_hex (&work_see.x, &work_see.y);
		  
		  if (work_see.x > -1 && work_see.x < world->x &&
		      work_see.y > -1 && work_see.y < world->y)
		    {
		      see_ptr [see_hex] = work_see;
		      
		      see_hex ++;
		    }
		}
	      
	      for (work2 = 0; work2 < 18; work2 ++)
		{
		  work_see.x = player_units [player][work]->x +
		    three_hex_even [work2][1];
		  work_see.y = player_units [player][work]->y +
		    three_hex_even [work2][0];
		  
		  see_wrap_hex (&work_see.x, &work_see.y);
		  
		  if (work_see.x > -1 && work_see.x < world->x &&
		      work_see.y > -1 && work_see.y < world->y)
		    {
		      see_ptr [see_hex] = work_see;
		      
		      see_hex ++;
		    }
		}
	    }
	} else {
	  if (player_units [player][work]->x % 2 == 0)
	    {
	      for (work2 = 0; work2 < 6; work2 ++)
		{
		  work_see.x = player_units [player][work]->x +
		    one_hex_odd [work2][1];
		  work_see.y = player_units [player][work]->y +
		    one_hex_odd [work2][0];
		  
		  see_wrap_hex (&work_see.x, &work_see.y);
		  
		  if (work_see.x > -1 && work_see.x < world->x &&
		      work_see.y > -1 && work_see.y < world->y)
		    {
		      see_ptr [see_hex] = work_see;
		      
		      see_hex ++;
		    }
		}
	      
	      for (work2 = 0; work2 < 12; work2 ++)
		{
		  work_see.x = player_units [player][work]->x +
		    two_hex_odd [work2][1];
		  work_see.y = player_units [player][work]->y +
		    two_hex_odd [work2][0];
		  
		  see_wrap_hex (&work_see.x, &work_see.y);
		  
		  if (work_see.x > -1 && work_see.x < world->x &&
		      work_see.y > -1 && work_see.y < world->y)
		    {
		      see_ptr [see_hex] = work_see;
		      
		      see_hex ++;
		    }
		}
	    } else {
	      for (work2 = 0; work2 < 6; work2 ++)
		{
		  work_see.x = player_units [player][work]->x +
		    one_hex_even [work2][1];
		  work_see.y = player_units [player][work]->y +
		    one_hex_even [work2][0];
		  
		  see_wrap_hex (&work_see.x, &work_see.y);
		  
		  if (work_see.x > -1 && work_see.x < world->x &&
		      work_see.y > -1 && work_see.y < world->y)
		    {
		      see_ptr [see_hex] = work_see;
		      
		      see_hex ++;
		    }
		}
	      
	      for (work2 = 0; work2 < 12; work2 ++)
		{
		  work_see.x = player_units [player][work]->x +
		    two_hex_even [work2][1];
		  work_see.y = player_units [player][work]->y +
		    two_hex_even [work2][0];
		  
		  see_wrap_hex (&work_see.x, &work_see.y);
		  
		  if (work_see.x > -1 && work_see.x < world->x &&
		      work_see.y > -1 && work_see.y < world->y)
		    {
		      see_ptr [see_hex] = work_see;
		      
		      see_hex ++;
		    }
		}
	    }
	}
    }
  
  *list_len = see_hex;
  
  for (work = 0; work < see_hex; work ++)
    for (work2 = 0; work2 < see_hex; work2 ++)
      if (see_ptr [work].x == see_ptr [work2].x &&
	  see_ptr [work].y == see_ptr [work2].y &&
	  work != work2)
	{
	  see_ptr [work2].x = 127;
	  see_ptr [work2].y = 127;
	}

  qsort (see_ptr, see_hex, sizeof (see_t), see_list_y_sort);
  
  for (work = 0; work < see_hex; work ++)
    if (see_ptr [work].y == 127)
      {
	*list_len = work;
	break;
      }
  
  return (see_ptr);
}

see_t *centre_see_list (see_t *see_list, int player, int list_len)
{
  unsigned int loop;

  base.num_elements = list_len;

  if ((base.listptr = malloc (sizeof (element_t) * list_len)) == NULL)
    {
      printf ("FATAL ERROR: Out of memory.\n");

      exit (-1);
    }

  if ((weights = malloc (sizeof (int) * list_len)) == NULL)
    {
      printf ("FATAL ERROR: Out of memory.\n");

      exit (-1);
    }

  for (loop = 0; loop < list_len; loop ++)
    {
      base.listptr [loop].x = see_list [loop].x;

      base.listptr [loop].y = see_list [loop].y;
    }

  weight_points (&base, weights);

  find_weighted_centre (&base, weights, &centre_x, &centre_y);

  centre_points (&base, centre_x, centre_y);

  for (loop = 0; loop < list_len; loop ++)
    {
      see_list [loop].centred_x = base.listptr [loop].x;
      see_list [loop].centred_y = base.listptr [loop].y;
    }

  free (base.listptr);
  free (weights);

  qsort (see_list, list_len, sizeof (see_t), see_list_centred_y_sort);

  return (see_list);
}

void centre_points (anchor_t *base, int centre_x, int centre_y)
{
  element_t *ptr;

  int loop,
    x,
    y;

  if (base->listptr == NULL) return;

  for ( loop = 0; loop < base->num_elements; loop ++ )
    {
      ptr = &base->listptr [loop];

      x = ptr->x - centre_x + (world->x / 2);

      if (ptr->x % 2 != x % 2)
	x += 1;

      if (x < 0) x += world->x;
      if (x >= world->x) x = x % world->x;

      y = ptr->y - centre_y + (world->y / 2);

      if (y < 0) y += world->y;
      if (y >= world->y) y = y % world->y;

      ptr->x = x;
      ptr->y = y;
    }
}

int sum_distances (anchor_t *base, int x, int y)
{
  element_t *ptr;

  int loop,
    i,
    j,
    total = 0;

  if (base->listptr == NULL) return (0);

  for (loop = 0; loop < base->num_elements; loop ++)
    {
      ptr = &base->listptr [loop];

      if (ptr->x < x)
	{
	  i = ptr->x;
	  j = x;
	} else {
	  i = x;
	  j = ptr->x;
	}

      total += min (j - i, world->x - j + i);

      if (ptr->y < y)
	{
	  i = ptr->y;
	  j = y;
	} else {
	  i = y;
	  j = ptr->y;
	}

      total += min (j - i, world->y - j + i);
    }

  return (total);
}

void weight_points (anchor_t *base, int *weights)
{
  element_t *ptr;

  int loop,
    i = 0;

  if (base->listptr == NULL) return;

  for (loop = 0; loop < base->num_elements; loop ++)
    {
      ptr = &base->listptr [loop];

      weights [i++] = sum_distances (base, ptr->x, ptr->y);
    }
}

void find_weighted_centre (anchor_t *base, int *weights, int *centre_x, int *centre_y)
{
  element_t *ptr;

  int loop,
    i = 0,
    lowest;

  if (base->listptr == NULL) return;

  lowest = weights [0];

  for (loop = 0; loop < base->num_elements; loop ++)
    {
      ptr = &base->listptr [loop];

      if (lowest >= weights [i])
	{
	  *centre_x = ptr->x;
	  *centre_y = ptr->y;
	  lowest = weights [i];
	}

      i ++;
    }
}

int see_list_centred_y_sort (const void *elem1, const void *elem2)
{
  if (((see_t *)elem1)->centred_y < ((see_t *)elem2)->centred_y)
    return (-1);
  else
    if (((see_t *)elem1)->centred_y > ((see_t *)elem2)->centred_y)
      return (1);
    else
      return (0);
}

int see_list_y_sort (const void *elem1, const void *elem2)
{
  if (((see_t *)elem1)->y < ((see_t *)elem2)->y)
    return (-1);
  else
    if (((see_t *)elem1)->y > ((see_t *)elem2)->y)
      return (1);
    else
      return (0);
}

int see_list_x_sort (const void *elem1, const void *elem2)
{
  if (((see_t *)elem1)->x < ((see_t *)elem2)->x)
    return (-1);
  else
    if (((see_t *)elem1)->x > ((see_t *)elem2)->x)
      return (1);
    else
      return (0);
}

void ps_show_hex (int x, int y, int player, FILE *fptr)
{
  char string [80];

  hex_t hex;

  get_hex (world, x, y, &hex);

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

  if (generic_on)
    {
      xy_to_string (x, y, string);

      fprintf (fptr, "(%s) ", string);
    } else
      fprintf (fptr, "() ");

  if (hex.terrain == DEN || hex.terrain == HOME_DEN)
    {
      if (hex.hex_owner == 0)
	{
	  fprintf (fptr, "Small (Free) Owner\n");
	} else {
	  if (hex.hex_owner == player)
	    {
	      fprintf (fptr, "Small (%s) Owner (%d) Troop_Number\n",
		       world->player[hex.hex_owner].code, lizards_in_hex (&hex) + hex.lizard_pop);
	    } else {
	      fprintf (fptr, "Small (%s) Owner\n", world->player[hex.hex_owner].code);
	    }
	}
    } else {
      if (lizards_in_hex (&hex) > 0)
	{
	  if ((hex.terrain != SCRUB && hex.terrain != RUIN) ||
	      hex.hex_owner == player)
	    {
	      fprintf (fptr, "Small (%s) Owner (%d) Troop_Number\n",
		       world->player[hex.hex_owner].code, lizards_in_hex (&hex));
	    } else {
	      if (spy_at (x, y, player))
		{
		  fprintf (fptr, "Small (%s) Owner (%d) Troop_Number\n",
			   world->player[hex.hex_owner].code, lizards_in_hex (&hex));
		} else {
		  fprintf (fptr, "Small\n");
		}
	    }
	} else {
	  if (spy_at (x, y, player))
	    {
	      fprintf (fptr, "Small (%s) Owner (SPY) Troop_Number\n",
		       world->player[player].code);
	    } else {
	      if ((hex.bridge_here == 1 || hex.terrain == TEMPLE) &&
		  hex.hex_owner > 0)
		{
		  fprintf (fptr, "Small (%s) Owner\n", world->player[hex.hex_owner].code);
		} else {
		  fprintf (fptr, "Small\n");
		}
	    }
	}
    }
}

int spy_at (int x, int y, int player)
{
  int work;

  for (work = 0; work < world->num_spies; work ++)
    if ((world->spylist [work]->x_hex == x) &&
	(world->spylist [work]->y_hex == y) &&
	(world->spylist [work]->player == player))
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

  for (work = 0, work2 = 0; work < strlen (string); work ++, work2 ++)
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

void build_unit_list (void)
{
  int work,
    work2,
    work3,
    work4,
    x,
    y;

  hex_t world_hex;

  unit_t work_unit;

  for (work = 1; work <= world->num_players; work ++)
    number_units [work] = 0;

  for (x = 0; x < world->x; x ++)
    {
      for (y = 0; y < world->y; y ++)
	{
	  get_hex (world, x, y, &world_hex);

	  if (world_hex.hex_owner > 0 && lizards_in_hex (&world_hex) > 0)
	    {
	      work = world_hex.hex_owner;

	      for (work2 = 0, work3 = 0; work2 < number_units [work]; work2 ++)
		if (player_units [work][work2]->x == x &&
		    player_units [work][work2]->y == y)
		  {
		    work3 = 1;

		    break;
		  }

	      if (work3 == 0)
		{
		  work_unit.x = x;
		  work_unit.y = y;
		  work_unit.spy = 0;

		  if (world_hex.terrain == PEAK)
		    work_unit.peak = 1;
		  else
		    work_unit.peak = 0;

		  if ((player_units [work][number_units [work]] =
		       malloc (sizeof (unit_t))) == NULL)
		    {
		      printf ("FATAL ERROR: Out of memory.\n");

		      

		      exit (EXIT_FAILURE);
		    }

		  *player_units [work][number_units [work]] = work_unit;

		  number_units [work] ++;
		}
	    }
	}
    }

  for (work = 0; work < world->num_spies; work ++)
    {
      if (world->spylist [work]->player > 0)
	{
	  work2 = world->spylist [work]->player;

	  for (work3 = 0, work4 = 0; work3 < number_units [work2]; work3 ++)
	    if (player_units[work2][work3]->x == world->spylist[work]->x_hex &&
		player_units[work2][work3]->y == world->spylist[work]->y_hex)
	      {
		work4 = 1;
		break;
	      }

	  if (work4 == 0)
	    {
	      work_unit.x = world->spylist [work]->x_hex;
	      work_unit.y = world->spylist [work]->y_hex;
	      work_unit.spy = 1;

	      get_hex (world, work_unit.x, work_unit.y, &world_hex);

	      if (world_hex.terrain == PEAK)
		work_unit.peak = 1;
	      else
		work_unit.peak = 0;

	      if ((player_units [work2][number_units [work2]] =
		   malloc (sizeof (unit_t))) == NULL)
		{
		  printf ("FATAL ERROR: Out of memory.\n");

		  

		  exit (EXIT_FAILURE);
		}

	      *player_units [work2][number_units [work2]] = work_unit;

	      number_units [work2] ++;
	    }
	}
    }
}
