## Makefile.am for xbm file meta info plugin

# set the include path for X, qt and KDE
INCLUDES         = $(all_includes)
KDE_CXXFLAGS = -UQT3_SUPPORT
# these are the headers for your project
noinst_HEADERS   = kfile_xbm.h

kde_module_LTLIBRARIES = kfile_xbm.la

kfile_xbm_la_SOURCES = kfile_xbm.cpp 
kfile_xbm_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
kfile_xbm_la_LIBADD = $(LIB_KSYCOCA)

# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO


services_DATA = kfile_xbm.desktop
servicesdir = $(kde_servicesdir)
