



########### next target ###############

set(kfile_xbm_PART_SRCS kfile_xbm.cpp )

kde4_automoc(${kfile_xbm_PART_SRCS})

kde4_add_plugin(kfile_xbm ${kfile_xbm_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kfile_xbm )

target_link_libraries(kfile_xbm  ${KDE4_KIO_LIBS} )

install_targets(${PLUGIN_INSTALL_DIR} kfile_xbm )


########### install files ###############

install_files( ${SERVICES_INSTALL_DIR} FILES kfile_xbm.desktop )




#original Makefile.am contents follow:

### Makefile.am for xbm file meta info plugin
#
## set the include path for X, qt and KDE
#INCLUDES         = $(all_includes)
#KDE_CXXFLAGS = -UQT3_SUPPORT
## these are the headers for your project
#noinst_HEADERS   = kfile_xbm.h
#
#kde_module_LTLIBRARIES = kfile_xbm.la
#
#kfile_xbm_la_SOURCES = kfile_xbm.cpp 
#kfile_xbm_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
#kfile_xbm_la_LIBADD = $(LIB_KSYCOCA)
#
## let automoc handle all of the meta source files (moc)
#METASOURCES = AUTO
#
#messages: rc.cpp
#	$(XGETTEXT) kfile_xbm.cpp -o $(podir)/kfile_xbm.pot
#
#services_DATA = kfile_xbm.desktop
#servicesdir = $(kde_servicesdir)
