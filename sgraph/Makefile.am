EXTRA_DIST = \
	autogen.sh 

bin_PROGRAMS = sgraph rtdemo

COMMON_SRCS = data.hh data.cpp draw.hh draw.cpp options.hh options.cpp defaults.hh view.hh view.cpp randfuncs.cpp randfuncs.hh sdldraw.hh sdldraw.cpp

sgraph_SOURCES = $(COMMON_SRCS) sgraph.cpp
rtdemo_SOURCES = $(COMMON_SRCS) rtdemo.cpp demodata.cpp demodata.hh





install-data-local: 
	$(INSTALL) -d $(DESTDIR)/usr/share/sgraph
	$(INSTALL) $(srcdir)/cmss12.ttf $(DESTDIR)/usr/share/sgraph/cmss12.ttf

doxygen:
	doxygen dox.conf 

man_MANS = sgraph.1