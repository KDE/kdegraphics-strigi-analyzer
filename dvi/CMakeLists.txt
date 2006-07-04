



########### next target ###############

set(kfile_dvi_PART_SRCS kfile_dvi.cpp )

kde4_automoc(${kfile_dvi_PART_SRCS})

kde4_add_plugin(kfile_dvi ${kfile_dvi_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kfile_dvi )

target_link_libraries(kfile_dvi  ${KDE4_KIO_LIBS} )

install(TARGETS kfile_dvi  DESTINATION ${PLUGIN_INSTALL_DIR} )


########### install files ###############

install( FILES kfile_dvi.desktop  DESTINATION  ${SERVICES_INSTALL_DIR} )




#original Makefile.am contents follow:

### Makefile.am for the dvi file meta info plugin
#
## set the include path for X, qt and KDE
#INCLUDES         = $(all_includes)
#KDE_CXXFLAGS = -UQT3_SUPPORT
## these are the headers for your project
#noinst_HEADERS   = kfile_dvi.h
#
#kde_module_LTLIBRARIES = kfile_dvi.la
#
#kfile_dvi_la_SOURCES = kfile_dvi.cpp
#kfile_dvi_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
#kfile_dvi_la_LIBADD = $(LIB_KIO)
#
## let automoc handle all of the meta source files (moc)
#METASOURCES = AUTO
#
#messages: 
#	$(XGETTEXT) *.cpp -o $(podir)/kfile_dvi.pot
#
#services_DATA = kfile_dvi.desktop
#servicesdir = $(kde_servicesdir)
