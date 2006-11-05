



########### next target ###############

set(kfile_gif_PART_SRCS kfile_gif.cpp )

kde4_automoc(${kfile_gif_PART_SRCS})

kde4_add_plugin(kfile_gif ${kfile_gif_PART_SRCS})


target_link_libraries(kfile_gif  ${KDE4_KIO_LIBS} )

install(TARGETS kfile_gif  DESTINATION ${PLUGIN_INSTALL_DIR} )


########### install files ###############

install( FILES kfile_gif.desktop  DESTINATION  ${SERVICES_INSTALL_DIR} )


