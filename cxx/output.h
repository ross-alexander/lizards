/* ----------------------------------------------------------------------

      0.5h,-sqrt(3)/2*r    
            +------+------+           
           / \     |     / \
          /   \    |    /   \   
         /     \ sqrt(3)r    \  
        /       \  |  /       \ 
       /         \ | /         \
      /           \|/           \
      +---- r -----+-------------+
                 0,0            +r,0

System uses raster (top = 0) coordinates

--------------------------------------------------------------------- */

enum Outputs {
  OUTPUT_BLANK,
  OUTPUT_WATER,
  OUTPUT_WHIRLPOOL,
  OUTPUT_BRIDGE,
  OUTPUT_RAFT,
  OUTPUT_PLAIN,
  OUTPUT_FERTILE,
  OUTPUT_SWAMP,
  OUTPUT_SCRUB,
  OUTPUT_PEAK,
  OUTPUT_VOLCANO,
  OUTPUT_BUILDING,
  OUTPUT_DEN,
  OUTPUT_HOME_DEN,
  OUTPUT_EMPTY_DEN,
  OUTPUT_RED_DEN,
  OUTPUT_GREEN_DEN,
  OUTPUT_GREY_DEN,
  OUTPUT_BLACK_DEN,
  OUTPUT_YELLOW_DEN,
  OUTPUT_TEMPLE,
  OUTPUT_CURSED,
  OUTPUT_RUIN,
  OUTPUT_BAND,
  OUTPUT_LIZARD,
  OUTPUT_LAST,
};

class output_t {
 public:
  world_t *world;
  output_t(world_t*);
  Outputs order(hex_t*);
  void display();
};
    
class output_ps_t : output_t {
  void show_hex(int, int, int, FILE*);
  void show_world(int, FILE*);
 public:
  output_ps_t(world_t*);
  void display();
};

class output_svg_t : output_t {
 public:
  output_svg_t(world_t*);
  void display();
};

class output_glx_t : output_t {
 private:
  int argc;
  char **argv;
 public:
  output_glx_t(world_t*, int, char **);
  void display();
  world_t *w();
};

class output_glu_t : output_t {
 private:
  int argc;
  char **argv;
  int quit;
 public:
  output_glu_t(world_t*, int, char **);
  void display();
  world_t *w();
};

class output_cairo_t : public output_t {
 protected:
  int argc;
  char **argv;
 public:
  RsvgHandle * cairo_svg_outputs[OUTPUT_LAST];
  class cairo_tab_t **tabs;

  output_cairo_t(world_t*, int, char **);
  void display();
};

class output_gtk_t : public output_cairo_t {
 public:
  output_gtk_t(world_t*, int, char **);
  void display();
  void draw(cairo_t *, struct gtk_tab_t *);
};

class cairo_tab_t {
public:
  world_t *world;
  output_cairo_t *parent;
  int player;
  char *label;
  hexlist_t *hlist;
  fbox_t box, edge, border;
  double radius;
  int width;
  int height;
  void draw(cairo_t*);
  cairo_tab_t(output_cairo_t *, int);
};
