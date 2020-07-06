# 1 "symbols.c"
# 1 "<built-in>"
# 1 "<command line>"
# 1 "symbols.c"
# 1 "/opt/cairo/include/cairo/cairo.h" 1
# 41 "/opt/cairo/include/cairo/cairo.h"
# 1 "/opt/cairo/include/cairo/cairo-features.h" 1
# 42 "/opt/cairo/include/cairo/cairo.h" 2


# 55 "/opt/cairo/include/cairo/cairo.h"
int
cairo_version (void);

const char*
cairo_version_string (void);
# 75 "/opt/cairo/include/cairo/cairo.h"
typedef int cairo_bool_t;







typedef struct _cairo cairo_t;
# 97 "/opt/cairo/include/cairo/cairo.h"
typedef struct _cairo_surface cairo_surface_t;
# 116 "/opt/cairo/include/cairo/cairo.h"
typedef struct _cairo_matrix {
    double xx; double yx;
    double xy; double yy;
    double x0; double y0;
} cairo_matrix_t;

typedef struct _cairo_pattern cairo_pattern_t;
# 132 "/opt/cairo/include/cairo/cairo.h"
typedef void (*cairo_destroy_func_t) (void *data);
# 144 "/opt/cairo/include/cairo/cairo.h"
typedef struct _cairo_user_data_key {
    int unused;
} cairo_user_data_key_t;
# 176 "/opt/cairo/include/cairo/cairo.h"
typedef enum _cairo_status {
    CAIRO_STATUS_SUCCESS = 0,
    CAIRO_STATUS_NO_MEMORY,
    CAIRO_STATUS_INVALID_RESTORE,
    CAIRO_STATUS_INVALID_POP_GROUP,
    CAIRO_STATUS_NO_CURRENT_POINT,
    CAIRO_STATUS_INVALID_MATRIX,
    CAIRO_STATUS_INVALID_STATUS,
    CAIRO_STATUS_NULL_POINTER,
    CAIRO_STATUS_INVALID_STRING,
    CAIRO_STATUS_INVALID_PATH_DATA,
    CAIRO_STATUS_READ_ERROR,
    CAIRO_STATUS_WRITE_ERROR,
    CAIRO_STATUS_SURFACE_FINISHED,
    CAIRO_STATUS_SURFACE_TYPE_MISMATCH,
    CAIRO_STATUS_PATTERN_TYPE_MISMATCH,
    CAIRO_STATUS_INVALID_CONTENT,
    CAIRO_STATUS_INVALID_FORMAT,
    CAIRO_STATUS_INVALID_VISUAL,
    CAIRO_STATUS_FILE_NOT_FOUND,
    CAIRO_STATUS_INVALID_DASH
} cairo_status_t;
# 215 "/opt/cairo/include/cairo/cairo.h"
typedef cairo_status_t (*cairo_write_func_t) (void *closure,
           const unsigned char *data,
           unsigned int length);
# 235 "/opt/cairo/include/cairo/cairo.h"
typedef cairo_status_t (*cairo_read_func_t) (void *closure,
          unsigned char *data,
          unsigned int length);


cairo_t *
cairo_create (cairo_surface_t *target);

cairo_t *
cairo_reference (cairo_t *cr);

void
cairo_destroy (cairo_t *cr);

void
cairo_save (cairo_t *cr);

void
cairo_restore (cairo_t *cr);
# 265 "/opt/cairo/include/cairo/cairo.h"
typedef enum _cairo_operator {
    CAIRO_OPERATOR_CLEAR,

    CAIRO_OPERATOR_SOURCE,
    CAIRO_OPERATOR_OVER,
    CAIRO_OPERATOR_IN,
    CAIRO_OPERATOR_OUT,
    CAIRO_OPERATOR_ATOP,

    CAIRO_OPERATOR_DEST,
    CAIRO_OPERATOR_DEST_OVER,
    CAIRO_OPERATOR_DEST_IN,
    CAIRO_OPERATOR_DEST_OUT,
    CAIRO_OPERATOR_DEST_ATOP,

    CAIRO_OPERATOR_XOR,
    CAIRO_OPERATOR_ADD,
    CAIRO_OPERATOR_SATURATE
} cairo_operator_t;

void
cairo_set_operator (cairo_t *cr, cairo_operator_t op);

void
cairo_set_source (cairo_t *cr, cairo_pattern_t *source);

void
cairo_set_source_rgb (cairo_t *cr, double red, double green, double blue);

void
cairo_set_source_rgba (cairo_t *cr,
         double red, double green, double blue,
         double alpha);

void
cairo_set_source_surface (cairo_t *cr,
     cairo_surface_t *surface,
     double x,
     double y);

void
cairo_set_tolerance (cairo_t *cr, double tolerance);
# 321 "/opt/cairo/include/cairo/cairo.h"
typedef enum _cairo_antialias {
    CAIRO_ANTIALIAS_DEFAULT,
    CAIRO_ANTIALIAS_NONE,
    CAIRO_ANTIALIAS_GRAY,
    CAIRO_ANTIALIAS_SUBPIXEL
} cairo_antialias_t;

void
cairo_set_antialias (cairo_t *cr, cairo_antialias_t antialias);
# 352 "/opt/cairo/include/cairo/cairo.h"
typedef enum _cairo_fill_rule {
    CAIRO_FILL_RULE_WINDING,
    CAIRO_FILL_RULE_EVEN_ODD
} cairo_fill_rule_t;

void
cairo_set_fill_rule (cairo_t *cr, cairo_fill_rule_t fill_rule);

void
cairo_set_line_width (cairo_t *cr, double width);
# 372 "/opt/cairo/include/cairo/cairo.h"
typedef enum _cairo_line_cap {
    CAIRO_LINE_CAP_BUTT,
    CAIRO_LINE_CAP_ROUND,
    CAIRO_LINE_CAP_SQUARE
} cairo_line_cap_t;

void
cairo_set_line_cap (cairo_t *cr, cairo_line_cap_t line_cap);

typedef enum _cairo_line_join {
    CAIRO_LINE_JOIN_MITER,
    CAIRO_LINE_JOIN_ROUND,
    CAIRO_LINE_JOIN_BEVEL
} cairo_line_join_t;

void
cairo_set_line_join (cairo_t *cr, cairo_line_join_t line_join);

void
cairo_set_dash (cairo_t *cr,
  double *dashes,
  int num_dashes,
  double offset);

void
cairo_set_miter_limit (cairo_t *cr, double limit);

void
cairo_translate (cairo_t *cr, double tx, double ty);

void
cairo_scale (cairo_t *cr, double sx, double sy);

void
cairo_rotate (cairo_t *cr, double angle);

void
cairo_transform (cairo_t *cr,
   const cairo_matrix_t *matrix);

void
cairo_set_matrix (cairo_t *cr,
    const cairo_matrix_t *matrix);

void
cairo_identity_matrix (cairo_t *cr);

void
cairo_user_to_device (cairo_t *cr, double *x, double *y);

void
cairo_user_to_device_distance (cairo_t *cr, double *dx, double *dy);

void
cairo_device_to_user (cairo_t *cr, double *x, double *y);

void
cairo_device_to_user_distance (cairo_t *cr, double *dx, double *dy);


void
cairo_new_path (cairo_t *cr);

void
cairo_move_to (cairo_t *cr, double x, double y);

void
cairo_line_to (cairo_t *cr, double x, double y);

void
cairo_curve_to (cairo_t *cr,
  double x1, double y1,
  double x2, double y2,
  double x3, double y3);

void
cairo_arc (cairo_t *cr,
    double xc, double yc,
    double radius,
    double angle1, double angle2);

void
cairo_arc_negative (cairo_t *cr,
      double xc, double yc,
      double radius,
      double angle1, double angle2);
# 467 "/opt/cairo/include/cairo/cairo.h"
void
cairo_rel_move_to (cairo_t *cr, double dx, double dy);

void
cairo_rel_line_to (cairo_t *cr, double dx, double dy);

void
cairo_rel_curve_to (cairo_t *cr,
      double dx1, double dy1,
      double dx2, double dy2,
      double dx3, double dy3);

void
cairo_rectangle (cairo_t *cr,
   double x, double y,
   double width, double height);






void
cairo_close_path (cairo_t *cr);


void
cairo_paint (cairo_t *cr);

void
cairo_paint_with_alpha (cairo_t *cr,
   double alpha);

void
cairo_mask (cairo_t *cr,
     cairo_pattern_t *pattern);

void
cairo_mask_surface (cairo_t *cr,
      cairo_surface_t *surface,
      double surface_x,
      double surface_y);

void
cairo_stroke (cairo_t *cr);

void
cairo_stroke_preserve (cairo_t *cr);

void
cairo_fill (cairo_t *cr);

void
cairo_fill_preserve (cairo_t *cr);

void
cairo_copy_page (cairo_t *cr);

void
cairo_show_page (cairo_t *cr);


