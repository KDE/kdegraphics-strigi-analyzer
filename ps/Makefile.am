## Makefile.am for the ps file meta info plugin

# set the include path for X, qt and KDE
INCLUDES         = $(all_includes)

# these are the headers for your project
noinst_HEADERS   = kfile_ps.h kmaybe.h dscparse.h dscparse_adapter.h

kde_module_LTLIBRARIES = kfile_ps.la

kfile_ps_la_SOURCES = dscparse.cpp kfile_ps.cpp dscparse_adapter.cpp
kfile_ps_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
kfile_ps_la_LIBADD = $(LIB_KIO)

# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO

messages: 
	$(XGETTEXT) *.cpp -o $(podir)/kfile_ps.pot

services_DATA = kfile_ps.desktop
servicesdir = $(kde_servicesdir)
