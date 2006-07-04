



########### next target ###############

set(kfile_pcx_PART_SRCS kfile_pcx.cpp )

kde4_automoc(${kfile_pcx_PART_SRCS})

kde4_add_plugin(kfile_pcx ${kfile_pcx_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kfile_pcx )

target_link_libraries(kfile_pcx  ${KDE4_KIO_LIBS} )

install(TARGETS kfile_pcx  DESTINATION ${PLUGIN_INSTALL_DIR} )


########### install files ###############

install( FILES kfile_pcx.desktop  DESTINATION  ${SERVICES_INSTALL_DIR} )




#original Makefile.am contents follow:

### Makefile.am for PCX file meta info plugin
#
## set the include path for X, qt and KDE
#INCLUDES         = $(all_includes)
#KDE_CXXFLAGS = -UQT3_SUPPORT
#
#noinst_HEADERS   = kfile_pcx.h
#
#kde_module_LTLIBRARIES = kfile_pcx.la
#
#kfile_pcx_la_SOURCES = kfile_pcx.cpp
#kfile_pcx_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
#kfile_pcx_la_LIBADD = $(LIB_KIO) $(LIBTIFF)
#
## let automoc handle all of the meta source files (moc)
#METASOURCES = AUTO
#
#messages:
#	$(XGETTEXT) *.cpp -o $(podir)/kfile_pcx.pot
#
#services_DATA = kfile_pcx.desktop
#servicesdir = $(kde_servicesdir)
