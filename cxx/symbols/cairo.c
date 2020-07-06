#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <gtk/gtk.h>
#include <cairo.h>
#include <cairo-xlib.h>
#include <cairo-svg.h>
#include <getopt.h>
#include "lizardface.h"

#include "symbols.h"
#include "symbols.c"

int cairo_expose(cairo_t *cairo, int width, int height)
{
  int box = 500;
  int cols = 6;
  cairo_scale(cairo, 0.4, 0.4);
  for (int j = 0; j < 4; j++)
    for (int i = 0; i < cols; i++)
      {
	//	cairo_push_group(cairo);
	cairo_save(cairo);
	cairo_translate(cairo, box * i, box * j);

  /* --------------------
     drawing commands
     -------------------- */

	cairo_new_path(cairo);
	cairo_move_to(cairo, 0, 0);
	cairo_line_to(cairo,  box, 0);
	cairo_line_to(cairo,  box, box);
	cairo_line_to(cairo, 0, box);
	cairo_close_path(cairo);

	cairo_set_source_rgb(cairo, 0.4, 0.95, 0.37);
	cairo_set_line_cap(cairo, CAIRO_LINE_CAP_SQUARE);
	cairo_set_line_width(cairo, 1.0);
	cairo_set_source_rgb(cairo, 0, 0, 0);
	cairo_stroke(cairo);

	cairo_translate(cairo, 0, box);
	cairo_scale(cairo, 1.0, -1.0);

	cairo_save(cairo);

	cairo_translate(cairo, box/2, 0);

	/* left triangle */

	cairo_set_source_rgb(cairo, 0.7,0.7,1);
	cairo_move_to(cairo, 0, 0);
	cairo_line_to(cairo, 250, 0);
	cairo_line_to(cairo, 250, 250);
	cairo_fill(cairo);

	/* right triangle */

	cairo_set_source_rgb(cairo, 0.7,1,1);
	cairo_move_to(cairo, 0, 0);
	cairo_line_to(cairo, -250, 0);
	cairo_line_to(cairo, -250, 250);
	cairo_fill(cairo);

	cairo_set_source_rgb(cairo, 0,0,0);

	cairo_save(cairo);
	int s = j*cols+i;
	if (s < symbols_n)
	  (*symbols[s])(cairo);
	cairo_restore(cairo);	
	cairo_restore(cairo);
	cairo_restore(cairo);
      }
  cairo_show_page(cairo);
}

void xlib_expose(cairo_surface_t *surface, int width, int height)
{
  cairo_t *cairo = cairo_create(surface);
  cairo_expose(cairo, width, height);
  cairo_destroy(cairo);
}

int xlib(int argc, char *argv[], int width, int height)
{
  /* --------------------
     Xlib variable
     -------------------- */

  Display *dpy;
  int screen;
  Visual *visual;
  Window root, win;
  Atom wm_protocols, wm_delete_window;
  GC gc;
  XEvent event;
  int background;

  /* --------------------
     cairo vairables
  -------------------- */

  cairo_surface_t *surface;


  dpy = XOpenDisplay(NULL);
  if (dpy == NULL) {
    fprintf(stderr, "%s:  unable to open display \"%s\"\n",
	    argv[0], XDisplayName(":0"));
    exit(1);
  }

  screen = XDefaultScreen(dpy);
  visual = XDefaultVisual(dpy, screen);
  root = XRootWindow(dpy, screen);
  background = WhitePixel(dpy, screen);

  
  printf("Creating Simple Window\n");

  win = XCreateSimpleWindow(dpy, RootWindow(dpy, screen), -width/2, -height/2, width,
			    height, 5, 1, background);


  /* --------------------
     Create Xlib surface
     -------------------- */

  surface = cairo_xlib_surface_create(dpy, win, visual, width, height);

  XSelectInput(dpy, win,
	       ExposureMask | ButtonPressMask | StructureNotifyMask);


  XMapWindow(dpy, win);
  wm_protocols = XInternAtom(dpy, "WM_PROTOCOLS", False);
  wm_delete_window = XInternAtom(dpy, "WM_DELETE_WINDOW", False);
  XSetWMProtocols(dpy, win, &wm_delete_window, 1);
  while(1)
    {
      XNextEvent(dpy, &event);
      switch(event.type)
	{
	case ClientMessage:
	  if (event.xclient.message_type == wm_protocols &&
	      event.xclient.data.l[0] == wm_delete_window)
	    {
	      XCloseDisplay(dpy);
	      exit(0);
	    }
	  break;
	case Expose:
	  //	  printf("Expose\n");
	  xlib_expose(surface, width, height);
	  //	  XCopyArea(dpy, PicturePixmap, win, gc,
	  //		    0, 0, width, height, 0, 0);
	  break;
	}
    }
}

G_DEFINE_TYPE(LizardFace, lizard_face, GTK_TYPE_DRAWING_AREA);

static gboolean lizard_face_expose (GtkWidget *clock, GdkEventExpose *event)
 {
   int width, height;
   cairo_t *cr;
   gdk_drawable_get_size(clock->window, &width, &height);


   /* get a cairo_t */
   cr = gdk_cairo_create (clock->window);
   cairo_expose(cr, width, height);
   cairo_destroy (cr);
   return FALSE;
 }

 static void lizard_face_class_init (LizardFaceClass *class)
 {
        GtkWidgetClass *widget_class;

        widget_class = GTK_WIDGET_CLASS (class);

        widget_class->expose_event = lizard_face_expose;
 }

 static void lizard_face_init (LizardFace *clock)
 {
 }

 GtkWidget * lizard_face_new (void)
 {
        return g_object_new (TYPE_LIZARD_FACE, NULL);
 }


int gtk(int argc, char *argv[], int width, int height)
{
  GtkWidget *window;
  GtkWidget *clock;

  gtk_init (&argc, &argv);

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);


  clock = lizard_face_new ();

  gtk_widget_set_size_request(GTK_WIDGET(clock), width, height);

  gtk_container_add (GTK_CONTAINER (window), clock);

  g_signal_connect (window, "destroy", G_CALLBACK (gtk_main_quit), NULL);

  gtk_widget_show_all (window);
  
  gtk_main ();
}


int svg(int argc, char *argv[], int width, int height)
{
  cairo_surface_t *surface = cairo_svg_surface_create("symbols.svg", width, height);
  cairo_t *cairo = cairo_create(surface);
  cairo_expose(cairo, width, height);
  cairo_destroy(cairo);
  cairo_surface_destroy(surface);
} 

int main(int argc, char *argv[])
{
  int width = 1200;
  int height = 800;

  int ch;
  while ((ch = getopt(argc, argv, "gsx")) != EOF)
    switch(ch)
      {
      case 'x':
	xlib(argc, argv, width, height);
	break;
      case 'g':
	gtk(argc, argv, width, height);
	break;
      case 's':
	svg(argc, argv, width, height);
	break;
      }
}
