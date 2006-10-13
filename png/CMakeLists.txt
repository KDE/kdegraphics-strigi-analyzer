



########### next target ###############

set(kfile_png_PART_SRCS kfile_png.cpp )

kde4_automoc(kfile_png ${kfile_png_PART_SRCS})

kde4_add_plugin(kfile_png ${kfile_png_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kfile_png )

target_link_libraries(kfile_png  ${KDE4_KIO_LIBS} ${PNG_LIBRARIES})

install(TARGETS kfile_png  DESTINATION ${PLUGIN_INSTALL_DIR} )


########### install files ###############

install( FILES kfile_png.desktop  DESTINATION  ${SERVICES_INSTALL_DIR} )

