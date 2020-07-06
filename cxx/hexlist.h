struct unit_t
{
  point_t xy;
  int spy, peak;
};

struct bbox_t {
  int left;
  int top;
  int right;
  int bottom;
};

struct fbox_t {
  double left;
  double top;
  double right;
  double bottom;
};

class bandlist_t {
 private:
  world_t *world;
  int lizards_in_hex(hex_t*);
 public:
  int cnt;
  unit_t *list;
  bandlist_t(world_t *, int);
  ~bandlist_t();
};

struct see_t
{
  hex_t *hex;
  int x, y, cx, cy, away, wrapped, weight;
  double mx, my;
};

class hexlist_t {
private:
  world_t *world;

  point_t find_weighted_centre(void);
  void center_points(int, int);
  void zero_points(int, int);
  int sum_distances(int, int);
  void weight_points(void);
  void mark_hex(int*, point_t, int);
  void build_list(bandlist_t*);

public:
  int cnt;
  see_t *list;

  hexlist_t(world_t *, bandlist_t*);
  hexlist_t(world_t *, int);
  ~hexlist_t();
  bbox_t center(void);
  bbox_t zero(void);
  bbox_t edge(void);
  fbox_t fcenter(void);
};
