## Makefile.am for gif file meta info plugin

KDE_CXXFLAGS = $(USE_EXCEPTIONS)

# set the include path for X, qt and KDE
INCLUDES         = $(all_includes)

# these are the headers for your project
noinst_HEADERS   = kfile_gif.h

kde_module_LTLIBRARIES = kfile_gif.la

kfile_gif_la_SOURCES = kfile_gif.cpp
kfile_gif_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
kfile_gif_la_LIBADD = $(LIB_KIO)

# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO

messages:
	$(XGETTEXT) *.cpp -o $(podir)/kfile_gif.pot

services_DATA = kfile_gif.desktop
servicesdir = $(kde_servicesdir)
