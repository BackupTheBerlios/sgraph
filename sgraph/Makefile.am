EXTRA_DIST = \
	autogen.sh 

bin_PROGRAMS = sgraph

sgraph_SOURCES = sgraph.cpp data.hh data.cpp draw.hh draw.cpp options.hh options.cpp defaults.hh view.hh view.cpp randfuncs.cpp randfuncs.hh sdldraw.hh sdldraw.cpp

install-data-local: 
	$(INSTALL) -d $(DESTDIR)/usr/share/sgraph
	$(INSTALL) $(srcdir)/cmss12.ttf $(DESTDIR)/usr/share/sgraph/cmss12.ttf

doxygen:
	doxygen dox.conf 