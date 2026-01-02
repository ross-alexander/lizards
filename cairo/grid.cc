#include <string>
#include <map>
#include <vector>
#include <typeinfo>
#include <assert.h>
#include <sys/syslog.h>

#include <fmt/format.h>
#include <fmt/printf.h>

#include <cairomm/context.h>
#include <librsvg/rsvg.h>

#include "lizards.h"

/* ----------------------------------------------------------------------
--
-- grid_t
--
---------------------------------------------------------------------- */
grid_t::grid_t(int w, int h)
{
  assert(w > 0);
  assert(h > 0);
  size = w * h;
  grid = new hex_t*[size];
  for (int i = 0; i < size; i++)
    grid[i] = 0;
  width = w;
  height = h;
  misc_t::log(LOG_NOTICE, "grid (%d x %d) created", width, height);
}

hex_t* grid_t::operator()(int x, int y)
{
  assert((x >= 0) && (x < width) && (y >= 0) && (y < height));
  return (*this)[y * width + x];
}

hex_t* grid_t::operator()(point_t p)
{
  return (*this)(p.x, p.y);
}

hex_t* grid_t::operator[](int i)
{
  assert((i >= 0) && i < (width * height));
  return grid[i];
}

/* ----------------------------------------------------------------------
--
-- place_template
--
---------------------------------------------------------------------- */

void grid_t::place_template(template_t *tmpl, int owner, point_t c, int rename)
{
  assert(c.x + tmpl->layout->x_min >= 0);
  assert(c.x - tmpl->layout->x_max < width);

  assert(c.y + tmpl->layout->y_min >= 0);
  assert(c.y - tmpl->layout->y_max < height);

  for (auto &i : tmpl->map)
    {
      hex_t *t = i.second;
      point_t p = c + t->xy;
      hex_t *old = dynamic_cast<hex_t*>((*this)(p));
      if (old) delete old;
      t->xy = p;
      t->owner = owner;
      set(p, t, rename);
    }
}

/* ----------------------------------------------------------------------
--
-- fbox
--
-- Return flouting point bounding box based radius r
--
---------------------------------------------------------------------- */


fbox_t grid_t::fbox(double r)
{
  fbox_t f;
  int k;
  for (k = 0; (k < size) && ((*this)[k] == 0); k++)
    ;
  if (k == size)
    return f;

  double of = abs(grid[k]->xy.x%2)/2.0;
  f.left = f.right = grid[k]->xy.x;
  f.top = f.bottom = grid[k]->xy.y - of;

  for (; k < size; k++)
    {
      if (grid[k] == 0)
	continue;
      point_t p = (*this)[k]->xy;
      double of = abs(p.x%2)/2.0;
      if (f.left > p.x) f.left = p.x;
      if (f.right < p.x) f.right = p.x;
      if (f.bottom > p.y - of) f.bottom = p.y - of;
      if (f.top < p.y - of) f.top = p.y - of;
    }

  f.left = (1.5*f.left - 1) * r;
  f.bottom = (f.bottom - 0.5) * sqrt(3)*r;
  f.right = (1.5*f.right + 1) * r;
  f.top = (f.top + 0.5) * sqrt(3)*r;
  return f;
}

/* ----------------------------------------------------------------------
--
-- move
--
---------------------------------------------------------------------- */

point_t grid_t::move(point_t p, int dir)
{
  point_t np = p;
  assert(dir >= NORTH && dir <= NORTHWEST);
  switch (dir)
    {
    case NORTH:
      np.y -= 1;
      break;
    case NORTHEAST:
      np.x += 1;
      if ((p.x % 2) == 1)	// x == odd
	np.y -= 1;
      break;
    case SOUTHEAST:
      np.x += 1;
      if ((p.x % 2) == 0)	// x == even
	np.y += 1;
      break;
    case SOUTH:
      np.y += 1;
      break;
    case SOUTHWEST:
      np.x -= 1;
      if ((p.x % 2) == 0)	// x == odd
	np.y += 1;
      break;
    case NORTHWEST:
      np.x -= 1;
      if ((p.x % 2) == 1)	// x == even
	np.y -= 1;
      break;
    case DIRECTIONS:
      break;
    }
  if (np.x < 0)
    np.x += width;
  else if (np.x >= width)
    np.x -= width;
  
  if (np.y < 0)
    np.y += height;
  else if (np.y >= height)
    np.y -= height;
  
  return np;
}

