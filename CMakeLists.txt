

message(STATUS "!!!!!!!!!!!!!!!!!!!! FIX THE KFILE PLUGINS !!!!!!!!!!!!!!!!!!!!!!!!!")

macro_optional_find_package(PNG)
macro_optional_find_package(OpenEXR)
macro_optional_find_package(TIFF)
macro_optional_find_package(PopplerQt4)
add_subdirectory( dvi )

if(OPENEXR_FOUND)
#add_subdirectory(exr)
endif(OPENEXR_FOUND)

if(TIFF_FOUND)
#add_subdirectory(tiff)
endif(TIFF_FOUND)

if(HAVE_POPPLER_HEAD)
    add_subdirectory(pdf)
endif(HAVE_POPPLER_HEAD)

add_subdirectory( ps )
add_subdirectory( jpeg )
#add_subdirectory( rgb )
add_subdirectory( ico )
#add_subdirectory( pnm )
#add_subdirectory( dds )
#add_subdirectory( gif )
if ( UNIX )
	#  add_subdirectory( raw )
else( UNIX )
  MESSAGE(STATUS "index function is not found under Windows")	
endif( UNIX )
#add_subdirectory( xps )
