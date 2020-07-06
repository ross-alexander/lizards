#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#include <libxml/tree.h>
#include <libxml++/libxml++.h>

#include "lizards.h"
#include "hexlist.h"


/* ----------------------------------------------------------------------
--
-- min
--
---------------------------------------------------------------------- */

static int min(int a, int b)
{
  return a > b ? b : a;
}

/* ----------------------------------------------------------------------
--
-- edge
--
---------------------------------------------------------------------- */

bbox_t hexlist_t::edge(void)
{
  int l = 2 * world->map->width;
  int r = -l;
  int t = 2 * world->map->height;
  int b = -t;

  for (int i = 0; i < cnt; i++)
    {
      int x = list[i].x;
      int y = list[i].y;
      if (x < l) l = x;
      if (x > r) r = x;
      if (y > b) b = y;
      if (y < t) t = y;
    }
  bbox_t bb;
  bb.right = r;
  bb.bottom = b;
  bb.left = l;
  bb.top = t;
  return bb;
}

/* ----------------------------------------------------------------------
--
--
--
---------------------------------------------------------------------- */

static int see_list_centred_y_sort (const void *elem1, const void *elem2)
{
  if (((see_t *)elem1)->cy < ((see_t *)elem2)->cy)
    return (-1);
  else
    if (((see_t *)elem1)->cy > ((see_t *)elem2)->cy)
      return (1);
    else
      return (0);
}

int hexlist_t::sum_distances (int x, int y)
{
  int total = 0;
  for (int i = 0; i < cnt; i++)
    {
      see_t* ptr = &list[i];
      int xd = abs(ptr->x - x);
      int yd = abs(ptr->y - y);
      total += min(xd, world->map->width - xd);
      total += min(yd, world->map->height - yd);
    }
  return (total);
}

void hexlist_t::weight_points(void)
{
  if (cnt == 0) return;
  for (int i = 0; i < cnt; i++)
    list[i].weight = sum_distances(list[i].x, list[i].y);
}

point_t hexlist_t::find_weighted_centre(void)
{
  int lowest = 0;
  for (int i = 0; i < cnt; i++)
    {
      if (list[i].weight < list[lowest].weight)
	lowest = i;
    }
  return point_t(list[lowest].x, list[lowest].y);
}

void hexlist_t::center_points (int cx, int cy)
{
  cx -= cx%2;
  for (int i = 0; i < cnt; i++)
    {
      see_t *ptr = &list[i];

      //      x = ptr->x - cx + (world->map->width / 2);
      //      if (x < 0) x += world->map->width;
      //      if (x >= world->map->width) x = x % world->map->width;
      //      x = (ptr->x - cx + (world->map->width * 3 / 2)) % world->map->width;

      //      y = ptr->y - cy + (world->map->height / 2);
      //      if (y < 0) y += world->map->height;
      //      if (y >= world->map->height) y = y % world->map->height;

      ptr->cx = (ptr->x - cx + (world->map->width * 3 / 2)) % world->map->width;
      ptr->cy = (ptr->y - cy + (world->map->height * 3 / 2)) % world->map->height;
    }
}

bbox_t hexlist_t::center(void)
{
  weight_points();
  point_t centre = find_weighted_centre();
  center_points(centre.x, centre.y);
  qsort (list, cnt, sizeof (see_t), see_list_centred_y_sort);
  return edge();
}

void hexlist_t::zero_points (int cx, int cy)
{
  cx -= cx%2;
  for (int i = 0; i < cnt; i++)
    {
      see_t *ptr = &list[i];
      ptr->cx = ptr->cx - cx;
      ptr->cy = ptr->cy - cy;
      printf("zero (%s %d, %d) - (%d, %d) = (%d, %d)\n", (*world->map)(ptr->x, ptr->y)->title, ptr->x, ptr->y, cx, cy, ptr->cx, ptr->cy);
    }
}

bbox_t hexlist_t::zero(void)
{
  weight_points();
  point_t centre = find_weighted_centre();

  center_points(centre.x, centre.y);

  qsort (list, cnt, sizeof (see_t), see_list_centred_y_sort);
  return edge();
}

fbox_t hexlist_t::fcenter(void)
{
  weight_points();
  point_t centre = find_weighted_centre();
  center_points(centre.x, centre.y);
  qsort (list, cnt, sizeof (see_t), see_list_centred_y_sort);

  fbox_t fb;
  fb.left = 2 * world->map->width;
  fb.right = -2 * world->map->width;
  fb.top = 2 * world->map->height;
  fb.bottom = -2 * world->map->height;

  for (int i = 0; i < cnt; i++)
    {
      see_t *see = &list[i];
      see->mx = 1.5 * see->cx;
      see->my = sqrt(3) * (see->cy - abs(see->cx%2)/2.0);

      if ((see->mx - 1.0) < fb.left)
	fb.left = see->mx - 1.0;
      if ((see->mx + 1.0) > fb.right)
	fb.right = see->mx + 1.0;
      if ((see->my - sqrt(3)/2.0) < fb.top)
	fb.top = see->my - sqrt(3)/2.0;
      if ((see->my + sqrt(3)/2.0) > fb.bottom)
	fb.bottom = see->my + sqrt(3)/2.0;
    }
  return fb;
}

