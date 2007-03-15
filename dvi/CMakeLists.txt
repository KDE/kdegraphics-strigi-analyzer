########### next target ###############

# build the analyzer as a module
add_library(dvi MODULE dvithroughanalyzer.cpp)

# link with the required libraries
target_link_libraries(dvi ${STREAMANALYZER_LIBRARY})

# adjust the name so the module is recognized as a strigi plugin
set_target_properties(dvi PROPERTIES
    PREFIX strigita_)

# install the module in the right directory so it is picked up
install(TARGETS dvi LIBRARY DESTINATION ${LIB_DESTINATION}/strigi)

