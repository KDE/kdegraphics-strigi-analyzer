## Makefile.am for EXR file meta info plugin

KDE_CXXFLAGS = $(USE_EXCEPTIONS)

# set the include path for X, qt and KDE
INCLUDES         = -Drestrict= $(all_includes) $(EXR_FLAGS) 
# INCLUDES         = $(all_includes) $(EXR_FLAGS) 

# these are the headers for your project
noinst_HEADERS   = kfile_exr.h 

kde_module_LTLIBRARIES = kfile_exr.la

kfile_exr_la_SOURCES = kfile_exr.cpp
kfile_exr_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
kfile_exr_la_LIBADD = $(LIB_KIO) $(LIB_EXR)

# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO

messages:
	$(XGETTEXT) *.cpp -o $(podir)/kfile_exr.pot

services_DATA = kfile_exr.desktop
servicesdir = $(kde_servicesdir)
