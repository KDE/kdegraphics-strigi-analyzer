
include_directories(${POPPLER_QT4_INCLUDE_DIR} )


########### next target ###############

# build the analyzer as a module
add_library(pdf MODULE pdfthroughanalyzer.cpp)

# link with the required libraries
target_link_libraries(pdf ${POPPLER_QT4_LIBRARIES} ${STREAMANALYZER_LIBRARY} ${KDE4_KIO_LIBS})

# adjust the name so the module is recognized as a strigi plugin
set_target_properties(pdf PROPERTIES
    PREFIX strigita_)

# install the module in the right directory so it is picked up
install(TARGETS pdf LIBRARY DESTINATION ${LIB_INSTALL_DIR}/strigi)


########### install files ###############

install( FILES kfile_pdf.desktop  DESTINATION  ${SERVICES_INSTALL_DIR} )


