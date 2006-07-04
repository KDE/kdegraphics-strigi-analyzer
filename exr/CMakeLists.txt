
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${KDE4_ENABLE_EXCEPTIONS}")
include_directories( ${OPENEXR_INCLUDE_DIR}  )


########### next target ###############

set(kfile_exr_PART_SRCS kfile_exr.cpp )

kde4_automoc(${kfile_exr_PART_SRCS})

kde4_add_plugin(kfile_exr ${kfile_exr_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kfile_exr )

target_link_libraries(kfile_exr  ${KDE4_KIO_LIBS} ${OPENEXR_LIBRARIES} )

install(TARGETS kfile_exr  DESTINATION ${PLUGIN_INSTALL_DIR} )


########### install files ###############

install( FILES kfile_exr.desktop  DESTINATION  ${SERVICES_INSTALL_DIR} )




#original Makefile.am contents follow:

### Makefile.am for EXR file meta info plugin
#
#KDE_CXXFLAGS = $(USE_EXCEPTIONS) 
#
## set the include path for X, qt and KDE
#INCLUDES         = -Drestrict= $(all_includes) $(EXR_FLAGS) 
## INCLUDES         = $(all_includes) $(EXR_FLAGS) 
#
## these are the headers for your project
#noinst_HEADERS   = kfile_exr.h 
#
#kde_module_LTLIBRARIES = kfile_exr.la
#
#kfile_exr_la_SOURCES = kfile_exr.cpp
#kfile_exr_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
#kfile_exr_la_LIBADD = $(LIB_KIO) $(LIB_EXR)
#
## let automoc handle all of the meta source files (moc)
#METASOURCES = AUTO
#
#messages:
#	$(XGETTEXT) *.cpp -o $(podir)/kfile_exr.pot
#
#services_DATA = kfile_exr.desktop
#servicesdir = $(kde_servicesdir)
