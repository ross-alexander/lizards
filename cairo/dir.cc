
dir_t::dir_t()
{
  d = -1;
}

dir_t::dir_t(int p)
{
  d = p;
}

struct dirmap_t {
  const char *s;
  const char *l;
  Direction d;
};

static dirmap_t dirs[] = {
  { "N",  "North",     NORTH },
  { "NE", "Northeast", NORTHEAST },
  { "SE", "Southeast", SOUTHEAST },
  { "S",  "South",     SOUTH },
  { "SW", "Southwest", SOUTHWEST },
  { "NW", "Northwest", NORTHWEST },
};

dir_t::dir_t(const char *dir)
{
  d = -1;
  for (unsigned int i = 0; i < DIRECTIONS; i++)
    {
      if (strcmp(dir, dirs[i].s) == 0)
	d = dirs[i].d;
    }
}

dir_t::operator char*()
{
  if (d < 0 || d >= (int)DIRECTIONS)
    return (char*)"Illegal direction";
  else
    return (char*)dirs[d].s;
}

dir_t::operator int()
{
  return d;
}

dir_t& dir_t::operator-=(int i)
{
  i %= DIRECTIONS;
  d -= i;
  if (d < 0)
    d += DIRECTIONS;
  d %= DIRECTIONS;
  return *this;
}

dir_t& dir_t::operator+=(int i)
{
  i %= DIRECTIONS;
  d += i;
  d %= DIRECTIONS;
  return *this;
}

int dir_t::okay()
{
  return (d < 0 || d > (int)DIRECTIONS) ? 0 : 1;
}

int dir_t::operator()(char*str)
{
  dir_t tmp(str);
  d = tmp.d;
  return d;
}

dirs_t::dirs_t()
{
  text = NULL;
}

dirs_t::dirs_t(dirs_t&a)
{
  d[0] = a.d[0];
  d[1] = a.d[1];
  text = NULL;
}

dirs_t::dirs_t(dir_t d1, dir_t d2)
{
  text = NULL;
  d[0] = d1;
  d[1] = d2;
}

dirs_t::~dirs_t()
{
  if (text)
    delete text;
}

dir_t& dirs_t::operator[](int i)
{
  assert(i >= 0 && i < 2);
  return d[i];
}

dirs_t::operator char*()
{
  //  assert(text == NULL);

  if (text)
    return text;
  if (!d[0].okay())
    {
      text = g_strdup_printf("%s", "");
    }
  else
    {
      if (d[1].okay())
	text = g_strdup_printf("%s %s", (char*)d[0], (char*)d[1]);
      else
	text = g_strdup_printf("%s", (char*)d[0]);
    }
  return text;
}