cairo_bool_t
cairo_in_stroke (cairo_t *cr, double x, double y);

cairo_bool_t
cairo_in_fill (cairo_t *cr, double x, double y);


void
cairo_stroke_extents (cairo_t *cr,
        double *x1, double *y1,
        double *x2, double *y2);

void
cairo_fill_extents (cairo_t *cr,
      double *x1, double *y1,
      double *x2, double *y2);


void
cairo_reset_clip (cairo_t *cr);

void
cairo_clip (cairo_t *cr);

void
cairo_clip_preserve (cairo_t *cr);
# 566 "/opt/cairo/include/cairo/cairo.h"
typedef struct _cairo_scaled_font cairo_scaled_font_t;
# 578 "/opt/cairo/include/cairo/cairo.h"
typedef struct _cairo_font_face cairo_font_face_t;
# 604 "/opt/cairo/include/cairo/cairo.h"
typedef struct {
  unsigned long index;
  double x;
  double y;
} cairo_glyph_t;
# 637 "/opt/cairo/include/cairo/cairo.h"
typedef struct {
    double x_bearing;
    double y_bearing;
    double width;
    double height;
    double x_advance;
    double y_advance;
} cairo_text_extents_t;
# 687 "/opt/cairo/include/cairo/cairo.h"
typedef struct {
    double ascent;
    double descent;
    double height;
    double max_x_advance;
    double max_y_advance;
} cairo_font_extents_t;

typedef enum _cairo_font_slant {
  CAIRO_FONT_SLANT_NORMAL,
  CAIRO_FONT_SLANT_ITALIC,
  CAIRO_FONT_SLANT_OBLIQUE
} cairo_font_slant_t;

typedef enum _cairo_font_weight {
  CAIRO_FONT_WEIGHT_NORMAL,
  CAIRO_FONT_WEIGHT_BOLD
} cairo_font_weight_t;
# 723 "/opt/cairo/include/cairo/cairo.h"
typedef enum _cairo_subpixel_order {
    CAIRO_SUBPIXEL_ORDER_DEFAULT,
    CAIRO_SUBPIXEL_ORDER_RGB,
    CAIRO_SUBPIXEL_ORDER_BGR,
    CAIRO_SUBPIXEL_ORDER_VRGB,
    CAIRO_SUBPIXEL_ORDER_VBGR
} cairo_subpixel_order_t;
# 751 "/opt/cairo/include/cairo/cairo.h"
typedef enum _cairo_hint_style {
    CAIRO_HINT_STYLE_DEFAULT,
    CAIRO_HINT_STYLE_NONE,
    CAIRO_HINT_STYLE_SLIGHT,
    CAIRO_HINT_STYLE_MEDIUM,
    CAIRO_HINT_STYLE_FULL
} cairo_hint_style_t;
# 772 "/opt/cairo/include/cairo/cairo.h"
typedef enum _cairo_hint_metrics {
    CAIRO_HINT_METRICS_DEFAULT,
    CAIRO_HINT_METRICS_OFF,
    CAIRO_HINT_METRICS_ON
} cairo_hint_metrics_t;

typedef struct _cairo_font_options cairo_font_options_t;

cairo_font_options_t *
cairo_font_options_create (void);

cairo_font_options_t *
cairo_font_options_copy (const cairo_font_options_t *original);

void
cairo_font_options_destroy (cairo_font_options_t *options);

cairo_status_t
cairo_font_options_status (cairo_font_options_t *options);

void
cairo_font_options_merge (cairo_font_options_t *options,
     const cairo_font_options_t *other);
cairo_bool_t
cairo_font_options_equal (const cairo_font_options_t *options,
     const cairo_font_options_t *other);

unsigned long
cairo_font_options_hash (const cairo_font_options_t *options);

void
cairo_font_options_set_antialias (cairo_font_options_t *options,
      cairo_antialias_t antialias);
cairo_antialias_t
cairo_font_options_get_antialias (const cairo_font_options_t *options);

void
cairo_font_options_set_subpixel_order (cairo_font_options_t *options,
           cairo_subpixel_order_t subpixel_order);
cairo_subpixel_order_t
cairo_font_options_get_subpixel_order (const cairo_font_options_t *options);

void
cairo_font_options_set_hint_style (cairo_font_options_t *options,
       cairo_hint_style_t hint_style);
cairo_hint_style_t
cairo_font_options_get_hint_style (const cairo_font_options_t *options);

void
cairo_font_options_set_hint_metrics (cairo_font_options_t *options,
         cairo_hint_metrics_t hint_metrics);
cairo_hint_metrics_t
cairo_font_options_get_hint_metrics (const cairo_font_options_t *options);





void
cairo_select_font_face (cairo_t *cr,
   const char *family,
   cairo_font_slant_t slant,
   cairo_font_weight_t weight);

void
cairo_set_font_size (cairo_t *cr, double size);

void
cairo_set_font_matrix (cairo_t *cr,
         const cairo_matrix_t *matrix);

void
cairo_get_font_matrix (cairo_t *cr,
         cairo_matrix_t *matrix);

void
cairo_set_font_options (cairo_t *cr,
   const cairo_font_options_t *options);

void
cairo_get_font_options (cairo_t *cr,
   cairo_font_options_t *options);

void
cairo_show_text (cairo_t *cr, const char *utf8);

void
cairo_show_glyphs (cairo_t *cr, cairo_glyph_t *glyphs, int num_glyphs);

cairo_font_face_t *
cairo_get_font_face (cairo_t *cr);

void
cairo_font_extents (cairo_t *cr,
      cairo_font_extents_t *extents);

void
cairo_set_font_face (cairo_t *cr, cairo_font_face_t *font_face);

void
cairo_text_extents (cairo_t *cr,
      const char *utf8,
      cairo_text_extents_t *extents);

void
cairo_glyph_extents (cairo_t *cr,
       cairo_glyph_t *glyphs,
       int num_glyphs,
       cairo_text_extents_t *extents);

void
cairo_text_path (cairo_t *cr, const char *utf8);

void
cairo_glyph_path (cairo_t *cr, cairo_glyph_t *glyphs, int num_glyphs);



cairo_font_face_t *
cairo_font_face_reference (cairo_font_face_t *font_face);

void
cairo_font_face_destroy (cairo_font_face_t *font_face);

cairo_status_t
cairo_font_face_status (cairo_font_face_t *font_face);

void *
cairo_font_face_get_user_data (cairo_font_face_t *font_face,
          const cairo_user_data_key_t *key);

cairo_status_t
cairo_font_face_set_user_data (cairo_font_face_t *font_face,
          const cairo_user_data_key_t *key,
          void *user_data,
          cairo_destroy_func_t destroy);



cairo_scaled_font_t *
cairo_scaled_font_create (cairo_font_face_t *font_face,
     const cairo_matrix_t *font_matrix,
     const cairo_matrix_t *ctm,
     const cairo_font_options_t *options);

cairo_scaled_font_t *
cairo_scaled_font_reference (cairo_scaled_font_t *scaled_font);

void
cairo_scaled_font_destroy (cairo_scaled_font_t *scaled_font);

cairo_status_t
cairo_scaled_font_status (cairo_scaled_font_t *scaled_font);

void
cairo_scaled_font_extents (cairo_scaled_font_t *scaled_font,
      cairo_font_extents_t *extents);

void
cairo_scaled_font_glyph_extents (cairo_scaled_font_t *scaled_font,
     cairo_glyph_t *glyphs,
     int num_glyphs,
     cairo_text_extents_t *extents);



cairo_operator_t
cairo_get_operator (cairo_t *cr);

cairo_pattern_t *
cairo_get_source (cairo_t *cr);

double
cairo_get_tolerance (cairo_t *cr);

cairo_antialias_t
cairo_get_antialias (cairo_t *cr);

void
cairo_get_current_point (cairo_t *cr, double *x, double *y);

cairo_fill_rule_t
cairo_get_fill_rule (cairo_t *cr);

double
cairo_get_line_width (cairo_t *cr);

cairo_line_cap_t
cairo_get_line_cap (cairo_t *cr);

cairo_line_join_t
cairo_get_line_join (cairo_t *cr);

double
cairo_get_miter_limit (cairo_t *cr);



void
cairo_get_matrix (cairo_t *cr, cairo_matrix_t *matrix);

cairo_surface_t *
cairo_get_target (cairo_t *cr);

typedef enum _cairo_path_data_type {
    CAIRO_PATH_MOVE_TO,
    CAIRO_PATH_LINE_TO,
    CAIRO_PATH_CURVE_TO,
    CAIRO_PATH_CLOSE_PATH
} cairo_path_data_type_t;
# 1042 "/opt/cairo/include/cairo/cairo.h"
typedef union _cairo_path_data_t cairo_path_data_t;
union _cairo_path_data_t {
    struct {
 cairo_path_data_type_t type;
 int length;
    } header;
    struct {
 double x, y;
    } point;
};
# 1072 "/opt/cairo/include/cairo/cairo.h"
typedef struct cairo_path {
    cairo_status_t status;
    cairo_path_data_t *data;
    int num_data;
} cairo_path_t;

