set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  ${KDE4_ENABLE_EXCEPTIONS}")

########### next target ###############

# build the analyzer as a module
add_library(jpeg MODULE jpegendanalyzer.cpp exif.cpp kfile_setcomment.cpp)

# link with the required libraries
target_link_libraries(jpeg ${STREAMANALYZER_LIBRARY} ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${KDE4_KDECORE_LIBRARY})

# adjust the name so the module is recognized as a strigi plugin
set_target_properties(jpeg PROPERTIES
    PREFIX strigiea_)

# install the module in the right directory so it is picked up
install(TARGETS jpeg LIBRARY DESTINATION ${LIB_INSTALL_DIR}/strigi)

########### next target ###############

set(kfilewrite_jpeg_PART_SRCS kfilewrite_jpeg.cpp )

kde4_automoc(${kfilewrite_jpeg_PART_SRCS})

kde4_add_plugin(kfilewrite_jpeg ${kfilewrite_jpeg_PART_SRCS})

target_link_libraries(kfilewrite_jpeg ${KDE4_KIO_LIBS})

install(TARGETS kfilewrite_jpeg  DESTINATION ${PLUGIN_INSTALL_DIR} )


########### install files ###############

install( FILES kfilewrite_jpeg.desktop  DESTINATION  ${SERVICES_INSTALL_DIR} )

