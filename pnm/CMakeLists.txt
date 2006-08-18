



########### next target ###############

set(kfile_pnm_PART_SRCS kfile_pnm.cpp )

kde4_automoc(${kfile_pnm_PART_SRCS})

kde4_add_plugin(kfile_pnm ${kfile_pnm_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kfile_pnm )

target_link_libraries(kfile_pnm  ${KDE4_KIO_LIBS} )

install(TARGETS kfile_pnm  DESTINATION ${PLUGIN_INSTALL_DIR} )


########### install files ###############

install( FILES kfile_pnm.desktop  DESTINATION  ${SERVICES_INSTALL_DIR} )