cairo_path_t *
cairo_copy_path (cairo_t *cr);

cairo_path_t *
cairo_copy_path_flat (cairo_t *cr);

void
cairo_append_path (cairo_t *cr,
     cairo_path_t *path);

void
cairo_path_destroy (cairo_path_t *path);



cairo_status_t
cairo_status (cairo_t *cr);

const char *
cairo_status_to_string (cairo_status_t status);
# 1115 "/opt/cairo/include/cairo/cairo.h"
typedef enum _cairo_content {
    CAIRO_CONTENT_COLOR = 0x1000,
    CAIRO_CONTENT_ALPHA = 0x2000,
    CAIRO_CONTENT_COLOR_ALPHA = 0x3000
} cairo_content_t;

cairo_surface_t *
cairo_surface_create_similar (cairo_surface_t *other,
         cairo_content_t content,
         int width,
         int height);

cairo_surface_t *
cairo_surface_reference (cairo_surface_t *surface);

void
cairo_surface_destroy (cairo_surface_t *surface);

cairo_status_t
cairo_surface_status (cairo_surface_t *surface);

void
cairo_surface_finish (cairo_surface_t *surface);



cairo_status_t
cairo_surface_write_to_png (cairo_surface_t *surface,
       const char *filename);

cairo_status_t
cairo_surface_write_to_png_stream (cairo_surface_t *surface,
       cairo_write_func_t write_func,
       void *closure);



void *
cairo_surface_get_user_data (cairo_surface_t *surface,
        const cairo_user_data_key_t *key);

cairo_status_t
cairo_surface_set_user_data (cairo_surface_t *surface,
        const cairo_user_data_key_t *key,
        void *user_data,
        cairo_destroy_func_t destroy);

void
cairo_surface_get_font_options (cairo_surface_t *surface,
    cairo_font_options_t *options);

void
cairo_surface_flush (cairo_surface_t *surface);

void
cairo_surface_mark_dirty (cairo_surface_t *surface);

void
cairo_surface_mark_dirty_rectangle (cairo_surface_t *surface,
        int x,
        int y,
        int width,
        int height);

void
cairo_surface_set_device_offset (cairo_surface_t *surface,
     double x_offset,
     double y_offset);
# 1208 "/opt/cairo/include/cairo/cairo.h"
typedef enum _cairo_format {
    CAIRO_FORMAT_ARGB32,
    CAIRO_FORMAT_RGB24,
    CAIRO_FORMAT_A8,
    CAIRO_FORMAT_A1
} cairo_format_t;

cairo_surface_t *
cairo_image_surface_create (cairo_format_t format,
       int width,
       int height);

cairo_surface_t *
cairo_image_surface_create_for_data (unsigned char *data,
         cairo_format_t format,
         int width,
         int height,
         int stride);

int
cairo_image_surface_get_width (cairo_surface_t *surface);

int
cairo_image_surface_get_height (cairo_surface_t *surface);



cairo_surface_t *
cairo_image_surface_create_from_png (const char *filename);

cairo_surface_t *
cairo_image_surface_create_from_png_stream (cairo_read_func_t read_func,
         void *closure);





cairo_pattern_t *
cairo_pattern_create_rgb (double red, double green, double blue);

cairo_pattern_t *
cairo_pattern_create_rgba (double red, double green, double blue,
      double alpha);

cairo_pattern_t *
cairo_pattern_create_for_surface (cairo_surface_t *surface);

cairo_pattern_t *
cairo_pattern_create_linear (double x0, double y0,
        double x1, double y1);

cairo_pattern_t *
cairo_pattern_create_radial (double cx0, double cy0, double radius0,
        double cx1, double cy1, double radius1);

cairo_pattern_t *
cairo_pattern_reference (cairo_pattern_t *pattern);

void
cairo_pattern_destroy (cairo_pattern_t *pattern);

cairo_status_t
cairo_pattern_status (cairo_pattern_t *pattern);

void
cairo_pattern_add_color_stop_rgb (cairo_pattern_t *pattern,
      double offset,
      double red, double green, double blue);

void
cairo_pattern_add_color_stop_rgba (cairo_pattern_t *pattern,
       double offset,
       double red, double green, double blue,
       double alpha);

void
cairo_pattern_set_matrix (cairo_pattern_t *pattern,
     const cairo_matrix_t *matrix);

void
cairo_pattern_get_matrix (cairo_pattern_t *pattern,
     cairo_matrix_t *matrix);

typedef enum _cairo_extend {
    CAIRO_EXTEND_NONE,
    CAIRO_EXTEND_REPEAT,
    CAIRO_EXTEND_REFLECT
} cairo_extend_t;

void
cairo_pattern_set_extend (cairo_pattern_t *pattern, cairo_extend_t extend);

cairo_extend_t
cairo_pattern_get_extend (cairo_pattern_t *pattern);

typedef enum _cairo_filter {
    CAIRO_FILTER_FAST,
    CAIRO_FILTER_GOOD,
    CAIRO_FILTER_BEST,
    CAIRO_FILTER_NEAREST,
    CAIRO_FILTER_BILINEAR,
    CAIRO_FILTER_GAUSSIAN
} cairo_filter_t;

void
cairo_pattern_set_filter (cairo_pattern_t *pattern, cairo_filter_t filter);

cairo_filter_t
cairo_pattern_get_filter (cairo_pattern_t *pattern);



void
cairo_matrix_init (cairo_matrix_t *matrix,
     double xx, double yx,
     double xy, double yy,
     double x0, double y0);

void
cairo_matrix_init_identity (cairo_matrix_t *matrix);

void
cairo_matrix_init_translate (cairo_matrix_t *matrix,
        double tx, double ty);

void
cairo_matrix_init_scale (cairo_matrix_t *matrix,
    double sx, double sy);

void
cairo_matrix_init_rotate (cairo_matrix_t *matrix,
     double radians);

void
cairo_matrix_translate (cairo_matrix_t *matrix, double tx, double ty);

void
cairo_matrix_scale (cairo_matrix_t *matrix, double sx, double sy);

void
cairo_matrix_rotate (cairo_matrix_t *matrix, double radians);

cairo_status_t
cairo_matrix_invert (cairo_matrix_t *matrix);

void
cairo_matrix_multiply (cairo_matrix_t *result,
         const cairo_matrix_t *a,
         const cairo_matrix_t *b);


void
cairo_matrix_transform_distance (const cairo_matrix_t *matrix,
     double *dx, double *dy);


void
cairo_matrix_transform_point (const cairo_matrix_t *matrix,
         double *x, double *y);
# 1453 "/opt/cairo/include/cairo/cairo.h"

# 2 "symbols.c" 2
# 1 "symbols.h" 1
# 16 "symbols.h"
typedef void (*vf)(cairo_t*);
# 3 "symbols.c" 2
void swamp(cairo_t *cr);
{
cairo_save(cr);
cairo_set_source_rgb(cr, 0.000000, 0.700000, 0.000000);
cairo_line_to(cr, 175, 100);
cairo_line_to(cr, 325, 100);
cairo_stroke(cr);
cairo_line_to(cr, 250, 100);
cairo_line_to(cr, 180, 160);
cairo_stroke(cr);
cairo_line_to(cr, 250, 100);
cairo_line_to(cr, 320, 130);
cairo_stroke(cr);
cairo_line_to(cr, 320, 180);
cairo_line_to(cr, 310, 180);
cairo_line_to(cr, 290, 190);
cairo_line_to(cr, 290, 200);
cairo_line_to(cr, 310, 210);
cairo_line_to(cr, 325, 190);
cairo_line_to(cr, 325, 180);
cairo_line_to(cr, 310, 180);
cairo_save(cr);
cairo_set_source_rgb(cr, 0.500000, 0.900000, 0.400000);
cairo_fill_preserve(cr);
cairo_restore(cr);
cairo_stroke(cr);
cairo_line_to(cr, 250, 100);
cairo_line_to(cr, 250, 230);
cairo_line_to(cr, 320, 180);
cairo_stroke(cr);
cairo_restore(cr);
}


