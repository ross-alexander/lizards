#include <GL/openglut.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <gts.h>

#include <gtk/gtk.h>
#include <gtk/gtkgl.h>
#include <libxml/tree.h>

#include "lizards.h"
#include "hexlist.h"
#include "output.h"

struct lizgl_bits {
  world_t *w;
};

lizgl_bits *bits;

double d = 0.3;
double light_switch = 1.0;
double eye_x = 0.0;
double eye_y = 0.0;
double eye_z = 5.0;
double mouse_x;
double mouse_y;
double alpha = 0.0;
double beta = 0.0;
int model = 1;

void lizgl_key(unsigned char k)
{
  switch (k) {
  case 27:  /* Escape */
    exit(0);
    break;

  case 'a':
    eye_z += 1;
    break;

  case 's':
    eye_z -= 1;
    break;

  case 'i':
    alpha += 10;
    break;

  case 'j':
    alpha -= 10;
    break;

  case 'k':
    beta -= 10;
    break;

  case 'l':
    beta += 10;
    break;

  case 'z':
    light_switch = 1.0 - light_switch;
    break;

  case ' ':
    model = 1 - model;
    break;

  case 'q':
    exit(0);
    break;

  default:
    printf("Unknown key %d\n", k);
  }
}

void key(unsigned char k, int x, int y)
{
  lizgl_key(k);
  glutPostRedisplay();
}

gint draw_triangle(GtsFace *f, void *d)
{
  //  double nx, ny, nz;
  GtsVertex *v[3];
  GtsTriangle *t = &f->triangle;

  gts_triangle_vertices(t, &v[0], &v[1], &v[2]);

  //  gts_triangle_normal(t, &nx, &ny, &nz);
  //  glNormal3f(nx, ny, nz);

  for (int i = 0; i < 3; i++)
    {
      double nf[3] = {0.0, 0.0, 0.0};
      GSList *lt, *l = gts_vertex_triangles(v[i], NULL);
      
      for (lt = l; lt; lt = lt->next)
	{
	  double nt[3];
	  gts_triangle_normal((GtsTriangle*)lt->data, &nt[0], &nt[1], &nt[2]);
	  nf[0] += nt[0];
	  nf[1] += nt[1];
	  nf[2] += nt[2];
	}
      g_slist_free(l);
      glNormal3f(nf[0], nf[1], nf[2]);
      glColor3f(GTS_COLOR_VERTEX(v[i])->c.r,
		GTS_COLOR_VERTEX(v[i])->c.g,
		GTS_COLOR_VERTEX(v[i])->c.b);
      glVertex3f(v[i]->p.x, v[i]->p.y, v[i]->p.z);
    }
  return 1;
}

/* ----------------------------------------------------------------------
--
-- check_adj
--
---------------------------------------------------------------------- */
double check_adj(hex_t *hex, Direction d1, Direction d2, double a, double b)
{
  Terrain t = hex->terrain;
  int adj = (hex->adjacent(d1)->terrain == t) && (hex->adjacent(d2)->terrain == t);
  //  printf("%d %d %d %d\n", hex->terrain, hex->adjacent(d1)->terrain, hex->adjacent(d2)->terrain, adj);
  if (adj)
    return a;
  else
    return b;
}

