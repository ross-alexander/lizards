#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include "lizards.h"

void debug(world_t *w, char *s, ...)
{
  char timestr[256];
  time_t t;
  struct tm *tm;
  va_list ap;
  time(&t);
  tm = localtime(&t);
  strftime(timestr, 255, "%Y-%m-%d %H:%M", tm);
  fprintf(w->debug,"%s%s %s ", w->path, w->game, timestr);
  va_start(ap, s);
  vfprintf(w->debug, s, ap);
  fprintf (w->debug, "\n");
  fflush (w->debug);
  va_end(ap);
}

void player_mesg(world_t *w, int p, char *s, ...)
{
  va_list ap;
  FILE *stream;
  va_start(ap, s);
  if (w->player[p] && (stream = w->player[p]->stream))
    vfprintf(stream, s, ap);
  va_end(ap);
}

int new_add_order(world_t *world_t, order_t *order)
{
  world_t->order_list[world_t->num_orders] = (order_t*)malloc(sizeof(order_t));
  memcpy(world_t->order_list[world_t->num_orders++], order, sizeof(order_t));
  return 1;
}

hex_t* get_hex(world_t *world_t, coord xy)
{
  if ((xy.x < 0) || (xy.x >= world_t->x) || (xy.y < 0) || (xy.y >= world_t->y))
    {
      fprintf (stdout, "Illegal hex (%d,%d)\n", xy.x, xy.y);
      exit(EXIT_FAILURE);
    }
  return &world_t->hexes[xy.y * world_t->x + xy.x];
}

coord move_coords(world_t *world_t, hex_t* hex, int dir)
{
  if (dir)
    return hex->edge[dir - 1];
  else
    return hex->xy;
}

int num_features(hex_t *hex)
{
  feature_t *ftmp;
  int cnt = 0;
  for (ftmp = hex->chain; ftmp; ftmp = ftmp->chain)
    cnt++;
  return cnt;
}

feature_t *has_feature(hex_t *hex, int f)
{
  feature_t *ftmp;
  for (ftmp = hex->chain; ftmp; ftmp = ftmp->chain)
    {
      if (ftmp->type == f) return ftmp;
    }
  return ftmp;
}

int terrain_adjacent(world_t *w, coord a, int t)
{
  int i, j;
  hex_t *hex = get_hex(w, a);
  for (j = 0, i =  0; i < 6; i++)
    if (!hex->edge[i].invalid && (get_hex(w, hex->edge[i])->terrain == t))
      j++;
  return j;
}


int feature_adjacent(world_t *w, coord a, int t)
{
  int i, j;
  hex_t *hex = get_hex(w, a);
  for (j = 0, i =  0; i < 6; i++)
    if (!hex->edge[i].invalid && (has_feature(get_hex(w, hex->edge[i]), t)))
      j++;
  return j;
}

coord new_string_to_loc (char *s)
{
  coord xy;
  char *tmp;
  tmp = (char*)alloca(strlen(s) + 1);
  strcpy(tmp, s);

  xy.y = atoi(tmp);
  tmp += xy.y < 10 ? 1 : 2;

  for (xy.x = 0; tmp[0]; tmp++)
    xy.x = (xy.x * 26) + (toupper(tmp[0]) - 'A' + 1);

  xy.x -= 1;
  xy.y -= 1;
  xy.invalid = 0;
  return xy;
}

order_t *new_order(int p, int t, coord xy)
{
  order_t *order = (order_t*)calloc(sizeof(order_t), 1);
  order->player = p;
  order->type = t;
  order->source = xy;
  order->status = ORDER_PENDING;
  return order;
}

order_t* new_order_append(order_t *a, order_t *b)
{
  order_t *c = a;
  b->chain = NULL;
  if (a == NULL)
    return b;
  while (a->chain)
    a = a->chain;
  a->chain = b;
  return c;
}

int lizards_in_band(feature_t *f)
{
  int i, j = 0;
  if (f->type == BAND)
    for (i = 0; i < LIZARD_TYPES; i++)
      j += f->f.band.type[i];
  return j;
}

int lizards_in_hex(hex_t *hex)
{
  feature_t *f;
  if ((f = has_feature(hex, BAND)))
    return lizards_in_band(f);
  else
    return 0;
}

int coord_eq(coord a, coord b)
{
  return (a.x == b.x && a.y == b.y);
}

