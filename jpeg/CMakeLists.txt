set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  ${KDE4_ENABLE_EXCEPTIONS}")

########### next target ###############

# build the analyzer as a module
add_library(jpeg MODULE jpegendanalyzer.cpp exif.cpp kfile_setcomment.cpp)

# link with the required libraries
target_link_libraries(jpeg ${STREAMANALYZER_LIBRARY} ${KDE4_KIO_LIBS})

# adjust the name so the module is recognized as a strigi plugin
set_target_properties(jpeg PROPERTIES
    PREFIX strigiea_)

# install the module in the right directory so it is picked up
install(TARGETS jpeg LIBRARY DESTINATION ${LIB_DESTINATION}/strigi)


########### install files ###############

install( FILES kfile_jpeg.desktop  DESTINATION  ${SERVICES_INSTALL_DIR} )



