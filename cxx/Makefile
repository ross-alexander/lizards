CC	= gcc

# GLUT_CFLAGS	= -I/opt/openglut/include
# GLUT_LIBS	= -L/opt/openglut/lib -lglut

# GTS_CFLAGS	= @GTS_CFLAGS@
# GTS_LIBS	= @GTS_LIBS@

# GTKGL_CFLAGS	= @GTKGLEXT_CFLAGS@
# GTKGL_LIBS	= @GTKGLEXT_LIBS@

GTK_CFLAGS	= -pthread -I/usr/include/gtk-2.0 -I/usr/lib64/gtk-2.0/include -I/usr/include/atk-1.0 -I/usr/include/cairo -I/usr/include/gdk-pixbuf-2.0 -I/usr/include/pango-1.0 -I/usr/include/glib-2.0 -I/usr/lib64/glib-2.0/include -I/usr/include/harfbuzz -I/usr/include/freetype2 -I/usr/include/libpng16 -I/usr/include/pixman-1  
GTK_LIBS	= -L/usr/lib64 -lgtk-x11-2.0 -lgdk-x11-2.0 -latk-1.0 -lgio-2.0 -lpangoft2-1.0 -lpangocairo-1.0 -lgdk_pixbuf-2.0 -lcairo -lpango-1.0 -lfontconfig -lgobject-2.0 -lglib-2.0 -lfreetype  

GLIB_CFLAGS	= -I/usr/include/glib-2.0 -I/usr/lib64/glib-2.0/include  
GLIB_LIBS	= -L/usr/lib64 -lglib-2.0  

XML_CFLAGS	= -I/usr/include/libxml++-3.0 -I/usr/lib64/libxml++-3.0/include -I/usr/include/libxml2 -I/usr/include/glibmm-2.4 -I/usr/lib64/glibmm-2.4/include -I/usr/include/sigc++-2.0 -I/usr/lib64/sigc++-2.0/include -I/usr/include/glib-2.0 -I/usr/lib64/glib-2.0/include  
XML_LIBS	= -L/usr/lib64 -lxml++-3.0 -lxml2 -lglibmm-2.4 -lgobject-2.0 -lsigc-2.0 -lglib-2.0  

RSVG_CFLAGS	= -pthread -I/usr/include/librsvg-2.0 -I/usr/include/glib-2.0 -I/usr/lib64/glib-2.0/include -I/usr/include/gdk-pixbuf-2.0 -I/usr/include/cairo -I/usr/include/libpng16 -I/usr/include/pixman-1 -I/usr/include/freetype2 -I/usr/include/harfbuzz  
RSVG_LIBS	= -L/usr/lib64 -lrsvg-2 -lm -lgio-2.0 -lgdk_pixbuf-2.0 -lcairo -lgobject-2.0 -lglib-2.0  

# GL_LIBS		= -lGL -lGLU -lglut
CAIRO_CFLAGS	= 
CAIRO_LIBS	= 


CAIROMM_CFLAGS	= -I/usr/include/cairomm-1.0 -I/usr/lib64/cairomm-1.0/include -I/usr/include/cairo -I/usr/include/sigc++-2.0 -I/usr/lib64/sigc++-2.0/include -I/usr/include/glib-2.0 -I/usr/lib64/glib-2.0/include -I/usr/include/pixman-1 -I/usr/include/freetype2 -I/usr/include/libpng16 -I/usr/include/harfbuzz  
CAIROMM_LIBS	= -L/usr/lib64 -lcairomm-1.0 -lcairo -lsigc-2.0  

CXXFLAGS = -g -Wall $(CAIRO_CFLAGS) $(RSVG_CFLAGS) $(GTK_CFLAGS) $(GLIB_CFLAGS) $(XML_CFLAGS)
LDLIBS	= $(RSVG_LIBS) $(GTK_LIBS) $(GLIB_LIBS) $(XML_LIBS) $(CAIRO_LIBS)

OBJECTS = debug.o misc.o base.o point.o feature.o hex.o map.o player.o world.o create.o hexlist.o \
output-cairo.o output-svg.o output-cairo.o output-ps.o output.o \
orders.o \
worldopen.o worldmast.o worldshut.o tordbord.o clanreport.o clanrank.o terrain.o terrmast.o bordshuffle.o turn.o lizards.o

all: lizards cairo

Test0001.svg: lizards defs.svg
	./lizards -osvg

lizards: $(OBJECTS)
	$(CXX) $(LDFLAGS) $^ $(LDLIBS) -o $@

lizards.o: lizards.cc lizards.h

feature.o: feature.cc lizards.h

hex.o: hex.cc lizards.h

point.o: point.cc lizards.h

map.o: map.cc lizards.h

create.o: create.cc lizards.h

psprint.o: psprint.cc lizards.h

world.o: world.cc lizards.h

clanreport.o: clanreport.cc lizards.h

clanrank.o: clanrank.cc lizards.h

# ----------------------------------------------------------------------
#
# cairo
#
# ----------------------------------------------------------------------

cairo:	point.o cairo.o
	$(CXX) -g -Wall $^ $(XML_LIBS) $(GLIB_LIBS) $(RSVG_LIBS) $(CAIROMM_LIBS) -o $@

cairo.o	: cairo.cc
	$(CXX) -g -Wall $(XML_CFLAGS) $(GLIB_CFLAGS) $(SVGCAIRO_CFLAGS) $(CAIROMM_CFLAGS) $(RSVG_CFLAGS) -c $^ -o $@

clean:
	rm -f lizards $(OBJECTS) *~
