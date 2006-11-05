



########### next target ###############

set(kfile_tiff_PART_SRCS kfile_tiff.cpp )

kde4_automoc(${kfile_tiff_PART_SRCS})

kde4_add_plugin(kfile_tiff ${kfile_tiff_PART_SRCS})


target_link_libraries(kfile_tiff  ${KDE4_KIO_LIBS} ${TIFF_LIBRARIES} )

install(TARGETS kfile_tiff  DESTINATION ${PLUGIN_INSTALL_DIR} )


########### install files ###############

install( FILES kfile_tiff.desktop  DESTINATION  ${SERVICES_INSTALL_DIR} )