void temple(cairo_t *cr);
{
cairo_line_to(cr, 125, 100);
cairo_line_to(cr, 165, 100);
cairo_stroke(cr);
cairo_line_to(cr, 145, 100);
cairo_line_to(cr, 205, 180);
cairo_line_to(cr, 285, 180);
cairo_line_to(cr, 365, 100);
cairo_line_to(cr, 345, 100);
cairo_line_to(cr, 285, 180);
cairo_stroke(cr);
cairo_line_to(cr, 245, 185);
cairo_arc(cr, 245, 185, 30, 356 * M_PI/180, 185 * M_PI/180);
cairo_stroke(cr);
cairo_line_to(cr, 245, 185);
cairo_arc(cr, 245, 185, 25, 0 * M_PI/180, 80 * M_PI/180);
cairo_stroke(cr);
cairo_line_to(cr, 375, 100);
cairo_line_to(cr, 325, 100);
cairo_stroke(cr);
cairo_line_to(cr, 235, 180);
cairo_line_to(cr, 205, 100);
cairo_stroke(cr);
cairo_line_to(cr, 255, 180);
cairo_line_to(cr, 275, 100);
cairo_stroke(cr);
cairo_line_to(cr, 235, 180);
cairo_line_to(cr, 195, 100);
cairo_line_to(cr, 205, 100);
cairo_stroke(cr);
cairo_line_to(cr, 210, 100);
cairo_line_to(cr, 270, 100);
cairo_stroke(cr);
cairo_line_to(cr, 213, 120);
cairo_line_to(cr, 267, 120);
cairo_stroke(cr);
cairo_line_to(cr, 220, 140);
cairo_line_to(cr, 260, 140);
cairo_stroke(cr);
cairo_line_to(cr, 230, 160);
cairo_line_to(cr, 255, 160);
cairo_stroke(cr);
}


void fertile(cairo_t *cr);
{
cairo_line_to(cr, 180, 100);
cairo_line_to(cr, 180, 180);
cairo_line_to(cr, 180, 150);
cairo_line_to(cr, 170, 170);
cairo_line_to(cr, 160, 170);
cairo_line_to(cr, 160, 160);
cairo_line_to(cr, 180, 150);
cairo_line_to(cr, 190, 170);
cairo_line_to(cr, 190, 160);
cairo_line_to(cr, 180, 150);
cairo_line_to(cr, 180, 130);
cairo_line_to(cr, 170, 150);
cairo_line_to(cr, 160, 150);
cairo_line_to(cr, 160, 140);
cairo_line_to(cr, 180, 130);
cairo_line_to(cr, 190, 150);
cairo_line_to(cr, 200, 150);
cairo_line_to(cr, 180, 130);
cairo_stroke(cr);
cairo_line_to(cr, 250, 100);
cairo_line_to(cr, 250, 200);
cairo_line_to(cr, 250, 180);
cairo_line_to(cr, 240, 200);
cairo_line_to(cr, 230, 200);
cairo_line_to(cr, 230, 190);
cairo_line_to(cr, 250, 180);
cairo_line_to(cr, 250, 170);
cairo_line_to(cr, 260, 190);
cairo_line_to(cr, 270, 190);
cairo_line_to(cr, 270, 180);
cairo_line_to(cr, 250, 170);
cairo_line_to(cr, 250, 160);
cairo_line_to(cr, 240, 180);
cairo_line_to(cr, 230, 180);
cairo_line_to(cr, 230, 170);
cairo_line_to(cr, 250, 160);
cairo_line_to(cr, 250, 150);
cairo_line_to(cr, 260, 170);
cairo_line_to(cr, 270, 170);
cairo_line_to(cr, 270, 160);
cairo_line_to(cr, 250, 150);
cairo_line_to(cr, 250, 150);
cairo_line_to(cr, 240, 160);
cairo_line_to(cr, 230, 160);
cairo_line_to(cr, 230, 150);
cairo_line_to(cr, 250, 140);
cairo_line_to(cr, 250, 120);
cairo_line_to(cr, 240, 140);
cairo_line_to(cr, 230, 140);
cairo_line_to(cr, 230, 130);
cairo_line_to(cr, 250, 120);
cairo_stroke(cr);
cairo_line_to(cr, 320, 100);
cairo_line_to(cr, 320, 170);
cairo_line_to(cr, 320, 150);
cairo_line_to(cr, 310, 170);
cairo_line_to(cr, 300, 170);
cairo_line_to(cr, 300, 160);
cairo_line_to(cr, 320, 150);
cairo_line_to(cr, 320, 140);
cairo_line_to(cr, 330, 160);
cairo_line_to(cr, 340, 160);
cairo_line_to(cr, 340, 150);
cairo_line_to(cr, 320, 140);
cairo_line_to(cr, 320, 130);
cairo_line_to(cr, 310, 150);
cairo_line_to(cr, 300, 150);
cairo_line_to(cr, 300, 140);
cairo_line_to(cr, 320, 130);
cairo_line_to(cr, 320, 120);
cairo_line_to(cr, 330, 140);
cairo_line_to(cr, 340, 140);
cairo_line_to(cr, 340, 130);
cairo_line_to(cr, 320, 120);
cairo_stroke(cr);
}


void hill(cairo_t *cr);
{
cairo_line_to(cr, 120, 100);
cairo_line_to(cr, 200, 200);
cairo_line_to(cr, 225, 155);
cairo_stroke(cr);
cairo_line_to(cr, 200, 115);
cairo_line_to(cr, 300, 250);
cairo_line_to(cr, 400, 120);
cairo_stroke(cr);
cairo_line_to(cr, 300, 250);
cairo_line_to(cr, 250, 144);
cairo_stroke(cr);
cairo_line_to(cr, 300, 250);
cairo_line_to(cr, 270, 160);
cairo_stroke(cr);
cairo_line_to(cr, 300, 250);
cairo_line_to(cr, 275, 175);
cairo_stroke(cr);
}


void volcano(cairo_t *cr);
{
cairo_line_to(cr, 120, 100);
cairo_line_to(cr, 200, 200);
cairo_line_to(cr, 225, 155);
cairo_stroke(cr);
cairo_line_to(cr, 200, 195);
cairo_line_to(cr, 200, 165);
cairo_stroke(cr);
cairo_line_to(cr, 200, 115);
cairo_line_to(cr, 275, 220);
cairo_line_to(cr, 325, 230);
cairo_line_to(cr, 400, 120);
cairo_stroke(cr);
cairo_line_to(cr, 285, 220);
cairo_line_to(cr, 250, 144);
cairo_stroke(cr);
cairo_line_to(cr, 295, 220);
cairo_line_to(cr, 270, 160);
cairo_stroke(cr);
cairo_line_to(cr, 300, 230);
cairo_line_to(cr, 275, 175);
cairo_stroke(cr);
cairo_line_to(cr, 310, 230);
cairo_line_to(cr, 300, 200);
cairo_stroke(cr);
cairo_line_to(cr, 320, 230);
cairo_line_to(cr, 340, 180);
cairo_stroke(cr);
cairo_line_to(cr, 315, 250);
cairo_line_to(cr, 250, 250);
cairo_stroke(cr);
cairo_line_to(cr, 230, 250);
cairo_line_to(cr, 130, 250);
cairo_stroke(cr);
cairo_line_to(cr, 285, 235);
cairo_line_to(cr, 85, 235);
cairo_stroke(cr);
cairo_line_to(cr, 275, 223);
cairo_line_to(cr, 120, 223);
cairo_stroke(cr);
cairo_line_to(cr, 270, 210);
cairo_line_to(cr, 240, 210);
cairo_stroke(cr);
cairo_line_to(cr, 220, 210);
cairo_line_to(cr, 100, 210);
cairo_stroke(cr);
}


void plains(cairo_t *cr);
{
cairo_line_to(cr, 180, 100);
cairo_line_to(cr, 180, 140);
cairo_stroke(cr);
cairo_line_to(cr, 190, 100);
cairo_line_to(cr, 190, 130);
cairo_stroke(cr);
cairo_line_to(cr, 210, 100);
cairo_line_to(cr, 210, 150);
cairo_stroke(cr);
cairo_line_to(cr, 220, 100);
cairo_line_to(cr, 220, 130);
cairo_stroke(cr);
cairo_line_to(cr, 220, 140);
cairo_line_to(cr, 220, 150);
cairo_stroke(cr);
cairo_line_to(cr, 240, 100);
cairo_line_to(cr, 240, 140);
cairo_stroke(cr);
cairo_line_to(cr, 250, 100);
cairo_line_to(cr, 250, 170);
cairo_stroke(cr);
cairo_line_to(cr, 270, 100);
cairo_line_to(cr, 270, 140);
cairo_stroke(cr);
cairo_line_to(cr, 290, 100);
cairo_line_to(cr, 290, 140);
cairo_stroke(cr);
cairo_line_to(cr, 320, 100);
cairo_line_to(cr, 320, 160);
cairo_stroke(cr);
}


void cursed(cairo_t *cr);
{
cairo_line_to(cr, 125, 160);
cairo_line_to(cr, 220, 165);
cairo_line_to(cr, 320, 160);
cairo_stroke(cr);
cairo_line_to(cr, 125, 150);
cairo_line_to(cr, 320, 150);
cairo_stroke(cr);
cairo_line_to(cr, 145, 140);
cairo_line_to(cr, 310, 140);
cairo_stroke(cr);
cairo_line_to(cr, 150, 130);
cairo_line_to(cr, 220, 130);
cairo_stroke(cr);
cairo_line_to(cr, 135, 120);
cairo_line_to(cr, 275, 120);
cairo_stroke(cr);
cairo_line_to(cr, 145, 100);
cairo_line_to(cr, 220, 110);
cairo_line_to(cr, 275, 100);
cairo_stroke(cr);
}


