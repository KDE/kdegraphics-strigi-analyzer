## Makefile.am for rgb file meta info plugin

# set the include path for X, qt and KDE
INCLUDES         = $(all_includes)

# these are the headers for your project
noinst_HEADERS   = kfile_rgb.h

kde_module_LTLIBRARIES = kfile_rgb.la

kfile_rgb_la_SOURCES = kfile_rgb.cpp
kfile_rgb_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
kfile_rgb_la_LIBADD = $(LIB_KSYCOCA)

# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO

messages: rc.cpp
	$(XGETTEXT) kfile_rgb.cpp -o $(podir)/kfile_rgb.pot

services_DATA = kfile_rgb.desktop
servicesdir = $(kde_servicesdir)
