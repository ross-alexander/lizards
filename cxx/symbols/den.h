#define L(x,y) cairo_line_to(cairo, x, y)
#define M(x,y) cairo_move_to(cairo, x, y)


int cairo_den(cairo_t *cairo)
{
  cairo_save(cairo);
  cairo_new_path(cairo);

  M(36, 0);
  L(36, -8);
  L(28,-16);
  L(12,-24);
  L(-8,-24);
  L(-24,-16);
  L(-36,-4);
  L(-36, 0);
  L( 36, 0);

  M(-28, 0);
  L(-28, -8);
  L(-24, -12);
  L(-16, -12);
  L(-12, -8);
  L(-12, 0);
  L(-28, 0);

  M(-8, 0);
  L(-8, -16);
  L(-4, -20);
  L(12, -20);
  L(24, -16);
  L(28, -12);
  L(28, 0);
  L(-8, 0);

  M(-26, -6);
  L(-14, -6);

  M(-6, -8);
  L(26, -8);

  M(-4, -2);
  L(-4, -16);
  M(0, -2);
  L(0, -16);
  M(4, -2);
  L(4, -16);
  M(8, -2);
  L(8, -16);
  M(12, -2);
  L(12, -16);
  M(16, -2);
  L(16, -16);
  M(20, -2);
  L(20, -14);
  M(24, -2);
  L(24, -12);

  cairo_close_path(cairo);

  cairo_set_source_rgb(cairo, 100, 0, 0);
  cairo_fill_preserve(cairo);
  cairo_set_source_rgb(cairo, 0, 0, 200);
  cairo_set_line_cap(cairo, CAIRO_LINE_CAP_SQUARE);
  cairo_set_line_width(cairo, 0.1);
  cairo_stroke(cairo);
  cairo_restore(cairo);
}

int cairo_plains(cairo_t *cairo)
{
  cairo_save(cairo);
  cairo_new_path(cairo);

M(-17.5,0);
L(-17.5,-20);
L(-17.5,-12.5);
L(-20,-17.5);
L(-22.5,-17.5);
L(-22.5,-15);
L(-17.5,-12.5);
L(-15,-17.5);
L(-15,-15);
L(-17.5,-12.5);
L(-17.5,-7.5);
L(-20,-12.5);
L(-22.5,-12.5);
L(-22.5,-10);
L(-17.5,-7.5);
L(-15,-12.5);
L(-12.5,-12.5);
L(-17.5,-7.5);
M(0,0);
L(0,-25);
L(0,-20);
L(-2.5,-25);
L(-5,-25);
L(-5,-22.5);
L(0,-20);
L(0,-17.5);
L(2.5,-22.5);
L(5,-22.5);
L(5,-20);
L(0,-17.5);
L(0,-15);
L(-2.5,-20);
L(-5,-20);
L(-5,-17.5);
L(0,-15);
L(0,-12.5);
L(2.5,-17.5);
L(5,-17.5);
L(5,-15);
L(0,-12.5);
L(0,-12.5);
L(-2.5,-15);
L(-5,-15);
L(-5,-12.5);
L(0,-10);
L(0,-5);
L(-2.5,-10);
L(-5,-10);
L(-5,-7.5);
L(0,-5);
M(17.5,0);
L(17.5,-17.5);
L(17.5,-12.5);
L(15,-17.5);
L(12.5,-17.5);
L(12.5,-15);
L(17.5,-12.5);
L(17.5,-10);
L(20,-15);
L(22.5,-15);
L(22.5,-12.5);
L(17.5,-10);
L(17.5,-7.5);
L(15,-12.5);
L(12.5,-12.5);
L(12.5,-10);
L(17.5,-7.5);
L(17.5,-5);
L(20,-10);
L(22.5,-10);
L(22.5,-7.5);
L(17.5,-5);

  cairo_close_path(cairo);
  cairo_set_source_rgb(cairo, 100, 0, 0);
  cairo_fill_preserve(cairo);
  cairo_set_source_rgb(cairo, 0, 0, 200);
  cairo_set_line_cap(cairo, CAIRO_LINE_CAP_SQUARE);
  cairo_set_line_width(cairo, 0.1);
  cairo_stroke(cairo);
  cairo_restore(cairo);
}

int cairo_fertile(cairo_t *cairo)
{
  cairo_save(cairo);
  cairo_new_path(cairo);
M(-28,0);
L(-28,-16);
M(-24,0);
L(-24,-12);
M(-16,0);
L(-16,-20);
M(-12,0);
L(-12,-12);
M(-12,-16);
L(-12,-20);
M(-4,0);
L(-4,-16);
M(0,0);
L(0,-28);
M(8,0);
L(8,-16);
M(16,0);
L(16,-16);
M(28,0);
L(28,-24);
  cairo_close_path(cairo);
  cairo_set_source_rgb(cairo, 100, 0, 0);
  cairo_fill_preserve(cairo);
  cairo_set_source_rgb(cairo, 0, 0, 200);
  cairo_set_line_cap(cairo, CAIRO_LINE_CAP_SQUARE);
  cairo_set_line_width(cairo, 0.1);
  cairo_stroke(cairo);
  cairo_restore(cairo);
}
