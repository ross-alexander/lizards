#include <stdlib.h>
#include <math.h>
#include <libxml/tree.h>
#include <libxml++/libxml++.h>
#include <gtk/gtk.h>
#include <librsvg/rsvg.h>

#include "lizards.h"
#include "hexlist.h"
#include "output.h"

#include "symbols/symbols.h"
#include "symbols/symbols.c"

typedef struct _SwampMap            SwampMap;
typedef struct _SwampMapClass       SwampMapClass;

struct gtk_tab_t {
  int player;
  char *label;
  SwampMap *sm;
  cairo_tab_t* ct;
  int width;
  int height;
};

/* ----------------------------------------------------------------------
--
-- GObject stuff
--
---------------------------------------------------------------------- */

struct _SwampMap
{
  GtkDrawingArea parent;
  output_gtk_t *gtk;
  gtk_tab_t *tab;
  cairo_surface_t *surface;
  /* private */
};

struct _SwampMapClass
{
  GtkDrawingAreaClass parent_class;
};

#define TYPE_SWAMP_MAP             (swamp_map_get_type ())
#define SWAMP_MAP(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), TYPE_SWAMP_MAP, SwampMap))

G_DEFINE_TYPE(SwampMap, swamp_map, GTK_TYPE_DRAWING_AREA);

static gboolean swamp_map_expose (GtkWidget *smw, GdkEventExpose *event)
{
  SwampMap *sm = SWAMP_MAP(smw);
  cairo_t *cr = gdk_cairo_create(smw->window);
  cairo_set_source_surface(cr, sm->surface, 0, 0);
  cairo_paint(cr);
  sm->tab->ct->draw(cr);
  cairo_destroy(cr);
  return 0;
}

static gboolean swamp_map_configure(GtkWidget *smw, GdkEventConfigure* ev)
{
  SwampMap *sm = SWAMP_MAP(smw);

  printf("Reconfiguring map for player %d (%d x %d)\n", sm->tab->ct->player, sm->tab->width, sm->tab->height);

  if (sm->surface == NULL)
    {
      sm->surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, sm->tab->width, sm->tab->height);
      cairo_t *cr = cairo_create(sm->surface);
      cairo_rectangle(cr, 0, 0, sm->tab->width, sm->tab->height);
      cairo_set_source_rgb(cr, 1,1,1);
      cairo_fill_preserve(cr);
      cairo_stroke(cr);

      sm->gtk->draw(cr, sm->tab);

      cairo_destroy(cr);
      cairo_surface_flush(sm->surface);
    }
  return 1;
}

static void swamp_map_init (SwampMap *sm)
{
  sm->surface = NULL;
}

static void swamp_map_class_init (SwampMapClass *sm_class)
{
  GtkWidgetClass *widget_class;
  widget_class = GTK_WIDGET_CLASS (sm_class);
  widget_class->expose_event = swamp_map_expose;
  widget_class->configure_event = swamp_map_configure;
 }


GtkWidget *swamp_map_new (output_gtk_t *g, gtk_tab_t *t)
{
  SwampMap *sm = SWAMP_MAP(g_object_new (TYPE_SWAMP_MAP, NULL));
  sm->gtk = g;
  sm->tab = t;
  return GTK_WIDGET(sm);
}

/* ----------------------------------------------------------------------
--
--
--
---------------------------------------------------------------------- */
output_gtk_t::output_gtk_t(world_t *w, int p_argc, char *p_argv[]) : output_cairo_t(w, p_argc, p_argv)
{
}

double fill[][3] = {
  {0.1,0.1,0.9},
  {0.1,0.8,0.2},
  {0.7,0.4,0.1},
  {0.9,1.0,0.9},
  {0.8,0.8,0.0}};
  
svg_cairo_t *svg_outputs[OUTPUT_LAST];


void output_gtk_t::draw(cairo_t *cr, gtk_tab_t *tab)
{
}

/* ----------------------------------------------------------------------
--
--
--
---------------------------------------------------------------------- */

void output_gtk_t::display()
{
  int width = 600;
  int height = 600;

  gtk_init (&argc, &argv);

  /* --------------------
     Create window
     -------------------- */

  GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

  /* --------------------
     Create tabbed notebook
     -------------------- */

  GtkWidget *notebook = gtk_notebook_new();
  gtk_container_add(GTK_CONTAINER(window), notebook);


  /* --------------------
     Create tree
     -------------------- */

  GtkTreeStore *store = gtk_tree_store_new(1, G_TYPE_STRING);

  GtkTreeIter iter1, iter2;

  for (int i = 1; i < world->num_players; i++)
    {
      printf("%s\n", world->player[i]->clan);
      gtk_tree_store_append(store, &iter1, NULL);
      gtk_tree_store_set(store, &iter1, 0, world->player[i]->clan, -1);
      bandlist_t *b = new bandlist_t(world, i);
      for (int j = 0; j < b->cnt; j++)
	{
	  hex_t *hex = (*world->map)(b->list[j].xy);
	  gtk_tree_store_append(store, &iter2, &iter1);
	  gtk_tree_store_set(store, &iter2, 0, hex->title, -1);
	}
    }


  GtkWidget *tree = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));

  GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
  GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes("Clan",
								       renderer,
								       "text",
								       0,
								       NULL);
  gtk_tree_view_append_column(GTK_TREE_VIEW(tree), column);
							       

  gtk_notebook_append_page(GTK_NOTEBOOK(notebook), tree, NULL);

  /* --------------------
     Create per tab structures
     -------------------- */

  gtk_tab_t *maps = new gtk_tab_t[world->num_players];

  for (int i = 1; i < world->num_players; i++)
    {
      gtk_tab_t *tab = &maps[i-1];

      /* --------------------
	 Create hexlist to get bounding box
	 -------------------- */

      tab->ct = tabs[i];
      tab->width = tabs[i]->width;
      tab->height = tabs[i]->height;

      printf("Determining dimensions of map for player %d (%d x %d)\n",
	     i, tab->width, tab->height);

      char *label_text;
      GtkWidget *sm, *vbox, *label;
      vbox = gtk_vbox_new(FALSE, 0);

      /* --------------------
	 Create label and text
	 -------------------- */
      
      label_text = g_strdup_printf("Player %d - %s", i, world->player[i]->clan);
      label = gtk_label_new(label_text);

      /* --------------------
	 Create custom SwampMap widget
	 -------------------- */

      sm = swamp_map_new (this, tab);
      gtk_widget_set_size_request(GTK_WIDGET(sm), width, height);

      gtk_box_pack_start(GTK_BOX(vbox), label, 0, 0, 0);
      gtk_box_pack_start(GTK_BOX(vbox), sm, 1, 1, 0);
 
     gtk_notebook_append_page(GTK_NOTEBOOK(notebook), vbox, NULL);
    }

  /* --------------------
     Connect up main window and start the show
     -------------------- */

  g_signal_connect (G_OBJECT (window), "destroy", G_CALLBACK (gtk_main_quit), NULL);
  gtk_widget_show_all (window);
  gtk_main ();
}
