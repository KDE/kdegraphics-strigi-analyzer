## Makefile.am for the raw thumbnail extration plugin

# set the include path for X, qt and KDE
INCLUDES         = $(all_includes)

# these are the headers for your project
noinst_HEADERS = kcamerarawplugin.h

kde_module_LTLIBRARIES = kfile_raw.la

kfile_raw_la_SOURCES = kcamerarawplugin.cpp parse.c
kfile_raw_la_LIBADD = $(LIB_KIO) 
kfile_raw_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)

# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO

services_DATA = kfile_raw.desktop
servicesdir = $(kde_servicesdir)
