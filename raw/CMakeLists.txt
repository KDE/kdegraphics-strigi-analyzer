kde4_header()

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}  )


########### next target ###############

set(kfile_raw_PART_SRCS kcamerarawplugin.cpp parse.c )

kde4_automoc(${kfile_raw_PART_SRCS})

kde4_add_plugin(kfile_raw ${kfile_raw_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kfile_raw )

target_link_libraries(kfile_raw  ${LIB_KIO} )

install_targets(${LIB_INSTALL_DIR}/kde4 kfile_raw )


########### install files ###############

install_files( ${SERVICES_INSTALL_DIR} FILES kfile_raw.desktop )

kde4_footer()



#original Makefile.am contents follow:

### Makefile.am for the raw thumbnail extration plugin
#
## set the include path for X, qt and KDE
#INCLUDES         = $(all_includes)
#KDE_CXXFLAGS = -UQT3_SUPPORT
## these are the headers for your project
#noinst_HEADERS = kcamerarawplugin.h
#
#kde_module_LTLIBRARIES = kfile_raw.la
#
#kfile_raw_la_SOURCES = kcamerarawplugin.cpp parse.c
#kfile_raw_la_LIBADD = $(LIB_KIO) 
#kfile_raw_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
#
## let automoc handle all of the meta source files (moc)
#METASOURCES = AUTO
#
#services_DATA = kfile_raw.desktop
#servicesdir = $(kde_servicesdir)