void raftfill(cairo_t *cr);
{
cairo_line_to(cr, 170, 110);
cairo_line_to(cr, 230, 170);
cairo_line_to(cr, 350, 170);
cairo_line_to(cr, 350, 160);
cairo_line_to(cr, 290, 100);
cairo_line_to(cr, 170, 100);
cairo_line_to(cr, 170, 110);
cairo_close_path(cr);
cairo_fill(cr);
}


void raft(cairo_t *cr);
{
cairo_line_to(cr, 170, 110);
cairo_line_to(cr, 230, 170);
cairo_line_to(cr, 350, 170);
cairo_line_to(cr, 350, 160);
cairo_line_to(cr, 290, 100);
cairo_line_to(cr, 170, 100);
cairo_line_to(cr, 170, 110);
cairo_stroke(cr);
cairo_line_to(cr, 170, 110);
cairo_line_to(cr, 290, 110);
cairo_line_to(cr, 350, 170);
cairo_stroke(cr);
cairo_line_to(cr, 190, 100);
cairo_line_to(cr, 190, 110);
cairo_line_to(cr, 250, 170);
cairo_stroke(cr);
cairo_line_to(cr, 210, 100);
cairo_line_to(cr, 210, 110);
cairo_line_to(cr, 270, 170);
cairo_stroke(cr);
cairo_line_to(cr, 230, 100);
cairo_line_to(cr, 230, 110);
cairo_line_to(cr, 290, 170);
cairo_stroke(cr);
cairo_line_to(cr, 250, 100);
cairo_line_to(cr, 250, 110);
cairo_line_to(cr, 310, 170);
cairo_stroke(cr);
cairo_line_to(cr, 270, 100);
cairo_line_to(cr, 270, 110);
cairo_line_to(cr, 330, 170);
cairo_stroke(cr);
}


void scrubold(cairo_t *cr);
{
cairo_line_to(cr, 170, 100);
cairo_line_to(cr, 165, 120);
cairo_line_to(cr, 167, 120);
cairo_line_to(cr, 170, 110);
cairo_line_to(cr, 170, 130);
cairo_line_to(cr, 172, 128);
cairo_line_to(cr, 170, 100);
cairo_stroke(cr);
cairo_line_to(cr, 200, 100);
cairo_line_to(cr, 200, 120);
cairo_line_to(cr, 190, 130);
cairo_line_to(cr, 190, 140);
cairo_line_to(cr, 190, 130);
cairo_line_to(cr, 200, 140);
cairo_stroke(cr);
cairo_line_to(cr, 200, 110);
cairo_line_to(cr, 230, 130);
cairo_stroke(cr);
cairo_line_to(cr, 230, 100);
cairo_line_to(cr, 240, 120);
cairo_stroke(cr);
cairo_line_to(cr, 250, 100);
cairo_line_to(cr, 250, 120);
cairo_line_to(cr, 270, 130);
cairo_line_to(cr, 255, 120);
cairo_line_to(cr, 250, 130);
cairo_line_to(cr, 260, 140);
cairo_stroke(cr);
cairo_line_to(cr, 270, 100);
cairo_line_to(cr, 280, 130);
cairo_stroke(cr);
cairo_line_to(cr, 310, 100);
cairo_line_to(cr, 310, 120);
cairo_line_to(cr, 300, 120);
cairo_line_to(cr, 300, 100);
cairo_line_to(cr, 310, 100);
cairo_close_path(cr);
cairo_fill(cr);
cairo_line_to(cr, 310, 115);
cairo_line_to(cr, 320, 115);
cairo_line_to(cr, 330, 125);
cairo_line_to(cr, 330, 130);
cairo_line_to(cr, 310, 150);
cairo_line_to(cr, 300, 150);
cairo_line_to(cr, 290, 140);
cairo_line_to(cr, 285, 120);
cairo_line_to(cr, 290, 115);
cairo_line_to(cr, 300, 115);
cairo_stroke(cr);
cairo_line_to(cr, 310, 125);
cairo_line_to(cr, 310, 130);
cairo_line_to(cr, 320, 130);
cairo_line_to(cr, 310, 125);
cairo_close_path(cr);
cairo_fill(cr);
cairo_line_to(cr, 345, 100);
cairo_line_to(cr, 345, 120);
cairo_stroke(cr);
}


void scrub(cairo_t *cr);
{
cairo_line_to(cr, 170, 100);
cairo_line_to(cr, 325, 100);
cairo_stroke(cr);
cairo_line_to(cr, 185, 100);
cairo_line_to(cr, 175, 160);
cairo_line_to(cr, 180, 160);
cairo_line_to(cr, 190, 100);
cairo_stroke(cr);
cairo_line_to(cr, 170, 160);
cairo_line_to(cr, 165, 155);
cairo_line_to(cr, 160, 165);
cairo_line_to(cr, 160, 175);
cairo_line_to(cr, 165, 180);
cairo_line_to(cr, 175, 190);
cairo_line_to(cr, 185, 190);
cairo_line_to(cr, 185, 185);
cairo_line_to(cr, 195, 175);
cairo_line_to(cr, 195, 150);
cairo_line_to(cr, 185, 160);
cairo_line_to(cr, 175, 160);
cairo_stroke(cr);
cairo_line_to(cr, 185, 125);
cairo_line_to(cr, 190, 125);
cairo_line_to(cr, 190, 130);
cairo_stroke(cr);
cairo_line_to(cr, 215, 100);
cairo_line_to(cr, 210, 110);
cairo_line_to(cr, 215, 145);
cairo_line_to(cr, 215, 110);
cairo_line_to(cr, 220, 100);
cairo_close_path(cr);
cairo_fill(cr);
cairo_line_to(cr, 215, 110);
cairo_line_to(cr, 225, 120);
cairo_stroke(cr);
cairo_line_to(cr, 245, 100);
cairo_line_to(cr, 245, 120);
cairo_line_to(cr, 240, 130);
cairo_line_to(cr, 250, 140);
cairo_line_to(cr, 250, 150);
cairo_stroke(cr);
cairo_line_to(cr, 240, 130);
cairo_line_to(cr, 235, 135);
cairo_line_to(cr, 235, 145);
cairo_line_to(cr, 230, 150);
cairo_line_to(cr, 230, 165);
cairo_stroke(cr);
cairo_line_to(cr, 265, 100);
cairo_line_to(cr, 260, 105);
cairo_line_to(cr, 260, 120);
cairo_line_to(cr, 265, 140);
cairo_line_to(cr, 260, 160);
cairo_line_to(cr, 265, 160);
cairo_line_to(cr, 275, 135);
cairo_line_to(cr, 270, 135);
cairo_line_to(cr, 265, 120);
cairo_line_to(cr, 265, 105);
cairo_line_to(cr, 270, 100);
cairo_line_to(cr, 265, 100);
cairo_close_path(cr);
cairo_fill(cr);
cairo_line_to(cr, 255, 165);
cairo_line_to(cr, 255, 160);
cairo_line_to(cr, 250, 170);
cairo_line_to(cr, 250, 160);
cairo_line_to(cr, 240, 160);
cairo_line_to(cr, 245, 165);
cairo_line_to(cr, 245, 175);
cairo_line_to(cr, 250, 185);
cairo_line_to(cr, 255, 185);
cairo_line_to(cr, 255, 195);
cairo_line_to(cr, 265, 195);
cairo_line_to(cr, 265, 185);
cairo_line_to(cr, 270, 190);
cairo_line_to(cr, 280, 190);
cairo_line_to(cr, 280, 185);
cairo_line_to(cr, 285, 180);
cairo_line_to(cr, 285, 175);
cairo_line_to(cr, 295, 175);
cairo_line_to(cr, 295, 165);
cairo_line_to(cr, 285, 155);
cairo_line_to(cr, 280, 165);
cairo_line_to(cr, 260, 165);
cairo_stroke(cr);
cairo_line_to(cr, 305, 100);
cairo_line_to(cr, 305, 115);
cairo_line_to(cr, 310, 125);
cairo_line_to(cr, 310, 130);
cairo_line_to(cr, 302, 135);
cairo_line_to(cr, 300, 155);
cairo_line_to(cr, 305, 140);
cairo_line_to(cr, 315, 135);
cairo_line_to(cr, 320, 150);
cairo_line_to(cr, 320, 165);
cairo_line_to(cr, 315, 185);
cairo_line_to(cr, 320, 185);
cairo_line_to(cr, 325, 165);
cairo_line_to(cr, 325, 150);
cairo_line_to(cr, 310, 115);
cairo_line_to(cr, 310, 100);
cairo_line_to(cr, 310, 100);
cairo_close_path(cr);
cairo_fill(cr);
}