/* ----------------------------------------------------------------------
--
-- adjacent
--
---------------------------------------------------------------------- */

int grid_t::adjacent(point_t p, const char* t)
{
  int c = 0;
  for (int i = 0; i < DIRECTIONS; i++)
    {
      point_t q = move(p, i);
      if ((*this)(q)->isa(t))
	c++;
    }
  return c;
}

/* ----------------------------------------------------------------------
--
-- set
--
---------------------------------------------------------------------- */

hex_t* grid_t::set(point_t p, hex_t *h, int rename)
{
  assert(p.x >= 0 && p.x < width && p.y >= 0 && p.y < height);
  hex_t *t = grid[p.y * width + p.x];
  grid[p.y * width + p.x] = h;
  h->xy = p;

  char string[12]; // Maximum 26^3 x and 999 y
  sprintf(string, "%d", p.y + 1);
  int k = strlen(string);
  if (p.x >= 26)
    {
      string [k++] = ('A' + (p.x / 26) - 1);
    }
  string [k+0] = ('A' + p.x%26);
  string [k+1] = '\0';

  if (rename)
    h->setid(std::string(string));
  return t;
}

void grid_t::replace(point_t p, hex_t *h)
{
  delete set(p, h, 1);
}

hex_t* grid_t::random()
{
  return (*this)(misc_t::uniform(width), misc_t::uniform(height));
}

/* ----------------------------------------------------------------------
--
-- lay_island
--
---------------------------------------------------------------------- */

int grid_t::lay_island(point_t p, double land_density)
{
  int land = 0;

  if (misc_t::uniform(100) < (100 * land_density))
    {
      replace(p, new plains_t);
      land++;
    }

  for (int dir = 0; dir < DIRECTIONS; dir++)
    {
      point_t z = this->move(p, dir);
      replace(z, new plains_t);
      land++;
    }

  /* --------------------
     Check edge proximity
     -------------------- */
  
  int proximity =  ((p.x <= (width / 5) || p.x >= (width - (width / 5))) &&
		    (p.y <= (height / 5) || p.y >= (height - (height / 5)))) ?
    (140 * land_density) : (60 * land_density);

  if (misc_t::uniform(100) < proximity)
    {
      int start = misc_t::uniform(DIRECTIONS);
      int dir = (misc_t::uniform(100) < (100 * land_density)) ? 1 : -1;

      for (int i = 0; i < DIRECTIONS && misc_t::uniform(100) < 35; i++)
	{
	  start = (start + dir + DIRECTIONS) % DIRECTIONS;
	  point_t z = move(p, start);
	  hex_t *a_hex = (*this)(z);
	  if (dynamic_cast<plains_t*>(a_hex))
	    {
	      land--;
	      replace(p, new water_t);
	    }
	}
    }
  return land;
}

/* ----------------------------------------------------------------------
--
-- lay_islands
--
---------------------------------------------------------------------- */

int grid_t::lay_islands(double land_density)
{
  fprintf(stdout, "Layout down water...\n");
  for (int y = 0; y < height; y++)
    for (int x = 0; x < width; x++)
      set(point_t(x, y), new water_t, 1);

  check();

  fprintf(stdout, "Raising Islands I...\n");
  int land = 0;
  for (int y = 1; y < height; y += 4)
    for (int x = 1; x < width; x += 4)
      land += lay_island(point_t(x, y), land_density);

  check();

  int land_diff = (width * height * land_density - land);
  int diff_pcnt = ((double)(land_diff) / (width * height - land) * 100);

  fprintf(stdout, "Raising Islands II...\n");
  for (int y = 0; y < height; y++)
    for (int x = 0; x < width; x++)
      {
	hex_t *hex = (*this)(x, y);
	if (land_diff > 0)
	  {
	    if (dynamic_cast<water_t*>(hex) && (misc_t::uniform(100) < diff_pcnt))
	      {
		replace(point_t(x, y), new plains_t);
		land++;
	      }
	  }
	else
	  {
	    if (dynamic_cast<plains_t*>(hex) && (misc_t::uniform(100) < -diff_pcnt))
	      {
		replace(point_t(x, y), new water_t);
		land--;
	      }
	  }
      }

  check();
  return land;
}

