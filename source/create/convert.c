#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <obstack.h>

#define obstack_chunk_alloc malloc
#define obstack_chunk_free free

#include "lizards.h"
#include "lizhex.h"

void WorldNew(World*);
void WorldNew(World *world)
{
  coord xy;
  int x, y, i;
  hex_t *hex;
  new_hex_t *nh;
  feature_t *f;
  world->hexen = (new_hex_t*)calloc(sizeof(new_hex_t), world->x * world->y);
  for (y = 0; y < world->y; y++)
    {
      for (x = 0; x < world->x; x++)
	{
	  xy.x = x;
	  xy.y = y;
	  xy.z = 0;
	  xy.invalid = 0;
	  f = NULL;
	  nh = &world->hexen[world->x * y + x];
	  hex = get_hex_ref(world, x, y);
	  nh->xy = xy;
	  nh->owner = hex->hex_owner;
	  nh->terrain = PLAIN;
	  switch (hex->terrain)
	    {
	    case PLAIN:
	      break;
	    case RUIN:
	      f = (feature_t*)calloc(sizeof(feature_t), 1);
	      f->xy = xy;
	      f->type = RUIN;
	      add_feature(world, f, nh);
	      break;
	    case FERTILE:
	      f = (feature_t*)calloc(sizeof(feature_t), 1);
	      f->xy = xy;
	      f->type = FERTILE;
	      f->f.fertile.level = 30;
	      add_feature(world, f, nh);
	      break;
	    case SWAMP:
	      nh->terrain = SWAMP;
	      break;
	    case PEAK:
	      nh->terrain = PEAK;
	      break;
	    case VOLCANO:
	      f = (feature_t*)calloc(sizeof(feature_t), 1);
	      f->xy = xy;
	      f->type = VOLCANO;
	      add_feature(world, f, nh);
	      nh->terrain = PEAK;
	      break;
	    case WATER:
	      nh->terrain = WATER;
	      break;
	    case WHIRLPOOL:
	      nh->terrain = WATER;
	      f = (feature_t*)calloc(sizeof(feature_t), 1);
	      f->xy = xy;
	      f->type = WHIRLPOOL;
	      add_feature(world, f, nh);
	      break;
	    case SCRUB:
	      nh->terrain = SCRUB;
	      break;
	    case TEMPLE:
	      f = (feature_t*)calloc(sizeof(feature_t), 1);
	      f->xy = xy;
	      f->type = TEMPLE;
	      add_feature(world, f, nh);
	      break;
	    case CURSED:
	      f = (feature_t*)calloc(sizeof(feature_t), 1);
	      f->xy = xy;
	      f->type = CURSED;
	      add_feature(world, f, nh);
	      break;
	    case DEN:
	      f = (feature_t*)calloc(sizeof(feature_t), 1);
	      f->xy = xy;
	      f->type = DEN;
	      f->f.den.pop = hex->lizard_pop;
	      f->f.den.type = hex->den_lizard_type;
	      f->f.den.hunger = hex->turns_den_hungry;
	      add_feature(world, f, nh);
	      nh->terrain = PLAIN;
	      break;
	    case HOME_DEN:
	      f = (feature_t*)calloc(sizeof(feature_t), 1);
	      f->xy = xy;
	      f->type = DEN;
	      f->f.den.pop = hex->lizard_pop;
	      f->f.den.type = hex->den_lizard_type;
	      f->f.den.hunger = hex->turns_den_hungry;
	      f->f.den.home = 1;
	      add_feature(world, f, nh);
	      nh->terrain = PLAIN;
	      printf("Convert home den at %d, %d for %d type %d\n",
		     xy.x, xy.y, hex->hex_owner, f->f.den.type);
	      break;
	    }
	  if ((hex->red_lizards + hex->green_lizards + hex->grey_lizards +
	       hex->black_lizards + hex->yellow_lizards) > 0)
	    {
	      f = (feature_t*)calloc(sizeof(feature_t), 1);
	      f->xy = xy;
	      f->type = BAND;
	      f->f.band.type[RED] = hex->red_lizards;
	      f->f.band.type[GREEN] = hex->green_lizards;
	      f->f.band.type[GREY] = hex->grey_lizards;
	      f->f.band.type[BLACK] = hex->black_lizards;
	      f->f.band.type[YELLOW] = hex->yellow_lizards;
	      f->f.band.immobile = hex->lizards_immobile;
	      add_feature(world, f, nh);
	      printf("Convert band at %d, %d\n",
		     xy.x, xy.y);
	    }
	  if (hex->bridge_here)
	    {
	      f = (feature_t*)calloc(sizeof(feature_t), 1);
	      f->xy = xy;
	      f->type = BRIDGE;
	      add_feature(world, f, nh);
	    }
	  if (hex->raft_here)
	    {
	      f = (feature_t*)calloc(sizeof(feature_t), 1);
	      f->xy = xy;
	      f->type = RAFT;
	      add_feature(world, f, nh);
	    }
	  for (i = NORTH; i <= NORTH_WEST; i++)
	    {
	      move_coords(world, i, x, y,
			  &nh->edge[i-1].x,
			  &nh->edge[i-1].y);
	      nh->edge[i-1].invalid = 0;
	    }
	}
    }
}
