
include_directories( ${CMAKE_SOURCE_DIR}/kghostview  )

add_definitions (-DQT3_SUPPORT -DQT3_SUPPORT_WARNINGS)

########### next target ###############

kde4_add_library(ps MODULE psendanalyzer.cpp)
target_link_libraries(ps ${STRIGI_STREAMANALYZER_LIBRARY} ${KDE4_KDECORE_LIBS})
set_target_properties(ps PROPERTIES
    PREFIX strigiea_)
install(TARGETS ps LIBRARY DESTINATION ${LIB_INSTALL_DIR}/strigi)

########### next target ###############

set(gsthumbnail_PART_SRCS gscreator.cpp ${libdscparse_SRCS})

kde4_automoc(${gsthumbnail_PART_SRCS})

kde4_add_plugin(gsthumbnail ${gsthumbnail_PART_SRCS})


target_link_libraries(gsthumbnail  ${KDE4_KDECORE_LIBS} ${QT_QTGUI_LIBRARY} )

install(TARGETS gsthumbnail  DESTINATION ${PLUGIN_INSTALL_DIR} )


########### install files ###############

install( FILES kfile_ps.desktop gsthumbnail.desktop  DESTINATION  ${SERVICES_INSTALL_DIR} )

