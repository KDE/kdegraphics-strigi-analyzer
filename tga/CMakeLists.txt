



########### next target ###############

set(kfile_tga_PART_SRCS kfile_tga.cpp )

kde4_automoc(kfile_tga ${kfile_tga_PART_SRCS})

kde4_add_plugin(kfile_tga ${kfile_tga_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kfile_tga )

target_link_libraries(kfile_tga  ${KDE4_KIO_LIBS} )

install(TARGETS kfile_tga  DESTINATION ${PLUGIN_INSTALL_DIR} )


########### install files ###############

install( FILES kfile_tga.desktop  DESTINATION  ${SERVICES_INSTALL_DIR} )

