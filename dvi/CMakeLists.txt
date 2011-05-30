# build the analyzer as a module
kde4_add_library(dvi MODULE dviendanalyzer.cpp)

# link with the required libraries
target_link_libraries(dvi ${STRIGI_STREAMANALYZER_LIBRARY} ${STRIGI_STREAMS_LIBRARY})

# adjust the name so the module is recognized as a strigi plugin
set_target_properties(dvi PROPERTIES
    PREFIX strigiea_)

# install the module in the right directory so it is picked up
install(TARGETS dvi LIBRARY DESTINATION ${LIB_INSTALL_DIR}/strigi)

