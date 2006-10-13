########### next target ###############

set(kfile_xps_PART_SRCS kfile_xps.cpp )

kde4_automoc(kfile_xps ${kfile_xps_PART_SRCS})

kde4_add_plugin(kfile_xps ${kfile_xps_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kfile_xps )

target_link_libraries(kfile_xps  ${KDE4_KIO_LIBS})

install(TARGETS kfile_xps  DESTINATION ${PLUGIN_INSTALL_DIR} )


########### install files ###############

install( FILES kfile_xps.desktop  DESTINATION  ${SERVICES_INSTALL_DIR} )
