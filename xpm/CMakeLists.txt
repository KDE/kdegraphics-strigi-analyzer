kde4_header()




########### next target ###############

set(kfile_xpm_PART_SRCS kfile_xpm.cpp )

kde4_automoc(${kfile_xpm_PART_SRCS})

kde4_add_plugin(kfile_xpm ${kfile_xpm_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kfile_xpm )

target_link_libraries(kfile_xpm  ${KDE4_KIO_LIBS} )

install_targets(${PLUGIN_INSTALL_DIR} kfile_xpm )


########### install files ###############

install_files( ${SERVICES_INSTALL_DIR} FILES kfile_xpm.desktop )

kde4_footer()



#original Makefile.am contents follow:

### Makefile.am for xpm file meta info plugin
#
#INCLUDES         = $(all_includes)
#
#KDE_CXXFLAGS = -UQT3_SUPPORT
## these are the headers for your project
#noinst_HEADERS   = kfile_xpm.h
#
#kde_module_LTLIBRARIES = kfile_xpm.la
#
#kfile_xpm_la_SOURCES = kfile_xpm.cpp
#kfile_xpm_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
#kfile_xpm_la_LIBADD = $(LIB_KIO)
#kfile_xpm_la_METASOURCES = kfile_xpm.moc
#
## let automoc handle all of the meta source files (moc)
#METASOURCES = AUTO
#
#kde_services_DATA = kfile_xpm.desktop
#
#messages:
#	$(XGETTEXT) *.cpp -o $(podir)/kfile_xpm.pot
