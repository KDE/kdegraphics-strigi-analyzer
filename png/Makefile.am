## Makefile.am for png file meta info plugin

# set the include path for X, qt and KDE
INCLUDES         = $(all_includes)

# these are the headers for your project
noinst_HEADERS   = kfile_png.h 

kde_module_LTLIBRARIES = kfile_png.la

kfile_png_la_SOURCES = kfile_png.cpp
kfile_png_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
kfile_png_la_LIBADD = $(LIB_KIO)

# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO

messages:
	$(XGETTEXT) *.cpp -o $(podir)/kfile_png.pot

services_DATA = kfile_png.desktop
servicesdir = $(kde_servicesdir)