/* ----------------------------------------------------------------------
--
-- draw_poly
--
---------------------------------------------------------------------- */
void draw_poly(void)
{
  hexlist_t* hexlist = new hexlist_t(bits->w, 1);
  hexlist->fzero();

  for (int work = 0; work < hexlist->cnt; work ++)
    {
      see_t *see = &hexlist->list[work];
      
      glPushMatrix();

      hex_t *hex = bits->w->map.hex(see->x, see->y);
#ifdef OldColorCode
      switch (hex->terrain)
	{
	case WATER:
	  glColor3f(0.0, 0.0, 0.5);
	  break;
	case PLAIN:
	  glColor3f(0.8, 0.8, 0.0);
	  break;
	default:
	  glColor3f(0.0, 0.5, 0.0);
	  break;
	}
#endif
      GtsSurface *s = gts_surface_new(gts_surface_class(),
				      gts_face_class(),
				      gts_edge_class(),
				      gts_vertex_class());
      
      GtsVertex* v[7];
      double r = 1.0;
      double hr = 0.5 * r;
      double rs = sqrt(3) * hr;
      
      double cx = see->mx;
      double cy = -see->my;
      double cz;
      
      if (hex->terrain == WATER)
	cz = -d;
      else
	cz = d;
      

      v[0] = gts_vertex_new(gts_color_vertex_class(), 2 * +hr + cx, 0.0 + cy, check_adj(hex, NORTHEAST, SOUTHEAST, cz, 0.0));
      v[1] = gts_vertex_new(gts_color_vertex_class(), 1 * +hr + cx, +rs + cy, check_adj(hex, NORTHEAST, NORTH, cz, 0.0));
      v[2] = gts_vertex_new(gts_color_vertex_class(), 1 * -hr + cx, +rs + cy, check_adj(hex, NORTHWEST, NORTH, cz, 0.0));
      v[3] = gts_vertex_new(gts_color_vertex_class(), 2 * -hr + cx, 0.0 + cy, check_adj(hex, NORTHWEST, SOUTHWEST, cz, 0.0));
      v[4] = gts_vertex_new(gts_color_vertex_class(), 1 * -hr + cx, -rs + cy, check_adj(hex, SOUTHWEST, SOUTH, cz, 0.0));
      v[5] = gts_vertex_new(gts_color_vertex_class(), 1 * +hr + cx, -rs + cy, check_adj(hex, SOUTHEAST, SOUTH, cz, 0.0));
      v[6] = gts_vertex_new(gts_color_vertex_class(), cx, cy, cz);


      if (hex->terrain == WATER)
	{
	  GTS_COLOR_VERTEX(v[6])->c.r = 0.0;
	  GTS_COLOR_VERTEX(v[6])->c.g = 0.0;
	  GTS_COLOR_VERTEX(v[6])->c.b = 0.5;
	  for (int j = 0; j < 6; j++)
	    {
	      GTS_COLOR_VERTEX(v[j])->c.r = 0.0;
	      GTS_COLOR_VERTEX(v[j])->c.g = 0.0;
	      GTS_COLOR_VERTEX(v[j])->c.b = 0.5;
	    }
	}
      else
	{
	  GTS_COLOR_VERTEX(v[6])->c.r = 0.0;
	  GTS_COLOR_VERTEX(v[6])->c.g = 1.0;
	  GTS_COLOR_VERTEX(v[6])->c.b = 0.0;
	  for (int j = 0; j < 6; j++)
	    {
	      GTS_COLOR_VERTEX(v[j])->c.r = 0.0;
	      GTS_COLOR_VERTEX(v[j])->c.g = 1.0;
	      GTS_COLOR_VERTEX(v[j])->c.b = 0.0;
	    }
	}
      for (int j = 0; j < 7; j++)
	{
	  if (v[j]->p.z == 0.0)
	    {
	      GTS_COLOR_VERTEX(v[j])->c.r = 0.0;
	      GTS_COLOR_VERTEX(v[j])->c.b = 0.0;
	      GTS_COLOR_VERTEX(v[j])->c.g = 0.0;
	    }
	}
      
#ifdef UseGLTriangles
      for (int i = 1; i <= 6; i++)
	{
	  double nx, ny, nz;
	  int j = (i+1)%6 + 1;
	  
	  GtsEdge *e1 = gts_edge_new(gts_edge_class(), v[i], v[6]);
	  GtsEdge *e2 = gts_edge_new(gts_edge_class(), v[6], v[j]);
	  GtsEdge *e3 = gts_edge_new(gts_edge_class(), v[j], v[i]);
	  GtsTriangle *t = gts_triangle_new(gts_triangle_class(), e1, e2, e3);
	  gts_triangle_normal(t, &nx, &ny, &nz);
	  gts_surface_add_face(s, (GtsFace*)t);
	}

      glBegin(GL_TRIANGLES);
      gts_surface_foreach_face(s, (GtsFunc)draw_triangle, NULL);
      glEnd();
#elsif Different
      glBegin(GL_TRIANGLES);
      for (int j = 0; j < 6; j++)
	{
	  glColor3f(GTS_COLOR_VERTEX(v[6])->c.r,
		    GTS_COLOR_VERTEX(v[6])->c.g,
		    GTS_COLOR_VERTEX(v[6])->c.b);
	  glVertex3f(v[6]->p.x, v[6]->p.y, v[6]->p.z);
	  for (int i = 0; i < 2; i++)
	    {
	      int k = (j + i)%6;
	      glColor3f(GTS_COLOR_VERTEX(v[k])->c.r,
			GTS_COLOR_VERTEX(v[k])->c.g,
			GTS_COLOR_VERTEX(v[k])->c.b);
	      glVertex3f(v[k]->p.x, v[k]->p.y, v[k]->p.z);
	    }
	}
      glEnd();
#else
      glBegin(GL_TRIANGLE_FAN);
	  glColor3f(GTS_COLOR_VERTEX(v[6])->c.r,
		    GTS_COLOR_VERTEX(v[6])->c.g,
		    GTS_COLOR_VERTEX(v[6])->c.b);
	  glVertex3f(v[6]->p.x, v[6]->p.y, v[6]->p.z);
      for (int i = 0; i < 7; i++)
	{
	  int k = i%6;
	  glColor3f(GTS_COLOR_VERTEX(v[k])->c.r,
		    GTS_COLOR_VERTEX(v[k])->c.g,
		    GTS_COLOR_VERTEX(v[k])->c.b);
	  glVertex3f(v[k]->p.x, v[k]->p.y, v[k]->p.z);
	}
      glEnd();
#endif
      gts_object_destroy((GtsObject*)s);
      glPopMatrix();
    }
}