/* ----------------------------------------------------------------------
--
-- build_list
--
---------------------------------------------------------------------- */

void hexlist_t::build_list(bandlist_t *ulist)
{
  int work, i, j, *seen;
  see_t *see_ptr;
  int see_hex;

  if ((seen = (int*)calloc (world->map->size, sizeof(int))) == NULL)
    {
      printf ("FATAL ERROR: Out of memory line __LINE__ of __FILE__.\n");
      exit (EXIT_FAILURE);
    }

  for (work = 0; work < ulist->cnt; work++)
    mark_hex(seen, ulist->list[work].xy, ulist->list[work].peak ? 4 : 3);


  see_hex = 0;
  for (j = 0; j < world->map->height; j++)
    for (i = 0; i < world->map->width; i++)
      if (seen[j * world->map->width + i])
	see_hex++;

  if ((see_ptr = (see_t*)calloc (see_hex, sizeof (see_t))) == NULL)
    {
      printf ("FATAL ERROR: Out of memory line __LINE__ of __FILE__.\n");
      exit (EXIT_FAILURE);
    }
  see_hex = 0;
  for (j = 0; j < world->map->height; j++)
    for (i = 0; i < world->map->width; i++)
      if (seen[j * world->map->width + i])
	{
	  see_ptr[see_hex].x = i;
	  see_ptr[see_hex].y = j;
	  see_ptr[see_hex].away = seen[j * world->map->width + i];
	  see_ptr[see_hex].hex = (*world->map)(i, j);
	  see_hex++;
	}
  cnt = see_hex;
  list = see_ptr;
  free(seen);
}

hexlist_t::hexlist_t(world_t *w, bandlist_t* ulist)
{
  world = w; 
  build_list(ulist);
}

hexlist_t::hexlist_t(world_t *w, int p)
{
  world = w;
  bandlist_t* b = new bandlist_t(w, p);
  build_list(b);
  delete b;
}

void hexlist_t::mark_hex(int *seen, point_t xy, int depth)
{
  int i, j;
  hex_t *hex = (*world->map)(xy);
  j = world->map->width * hex->xy.y + hex->xy.x;

  if (seen[j] < depth)
    seen[j] = depth;

  assert(hex->map);

  if (depth > 1)
    for (i = 0; i < DIRECTIONS; i++)
      if (hex->adjacent(i))
	{
	  assert(hex->xy.x >= 0 && hex->xy.x < world->map->width);
	  assert(hex->xy.y >= 0 && hex->xy.y < world->map->height);
	  mark_hex(seen, hex->adjacent(i)->xy, depth - 1);
	}
}

hexlist_t::~hexlist_t()
{
  free(list);
}

/* ----------------------------------------------------------------------
--
-- build_unit_list
--
---------------------------------------------------------------------- */

int bandlist_t::lizards_in_hex(hex_t *hex)
{
  band_t *band;
  if ((band = (band_t*)hex->has_feature(BAND)))
    {
      return band->size();
    }
  else
    return 0;
}

bandlist_t::bandlist_t(world_t *world, int player)
{
  int x, y;
  hex_t *hex;
  unit_t *work_unit;

  printf("Building band list for player %d\n", player);

  cnt = 0;
  for (x = 0; x < world->map->width; x++)
    for (y = 0; y < world->map->height; y++)
      {
	hex = (*world->map)(x, y);
	if ((hex->owner == player) && (lizards_in_hex(hex) > 0))
	  cnt++;
      }

  /* cnt is upper limit on total number of possible units */

  if (!(list = (unit_t*)calloc(1, sizeof(unit_t) * cnt)))
    {
      fprintf(stderr, "calloc failed in line __LINE__ of file __FILE__\n");
      exit(EXIT_FAILURE);
    }
  cnt = 0;
  for (x = 0; x < world->map->width; x++)
    for (y = 0; y < world->map->height; y++)
      {
	hex = (*world->map)(x, y);
	if ((hex->owner == player) && (lizards_in_hex (hex) > 0))
	  {
	    work_unit = &list[cnt];
	    work_unit->xy.x = x;
	    work_unit->xy.y = y;
	    work_unit->spy = 0;
	    if (hex->has_feature(PEAK))
	      work_unit->peak = 1;
	    else
	      work_unit->peak = 0;
	    cnt++;
	  }
      }
  printf("Player %d had %d units.\n", player, cnt);
}

bandlist_t::~bandlist_t()
{
  free(list);
}
