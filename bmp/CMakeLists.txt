



########### next target ###############

set(kfile_bmp_PART_SRCS kfile_bmp.cpp )

kde4_automoc(${kfile_bmp_PART_SRCS})

kde4_add_plugin(kfile_bmp ${kfile_bmp_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kfile_bmp )

target_link_libraries(kfile_bmp  ${KDE4_KIO_LIBS} )

install(TARGETS kfile_bmp  DESTINATION ${PLUGIN_INSTALL_DIR} )


########### install files ###############

install( FILES kfile_bmp.desktop  DESTINATION  ${SERVICES_INSTALL_DIR} )




#original Makefile.am contents follow:

### Makefile.am for bmp file meta info plugin
#
## set the include path for X, qt and KDE
#INCLUDES         = $(all_includes)
#KDE_CXXFLAGS = -UQT3_SUPPORT
## these are the headers for your project
#noinst_HEADERS   = kfile_bmp.h
#
#kde_module_LTLIBRARIES = kfile_bmp.la
#
#kfile_bmp_la_SOURCES = kfile_bmp.cpp 
#kfile_bmp_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
#kfile_bmp_la_LIBADD = $(LIB_KSYCOCA)
#
## let automoc handle all of the meta source files (moc)
#METASOURCES = AUTO
#
#messages: rc.cpp
#	$(XGETTEXT) kfile_bmp.cpp -o $(podir)/kfile_bmp.pot
#
#services_DATA = kfile_bmp.desktop
#servicesdir = $(kde_servicesdir)
