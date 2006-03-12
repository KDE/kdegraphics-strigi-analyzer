kde4_header()

macro_optional_find_package(PNG)
add_subdirectory( dvi ) 
if(PNG_FOUND)
	add_subdirectory( png ) 
endif(PNG_FOUND)
add_subdirectory( ps ) 
add_subdirectory( jpeg ) 
add_subdirectory( xbm ) 
add_subdirectory( xpm ) 
add_subdirectory( bmp ) 
add_subdirectory( tga ) 
add_subdirectory( rgb ) 
add_subdirectory( ico ) 
add_subdirectory( pcx ) 
add_subdirectory( pnm ) 
add_subdirectory( dds ) 
add_subdirectory( gif ) 
add_subdirectory( raw ) 

message(STATUS "${CMAKE_CURRENT_SOURCE_DIR}: skipped subdir $(KFILE_TIFF_SUBDIR)")
message(STATUS "${CMAKE_CURRENT_SOURCE_DIR}: skipped subdir $(KFILE_EXR_SUBDIR)")
message(STATUS "${CMAKE_CURRENT_SOURCE_DIR}: skipped subdir $(KFILE_PDF_SUBDIR)")
include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}  )


########### install files ###############


kde4_footer()



#original Makefile.am contents follow:

#if include_EXR_MODULES
#KFILE_EXR_SUBDIR=exr
#endif
#
#if include_TIFF
#KFILE_TIFF_SUBDIR=tiff
#endif
#
#if include_PDF_kfile_plugin
#KFILE_PDF_SUBDIR=pdf
#endif
#
#SUBDIRS=dvi png ps jpeg xbm xpm bmp tga rgb ico pcx $(KFILE_TIFF_SUBDIR) pnm \
#	$(KFILE_EXR_SUBDIR) $(KFILE_PDF_SUBDIR) dds gif raw
