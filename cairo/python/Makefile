TOP		= ..

PYTHON_CFLAGS	= -I/usr/include/python2.7 -fno-strict-aliasing -g -O2 -DNDEBUG -g -fwrapv -O3 -Wall -Wstrict-prototypes

XML_CFLAGS	= -pthread -I/usr/include/libxml++-2.6 -I/usr/lib/libxml++-2.6/include -I/usr/include/libxml2 -I/usr/include/glibmm-2.4 -I/usr/lib/glibmm-2.4/include -I/usr/include/sigc++-2.0 -I/usr/lib/sigc++-2.0/include -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include  
XML_LIBS	= -pthread -lxml++-2.6 -lxml2 -lglibmm-2.4 -lgobject-2.0 -lsigc-2.0 -lgthread-2.0 -lrt -lglib-2.0  

RSVG_CFLAGS	= -pthread -I/usr/X11/include/pixman-1 -I/usr/X11/include -I/usr/include/librsvg-2.0 -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -I/usr/include/gdk-pixbuf-2.0 -I/usr/include/cairo -I/usr/include/libpng15 -I/usr/include/freetype2  
RSVG_LIBS	= -pthread -lrsvg-2 -lgio-2.0 -lgdk_pixbuf-2.0 -lm -lcairo -lgmodule-2.0 -lgobject-2.0 -lgthread-2.0 -lrt -lglib-2.0  

CAIRO_CFLAGS	= -pthread -I/usr/X11/include/pixman-1 -I/usr/X11/include -I/usr/include/cairo -I/usr/include/libxml2 -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -I/usr/include/freetype2 -I/usr/include/libpng15  
CAIRO_LIBS	= -lsvg-cairo -lsvg -lpng -ljpeg -lz -lcairo -lxml2  


CAIROMM_CFLAGS	= -pthread -I/usr/X11/include/pixman-1 -I/usr/X11/include -I/usr/include/cairomm-1.0 -I/usr/lib/cairomm-1.0/include -I/usr/include/cairo -I/usr/include/sigc++-2.0 -I/usr/lib/sigc++-2.0/include -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -I/usr/include/freetype2 -I/usr/include/libpng15  
CAIROMM_LIBS	= -lcairomm-1.0 -lcairo -lsigc-2.0  

JSON_LIBS	= -ljson

ARCH = $(TOP)/liblizards.a

CXXFLAGS = -g -Wall -I$(TOP) $(CAIRO_CFLAGS) $(RSVG_CFLAGS) $(GTK_CFLAGS) $(GLIB_CFLAGS) $(XML_CFLAGS) $(CAIROMM_CFLAGS) $(RSVG_CFLAGS)
LDLIBS	= $(RSVG_LIBS) $(GTK_LIBS) $(GLIB_LIBS) $(XML_LIBS) $(CAIRO_LIBS)

all	: _lizards.so

_lizards.so : python_lizards_wrap.o $(ARCH)
	$(CXX) -shared $^ $(XML_LIBS) $(GLIB_LIBS) $(RSVG_LIBS) $(CAIROMM_LIBS) -o $@

python_lizards_wrap.o : python_lizards_wrap.cxx
	$(CXX) $(PYTHON_CFLAGS) $(CXXFLAGS) -c $^ -o $@

python_lizards_wrap.cxx : $(TOP)/lizards.swg
	swig -c++ -python -o $@ $^

