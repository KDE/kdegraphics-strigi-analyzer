## Makefile.am for the ps file meta info plugin

# set the include path for X, qt and KDE
INCLUDES         = -I$(top_srcdir)/kghostview $(all_includes)

# these are the headers for your project
noinst_HEADERS   = kfile_ps.h gscreator.h

kde_module_LTLIBRARIES = kfile_ps.la gsthumbnail.la

kfile_ps_la_SOURCES = kfile_ps.cpp
kfile_ps_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
kfile_ps_la_LIBADD = $(LIB_KIO) ../../kghostview/libdscparse.la

gsthumbnail_la_SOURCES = gscreator.cpp
gsthumbnail_la_LIBADD = $(LIB_KDECORE) ../../kghostview/libdscparse.la
gsthumbnail_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)

# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO

messages: 
	$(XGETTEXT) *.cpp -o $(podir)/kfile_ps.pot

services_DATA = kfile_ps.desktop gsthumbnail.desktop
servicesdir = $(kde_servicesdir)
