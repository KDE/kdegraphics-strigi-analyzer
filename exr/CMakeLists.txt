set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${KDE4_ENABLE_EXCEPTIONS}")

include_directories( ${OPENEXR_INCLUDE_DIR}  )
include_directories(${STRIGI_INCLUDE_DIR})

set(kfile_exr_PART_SRCS kfile_exr.cpp )


kde4_add_plugin(kfile_exr ${kfile_exr_PART_SRCS})


target_link_libraries(kfile_exr  ${KDE4_KIO_LIBS} ${OPENEXR_LIBRARIES} )

install(TARGETS kfile_exr  DESTINATION ${PLUGIN_INSTALL_DIR} )


########### install files ###############

install( FILES kfile_exr.desktop  DESTINATION  ${SERVICES_INSTALL_DIR} )