spy_t* player_spy_here (world_t *world_t, int x, int y, int player)
{
  int work;

  for (work = 0; work < world_t->num_spies; work ++)
    if (world_t->spylist [work]->x_hex == x &&
	world_t->spylist [work]->y_hex == y &&
	world_t->spylist [work]->player == player)
      return (world_t->spylist[work]);
  return NULL;
}
char *new_show_lizards (feature_t *f,
			char *string)
{
  char number [20];
  int i, first = 1;
  string [0] = '\0';
  for (i = 0; i < LIZARD_TYPES; i++)
    if (f->f.band.type[i] > 0)
      {
	if (!first) strcat (string, ", ");
	strcat (string, itoa (f->f.band.type[i], number, 10));
	strcat (string, " ");
	strcat (string, liz_colours[i+1]);
	first = 0;
      }
  return (string);
}

int kill_lizards (feature_t *f, int deaths)
{
  int i,
    work3,
    work4;

  while (deaths > 0)
  {
    if (deaths > 10)
      i = random (deaths / 10) + 1;
    else
      i = 1;

    do
      {
	work3 = random (LIZARD_TYPES);
      }
    while ((work4 = f->f.band.type[work3]) == 0);
    if (i > work4) i = work4;
    f->f.band.type[work3] -= i;
    deaths -= i;
  }
  for (work3 = i = 0; i < LIZARD_TYPES; i++)
    work3 += f->f.band.type[i];
  return work3;
}

feature_t* create_feature(world_t *w, int type, coord xy)
{
  feature_t *f;
  if ((f = (feature_t*)calloc(sizeof(feature_t), 1)) == NULL)
    {
      fprintf(stderr, "Failed to allocate memory for a feature.\n");
      exit(EXIT_FAILURE);
    }
  f->type = type;
  f->xy = xy;
  return f;
}

feature_t* release_feature(world_t *w, feature_t *f)
{
  feature_t **ttmp;
  coord xy = f->xy;
  hex_t *hex = get_hex(w, xy);
  for (ttmp = &hex->chain; *ttmp; ttmp = &((*ttmp)->chain))
    if (*ttmp == f)
      {
	*ttmp = f->chain;
	return f;
      }
  return NULL;
}

feature_t* pop_feature(world_t *w, hex_t *hex)
{
  feature_t *f;
  if ((f = hex->chain))
    hex->chain = f->chain;
  return f;
}

void destroy_feature(world_t *w, feature_t *f)
{
  free(release_feature(w, f));
}

feature_t* add_feature(world_t *w, feature_t *f, hex_t *hex)
{
  f->chain = hex->chain;
  hex->chain = f;
  f->xy = hex->xy;
  return f;
}

feature_t* move_feature(world_t *w, feature_t *f, hex_t *hex)
{
  release_feature(w, f);
  add_feature(w, f, hex);
  return f;
}

feature_t* clone_feature(feature_t *f)
{
  feature_t *cl = (feature_t*)calloc(sizeof(feature_t), 1);
  memcpy(cl, f, sizeof(feature_t));
  return cl;
}

void clear_features (world_t *w, hex_t *hex)
{
  feature_t *f1, *f2;
  for (f1 = hex->chain; f1 != NULL; f1 = f2)
    {
      f2 = f1->chain;
      free(f1);
    }
  hex->chain = NULL;
}

int join_band(feature_t *a, feature_t *b)
{
  int i, cnt;
  for (i = 0; i < LIZARD_TYPES; i++)
    a->f.band.type[i] += b->f.band.type[i];
  for (cnt = i = 0; i < LIZARD_TYPES; i++)
    cnt += a->f.band.type[i];
  return cnt;
}

int split_band(feature_t *a, feature_t *b)
{
  int i, cnt;
  for (i = 0; i < LIZARD_TYPES; i++)
    a->f.band.type[i] -= b->f.band.type[i];
  for (cnt = i = 0; i < LIZARD_TYPES; i++)
    cnt += a->f.band.type[i];
  return cnt;
}

void inc_combat_points (feature_t *band)
{
  if (band->f.band.combat < MAX_COMBAT_POINTS)
    band->f.band.combat += 1;
}

void move_band (world_t *w, hex_t *new_hex, feature_t *band)
{
  feature_t *current;
  if (lizards_in_hex (new_hex) > 0)
    {
      new_hex->owner = band->owner;
#ifdef IgnoreThisStuff
      new_hex.combat_points =
	average_combat_points (red + green + grey + yellow + black,
			       old_hex.combat_points,
			       lizards_in_hex (&new_hex),
			       new_hex.combat_points);

      new_hex.turns_hungry =
	average_hunger_points (red + green + grey + yellow + black,
			       old_hex.turns_hungry,
			       lizards_in_hex (&new_hex),
			       new_hex.turns_hungry);
#endif
    }
  if ((current = has_feature(new_hex, BAND)))
    join_band(current, band);
  else
    add_feature(w, band, new_hex);
  new_hex->owner = band->owner;
}

coord coord_cons(int x, int y)
{
  coord xy;
  xy.x = x; xy.y = y; xy.invalid = 0;
  return xy;
}
