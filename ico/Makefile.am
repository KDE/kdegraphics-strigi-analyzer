## Makefile.am for ico file meta info plugin

# set the include path for X, qt and KDE
INCLUDES         = $(all_includes)

# these are the headers for your project
noinst_HEADERS   = kfile_ico.h

kde_module_LTLIBRARIES = kfile_ico.la

kfile_ico_la_SOURCES = kfile_ico.cpp 
kfile_ico_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
kfile_ico_la_LIBADD = $(LIB_KSYCOCA)

# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO

messages: rc.cpp
	$(XGETTEXT) kfile_ico.cpp -o $(podir)/kfile_ico.pot

services_DATA = kfile_ico.desktop
servicesdir = $(kde_servicesdir)
