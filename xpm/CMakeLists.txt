



########### next target ###############

set(kfile_xpm_PART_SRCS kfile_xpm.cpp )

kde4_automoc(kfile_xpm ${kfile_xpm_PART_SRCS})

kde4_add_plugin(kfile_xpm ${kfile_xpm_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kfile_xpm )

target_link_libraries(kfile_xpm  ${KDE4_KIO_LIBS} )

install(TARGETS kfile_xpm  DESTINATION ${PLUGIN_INSTALL_DIR} )


########### install files ###############

install( FILES kfile_xpm.desktop  DESTINATION  ${SERVICES_INSTALL_DIR} )


