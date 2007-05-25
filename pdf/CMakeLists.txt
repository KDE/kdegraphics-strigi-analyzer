include_directories(${STRIGI_INCLUDE_DIR})
include_directories(${POPPLER_INCLUDE_DIR}/poppler)

# build the analyzer as a module
kde4_add_library(pdf MODULE pdfthroughanalyzer.cpp)

# link with the required libraries
target_link_libraries(pdf ${POPPLER_LIBRARY} ${STREAMANALYZER_LIBRARY} ${KDE4_KDECORE_LIBS})

# adjust the name so the module is recognized as a strigi plugin
set_target_properties(pdf PROPERTIES
    PREFIX strigita_)

# install the module in the right directory so it is picked up
install(TARGETS pdf LIBRARY DESTINATION ${LIB_INSTALL_DIR}/strigi)

