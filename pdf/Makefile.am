## Makefile.am for the pdf file meta info plugin

# set the include path for X, qt and KDE
INCLUDES         = $(all_includes) $(POPPLER_CFLAGS)

# these are the headers for your project
noinst_HEADERS   = kfile_pdf.h

kde_module_LTLIBRARIES = kfile_pdf.la

kfile_pdf_la_SOURCES = kfile_pdf.cpp
kfile_pdf_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
kfile_pdf_la_LIBADD = $(LIB_KIO) $(POPPLER_LIBS)

# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO

messages: 
	$(XGETTEXT) *.cpp -o $(podir)/kfile_pdf.pot

services_DATA = kfile_pdf.desktop
servicesdir = $(kde_servicesdir)
