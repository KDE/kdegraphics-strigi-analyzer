## Makefile.am for dds file meta info plugin

# set the include path for X, qt and KDE
INCLUDES         = $(all_includes)

# these are the headers for your project
noinst_HEADERS   = kfile_dds.h

kde_module_LTLIBRARIES = kfile_dds.la

kfile_dds_la_SOURCES = kfile_dds.cpp 
kfile_dds_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
kfile_dds_la_LIBADD = $(LIB_KSYCOCA)

# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO

messages: rc.cpp
	$(XGETTEXT) kfile_dds.cpp -o $(podir)/kfile_dds.pot

services_DATA = kfile_dds.desktop
servicesdir = $(kde_servicesdir)
