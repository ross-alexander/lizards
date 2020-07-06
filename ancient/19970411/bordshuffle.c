#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "lizard.h"
#include "map.h"
#include "order.h"

#include "filefunc.h"
#include "lizgame.h"
#include "lizord.h"

order_t *order_list [MAX_ORDERS_PER_TURN];

int main (int argc, char *argv []);

int main (int argc, char *argv [])
{
  FILE *fptr;

  char filename [255];

  int work,
      work2,
      work3;

  order_t work_order,
	  *order_ptr;

  randomize ();

  show_title ();

  printf ("LIZARDS! Binary Orders Shuffler. %s Last compiled %s.\n\n",
	  VER_STRING, __DATE__);

  if (argc == 3)
  {
    data_path = LzCheckDir(argv[1], 0);
    game_code = (char*)malloc(strlen(argv[2]) + 1);
    strcpy (game_code, argv [2]);
    strupr (game_code);
  } else {
    fprintf (stderr, "FATAL ERROR: Turn and Game Number not specified on command line!\n"
	     "            Command Line Format: EXEFILE data_path world.turn game_code.\n"
	     "                            e.g. BORDSHUF C:\\ 1 A\n");

    exit (EXIT_FAILURE);
  }

  get_player_list ();

  sprintf (filename, "%s%s%03d.%s", data_path, ORDS_PROC_F, world.turn,
    game_code);

  if ((fptr = fopen (filename, "rb")) == NULL)
  {
    fprintf (stderr, "FATAL ERROR: Unable to read %s file.\n", filename);
    exit (EXIT_FAILURE);
  }

  number_orders = num_records (fptr, sizeof (order_t));

  for (work = 0; work < number_orders; work ++)
  {
    get_rec (fptr, sizeof (order_t), work, &work_order);

    if ((order_list [work] = malloc (sizeof (order_t))) == NULL)
    {
      fprintf (stderr, "FATAL ERROR: Out of memory.\n");
      exit (EXIT_FAILURE);
    }

    *(order_list [work]) = work_order;

    show_order (order_list [work]);
  }

  fclose (fptr);

  if (number_orders > 1)
    for (work = 0; work < number_orders; work ++)
    {
      work2 = random (number_orders);

      do
      {
	work3 = random (number_orders);
      } while (work3 == work2);

      order_ptr = order_list [work2];
      order_list [work2] = order_list [work3];
      order_list [work3] = order_ptr;
    }

  sprintf (filename, "%s%s%03d.%s", data_path, ORDS_SHUF_F, world.turn,
    game_code);

  if ((fptr = fopen (filename, "wb")) == NULL)
  {
    fprintf (stderr, "FATAL ERROR: Unable to write to %s file.\n", filename);
    exit (EXIT_FAILURE);
  }

  for (work = 0; work < number_orders; work ++)
  {
    put_rec (fptr, sizeof (order_t), work, order_list [work]);

    free (order_list [work]);
  }

  fclose (fptr);

  sprintf (filename, "%s%s%03d.%s", data_path, ORDS_PROC_F, world.turn,
    game_code);

  remove (filename); /* remove file once finished with it */

  printf ("Finished!\n");

  return (EXIT_SUCCESS);
}