void one(cairo_t *cr);
{
cairo_line_to(cr, 170, 160);
cairo_line_to(cr, 165, 155);
cairo_line_to(cr, 160, 165);
cairo_line_to(cr, 160, 175);
cairo_line_to(cr, 165, 180);
cairo_line_to(cr, 175, 190);
cairo_line_to(cr, 185, 190);
cairo_line_to(cr, 185, 185);
cairo_line_to(cr, 195, 175);
cairo_line_to(cr, 195, 150);
cairo_line_to(cr, 185, 160);
cairo_line_to(cr, 175, 160);
cairo_close_path(cr);
cairo_fill(cr);
}


void two(cairo_t *cr);
{
cairo_line_to(cr, 255, 165);
cairo_line_to(cr, 255, 160);
cairo_line_to(cr, 250, 170);
cairo_line_to(cr, 250, 160);
cairo_line_to(cr, 240, 160);
cairo_line_to(cr, 245, 165);
cairo_line_to(cr, 245, 175);
cairo_line_to(cr, 250, 185);
cairo_line_to(cr, 255, 185);
cairo_line_to(cr, 255, 195);
cairo_line_to(cr, 265, 195);
cairo_line_to(cr, 265, 185);
cairo_line_to(cr, 270, 190);
cairo_line_to(cr, 280, 190);
cairo_line_to(cr, 280, 185);
cairo_line_to(cr, 285, 180);
cairo_line_to(cr, 285, 175);
cairo_line_to(cr, 295, 175);
cairo_line_to(cr, 295, 165);
cairo_line_to(cr, 285, 155);
cairo_line_to(cr, 280, 165);
cairo_line_to(cr, 260, 165);
cairo_close_path(cr);
cairo_fill(cr);
}


void blackbridge(cairo_t *cr);
{
cairo_line_to(cr, 150, 110);
cairo_line_to(cr, 150, 120);
cairo_line_to(cr, 220, 150);
cairo_line_to(cr, 280, 150);
cairo_line_to(cr, 350, 120);
cairo_line_to(cr, 350, 120);
cairo_stroke(cr);
cairo_line_to(cr, 220, 150);
cairo_line_to(cr, 220, 140);
cairo_stroke(cr);
cairo_line_to(cr, 280, 150);
cairo_line_to(cr, 280, 140);
cairo_stroke(cr);
cairo_line_to(cr, 150, 100);
cairo_arc_negative(cr, 220, 100, 20, 180 * M_PI/180, 0 * M_PI/180);

cairo_arc_negative(cr, 280, 100, 20, 180 * M_PI/180, 0 * M_PI/180);

cairo_line_to(cr, 350, 100);
cairo_line_to(cr, 350, 110);
cairo_line_to(cr, 280, 140);
cairo_line_to(cr, 220, 140);
cairo_line_to(cr, 150, 110);
cairo_line_to(cr, 150, 100);
cairo_close_path(cr);
cairo_fill(cr);
}


void whitebridge(cairo_t *cr);
{
cairo_line_to(cr, 150, 110);
cairo_line_to(cr, 150, 120);
cairo_line_to(cr, 220, 150);
cairo_line_to(cr, 280, 150);
cairo_line_to(cr, 350, 120);
cairo_line_to(cr, 350, 120);
cairo_stroke(cr);
cairo_line_to(cr, 220, 150);
cairo_line_to(cr, 220, 140);
cairo_stroke(cr);
cairo_line_to(cr, 280, 150);
cairo_line_to(cr, 280, 140);
cairo_stroke(cr);
cairo_line_to(cr, 150, 100);
cairo_arc_negative(cr, 220, 100, 20, 180 * M_PI/180, 0 * M_PI/180);

cairo_arc_negative(cr, 280, 100, 20, 180 * M_PI/180, 0 * M_PI/180);

cairo_line_to(cr, 350, 100);
cairo_line_to(cr, 350, 110);
cairo_line_to(cr, 280, 140);
cairo_line_to(cr, 220, 140);
cairo_line_to(cr, 150, 110);
cairo_line_to(cr, 150, 100);
cairo_stroke(cr);
cairo_close_path(cr);
}


void whirlpool(cairo_t *cr);
{
cairo_scale(cr, 2.500000, 2.500000);
cairo_save(cr);
cairo_set_source_rgb(cr, 0.000000, 0.000000, 0.600000);
cairo_line_to(cr, 40, 58);
cairo_line_to(cr, 30, 56);
cairo_line_to(cr, 20, 48);
cairo_line_to(cr, 18, 40);
cairo_line_to(cr, 18, 30);
cairo_line_to(cr, 22, 24);
cairo_line_to(cr, 34, 18);
cairo_stroke(cr);
cairo_line_to(cr, 50, 56);
cairo_line_to(cr, 60, 52);
cairo_line_to(cr, 68, 46);
cairo_line_to(cr, 72, 38);
cairo_line_to(cr, 70, 30);
cairo_line_to(cr, 66, 22);
cairo_line_to(cr, 60, 16);
cairo_line_to(cr, 50, 14);
cairo_stroke(cr);
cairo_line_to(cr, 54, 50);
cairo_line_to(cr, 40, 52);
cairo_line_to(cr, 36, 52);
cairo_line_to(cr, 30, 46);
cairo_line_to(cr, 26, 42);
cairo_line_to(cr, 26, 36);
cairo_line_to(cr, 30, 30);
cairo_line_to(cr, 36, 26);
cairo_line_to(cr, 40, 26);
cairo_stroke(cr);
cairo_line_to(cr, 56, 46);
cairo_line_to(cr, 60, 42);
cairo_line_to(cr, 62, 36);
cairo_line_to(cr, 62, 28);
cairo_line_to(cr, 58, 22);
cairo_line_to(cr, 54, 20);
cairo_line_to(cr, 46, 20);
cairo_stroke(cr);
cairo_line_to(cr, 44, 46);
cairo_line_to(cr, 40, 46);
cairo_line_to(cr, 36, 44);
cairo_line_to(cr, 34, 40);
cairo_line_to(cr, 34, 36);
cairo_line_to(cr, 38, 32);
cairo_line_to(cr, 42, 30);
cairo_line_to(cr, 46, 30);
cairo_line_to(cr, 50, 32);
cairo_line_to(cr, 52, 36);
cairo_line_to(cr, 52, 38);
cairo_stroke(cr);
cairo_line_to(cr, 46, 42);
cairo_line_to(cr, 50, 42);
cairo_line_to(cr, 54, 40);
cairo_line_to(cr, 56, 36);
cairo_line_to(cr, 56, 32);
cairo_line_to(cr, 54, 28);
cairo_line_to(cr, 48, 26);
cairo_stroke(cr);
cairo_restore(cr);
}


void ruin(cairo_t *cr);
{
cairo_line_to(cr, 160, 100);
cairo_line_to(cr, 340, 100);
cairo_line_to(cr, 340, 120);
cairo_line_to(cr, 310, 140);
cairo_line_to(cr, 320, 130);
cairo_line_to(cr, 290, 130);
cairo_line_to(cr, 290, 135);
cairo_line_to(cr, 305, 135);
cairo_line_to(cr, 305, 145);
cairo_line_to(cr, 290, 145);
cairo_line_to(cr, 290, 150);
cairo_line_to(cr, 310, 150);
cairo_line_to(cr, 330, 130);
cairo_stroke(cr);
cairo_line_to(cr, 290, 130);
cairo_line_to(cr, 270, 130);
cairo_line_to(cr, 270, 120);
cairo_line_to(cr, 260, 120);
cairo_line_to(cr, 260, 110);
cairo_line_to(cr, 250, 110);
cairo_line_to(cr, 250, 120);
cairo_line_to(cr, 240, 120);
cairo_line_to(cr, 250, 115);
cairo_line_to(cr, 240, 115);
cairo_line_to(cr, 240, 120);
cairo_line_to(cr, 245, 115);
cairo_line_to(cr, 230, 115);
cairo_line_to(cr, 230, 120);
cairo_line_to(cr, 225, 120);
cairo_line_to(cr, 225, 130);
cairo_line_to(cr, 235, 130);
cairo_line_to(cr, 230, 135);
cairo_line_to(cr, 225, 135);
cairo_line_to(cr, 225, 150);
cairo_line_to(cr, 220, 150);
cairo_line_to(cr, 220, 100);
cairo_line_to(cr, 220, 120);
cairo_line_to(cr, 210, 120);
cairo_line_to(cr, 210, 130);
cairo_line_to(cr, 200, 130);
cairo_line_to(cr, 200, 100);
cairo_line_to(cr, 195, 100);
cairo_line_to(cr, 195, 130);
cairo_line_to(cr, 195, 120);
cairo_line_to(cr, 210, 120);
cairo_line_to(cr, 190, 120);
cairo_line_to(cr, 185, 115);
cairo_line_to(cr, 180, 120);
cairo_line_to(cr, 180, 110);
cairo_line_to(cr, 175, 110);
cairo_line_to(cr, 175, 130);
cairo_line_to(cr, 170, 130);
cairo_line_to(cr, 170, 100);
cairo_line_to(cr, 165, 100);
cairo_line_to(cr, 170, 120);
cairo_line_to(cr, 160, 110);
cairo_line_to(cr, 160, 100);
cairo_stroke(cr);
}


