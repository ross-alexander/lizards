#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "lizard.h"
#include "order.h"

#include "filefunc.h"
#include "lizgame.h"
#include "lizord.h"

void BinaryOrderShuffle(World *world);
void BinaryOrderShuffle(World *world)
{
  int work,
      work2,
      work3;

  order_t *order_ptr;

  for (work = 0; work < world->num_orders; work++)
    show_order(world->order_list[work]);

  if (world->num_orders > 1)
    for (work = 0; work < world->num_orders; work ++)
    {
      work2 = random (world->num_orders);
      do
	{
	  work3 = random (world->num_orders);
	}
      while (work3 == work2);
      
      order_ptr = world->order_list [work2];
      world->order_list [work2] = world->order_list [work3];
      world->order_list [work3] = order_ptr;
    }
}
/*
int main (int argc, char *argv []);
int main (int argc, char *argv [])
{
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

  world = get_world();
  BinaryOrderShuffle(world);
  printf ("Finished!\n");

  return (EXIT_SUCCESS);
}
*/
