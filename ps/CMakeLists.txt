kde4_header()

include_directories( ${CMAKE_SOURCE_DIR}/kghostview ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}  )


########### next target ###############

set(kfile_ps_PART_SRCS kfile_ps.cpp ${libdscparse_SRCS})

kde4_automoc(${kfile_ps_PART_SRCS})

kde4_add_plugin(kfile_ps ${kfile_ps_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kfile_ps )

target_link_libraries(kfile_ps  ${KDE4_KIO_LIBS} )

install_targets(${PLUGIN_INSTALL_DIR} kfile_ps )


########### next target ###############

set(gsthumbnail_PART_SRCS gscreator.cpp ${libdscparse_SRCS})

kde4_automoc(${gsthumbnail_PART_SRCS})

kde4_add_plugin(gsthumbnail ${gsthumbnail_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} gsthumbnail )

target_link_libraries(gsthumbnail  ${KDE4_KDECORE_LIBS} )

install_targets(${PLUGIN_INSTALL_DIR} gsthumbnail )


########### install files ###############

install_files( ${SERVICES_INSTALL_DIR} FILES kfile_ps.desktop gsthumbnail.desktop )

kde4_footer()



#original Makefile.am contents follow:

### Makefile.am for the ps file meta info plugin
#
## set the include path for X, qt and KDE
#INCLUDES         = -I$(top_srcdir)/kghostview $(all_includes)
#
## these are the headers for your project
#noinst_HEADERS   = kfile_ps.h gscreator.h
#
#kde_module_LTLIBRARIES = kfile_ps.la gsthumbnail.la
#
#kfile_ps_la_SOURCES = kfile_ps.cpp
#kfile_ps_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
#kfile_ps_la_LIBADD = $(LIB_KIO) ../../kghostview/libdscparse.la
#
#gsthumbnail_la_SOURCES = gscreator.cpp
#gsthumbnail_la_LIBADD = $(LIB_KDECORE) ../../kghostview/libdscparse.la
#gsthumbnail_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
#
## let automoc handle all of the meta source files (moc)
#METASOURCES = AUTO
#
#messages: 
#	$(XGETTEXT) *.cpp -o $(podir)/kfile_ps.pot
#
#services_DATA = kfile_ps.desktop gsthumbnail.desktop
#servicesdir = $(kde_servicesdir)
