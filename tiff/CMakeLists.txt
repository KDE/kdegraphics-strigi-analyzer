kde4_header()

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}  )


########### next target ###############

set(kfile_tiff_PART_SRCS kfile_tiff.cpp )

kde4_automoc(${kfile_tiff_PART_SRCS})

kde4_add_plugin(kfile_tiff ${kfile_tiff_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kfile_tiff )

target_link_libraries(kfile_tiff  ${KDE4_KIO_LIBS} ${TIFF_LIBRARIES} )

install_targets(${LIB_INSTALL_DIR}/kde4 kfile_tiff )


########### install files ###############

install_files( ${SERVICES_INSTALL_DIR} FILES kfile_tiff.desktop )

kde4_footer()



#original Makefile.am contents follow:

### Makefile.am for Tiff file meta info plugin
#
## set the include path for X, qt and KDE
#INCLUDES         = $(all_includes)
#KDE_CXXFLAGS = -UQT3_SUPPORT
#noinst_HEADERS   = kfile_tiff.h
#
#kde_module_LTLIBRARIES = kfile_tiff.la
#
#kfile_tiff_la_SOURCES = kfile_tiff.cpp
#kfile_tiff_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
#kfile_tiff_la_LIBADD = $(LIB_KIO) $(LIBTIFF)
#
## let automoc handle all of the meta source files (moc)
#METASOURCES = AUTO
#
#messages:
#	$(XGETTEXT) *.cpp -o $(podir)/kfile_tiff.pot
#
#services_DATA = kfile_tiff.desktop
#servicesdir = $(kde_servicesdir)