void halfden(cairo_t *cr);
{
cairo_line_to(cr, 160, 100);
cairo_line_to(cr, 340, 100);
cairo_stroke(cr);
cairo_line_to(cr, 340, 100);
cairo_line_to(cr, 340, 120);
cairo_line_to(cr, 320, 140);
cairo_line_to(cr, 300, 150);
cairo_line_to(cr, 300, 145);
cairo_line_to(cr, 310, 140);
cairo_line_to(cr, 320, 130);
cairo_line_to(cr, 320, 100);
cairo_line_to(cr, 340, 100);
cairo_close_path(cr);
cairo_fill(cr);
cairo_line_to(cr, 190, 140);
cairo_line_to(cr, 160, 110);
cairo_line_to(cr, 160, 100);
cairo_line_to(cr, 180, 100);
cairo_line_to(cr, 180, 120);
cairo_line_to(cr, 190, 130);
cairo_line_to(cr, 200, 130);
cairo_line_to(cr, 200, 135);
cairo_line_to(cr, 195, 135);
cairo_line_to(cr, 195, 140);
cairo_line_to(cr, 190, 140);
cairo_close_path(cr);
cairo_fill(cr);
cairo_line_to(cr, 210, 130);
cairo_line_to(cr, 220, 120);
cairo_line_to(cr, 220, 100);
cairo_line_to(cr, 230, 100);
cairo_line_to(cr, 230, 135);
cairo_line_to(cr, 235, 145);
cairo_line_to(cr, 220, 145);
cairo_line_to(cr, 220, 140);
cairo_line_to(cr, 215, 140);
cairo_line_to(cr, 215, 135);
cairo_line_to(cr, 210, 135);
cairo_line_to(cr, 210, 130);
cairo_close_path(cr);
cairo_fill(cr);
cairo_line_to(cr, 240, 100);
cairo_line_to(cr, 240, 135);
cairo_stroke(cr);
cairo_line_to(cr, 240, 155);
cairo_line_to(cr, 240, 160);
cairo_stroke(cr);
cairo_line_to(cr, 260, 100);
cairo_line_to(cr, 260, 160);
cairo_stroke(cr);
cairo_line_to(cr, 240, 100);
cairo_line_to(cr, 260, 125);
cairo_line_to(cr, 240, 125);
cairo_line_to(cr, 260, 150);
cairo_line_to(cr, 240, 150);
cairo_stroke(cr);
cairo_line_to(cr, 260, 125);
cairo_line_to(cr, 275, 100);
cairo_stroke(cr);
cairo_line_to(cr, 305, 100);
cairo_line_to(cr, 305, 135);
cairo_stroke(cr);
cairo_line_to(cr, 305, 150);
cairo_line_to(cr, 305, 160);
cairo_stroke(cr);
cairo_line_to(cr, 325, 140);
cairo_line_to(cr, 325, 160);
cairo_stroke(cr);
cairo_line_to(cr, 305, 100);
cairo_line_to(cr, 315, 110);
cairo_stroke(cr);
cairo_line_to(cr, 315, 125);
cairo_line_to(cr, 305, 125);
cairo_line_to(cr, 310, 130);
cairo_stroke(cr);
cairo_line_to(cr, 325, 150);
cairo_line_to(cr, 305, 150);
cairo_stroke(cr);
cairo_line_to(cr, 285, 100);
cairo_line_to(cr, 305, 125);
cairo_stroke(cr);
}


void den(cairo_t *cr);
{
cairo_line_to(cr, 160, 100);
cairo_line_to(cr, 340, 100);
cairo_stroke(cr);
cairo_line_to(cr, 340, 100);
cairo_line_to(cr, 340, 120);
cairo_line_to(cr, 320, 140);
cairo_line_to(cr, 280, 160);
cairo_line_to(cr, 230, 160);
cairo_line_to(cr, 190, 140);
cairo_line_to(cr, 160, 110);
cairo_line_to(cr, 160, 100);
cairo_line_to(cr, 180, 100);
cairo_line_to(cr, 180, 120);
cairo_line_to(cr, 190, 130);
cairo_line_to(cr, 210, 130);
cairo_line_to(cr, 220, 120);
cairo_line_to(cr, 220, 100);
cairo_line_to(cr, 230, 100);
cairo_line_to(cr, 230, 140);
cairo_line_to(cr, 240, 150);
cairo_line_to(cr, 280, 150);
cairo_line_to(cr, 310, 140);
cairo_line_to(cr, 320, 130);
cairo_line_to(cr, 320, 100);
cairo_line_to(cr, 340, 100);
cairo_close_path(cr);
cairo_fill(cr);
cairo_line_to(cr, 185, 115);
cairo_line_to(cr, 215, 115);
cairo_stroke(cr);
cairo_line_to(cr, 235, 120);
cairo_line_to(cr, 315, 120);
cairo_stroke(cr);
cairo_line_to(cr, 240, 105);
cairo_line_to(cr, 240, 140);
cairo_stroke(cr);
cairo_line_to(cr, 250, 105);
cairo_line_to(cr, 250, 140);
cairo_stroke(cr);
cairo_line_to(cr, 260, 105);
cairo_line_to(cr, 260, 140);
cairo_stroke(cr);
cairo_line_to(cr, 270, 105);
cairo_line_to(cr, 270, 140);
cairo_stroke(cr);
cairo_line_to(cr, 280, 105);
cairo_line_to(cr, 280, 140);
cairo_stroke(cr);
cairo_line_to(cr, 290, 105);
cairo_line_to(cr, 290, 140);
cairo_stroke(cr);
cairo_line_to(cr, 300, 105);
cairo_line_to(cr, 300, 135);
cairo_stroke(cr);
cairo_line_to(cr, 310, 105);
cairo_line_to(cr, 310, 130);
cairo_stroke(cr);
}


