## Makefile.am for xpm file meta info plugin

INCLUDES         = $(all_includes)

# these are the headers for your project
noinst_HEADERS   = kfile_xpm.h

kde_module_LTLIBRARIES = kfile_xpm.la

kfile_xpm_la_SOURCES = kfile_xpm.cpp
kfile_xpm_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
kfile_xpm_la_LIBADD = $(LIB_KIO)
kfile_xpm_la_METASOURCES = kfile_xpm.moc

# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO

kde_services_DATA = kfile_xpm.desktop

messages:
	$(XGETTEXT) *.cpp -o $(podir)/kfile_xpm.pot
