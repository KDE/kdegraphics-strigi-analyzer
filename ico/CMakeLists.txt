include_directories(${STRIGI_INCLUDE_DIR})

# build the analyzer as a module
kde4_add_library(ico MODULE icothroughanalyzer.cpp)

# link with the required libraries
target_link_libraries(ico ${STRIGI_STREAMANALYZER_LIBRARY} ${QT_QTCORE_LIBRARY})

# adjust the name so the module is recognized as a strigi plugin
set_target_properties(ico PROPERTIES
    PREFIX strigita_)

# install the module in the right directory so it is picked up
install(TARGETS ico LIBRARY DESTINATION ${LIB_INSTALL_DIR}/strigi)