void lizardspear(cairo_t *cr);
{
cairo_scale(cr, 5.000000, 5.000000);
cairo_line_to(cr, 23, 4);
cairo_line_to(cr, 36, 4);
cairo_line_to(cr, 35, 6);
cairo_line_to(cr, 37, 6);
cairo_line_to(cr, 35, 8);
cairo_line_to(cr, 35, 14);
cairo_line_to(cr, 36, 16);
cairo_line_to(cr, 38, 17);
cairo_line_to(cr, 40, 17);
cairo_line_to(cr, 41, 15);
cairo_line_to(cr, 41, 8);
cairo_line_to(cr, 39, 6);
cairo_line_to(cr, 41, 6);
cairo_line_to(cr, 40, 4);
cairo_line_to(cr, 53, 4);
cairo_line_to(cr, 52, 6);
cairo_line_to(cr, 48, 8);
cairo_line_to(cr, 48, 16);
cairo_line_to(cr, 47, 18);
cairo_line_to(cr, 52, 21);
cairo_line_to(cr, 54, 25);
cairo_line_to(cr, 56, 35);
cairo_line_to(cr, 58, 37);
cairo_line_to(cr, 60, 38);
cairo_line_to(cr, 62, 36);
cairo_line_to(cr, 63, 34);
cairo_line_to(cr, 64, 36);
cairo_line_to(cr, 66, 37);
cairo_line_to(cr, 65, 38);
cairo_line_to(cr, 65, 39);
cairo_line_to(cr, 67, 42);
cairo_line_to(cr, 64, 41);
cairo_line_to(cr, 63, 41);
cairo_line_to(cr, 63, 44);
cairo_line_to(cr, 60, 42);
cairo_line_to(cr, 58, 42);
cairo_line_to(cr, 54, 40);
cairo_line_to(cr, 49, 29);
cairo_line_to(cr, 47, 26);
cairo_line_to(cr, 44, 24);
cairo_line_to(cr, 45, 27);
cairo_line_to(cr, 47, 28);
cairo_line_to(cr, 48, 30);
cairo_line_to(cr, 46, 30);
cairo_line_to(cr, 47, 32);
cairo_line_to(cr, 49, 33);
cairo_line_to(cr, 50, 35);
cairo_line_to(cr, 47, 36);
cairo_line_to(cr, 49, 39);
cairo_line_to(cr, 47, 40);
cairo_line_to(cr, 47, 41);
cairo_line_to(cr, 51, 42);
cairo_line_to(cr, 49, 44);
cairo_line_to(cr, 49, 46);
cairo_line_to(cr, 50, 49);
cairo_line_to(cr, 47, 49);
cairo_line_to(cr, 46, 51);
cairo_line_to(cr, 44, 50);
cairo_line_to(cr, 43, 52);
cairo_line_to(cr, 42, 50);
cairo_line_to(cr, 40, 50);
cairo_line_to(cr, 38, 48);
cairo_line_to(cr, 34, 47);
cairo_line_to(cr, 33, 48);
cairo_line_to(cr, 32, 46);
cairo_line_to(cr, 31, 47);
cairo_line_to(cr, 30, 45);
cairo_line_to(cr, 29, 44);
cairo_line_to(cr, 30, 42);
cairo_line_to(cr, 32, 41);
cairo_line_to(cr, 38, 42);
cairo_line_to(cr, 37, 40);
cairo_line_to(cr, 32, 39);
cairo_line_to(cr, 30, 38);
cairo_line_to(cr, 29, 39);
cairo_line_to(cr, 26, 39);
cairo_line_to(cr, 20, 38);
cairo_line_to(cr, 17, 39);
cairo_line_to(cr, 17, 51);
cairo_line_to(cr, 15, 51);
cairo_line_to(cr, 15, 39);
cairo_line_to(cr, 14, 39);
cairo_line_to(cr, 13, 38);
cairo_line_to(cr, 13, 36);
cairo_line_to(cr, 12, 35);
cairo_line_to(cr, 12, 34);
cairo_line_to(cr, 13, 33);
cairo_line_to(cr, 13, 32);
cairo_line_to(cr, 14, 31);
cairo_line_to(cr, 15, 31);
cairo_line_to(cr, 15, 15);
cairo_line_to(cr, 17, 15);
cairo_line_to(cr, 17, 31);
cairo_line_to(cr, 19, 33);
cairo_line_to(cr, 27, 33);
cairo_line_to(cr, 30, 34);
cairo_line_to(cr, 33, 33);
cairo_line_to(cr, 32, 32);
cairo_line_to(cr, 32, 29);
cairo_line_to(cr, 33, 28);
cairo_line_to(cr, 35, 25);
cairo_line_to(cr, 35, 23);
cairo_line_to(cr, 31, 19);
cairo_line_to(cr, 29, 15);
cairo_line_to(cr, 29, 9);
cairo_line_to(cr, 25, 7);
cairo_line_to(cr, 23, 5);
cairo_line_to(cr, 23, 4);
cairo_line_to(cr, 42, 46);
cairo_line_to(cr, 39, 46);
cairo_line_to(cr, 42, 48);
cairo_line_to(cr, 42, 46);
cairo_close_path(cr);
cairo_fill(cr);
cairo_line_to(cr, 15, 14);
cairo_line_to(cr, 15, 12);
cairo_line_to(cr, 17, 12);
cairo_line_to(cr, 17, 14);
cairo_line_to(cr, 15, 14);
cairo_close_path(cr);
cairo_fill(cr);
cairo_line_to(cr, 15, 11);
cairo_line_to(cr, 15, 8);
cairo_line_to(cr, 16, 5);
cairo_line_to(cr, 17, 9);
cairo_line_to(cr, 17, 11);
cairo_line_to(cr, 15, 11);
cairo_close_path(cr);
cairo_fill(cr);
cairo_line_to(cr, 16, 75);
cairo_line_to(cr, 14, 72);
cairo_line_to(cr, 15, 70);
cairo_line_to(cr, 17, 70);
cairo_line_to(cr, 18, 72);
cairo_line_to(cr, 16, 75);
cairo_close_path(cr);
cairo_fill(cr);
cairo_line_to(cr, 15, 69);
cairo_line_to(cr, 15, 68);
cairo_line_to(cr, 5, 68);
cairo_line_to(cr, 5, 66);
cairo_line_to(cr, 15, 66);
cairo_line_to(cr, 15, 65);
cairo_line_to(cr, 6, 65);
cairo_line_to(cr, 6, 57);
cairo_line_to(cr, 5, 55);
cairo_line_to(cr, 3, 53);
cairo_line_to(cr, 2, 51);
cairo_line_to(cr, 7, 51);
cairo_line_to(cr, 8, 53);
cairo_line_to(cr, 8, 51);
cairo_line_to(cr, 19, 51);
cairo_line_to(cr, 20, 53);
cairo_line_to(cr, 22, 53);
cairo_line_to(cr, 21, 51);
cairo_line_to(cr, 22, 51);
cairo_line_to(cr, 25, 55);
cairo_line_to(cr, 26, 58);
cairo_line_to(cr, 26, 65);
cairo_line_to(cr, 17, 65);
cairo_line_to(cr, 17, 66);
cairo_line_to(cr, 27, 66);
cairo_line_to(cr, 27, 68);
cairo_line_to(cr, 17, 68);
cairo_line_to(cr, 17, 69);
cairo_line_to(cr, 15, 69);
cairo_close_path(cr);
cairo_fill(cr);
}


void lizardbanner(cairo_t *cr);
{
cairo_scale(cr, 5.000000, 5.000000);
cairo_line_to(cr, 10, 0);
cairo_line_to(cr, 30, 0);
cairo_line_to(cr, 30, 2);
cairo_line_to(cr, 32, 0);
cairo_line_to(cr, 30, 4);
cairo_line_to(cr, 30, 6);
cairo_line_to(cr, 32, 10);
cairo_line_to(cr, 40, 18);
cairo_line_to(cr, 46, 20);
cairo_line_to(cr, 50, 18);
cairo_line_to(cr, 56, 4);
cairo_line_to(cr, 48, 4);
cairo_line_to(cr, 46, 2);
cairo_line_to(cr, 46, 0);
cairo_line_to(cr, 64, 0);
cairo_line_to(cr, 64, 2);
cairo_line_to(cr, 66, 0);
cairo_line_to(cr, 64, 4);
cairo_line_to(cr, 62, 8);
cairo_line_to(cr, 56, 18);
cairo_line_to(cr, 58, 20);
cairo_line_to(cr, 58, 24);
cairo_line_to(cr, 74, 40);
cairo_line_to(cr, 80, 48);
cairo_line_to(cr, 82, 52);
cairo_line_to(cr, 84, 54);
cairo_line_to(cr, 88, 54);
cairo_line_to(cr, 90, 52);
cairo_line_to(cr, 90, 50);
cairo_line_to(cr, 94, 50);
cairo_line_to(cr, 94, 54);
cairo_line_to(cr, 90, 58);
cairo_line_to(cr, 82, 58);
cairo_line_to(cr, 78, 54);
cairo_line_to(cr, 72, 44);
cairo_line_to(cr, 68, 40);
cairo_line_to(cr, 64, 36);
cairo_line_to(cr, 60, 34);
cairo_line_to(cr, 56, 36);
cairo_line_to(cr, 56, 48);
cairo_line_to(cr, 58, 50);
cairo_line_to(cr, 60, 56);
cairo_line_to(cr, 60, 60);
cairo_line_to(cr, 54, 66);
cairo_line_to(cr, 54, 70);
cairo_line_to(cr, 50, 76);
cairo_line_to(cr, 40, 76);
cairo_line_to(cr, 38, 74);
cairo_line_to(cr, 22, 74);
cairo_line_to(cr, 20, 70);
cairo_line_to(cr, 22, 68);
cairo_line_to(cr, 42, 68);
cairo_line_to(cr, 44, 62);
cairo_line_to(cr, 42, 56);
cairo_line_to(cr, 38, 58);
cairo_line_to(cr, 16, 60);
cairo_line_to(cr, 6, 76);
cairo_line_to(cr, 10, 76);
cairo_line_to(cr, 2, 90);
cairo_line_to(cr, 2, 74);
cairo_line_to(cr, 2, 76);
cairo_line_to(cr, 12, 60);
cairo_line_to(cr, 10, 60);
cairo_line_to(cr, 12, 56);
cairo_line_to(cr, 14, 56);
cairo_line_to(cr, 24, 40);
cairo_line_to(cr, 28, 42);
cairo_line_to(cr, 24, 48);
cairo_line_to(cr, 22, 48);
cairo_line_to(cr, 22, 50);
cairo_line_to(cr, 18, 56);
cairo_line_to(cr, 28, 54);
cairo_line_to(cr, 42, 46);
cairo_line_to(cr, 42, 26);
cairo_line_to(cr, 30, 18);
cairo_line_to(cr, 28, 18);
cairo_line_to(cr, 24, 12);
cairo_line_to(cr, 24, 10);
cairo_line_to(cr, 20, 4);
cairo_line_to(cr, 12, 4);
cairo_line_to(cr, 10, 2);
cairo_line_to(cr, 10, 0);
cairo_close_path(cr);
cairo_fill(cr);
}


int symbols_n = 21;

vf symbols[] = {swamp,temple,fertile,hill,volcano,plains,cursed,raftfill,raft,scrubold,scrub,one,two,blackbridge,whitebridge,whirlpool,ruin,halfden,den,lizardspear,lizardbanner};
