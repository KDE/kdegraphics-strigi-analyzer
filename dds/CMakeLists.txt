include_directories(${STRIGI_INCLUDE_DIR})

set(kfile_dds_PART_SRCS kfile_dds.cpp )


kde4_add_plugin(kfile_dds ${kfile_dds_PART_SRCS})


target_link_libraries(kfile_dds  ${KDE4_KIO_LIBS} )

install(TARGETS kfile_dds  DESTINATION ${PLUGIN_INSTALL_DIR} )


########### install files ###############

install( FILES kfile_dds.desktop  DESTINATION  ${SERVICES_INSTALL_DIR} )

