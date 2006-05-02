kde4_header()

include_directories(${POPPLER_QT4_INCLUDE_DIR} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}  )


########### next target ###############

set(kfile_pdf_PART_SRCS kfile_pdf.cpp )

kde4_automoc(${kfile_pdf_PART_SRCS})

kde4_add_plugin(kfile_pdf ${kfile_pdf_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kfile_pdf )

target_link_libraries(kfile_pdf  ${KDE4_KIO_LIBS} ${POPPLER_QT4_LIBRARIES})

install_targets(${PLUGIN_INSTALL_DIR} kfile_pdf )


########### install files ###############

install_files( ${SERVICES_INSTALL_DIR} FILES kfile_pdf.desktop )

kde4_footer()



#original Makefile.am contents follow:

### Makefile.am for the pdf file meta info plugin
#
## set the include path for X, qt and KDE
#INCLUDES         = $(all_includes) $(POPPLER_QT4_4_CFLAGS)
#
## these are the headers for your project
#noinst_HEADERS   = kfile_pdf.h
#
#kde_module_LTLIBRARIES = kfile_pdf.la
#
#kfile_pdf_la_SOURCES = kfile_pdf.cpp
#kfile_pdf_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
#kfile_pdf_la_LIBADD = $(LIB_KIO) $(POPPLER_QT4_4_LIBS)
#
## let automoc handle all of the meta source files (moc)
#METASOURCES = AUTO
#
#messages: 
#	$(XGETTEXT) *.cpp -o $(podir)/kfile_pdf.pot
#
#services_DATA = kfile_pdf.desktop
#servicesdir = $(kde_servicesdir)
