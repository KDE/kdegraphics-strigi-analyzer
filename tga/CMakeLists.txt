kde4_header()

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}  )


########### next target ###############

set(kfile_tga_PART_SRCS kfile_tga.cpp )

kde4_automoc(${kfile_tga_PART_SRCS})

kde4_add_plugin(kfile_tga ${kfile_tga_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kfile_tga )

target_link_libraries(kfile_tga  ${KDE4_KIO_LIBS} )

install_targets(${LIB_INSTALL_DIR}/kde4 kfile_tga )


########### install files ###############

install_files( ${SERVICES_INSTALL_DIR} FILES kfile_tga.desktop )

kde4_footer()



#original Makefile.am contents follow:

### Makefile.am for tga file meta info plugin
#
## set the include path for X, qt and KDE
#INCLUDES         = $(all_includes)
#KDE_CXXFLAGS = -UQT3_SUPPORT
## these are the headers for your project
#noinst_HEADERS   = kfile_tga.h
#
#kde_module_LTLIBRARIES = kfile_tga.la
#
#kfile_tga_la_SOURCES = kfile_tga.cpp 
#kfile_tga_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
#kfile_tga_la_LIBADD = $(LIB_KSYCOCA)
#
## let automoc handle all of the meta source files (moc)
#METASOURCES = AUTO
#
#messages: rc.cpp
#	$(XGETTEXT) kfile_tga.cpp -o $(podir)/kfile_tga.pot
#
#services_DATA = kfile_tga.desktop
#servicesdir = $(kde_servicesdir)
