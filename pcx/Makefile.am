## Makefile.am for PCX file meta info plugin

# set the include path for X, qt and KDE
INCLUDES         = $(all_includes)

noinst_HEADERS   = kfile_pcx.h

kde_module_LTLIBRARIES = kfile_pcx.la

kfile_pcx_la_SOURCES = kfile_pcx.cpp
kfile_pcx_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
kfile_pcx_la_LIBADD = $(LIB_KIO) $(LIBTIFF)

# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO

messages:
	$(XGETTEXT) *.cpp -o $(podir)/kfile_pcx.pot

services_DATA = kfile_pcx.desktop
servicesdir = $(kde_servicesdir)
