## Makefile.am for bmp file meta info plugin

# set the include path for X, qt and KDE
INCLUDES         = $(all_includes)

# these are the headers for your project
noinst_HEADERS   = kfile_bmp.h

kde_module_LTLIBRARIES = kfile_bmp.la

kfile_bmp_la_SOURCES = kfile_bmp.cpp 
kfile_bmp_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
kfile_bmp_la_LIBADD = $(LIB_KSYCOCA)

# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO

messages: rc.cpp
	$(XGETTEXT) kfile_bmp.cpp -o $(podir)/kfile_bmp.pot

services_DATA = kfile_bmp.desktop
servicesdir = $(kde_servicesdir)
