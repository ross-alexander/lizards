#define L(x,y) cairo_line_to(cr, x, y)
#define M(x,y) cairo_line_to(cr, x, y)
#define A(x,y,r,a1,a2) cairo_arc(cr, x, y, r, a1 * M_PI/180, a2 * M_PI/180)
#define AN(x,y,r,a1,a2) cairo_arc_negative(cr, x, y, r, a1 * M_PI/180, a2 * M_PI/180)
#define F() cairo_fill(cr)
#define ST() cairo_stroke(cr)
#define C() cairo_close_path(cr)
#define GS() cairo_save(cr)
#define GR() cairo_restore(cr)
#define FP() cairo_fill_preserve(cr)
#define RGB(r,g,b) cairo_set_source_rgb(cr, r, g, b)
#define SC(x,y) cairo_scale(cr, x, y)

#define FN(s) static void s(cairo_t *cr)

typedef void (*vf)(cairo_t*);

#define SYMN(n) static int symbols_n = n;
#define SYMS(syms...) static vf symbols[] = {syms};
