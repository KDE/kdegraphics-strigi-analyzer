## Makefile.am for tga file meta info plugin

# set the include path for X, qt and KDE
INCLUDES         = $(all_includes)

# these are the headers for your project
noinst_HEADERS   = kfile_tga.h

kde_module_LTLIBRARIES = kfile_tga.la

kfile_tga_la_SOURCES = kfile_tga.cpp 
kfile_tga_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
kfile_tga_la_LIBADD = $(LIB_KSYCOCA)

# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO

messages: rc.cpp
	$(XGETTEXT) kfile_tga.cpp -o $(podir)/kfile_tga.pot

services_DATA = kfile_tga.desktop
servicesdir = $(kde_servicesdir)
