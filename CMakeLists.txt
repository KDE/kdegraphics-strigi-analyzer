project(kdegraphics-strigi-analyzers)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake/modules )

# search packages used by KDE
find_package(KDE4 REQUIRED)
include(KDE4Defaults)
include(MacroLibrary)
include(MacroOptionalAddSubdirectory)

add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
include_directories(${KDE4_INCLUDES})

find_package(Strigi REQUIRED)
include_directories(${STRIGI_INCLUDE_DIR})

macro_optional_find_package(TIFF)

macro_log_feature(TIFF_FOUND "libTIFF" "A library for reading and writing TIFF formatted files." "http://www.remotesensing.org/libtiff" FALSE "" "An analyzer for TIFF files.")

add_subdirectory( dvi )

if(TIFF_FOUND)
    add_subdirectory( tiff )
endif(TIFF_FOUND)

message(STATUS "!!!!!!!! port the following kfile plugins as strigi analyzer: dds, exr, pnm, raw, rgb, xps")

#macro_optional_find_package(OpenEXR)

#if(OPENEXR_FOUND)
#add_subdirectory(exr)
#endif(OPENEXR_FOUND)

#add_subdirectory( rgb )
#add_subdirectory( pnm )
#add_subdirectory( dds )
if ( UNIX )
    #  add_subdirectory( raw )
else( UNIX )
    # MESSAGE(STATUS "index function is not found under Windows")	
endif( UNIX )
#add_subdirectory( xps )
