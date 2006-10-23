



########### next target ###############

set(kfile_dvi_PART_SRCS kfile_dvi.cpp )

kde4_automoc(${kfile_dvi_PART_SRCS})

kde4_add_plugin(kfile_dvi ${kfile_dvi_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kfile_dvi )

target_link_libraries(kfile_dvi  ${KDE4_KIO_LIBS} )

install(TARGETS kfile_dvi  DESTINATION ${PLUGIN_INSTALL_DIR} )


########### install files ###############

install( FILES kfile_dvi.desktop  DESTINATION  ${SERVICES_INSTALL_DIR} )


