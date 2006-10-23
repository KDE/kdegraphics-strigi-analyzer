


set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  ${KDE4_ENABLE_EXCEPTIONS}")
########### next target ###############

set(kfile_jpeg_PART_SRCS kfile_jpeg.cpp exif.cpp kfile_setcomment.cpp )

kde4_automoc(${kfile_jpeg_PART_SRCS})

kde4_add_plugin(kfile_jpeg ${kfile_jpeg_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kfile_jpeg )

target_link_libraries(kfile_jpeg  ${KDE4_KIO_LIBS} )

install(TARGETS kfile_jpeg  DESTINATION ${PLUGIN_INSTALL_DIR} )


########### install files ###############

install( FILES kfile_jpeg.desktop  DESTINATION  ${SERVICES_INSTALL_DIR} )



