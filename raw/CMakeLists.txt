include_directories(${STRIGI_INCLUDE_DIR})

set(kfile_raw_PART_SRCS kcamerarawplugin.cpp parse.c )


kde4_add_plugin(kfile_raw ${kfile_raw_PART_SRCS})


target_link_libraries(kfile_raw  ${KDE4_KIO_LIBS} )

install(TARGETS kfile_raw  DESTINATION ${PLUGIN_INSTALL_DIR} )


########### install files ###############

install( FILES kfile_raw.desktop  DESTINATION  ${SERVICES_INSTALL_DIR} )
