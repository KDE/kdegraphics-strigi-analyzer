set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake/modules )

# search packages used by KDE
find_package(KDE4 REQUIRED)
include(KDE4Defaults)
include(MacroLibrary)
include(MacroOptionalAddSubdirectory)

add_definitions (${QT_DEFINITIONS} ${QT_QTDBUS_DEFINITIONS} ${KDE4_DEFINITIONS})
include_directories (${QDBUS_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${KDE4_INCLUDES})

macro_optional_find_package(TIFF)
find_package(Strigi REQUIRED)

add_subdirectory( dvi )

if(TIFF_FOUND)
    add_subdirectory( tiff )
endif(TIFF_FOUND)

message(STATUS "!!!!!!!!!!!!!!!!!!!! FIX THE KFILE PLUGINS FOR STRIGI-ANALYZER !!!!!!!!!!!!!!!!!!!!!!!!!")

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
