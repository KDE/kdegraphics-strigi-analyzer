kde4_header()

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}  )


########### next target ###############

set(kfile_gif_PART_SRCS kfile_gif.cpp )

kde4_automoc(${kfile_gif_PART_SRCS})

kde4_add_plugin(kfile_gif ${kfile_gif_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kfile_gif )

target_link_libraries(kfile_gif  ${KDE4_KIO_LIBS} )

install_targets(${LIB_INSTALL_DIR}/kde4 kfile_gif )


########### install files ###############

install_files( ${SERVICES_INSTALL_DIR} FILES kfile_gif.desktop )

kde4_footer()



#original Makefile.am contents follow:

### Makefile.am for gif file meta info plugin
#
#KDE_CXXFLAGS = $(USE_EXCEPTIONS)
#
## set the include path for X, qt and KDE
#INCLUDES         = $(all_includes)
#
## these are the headers for your project
#noinst_HEADERS   = kfile_gif.h
#
#kde_module_LTLIBRARIES = kfile_gif.la
#
#kfile_gif_la_SOURCES = kfile_gif.cpp
#kfile_gif_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
#kfile_gif_la_LIBADD = $(LIB_KIO)
#
## let automoc handle all of the meta source files (moc)
#METASOURCES = AUTO
#
#messages:
#	$(XGETTEXT) *.cpp -o $(podir)/kfile_gif.pot
#
#services_DATA = kfile_gif.desktop
#servicesdir = $(kde_servicesdir)
