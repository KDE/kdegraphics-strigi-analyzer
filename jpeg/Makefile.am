## Makefile.am for jpeg file meta info plugin

KDE_CXXFLAGS = $(USE_EXCEPTIONS)

# set the include path for X, qt and KDE
INCLUDES         = $(all_includes)

# these are the headers for your project
noinst_HEADERS   = kfile_jpeg.h exif.h

kde_module_LTLIBRARIES = kfile_jpeg.la

kfile_jpeg_la_SOURCES = kfile_jpeg.cpp exif.cpp kfile_setcomment.cpp
kfile_jpeg_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
kfile_jpeg_la_LIBADD = $(LIB_KIO)

# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO

messages:
	$(XGETTEXT) *.cpp -o $(podir)/kfile_jpeg.pot

services_DATA = kfile_jpeg.desktop
servicesdir = $(kde_servicesdir)
