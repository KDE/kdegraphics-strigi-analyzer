## Makefile.am for pnm file meta info plugin

# set the include path for X, qt and KDE
INCLUDES         = $(all_includes)

# these are the headers for your project
noinst_HEADERS   = kfile_pnm.h

kde_module_LTLIBRARIES = kfile_pnm.la

kfile_pnm_la_SOURCES = kfile_pnm.cpp
kfile_pnm_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
kfile_pnm_la_LIBADD = $(LIB_KIO)

# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO

messages:
	$(XGETTEXT) *.cpp -o $(podir)/kfile_pnm.pot

services_DATA = kfile_pnm.desktop
servicesdir = $(kde_servicesdir)
