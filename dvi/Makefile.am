## Makefile.am for the dvi file meta info plugin

# set the include path for X, qt and KDE
INCLUDES         = $(all_includes)

# these are the headers for your project
noinst_HEADERS   = kfile_dvi.h

kde_module_LTLIBRARIES = kfile_dvi.la

kfile_dvi_la_SOURCES = kfile_dvi.cpp
kfile_dvi_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
kfile_dvi_la_LIBADD = $(LIB_KIO)

# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO

messages: 
	$(XGETTEXT) *.cpp -o $(podir)/kfile_dvi.pot

services_DATA = kfile_dvi.desktop
servicesdir = $(kde_servicesdir)
