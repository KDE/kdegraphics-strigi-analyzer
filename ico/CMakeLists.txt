kde4_header()

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}  )


########### next target ###############

set(kfile_ico_PART_SRCS kfile_ico.cpp )

kde4_automoc(${kfile_ico_PART_SRCS})

kde4_add_plugin(kfile_ico ${kfile_ico_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kfile_ico )

target_link_libraries(kfile_ico  ${KDE4_KIO_LIBS} )

install_targets(${LIB_INSTALL_DIR}/kde4 kfile_ico )


########### install files ###############

install_files( ${SERVICES_INSTALL_DIR} FILES kfile_ico.desktop )

kde4_footer()



#original Makefile.am contents follow:

### Makefile.am for ico file meta info plugin
#
## set the include path for X, qt and KDE
#INCLUDES         = $(all_includes)
#KDE_CXXFLAGS = -UQT3_SUPPORT
#
## these are the headers for your project
#noinst_HEADERS   = kfile_ico.h
#
#kde_module_LTLIBRARIES = kfile_ico.la
#
#kfile_ico_la_SOURCES = kfile_ico.cpp 
#kfile_ico_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
#kfile_ico_la_LIBADD = $(LIB_KSYCOCA)
#
## let automoc handle all of the meta source files (moc)
#METASOURCES = AUTO
#
#messages: rc.cpp
#	$(XGETTEXT) kfile_ico.cpp -o $(podir)/kfile_ico.pot
#
#services_DATA = kfile_ico.desktop
#servicesdir = $(kde_servicesdir)
