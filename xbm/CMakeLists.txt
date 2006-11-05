



########### next target ###############

set(kfile_xbm_PART_SRCS kfile_xbm.cpp )

kde4_automoc(${kfile_xbm_PART_SRCS})

kde4_add_plugin(kfile_xbm ${kfile_xbm_PART_SRCS})


target_link_libraries(kfile_xbm  ${KDE4_KIO_LIBS} )

install(TARGETS kfile_xbm  DESTINATION ${PLUGIN_INSTALL_DIR} )


########### install files ###############

install( FILES kfile_xbm.desktop  DESTINATION  ${SERVICES_INSTALL_DIR} )


