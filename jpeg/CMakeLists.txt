set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  ${KDE4_ENABLE_EXCEPTIONS}")

include_directories(${STRIGI_INCLUDE_DIR})

########### next target ###############

set(kfilewrite_jpeg_PART_SRCS kfilewrite_jpeg.cpp )

kde4_automoc(${kfilewrite_jpeg_PART_SRCS})

kde4_add_plugin(kfilewrite_jpeg ${kfilewrite_jpeg_PART_SRCS})

target_link_libraries(kfilewrite_jpeg ${KDE4_KIO_LIBS})

install(TARGETS kfilewrite_jpeg  DESTINATION ${PLUGIN_INSTALL_DIR} )


########### install files ###############

install( FILES kfilewrite_jpeg.desktop  DESTINATION  ${SERVICES_INSTALL_DIR} )

