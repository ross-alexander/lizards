#include <string>
#include <map>
#include <vector>
#include <typeinfo>

#include <sys/syslog.h>
#include <getopt.h>

#include <libxml++/libxml++.h>

#include "lizards.h"

int main(int argc, char *argv[])
{
  int ch;
  int load_tmpl = 1;
  int svg_player = 0;
  while ((ch = getopt(argc, argv, "g:")) != EOF)
    switch(ch)
      {
      case 'g':
	load_tmpl = 0;
	svg_player = strtol(optarg, 0, 10);
	break;
      }

  if (load_tmpl == 0 && ((argc - optind) > 0))
    {
      const char *file = argv[optind];
      misc_t::log(LOG_NOTICE, "Loading js file %s", file);
      serial_t s = serial_t::from_file(argv[optind]);
      grid_t *g = new grid_t(s);
      output_t *o = new output_t(g);
      o->svg((char*)"gg.svg", svg_player);
    }

  if (load_tmpl && (argc > 2))
    {
      misc_t::rseed(time(0));
      layouts_t* layouts = new layouts_t(FORMAT_FILE_JS, "LAYOUT.JS");

      template_t *t = new template_t(layouts, FORMAT_FILE_JS, argv[1]);
      grid_t *g = t->realize();
      output_t(g).svg(argv[2], 0);

      if (argc > 3)
	{
	  xmlpp::Document doc("1.0");
	  xmlpp::Element *ul = doc.create_root_node("ul", "", "");
	  xmlpp::Element *li0 = ul->add_child("li");
	  li0->add_child_text(Glib::ustring::compose("The template costs %1 points", t->cost));

	  for (int x = 0; x < g->width; x++)
	    for (int y = 0; y < g->height; y++)
	      {
		point_t p(x, y);
		hex_t *hex = (*g)(p);
		if (hex && hex->get(BAND))
		  {
		    band_t *band = dynamic_cast<band_t*>(hex->get(BAND));
		    const char *colors[] = { "Red", "Green", "Grey", "Black", "Yellow" };
		    int last = 0;
		    Glib::ustring s = Glib::ustring::compose("Hex %1 has ", hex->getid());
		    for (int i = 0; i < UNSETTLED; i++)
		      {
			if(band->get(i))
			  {
			    last = i;
			    printf("%d %d\n", i, band->get(i));
			  }
		      }
		    printf("Count = %d\n", last);
		    int count = 0;
		    for (int i = 0; i < UNSETTLED; i++)
		      {
			if (band->get(i))
			  {
			    if ((count > 0) && (i != last))
			      s += ", ";
			    else if (count > 0 && i == last)
			      s += " and ";
			    s += Glib::ustring::compose("%1 %2 lizards", band->get(i), colors[i]);
			    count++;
			  }
		      }
		    xmlpp::Element *li0 = ul->add_child("li");
		    li0->add_child_text(s);
		  }
	      }
	  doc.write_to_file_formatted(argv[3]);
	}
    }
    exit(0);
}
