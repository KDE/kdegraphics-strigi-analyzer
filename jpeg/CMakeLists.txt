kde4_header()

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}  )


########### next target ###############

set(kfile_jpeg_PART_SRCS kfile_jpeg.cpp exif.cpp kfile_setcomment.cpp )

kde4_automoc(${kfile_jpeg_PART_SRCS})

kde4_add_plugin(kfile_jpeg ${kfile_jpeg_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kfile_jpeg )

target_link_libraries(kfile_jpeg  ${QT_AND_KDECORE_LIBS} kio )

install_targets(${LIB_INSTALL_DIR}/kde4 kfile_jpeg )


########### install files ###############

install_files( ${SERVICES_INSTALL_DIR} FILES kfile_jpeg.desktop )

kde4_footer()



#original Makefile.am contents follow:

### Makefile.am for jpeg file meta info plugin
#
#KDE_CXXFLAGS = $(USE_EXCEPTIONS)
#
## set the include path for X, qt and KDE
#INCLUDES         = $(all_includes)
#
## these are the headers for your project
#noinst_HEADERS   = kfile_jpeg.h exif.h
#
#kde_module_LTLIBRARIES = kfile_jpeg.la
#
#kfile_jpeg_la_SOURCES = kfile_jpeg.cpp exif.cpp kfile_setcomment.cpp
#kfile_jpeg_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
#kfile_jpeg_la_LIBADD = $(LIB_KIO)
#
## let automoc handle all of the meta source files (moc)
#METASOURCES = AUTO
#
#messages:
#	$(XGETTEXT) *.cpp -o $(podir)/kfile_jpeg.pot
#
#services_DATA = kfile_jpeg.desktop
#servicesdir = $(kde_servicesdir)