/* ----------------------------------------------------------------------
--
-- draw
--
---------------------------------------------------------------------- */

void draw()
{
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
  glLoadIdentity();

  // viewing transformation 

  gluLookAt (eye_x, eye_y, eye_z, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);


/* --------------------
   eye-x, eye-y, eye-z, lookat-x, lookat-y, lookat-z, up-x, up-y, up-z
   -------------------- */

  GLfloat light[4];
  light[0] = 0.0;
  light[1] = 0.0;
  light[2] = 4.0;
  light[3] = light_switch;

  glLightfv (GL_LIGHT0, GL_POSITION, light);

  glDisable(GL_LIGHTING);

  glPolygonMode(GL_FRONT, GL_POLYGON);

  glRotatef(alpha, 1.0, 0.0, 0.0);
  glRotatef(beta, 0.0, 1.0, 0.0);

  //  draw_poly();

  glCallList(1);

  glColor3f (1.0, 1.0, 1.0);
  //  glScalef (1.0, 2.0, 1.0);      // modeling transformation 
  // glutWireCylinder(0.2, 4.0, 20, 20); // radius, height, slices, stacks
  glutWireCube (1.0);

  glFlush ();
}

void reshape(int w, int h)
{
  glViewport (0, 0, w, h);
  glMatrixMode (GL_PROJECTION);
  glLoadIdentity ();

/* --------------------
   left, right, bottom, top, near, far
   -------------------- */

//  glFrustum (-1.0, 1.0, -1.0, 1.0, 1.5, 20.0);

  gluPerspective(60.0, 1.0, 1.5, 20.0);

  glMatrixMode (GL_MODELVIEW);
}

