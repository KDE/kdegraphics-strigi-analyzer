macro_optional_find_package(TIFF)

add_subdirectory( dvi )
add_subdirectory( mobi )

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
