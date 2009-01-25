
add_subdirectory( dvi )

message(STATUS "!!!!!!!!!!!!!!!!!!!! FIX THE KFILE PLUGINS FOR STRIGI-ANALYZER !!!!!!!!!!!!!!!!!!!!!!!!!")

#macro_optional_find_package(OpenEXR)
#macro_optional_find_package(TIFF)

#if(OPENEXR_FOUND)
#add_subdirectory(exr)
#endif(OPENEXR_FOUND)

#if(TIFF_FOUND)
#add_subdirectory(tiff)
#endif(TIFF_FOUND)

#add_subdirectory( rgb )
#add_subdirectory( pnm )
#add_subdirectory( dds )
if ( UNIX )
    #  add_subdirectory( raw )
else( UNIX )
    # MESSAGE(STATUS "index function is not found under Windows")	
endif( UNIX )
#add_subdirectory( xps )
