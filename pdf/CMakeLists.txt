
include_directories(${POPPLER_QT4_INCLUDE_DIR} )


########### next target ###############

set(kfile_pdf_PART_SRCS kfile_pdf.cpp )

kde4_automoc(${kfile_pdf_PART_SRCS})

kde4_add_plugin(kfile_pdf ${kfile_pdf_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kfile_pdf )

target_link_libraries(kfile_pdf  ${KDE4_KIO_LIBS} ${POPPLER_QT4_LIBRARIES})

install(TARGETS kfile_pdf  DESTINATION ${PLUGIN_INSTALL_DIR} )


########### install files ###############

install( FILES kfile_pdf.desktop  DESTINATION  ${SERVICES_INSTALL_DIR} )


