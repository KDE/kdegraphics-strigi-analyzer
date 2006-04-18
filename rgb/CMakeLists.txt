kde4_header()

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}  )


########### next target ###############

set(kfile_rgb_PART_SRCS kfile_rgb.cpp )

kde4_automoc(${kfile_rgb_PART_SRCS})

kde4_add_plugin(kfile_rgb ${kfile_rgb_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kfile_rgb )

target_link_libraries(kfile_rgb  ${KDE4_KIO_LIBS} )

install_targets(${PLUGIN_INSTALL_DIR} kfile_rgb )


########### install files ###############

install_files( ${SERVICES_INSTALL_DIR} FILES kfile_rgb.desktop )

kde4_footer()



#original Makefile.am contents follow:

### Makefile.am for rgb file meta info plugin
#
## set the include path for X, qt and KDE
#INCLUDES         = $(all_includes)
#
## these are the headers for your project
#noinst_HEADERS   = kfile_rgb.h
#
#kde_module_LTLIBRARIES = kfile_rgb.la
#
#kfile_rgb_la_SOURCES = kfile_rgb.cpp
#kfile_rgb_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
#kfile_rgb_la_LIBADD = $(LIB_KSYCOCA)
#
## let automoc handle all of the meta source files (moc)
#METASOURCES = AUTO
#
#messages: rc.cpp
#	$(XGETTEXT) kfile_rgb.cpp -o $(podir)/kfile_rgb.pot
#
#services_DATA = kfile_rgb.desktop
#servicesdir = $(kde_servicesdir)