/* ----------------------------------------------------------------------
--
-- lizgl_setup
--
---------------------------------------------------------------------- */
void lizgl_setup(void)
{
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glShadeModel(GL_SMOOTH);
  glFrontFace(GL_CCW);
  glDepthFunc(GL_LESS);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glEnable(GL_LIGHTING);
  glEnable(GL_NORMALIZE);
  glDepthFunc(GL_LESS);
  glEnable(GL_COLOR_MATERIAL);

#ifdef Y
  GLfloat mat_ambient[] = { 0.0, 0.0, 0.0, 1.0 };
  GLfloat mat_diffuse[] = { 0.4, 0.4, 0.4, 1.0 };
  GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
  GLfloat mat_shininess[] = { 15.0 };

  GLfloat light_ambient[] = { 0.0, 0.0, 0.0, 1.0 };
  GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
  GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
#else
  const GLfloat light_ambient[]  = { 0.0f, 0.0f, 0.0f, 1.0f };
  const GLfloat light_diffuse[]  = { 1.0f, 1.0f, 1.0f, 1.0f };
  const GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
  //  const GLfloat light_position[] = { 2.0f, 5.0f, 5.0f, 0.0f };
  
  const GLfloat mat_ambient[]    = { 0.7f, 0.7f, 0.7f, 1.0f };
  const GLfloat mat_diffuse[]    = { 0.8f, 0.8f, 0.8f, 1.0f };
  const GLfloat mat_specular[]   = { 1.0f, 1.0f, 1.0f, 1.0f };
  const GLfloat mat_shininess[] = { 100.0f };
#endif

  glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
  glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
  glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

  glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
  
  //  GLfloat lmodel_ambient[] = { 0.5, 0.5, 0.5, 1.0 };
  //  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
  
  glEnable(GL_LIGHT0);
}

/* ----------------------------------------------------------------------
--
-- display
--
---------------------------------------------------------------------- */

void output_glu_t::display(void)
{
  bits = new lizgl_bits;
  bits->w = world;
  
  glutInit(&argc, argv);
  glutInitDisplayMode (GLUT_SINGLE | GLUT_RGBA | GLUT_DEPTH | GLUT_MULTISAMPLE);
  glutInitWindowSize (500, 500);
  glutCreateWindow ("honeycomb");
  
  lizgl_setup();

  glutReshapeFunc(reshape);
  glutDisplayFunc(draw);
  glutKeyboardFunc(key);

  glNewList(1, GL_COMPILE);
  draw_poly();
  glEndList();
  
  glutMainLoop();
}

world_t *output_glu_t::w(void) { return world; }

output_glu_t::output_glu_t(world_t*w, int c, char **v) : output_t(w)
{
  argc = c;
  argv = v;
}


/* ----------------------------------------------------------------------
--
--
--
---------------------------------------------------------------------- */
static gboolean button_event(GtkWidget *widget,
			     GdkEventButton *event)
{
  if (event->type == GDK_BUTTON_PRESS)
    {
      mouse_x = event->x;
      mouse_y = event->y;
    }
  if (event->type == GDK_2BUTTON_PRESS)
    {
      eye_x = eye_y = 0;
      eye_z = 5.0;
      gdk_window_invalidate_rect (widget->window, &widget->allocation, TRUE);
    }
  return 1;
}

static gboolean scroll_event(GtkWidget *widget,
			     GdkEventScroll *event)
{
  if (event->direction == GDK_SCROLL_UP)
    eye_z += 1;
  if (event->direction == GDK_SCROLL_DOWN)
    eye_z -= 1;
  gdk_window_invalidate_rect (widget->window, &widget->allocation, TRUE);
  return 1;
}

static gboolean motion_event(GtkWidget *widget,
			     GdkEventMotion *event)
{
  eye_x += event->x - mouse_x;
  mouse_x = event->x;
  eye_y += event->y - mouse_y;
  mouse_y = event->y;
  gdk_window_invalidate_rect (widget->window, &widget->allocation, TRUE);
  return 1;
}

static gboolean key_event(GtkWidget   *widget,
			  GdkEventKey *event)
{
  lizgl_key(event->keyval);
  gdk_window_invalidate_rect (widget->window, &widget->allocation, TRUE);
  return TRUE;
}

static gboolean expose_event (GtkWidget      *widget,
			      GdkEventExpose *event,
			      gpointer        data)
{
  GdkGLContext *glcontext = gtk_widget_get_gl_context (widget);
  GdkGLDrawable *gldrawable = gtk_widget_get_gl_drawable (widget);

  if (!gdk_gl_drawable_gl_begin (gldrawable, glcontext))
    return FALSE;
  draw();
  gdk_gl_drawable_gl_end (gldrawable);
  return TRUE;
}

