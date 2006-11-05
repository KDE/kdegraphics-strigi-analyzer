



########### next target ###############

set(kfile_ico_PART_SRCS kfile_ico.cpp )

kde4_automoc(${kfile_ico_PART_SRCS})

kde4_add_plugin(kfile_ico ${kfile_ico_PART_SRCS})


target_link_libraries(kfile_ico  ${KDE4_KIO_LIBS} )

install(TARGETS kfile_ico  DESTINATION ${PLUGIN_INSTALL_DIR} )


########### install files ###############

install( FILES kfile_ico.desktop  DESTINATION  ${SERVICES_INSTALL_DIR} )


