



########### next target ###############

set(kfile_bmp_PART_SRCS kfile_bmp.cpp )

kde4_automoc(${kfile_bmp_PART_SRCS})

kde4_add_plugin(kfile_bmp ${kfile_bmp_PART_SRCS})


target_link_libraries(kfile_bmp  ${KDE4_KIO_LIBS} )

install(TARGETS kfile_bmp  DESTINATION ${PLUGIN_INSTALL_DIR} )


########### install files ###############

install( FILES kfile_bmp.desktop  DESTINATION  ${SERVICES_INSTALL_DIR} )
