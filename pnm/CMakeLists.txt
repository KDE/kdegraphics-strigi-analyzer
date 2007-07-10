include_directories(${STRIGI_INCLUDE_DIR})

set(kfile_pnm_PART_SRCS kfile_pnm.cpp )


kde4_add_plugin(kfile_pnm ${kfile_pnm_PART_SRCS})


target_link_libraries(kfile_pnm  ${KDE4_KIO_LIBS} )

install(TARGETS kfile_pnm  DESTINATION ${PLUGIN_INSTALL_DIR} )


########### install files ###############

install( FILES kfile_pnm.desktop  DESTINATION  ${SERVICES_INSTALL_DIR} )


