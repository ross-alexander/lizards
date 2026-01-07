#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "lizards.h"

void BinaryOrderShuffle(world_t *world)
{
  int work,
      work2,
      work3;

  order_t *order_ptr;
  for (work = 0; work < world->num_orders; work++)
    new_show_order(world, world->order_list[work]);

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
