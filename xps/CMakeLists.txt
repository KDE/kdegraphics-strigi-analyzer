include_directories(${STRIGI_INCLUDE_DIR})

set(kfile_xps_PART_SRCS kfile_xps.cpp )


kde4_add_plugin(kfile_xps ${kfile_xps_PART_SRCS})


target_link_libraries(kfile_xps  ${KDE4_KIO_LIBS})

install(TARGETS kfile_xps  DESTINATION ${PLUGIN_INSTALL_DIR} )


########### install files ###############

install( FILES kfile_xps.desktop  DESTINATION  ${SERVICES_INSTALL_DIR} )