void grid_t::generate_typical()
{
  double den_p = 0.18,
    ruin_p = 0.02,
    swamp_p = 0.18,
    fertile_p = 0.04,
    peak_p = 0.015,
    volcano_p = 0.015,
    whirlpool_p = 0.01,
    scrub_p = 0.15,
    temple_p = 0.02,
    cursed_p = 0.0,
    plain_p = 0.55,
    land_density = 0.50;

  double dens_p[UNSETTLED];
  dens_p[RED] = 0.20;
  dens_p[GREEN] = 0.20;
  dens_p[GREY] = 0.20;
  dens_p[BLACK] = 0.20;
  dens_p[YELLOW] = 0.20;

  int dens_a[UNSETTLED];
  int dens_n[UNSETTLED];

  int land = lay_islands(land_density);

  int den_n = (int)((double) land * den_p);
  for (int i = 0; i < UNSETTLED; i++)
    {
      dens_n[i] = den_n * dens_p[i];
      dens_a[i] = 0;
    }

  int ruin_n = (int)((double) land * ruin_p);
  int swamp_n = (int)((double) land * swamp_p);
  int fertile_n = (int)((double) land * fertile_p);
  int peak_n = (int)((double) land * peak_p);
  int volcano_n = (int)((double) land * volcano_p);
  int whirlpool_n = (int)((double) land * whirlpool_p);
  int scrub_n = (int)((double) land * scrub_p);
  int temple_n = (int)((double) land * temple_p);
  int plain_n = (int)((double) land * plain_p);
  int cursed_n = (int)((double) land * cursed_p);

  int work;
  hex_t *hex;

  while (dens_n[RED] + dens_n[GREEN] + dens_n[GREY] + dens_n[BLACK] + dens_n[YELLOW] < den_n)
    {
      int t = misc_t::uniform(UNSETTLED);
      if (dens_p[t] != 0.0)
	dens_n[t]++;
    }

  printf ("World has %d land hexes and %d water hexes.\n", land,
	  (width * height) - land);
  printf ("World needs %d den (%d red, %d green, %d grey, %d black, %d yellow),\n"
	  "%d ruin, %d swamp, %d fertile, %d peak, %d volcano, %d whirlpool,\n"
	  "%d scrub, %d temple, %d plain, %d cursed hexes.\n",
	  den_n, dens_n[RED], dens_n[GREEN], dens_n[GREY], dens_n[BLACK], dens_n[YELLOW],
	  ruin_n, swamp_n, fertile_n, peak_n, volcano_n, whirlpool_n, scrub_n, temple_n,
	  plain_n, cursed_n);

  int den_a = 0;

  int ruin_a = 0;
  int swamp_a = 0;
  int fertile_a = 0;
  int peak_a = 0;
  int volcano_a = 0;
  int whirlpool_a = 0;
  int scrub_a = 0;
  int temple_a = 0;
  int plain_a = 0;
  int cursed_a = 0;

  point_t q;
  printf ("Placing Dens...");
  while (den_a < den_n)
    {
      hex_t *hex;
      do hex = random(); while (hex->isa(WATER));
      q = hex->xy;
      if (!(hex->isa(PLAINS) &&
	    !hex->isa(DEN) &&
	    adjacent(q, PLAINS) > 0 &&
	    adjacent(q, FERTILE) == 0 &&
	    adjacent(q, DEN) == 0))
	  continue;
      den_t* den = 0;
      while (den == 0)
	{
	  int den_type = misc_t::uniform(UNSETTLED);
	  if (dens_a[den_type] < dens_n[den_type])
	    {
	      dens_a[den_type]++;
	      den = den_t::factory(den_type);
	      den_a++;
	      replace(q, den);

	      assert(den->xy == q);

	      int nfertile = 0;
	      for (int counter = 0; (nfertile == 0 && counter < 20); counter++)
		{
		  for (int d = 0; d < DIRECTIONS; d++)
		    {
		      point_t r = move(q, d);
		      hex_t *t = (*this)(r);
		      if (misc_t::uniform(100) < 50 && nfertile < 4 && t->isa(PLAINS))
			{
			  replace(r, new fertile_t);
			  nfertile++;
			}
		    }
		}
	      if (nfertile > 0)
		{
		  den->pop = DEN_LIZARD_LEVEL * nfertile;
		  int pop = (int)((double)(den->pop) * DEN_MILITIA_LEVEL);
		  band_t *band = new band_t();
		  band->set(den->type, pop);
		  den->set(band);
		}
	    }
	}
    }
  printf("\n");
  
  printf ("Creating Temples...");
  for (work = 0; work < temple_n; work ++)
    {
      do hex = random(); while (!hex->isa(PLAINS) && adjacent(hex->xy, DEN));
      temple_a++;
      replace(hex->xy, new temple_t);
    }
  printf (" \n");

  printf ("Weathering Ruins..");
  for (work = 0; work < ruin_n; work ++)
    {
      hex_t *hex;
      for (int j = 0; j < 100; j++)
	{
	  do {
	    q = point_t(misc_t::uniform(width), misc_t::uniform(height));
	    hex = (*this)(q);
	  } while (!hex->isa(PLAINS));
	  if (adjacent(q, DEN) == 0 &&
	      adjacent(q, TEMPLE) == 0)
	    {
	      ruin_a ++;
	      replace(q, new ruin_t);
	      break;
	    }
	}
    }

  for (int y = 0; y < height; y ++)
    for (int x = 0; x < width; x ++)
      assert((*this)(x, y));

  printf ("\n");


  printf ("Cursing Cursed...");
  for (work = 0; work < cursed_n; work ++)
    {
      do hex = (*this)(misc_t::uniform(width), misc_t::uniform(height)); while (!(hex->isa(PLAINS)));
      cursed_a ++;
      replace(hex->xy, new cursed_t());
    }
  printf (" \n");

  printf ("Sinking Swamps...");
  for (work = 0; work < swamp_n; work ++)
    {
      do hex = (*this)(misc_t::uniform(width), misc_t::uniform(height)); while (!(hex->isa(PLAINS)));
      swamp_a ++;
      replace(hex->xy, new swamp_t);
    }
  printf (" \n");

  printf ("Growing Fertile...");
  for (work = 0; work < fertile_n; work ++)
    {
      fertile_a ++;
      do hex = (*this)(misc_t::uniform(width), misc_t::uniform(height)); while (!(hex->isa(PLAINS)) && adjacent(hex->xy, DEN));
      replace(hex->xy, new fertile_t);
    }
  printf (" \n");

  printf ("Raising Peaks...");
  for (work = 0; work < peak_n; work ++)
    {
      do hex = (*this)(misc_t::uniform(width), misc_t::uniform(height)); while (!(hex->isa(PLAINS)));
      peak_a ++;
      replace(hex->xy, new peak_t);
    }
  printf (" \n");

  printf ("Raising Volcanos...");
  for (work = 0; work < volcano_n; work ++)
    {
      do hex = (*this)(misc_t::uniform(width), misc_t::uniform(height));
      while (!hex->isa(PLAINS) ||
	     adjacent(hex->xy, DEN) ||
	     adjacent(hex->xy, TEMPLE) ||
	     adjacent(hex->xy, FERTILE));
      volcano_a ++;
      replace(hex->xy, new volcano_t);
    }
  printf (" \n");

  printf ("Sinking Whirlpools...");
  for (work = 0; work < whirlpool_n; work ++)
    {
      do hex = (*this)(misc_t::uniform(width), misc_t::uniform(height));
      while (!hex->isa(WATER));
      whirlpool_a ++;
      replace(hex->xy, new whirlpool_t);
    }
  printf ("\n");

  printf ("Planting Scrub...");
  for (work = 0; work < scrub_n; work ++)
    {
      do hex = (*this)(misc_t::uniform(width), misc_t::uniform(height));
      while (!hex->isa(PLAINS));
      scrub_a ++;
      replace(hex->xy, new scrub_t);
    }
  printf ("\n");
  
  for (work = 0; work < size; work++)
    if ((*this)[work]->isa(PLAINS))
      plain_a++;

  printf ("World gets %d den (%d red, %d green, %d grey, %d black, %d yellow),\n"
	  "%d ruin, %d swamp, %d fertile, %d peak, %d volcano, %d whirlpool,\n"
	  "%d scrub, %d temple, %d plain, %d cursed hexes.\n", den_a,
	  dens_a[RED], dens_a[GREEN],dens_a[GREY], dens_a[BLACK], dens_a[YELLOW],
	  ruin_a, swamp_a,
	  fertile_a, peak_a, volcano_a, whirlpool_a, scrub_a, temple_a,
	  plain_a, cursed_a);
}

