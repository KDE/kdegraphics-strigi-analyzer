## Makefile.am for Tiff file meta info plugin

# set the include path for X, qt and KDE
INCLUDES         = $(all_includes)
KDE_CXXFLAGS = -UQT3_SUPPORT
noinst_HEADERS   = kfile_tiff.h

kde_module_LTLIBRARIES = kfile_tiff.la

kfile_tiff_la_SOURCES = kfile_tiff.cpp
kfile_tiff_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
kfile_tiff_la_LIBADD = $(LIB_KIO) $(LIBTIFF)

# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO


services_DATA = kfile_tiff.desktop
servicesdir = $(kde_servicesdir)
