



########### next target ###############

set(kfile_pcx_PART_SRCS kfile_pcx.cpp )

kde4_automoc(kfile_pcx ${kfile_pcx_PART_SRCS})

kde4_add_plugin(kfile_pcx ${kfile_pcx_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kfile_pcx )

target_link_libraries(kfile_pcx  ${KDE4_KIO_LIBS} )

install(TARGETS kfile_pcx  DESTINATION ${PLUGIN_INSTALL_DIR} )


########### install files ###############

install( FILES kfile_pcx.desktop  DESTINATION  ${SERVICES_INSTALL_DIR} )
