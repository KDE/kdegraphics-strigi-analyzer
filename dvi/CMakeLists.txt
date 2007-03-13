########### next target ###############

add_library(dvi MODULE dvithroughanalyzer.cpp)

target_link_libraries(dvi ${STREAMINDEXER_LIBRARY} ${KDE4_KIO_LIBS} ${KDE4_SOLID_LIBS})

set_target_properties(dvi PROPERTIES
    PREFIX strigita_)

install(TARGETS dvi LIBRARY DESTINATION ${LIB_DESTINATION}/strigi)

