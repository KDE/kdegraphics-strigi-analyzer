



########### next target ###############

set(kfile_rgb_PART_SRCS kfile_rgb.cpp )

kde4_automoc(kfile_rgb ${kfile_rgb_PART_SRCS})

kde4_add_plugin(kfile_rgb ${kfile_rgb_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kfile_rgb )

target_link_libraries(kfile_rgb  ${KDE4_KIO_LIBS} )

install(TARGETS kfile_rgb  DESTINATION ${PLUGIN_INSTALL_DIR} )


########### install files ###############

install( FILES kfile_rgb.desktop  DESTINATION  ${SERVICES_INSTALL_DIR} )


