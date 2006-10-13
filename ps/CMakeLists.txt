
include_directories( ${CMAKE_SOURCE_DIR}/kghostview  )


########### next target ###############

set(kfile_ps_PART_SRCS kfile_ps.cpp ${libdscparse_SRCS})

kde4_automoc(kfile_ps ${kfile_ps_PART_SRCS})

kde4_add_plugin(kfile_ps ${kfile_ps_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kfile_ps )

target_link_libraries(kfile_ps  ${KDE4_KIO_LIBS} )

install(TARGETS kfile_ps  DESTINATION ${PLUGIN_INSTALL_DIR} )


########### next target ###############

set(gsthumbnail_PART_SRCS gscreator.cpp ${libdscparse_SRCS})

kde4_automoc(gsthumbnail ${gsthumbnail_PART_SRCS})

kde4_add_plugin(gsthumbnail ${gsthumbnail_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} gsthumbnail )

target_link_libraries(gsthumbnail  ${KDE4_KDECORE_LIBS} )

install(TARGETS gsthumbnail  DESTINATION ${PLUGIN_INSTALL_DIR} )


########### install files ###############

install( FILES kfile_ps.desktop gsthumbnail.desktop  DESTINATION  ${SERVICES_INSTALL_DIR} )