static void realize (GtkWidget *widget, gpointer data)
{
  GdkGLContext *glcontext = gtk_widget_get_gl_context (widget);
  GdkGLDrawable *gldrawable = gtk_widget_get_gl_drawable (widget);

  if (!gdk_gl_drawable_gl_begin (gldrawable, glcontext))
    return;

  lizgl_setup();

  glNewList(1, GL_COMPILE);
  draw_poly();
  glEndList();

  gdk_gl_drawable_gl_end (gldrawable);
}

static gboolean configure_event (GtkWidget         *widget,
				 GdkEventConfigure *event,
				 gpointer           data)
{
  GdkGLContext *glcontext = gtk_widget_get_gl_context (widget);
  GdkGLDrawable *gldrawable = gtk_widget_get_gl_drawable (widget);

  if (!gdk_gl_drawable_gl_begin (gldrawable, glcontext))
    return FALSE;

  reshape(widget->allocation.width, widget->allocation.height);

  gdk_gl_drawable_gl_end (gldrawable);
  return TRUE;
}

world_t *output_glx_t::w(void) { return world; }

output_glx_t::output_glx_t(world_t*w, int c, char **v) : output_t(w)
{
  argc = c;
  argv = v;
}


void output_glx_t::display(void)
{
  bits = new lizgl_bits;
  bits->w = world;

  gtk_init (&argc, &argv);
  glutInit(&argc, argv);

  /*
   * Init GtkGLExt.
   */

  gtk_gl_init (&argc, &argv);

  GdkGLConfig *glconfig;
  glconfig = gdk_gl_config_new_by_mode (GdkGLConfigMode(GDK_GL_MODE_RGB   |
							GDK_GL_MODE_DEPTH));

  GtkWidget *vbox = gtk_vbox_new (FALSE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (vbox), 2);

  GtkWidget *drawing_area = gtk_drawing_area_new ();
  gtk_widget_set_size_request (drawing_area, 800, 800);

  /* Set OpenGL-capability to the widget. */

  gtk_widget_set_gl_capability (drawing_area,
                                glconfig,
                                NULL,
                                TRUE,
                                GDK_GL_RGBA_TYPE);
  
  gtk_widget_add_events (drawing_area,
                         GDK_BUTTON1_MOTION_MASK    |
                         GDK_BUTTON2_MOTION_MASK    |
                         GDK_BUTTON_PRESS_MASK      |
                         GDK_KEY_PRESS_MASK         |
                         GDK_VISIBILITY_NOTIFY_MASK);

  g_signal_connect (G_OBJECT (drawing_area), "key_press_event",
  		    G_CALLBACK (key_event), NULL);
  g_signal_connect (G_OBJECT (drawing_area), "button_press_event",
  		    G_CALLBACK (button_event), NULL);
  g_signal_connect (G_OBJECT (drawing_area), "scroll_event",
  		    G_CALLBACK (scroll_event), NULL);
  g_signal_connect (G_OBJECT (drawing_area), "motion_notify_event",
  		    G_CALLBACK (motion_event), NULL);

  g_signal_connect_after (G_OBJECT (drawing_area), "realize",
                          G_CALLBACK (realize), NULL);
  g_signal_connect (G_OBJECT (drawing_area), "configure_event",
		    G_CALLBACK (configure_event), NULL);
  g_signal_connect (G_OBJECT (drawing_area), "expose_event",
  		    G_CALLBACK (expose_event), NULL);


  GtkWidget *window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (window), "button");
  g_signal_connect (G_OBJECT (window), "delete_event",
                    G_CALLBACK (gtk_main_quit), NULL);

  gtk_container_add (GTK_CONTAINER (vbox), drawing_area);
  gtk_container_add (GTK_CONTAINER (window), vbox);

  gtk_widget_show (drawing_area);
  gtk_widget_show (vbox);
  gtk_widget_show (window);
  gtk_main ();
}