int grid_t::check()
{
  for (int x = 0; x < width; x++)
    for (int y = 0; y < height; y++)
      {
	hex_t *h = (*this)(x, y);
	assert(h != 0);
	assert(h->xy.x == x && h->xy.y == y);
      }
  return 1;
}

/* ----------------------------------------------------------------------
--
-- output
--
---------------------------------------------------------------------- */

output_t* grid_t::output(int p)
{
  output_t* output = new output_t(this, p);
  return output;
}

/* ----------------------------------------------------------------------
--
-- save
--
---------------------------------------------------------------------- */
serial_t grid_t::serialize()
{
  serial_map_t g;
  g.add("width", width);
  g.add("height", height);
  serial_array_t hexes;
  g.add("hexes", hexes);
  for (int j = 0; j < height; j++)
    for (int i = 0; i < width; i++)
      hexes.add((*this)(i,j)->serialize());
  return g;
}

grid_t::grid_t(serial_t s)
{
  assert(s.get("width", width));
  assert(s.get("height", height));
  misc_t::log(LOG_DEBUG, "grid_t reloading %dx%d", width, height);
  serial_array_t h;
  int size = width * height;
  grid = new hex_t*[size];
  for (int i = 0; i < size; i++)
    grid[i] = 0;

  assert(s.get("hexes", h));
  size = h.size();

  misc_t::log(LOG_DEBUG, "grid_t with %d hexes", size);

  for(int i = 0; i < size; i++)
    {
      hex_t *hex = hex_t::factory(h[i]);
      set(hex->xy, hex, 1);
    }
}

/* ----------------------------------------------------------------------
--
-- describe
--
---------------------------------------------------------------------- */
std::string grid_t::tostring()
{
  return fmt::sprintf("grid (%02d x %02d)", width, height);
}

/* ----------------------------------------------------------------------
--
-- owned
--
---------------------------------------------------------------------- */

std::vector<hex_t*> grid_t::owned(int p, int which)
{
  std::vector<hex_t*> owned;
  for (int i = 0; i < width; i++)
    for (int j = 0; j < height; j++)
      {
	hex_t* h = (*this)(i, j);
	switch(which)
	  {
	  case OWNED_BAND_ONLY:
	    if (h->owner == p && h->get(BAND))
	      owned.push_back(h);
	    break;
	  case OWNED_ALL:
	    if (h->owner == p)
	      owned.push_back(h);
	    break;
	  }
      }
  return owned;
}

/* ----------------------------------------------------------------------
--
-- all
--
---------------------------------------------------------------------- */

std::vector<hex_t*>grid_t::all()
{
  std::vector<hex_t*> all;
  for (int i = 0; i < width; i++)
    for (int j = 0; j < height; j++)
      {
	hex_t* hex = (*this)(i, j);
	if (hex) all.push_back(hex);
      }
  return all;
}
