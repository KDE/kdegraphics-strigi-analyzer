kde4_header()

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}  )


########### next target ###############

set(kfile_dds_PART_SRCS kfile_dds.cpp )

kde4_automoc(${kfile_dds_PART_SRCS})

kde4_add_plugin(kfile_dds ${kfile_dds_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kfile_dds )

target_link_libraries(kfile_dds  ${KDE4_KDECORE_LIBS} kio )

install_targets(${LIB_INSTALL_DIR}/kde4 kfile_dds )


########### install files ###############

install_files( ${SERVICES_INSTALL_DIR} FILES kfile_dds.desktop )

kde4_footer()



#original Makefile.am contents follow:

### Makefile.am for dds file meta info plugin
#
## set the include path for X, qt and KDE
#INCLUDES         = $(all_includes)
#KDE_CXXFLAGS = -UQT3_SUPPORT
## these are the headers for your project
#noinst_HEADERS   = kfile_dds.h
#
#kde_module_LTLIBRARIES = kfile_dds.la
#
#kfile_dds_la_SOURCES = kfile_dds.cpp 
#kfile_dds_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
#kfile_dds_la_LIBADD = $(LIB_KSYCOCA)
#
## let automoc handle all of the meta source files (moc)
#METASOURCES = AUTO
#
#messages: rc.cpp
#	$(XGETTEXT) kfile_dds.cpp -o $(podir)/kfile_dds.pot
#
#services_DATA = kfile_dds.desktop
#servicesdir = $(kde_servicesdir)
