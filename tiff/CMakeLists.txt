
set(tiffanalyzer_SRCS
  tiffendanalyzer.cpp
)

kde4_add_library(tiff MODULE ${tiffanalyzer_SRCS})
target_link_libraries(tiff ${TIFF_LIBRARIES} ${STRIGI_STREAMS_LIBRARY} ${STRIGI_STREAMANALYZER_LIBRARY})
set_target_properties(tiff PROPERTIES PREFIX strigiea_)
install(TARGETS tiff LIBRARY DESTINATION ${LIB_INSTALL_DIR}/strigi)
