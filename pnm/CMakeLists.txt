kde4_header()




########### next target ###############

set(kfile_pnm_PART_SRCS kfile_pnm.cpp )

kde4_automoc(${kfile_pnm_PART_SRCS})

kde4_add_plugin(kfile_pnm ${kfile_pnm_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kfile_pnm )

target_link_libraries(kfile_pnm  ${KDE4_KIO_LIBS} )

install_targets(${PLUGIN_INSTALL_DIR} kfile_pnm )


########### install files ###############

install_files( ${SERVICES_INSTALL_DIR} FILES kfile_pnm.desktop )

kde4_footer()



#original Makefile.am contents follow:

### Makefile.am for pnm file meta info plugin
#
## set the include path for X, qt and KDE
#INCLUDES         = $(all_includes)
#KDE_CXXFLAGS = -UQT3_SUPPORT
## these are the headers for your project
#noinst_HEADERS   = kfile_pnm.h
#
#kde_module_LTLIBRARIES = kfile_pnm.la
#
#kfile_pnm_la_SOURCES = kfile_pnm.cpp
#kfile_pnm_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
#kfile_pnm_la_LIBADD = $(LIB_KIO)
#
## let automoc handle all of the meta source files (moc)
#METASOURCES = AUTO
#
#messages:
#	$(XGETTEXT) *.cpp -o $(podir)/kfile_pnm.pot
#
#services_DATA = kfile_pnm.desktop
#servicesdir = $(kde_servicesdir)
