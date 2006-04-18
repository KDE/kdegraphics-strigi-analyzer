kde4_header()

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}  )


########### next target ###############

set(kfile_png_PART_SRCS kfile_png.cpp )

kde4_automoc(${kfile_png_PART_SRCS})

kde4_add_plugin(kfile_png ${kfile_png_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kfile_png )

target_link_libraries(kfile_png  ${KDE4_KIO_LIBS} ${PNG_LIBRARIES})

install_targets(${PLUGIN_INSTALL_DIR} kfile_png )


########### install files ###############

install_files( ${SERVICES_INSTALL_DIR} FILES kfile_png.desktop )

kde4_footer()



#original Makefile.am contents follow:

### Makefile.am for png file meta info plugin
#
## set the include path for X, qt and KDE
#INCLUDES         = $(all_includes)
#
## these are the headers for your project
#noinst_HEADERS   = kfile_png.h 
#
#kde_module_LTLIBRARIES = kfile_png.la
#
#kfile_png_la_SOURCES = kfile_png.cpp
#kfile_png_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
#kfile_png_la_LIBADD = $(LIB_KIO)
#
## let automoc handle all of the meta source files (moc)
#METASOURCES = AUTO
#
#messages:
#	$(XGETTEXT) *.cpp -o $(podir)/kfile_png.pot
#
#services_DATA = kfile_png.desktop
#servicesdir = $(kde_servicesdir)
