/*
** $Id$
**
** Minimal GIF parser, for use in extracting and setting metadata.
** Modified for standalone & KDE calling by Bryce Nesbitt
** 
**  TODO:
**      Support gif comments that span more than one comment block.
**      Verify that Unicode utf-8 is fully unmolested by this code.
**      Implement gif structure verifier.
**
** Based on: GIFtrans v1.12.2
** Copyright (C) 24.2.94 by Andreas Ley <ley@rz.uni-karlsruhe.de>
**
*******************************************************************************
**
** Original distribution site is
**      ftp://ftp.rz.uni-karlsruhe.de/pub/net/www/tools/giftrans/giftrans.c
** A man-page by knordlun@fltxa.helsinki.fi (Kai Nordlund) is at
**      ftp://ftp.rz.uni-karlsruhe.de/pub/net/www/tools/giftrans/giftrans.1
** An online version by taylor@intuitive.com (Dave Taylor) is at
**         http://www.intuitive.com/coolweb/Addons/giftrans-doc.html
** To compile for MS-DOS or OS/2, you need getopt:
**      ftp://ftp.rz.uni-karlsruhe.de/pub/net/www/tools/giftrans/getopt.c
** MS-DOS executable can be found at
**      ftp://ftp.rz.uni-karlsruhe.de/pub/net/www/tools/giftrans/giftrans.exe
** OS/2 executable can be found at
**      ftp://ftp.rz.uni-karlsruhe.de/pub/net/www/tools/giftrans/giftrans.os2.exe
** A template rgb.txt for use with the MS-DOS version can be found at
**      ftp://ftp.rz.uni-karlsruhe.de/pub/net/www/tools/giftrans/rgb.txt
** Additional info can be found on
**      http://melmac.corp.harris.com/transparent_images.html
**
** The GIF file format is documented in
**      ftp://ftp.uu.net/doc/literary/obi/Standards/Graphics/Formats/gif89a.doc.Z
** A good quick reference is at:
**      http://www.goice.co.jp/member/mo/formats/gif.html
**
*******************************************************************************
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
**
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define  STANDALONE_COMPILE

extern int set_gif_comment( const char * original_filename, char * comment );
extern void * get_gif_info( const char * original_filename );
extern int validate_gif_structure( const char * original_filename );
int giftrans( FILE * src, FILE * dest);

#define WARNING_GARBAGE     1   /* Original file had some unspecified content */
#define ERROR_NOT_A_JPEG    5   /* Original file not a proper jpeg (must be 1st) */
#define ERROR_TEMP_FILE     6   /* Problem writing temporay file */
#define ERROR_SCREWUP       7   /* Original file is now damaged.  Ooops. */
#define ERROR_PREMATURE_EOF 8   /* Unexpected end of file */
#define ERROR_BAD_MARKER    9   /* Marker with illegal length */
#define ERROR_MARKER_ORDER  10  /* File seems to be mixed up */

/*****************************************************************************/
#ifndef FALSE
#define FALSE   (0)             /* This is the naked Truth */
#define TRUE    (1)             /* and this is the Light */
#endif

#ifdef DONT_USE_B_MODE          /* define mode parameters for fopen() */
#define READ_BINARY     "r"
#define WRITE_BINARY    "w"
#else
#ifdef VMS                      /* VMS is very nonstandard */
#define READ_BINARY     "rb", "ctx=stm"
#define WRITE_BINARY    "wb", "ctx=stm"
#else                           /* standard ANSI-compliant case */
#define READ_BINARY     "rb"
#define WRITE_BINARY    "wb"
#endif
#endif

#define SUCCESS (0)
#define FAILURE (1)

static char     skipcomment,list,verbose,output,debug;
char     *global_comment;
static long int pos;

static char     true[] = "True";
static char     false[] = "False";
static char     id[] = "$Id$";

/*****************************************************************************/

#define readword(buffer)        ((buffer)[0]+256*(buffer)[1])
#define readflag(buffer)        ((buffer)?true:false)
#define hex(c)                  ('a'<=(c)&&(c)<='z'?(c)-'a'+10:'A'<=(c)&&(c)<='Z'?(c)-'A'+10:(c)-'0')

void dump(adr,data,len)
long int        adr;
unsigned char   *data;
size_t          len;
{
        int     i;

        while (len>0) {
                (void)fprintf(stderr,"%08lx:%*s",adr,(int)((adr%16)*3+(adr%16>8?1:0)),"");
                for (i=adr%16;i<16&&len>0;i++,adr++,data++,len--)
                        (void)fprintf(stderr,"%s%02x",i==8?"  ":" ",*data);
                (void)fprintf(stderr,"\n");
        }
}

void writedata(dest,data,len)
FILE            *dest;
unsigned char   *data;
size_t          len;
{
        unsigned char   size;

        while (len) {
                size=len<256?len:255;
                (void)fwrite((void *)&size,1,1,dest);
                (void)fwrite((void *)data,(size_t)size,1,dest);
                data+=size;
                len-=size;
        }
        size=0;
        (void)fwrite((void *)&size,1,1,dest);
}

void skipdata(src)
FILE    *src;
{
        unsigned char   size,buffer[256];

        do {
                pos=ftell(src);
                (void)fread((void *)&size,1,1,src);
                if (debug)
                        dump(pos,&size,1);
                if (debug) {
                        pos=ftell(src);
                        (void)fread((void *)buffer,(size_t)size,1,src);
                        dump(pos,buffer,(size_t)size);
                }
                else
                        (void)fseek(src,(long int)size,SEEK_CUR);
        } while (!feof(src)&&size>0);
}

void transblock(src,dest)
FILE    *src;
FILE    *dest;
{
        unsigned char   size,buffer[256];

        pos=ftell(src);
        (void)fread((void *)&size,1,1,src);
        if (debug)
                dump(pos,&size,1);
        if (output)
                (void)fwrite((void *)&size,1,1,dest);
        pos=ftell(src);
        (void)fread((void *)buffer,(size_t)size,1,src);
        if (debug)
                dump(pos,buffer,(size_t)size);
        if (output)
                (void)fwrite((void *)buffer,(size_t)size,1,dest);
}

void dumpcomment(src)
FILE    *src;
{
        unsigned char   size,buffer[256];
        size_t i;

        pos=ftell(src);
        (void)fread((void *)&size,1,1,src);
        if (debug)
                dump(pos,&size,1);
        (void)fread((void *)buffer,(size_t)size,1,src);
        if (debug)
                dump(pos+1,buffer,(size_t)size);
        for (i=0; i<(size_t)size; i++)
        {
                if ( buffer[i] >= 0x20 || buffer[i] == '\t' ||
                     buffer[i] =='\r'  || buffer[i] == '\n')
                        (void)putc(buffer[i],stderr);
                else
                        (void)fprintf(stderr,"\\%03o",buffer[i]);
        }
        (void)fseek(src,(long int)pos,SEEK_SET);
}

void transdata(src,dest)
FILE    *src;
FILE    *dest;
{
        unsigned char   size,buffer[256];

        do {
                pos=ftell(src);
                (void)fread((void *)&size,1,1,src);
                if (debug)
                        dump(pos,&size,1);
                if (output)
                        (void)fwrite((void *)&size,1,1,dest);
                pos=ftell(src);
                (void)fread((void *)buffer,(size_t)size,1,src);
                if (debug)
                        dump(pos,buffer,(size_t)size);
                if (output)
                        (void)fwrite((void *)buffer,(size_t)size,1,dest);
        } while (!feof(src)&&size>0);
}


/*****************************************************************************/

int giftrans(src,dest)
FILE    *src;
FILE    *dest;
{
        unsigned char   buffer[3*256],lsd[7],gct[3*256];
        unsigned int    cnt,cols,size,gct_size;

        /* Header */
        pos=ftell(src);
        (void)fread((void *)buffer,6,1,src);
        if (strncmp((char *)buffer,"GIF",3)) {
                (void)fprintf(stderr,"Not GIF file!\n");
                return(1);
        }
        if (verbose && debug) {
                buffer[6]='\0';
                (void)fprintf(stderr,"Header: \"%s\"\n",buffer);
        }
        if (debug)
                dump(pos,buffer,6);
        if (output) {
                (void)fwrite((void *)buffer,6,1,dest);
        }

        /* Logical Screen Descriptor */
        pos=ftell(src);
        (void)fread((void *)lsd,7,1,src);
        if (verbose) {
                //(void)fprintf(stderr,"Logical Screen Descriptor:\n");
                (void)fprintf(stderr,"Size   : %dx%d pixels\n",readword(lsd), readword(lsd+2));
                //(void)fprintf(stderr,"Global Color Table Flag: %s\n",readflag(lsd[4]&0x80));
                (void)fprintf(stderr,"Depth  : %d bits\n",(lsd[4]&0x70>>4)+1);
                //if (lsd[4]&0x80) {
                //        (void)fprintf(stderr,"\tSort Flag: %s\n",readflag(lsd[4]&0x8));
                //        (void)fprintf(stderr,"\tSize of Global Color Table: %d colors\n",2<<(lsd[4]&0x7));
                //        (void)fprintf(stderr,"\tBackground Color Index: %d\n",lsd[5]);
                //}
                if (lsd[6])
                        (void)fprintf(stderr,"Pixel Aspect Ratio: %d (Aspect Ratio %f)\n",lsd[6],((double)lsd[6]+15)/64);
        }
        if (debug)
                dump(pos,lsd,7);
        if (output)
                (void)fwrite((void *)lsd,7,1,dest);

        /* Global Color Table */
        if (lsd[4]&0x80) {
                gct_size=2<<(lsd[4]&0x7);
                pos=ftell(src);
                (void)fread((void *)gct,gct_size,3,src);
        }
        if (output) {
                (void)fwrite((void *)gct,gct_size,3,dest);
        }

        do {
                pos=ftell(src);
                (void)fread((void *)buffer,1,1,src);
                switch (buffer[0]) {
                case 0x2c:      /* Image Descriptor */
                        if (verbose && debug)
                                (void)fprintf(stderr,"Image Descriptor:\n");
                        (void)fread((void *)(buffer+1),9,1,src);
                        if (debug)
                                dump(pos,buffer,10);
                        if (output)
                                (void)fwrite((void *)buffer,10,1,dest);
                        /* Local Color Table */
                        if (buffer[8]&0x80) {
                                size=2<<(buffer[8]&0x7);
                                pos=ftell(src);
                                (void)fread((void *)buffer,size,3,src);
                                if (verbose && debug) {
                                        (void)fprintf(stderr,"Local Color Table:\n");
                                        for(cnt=0;cnt<size;cnt++)
                                                (void)fprintf(stderr,"\tColor %d: Red %d, Green %d, Blue %d\n",cnt,buffer[3*cnt],buffer[3*cnt+1],buffer[3*cnt+2]);
                                }
                                if (debug)
                                        dump(pos,buffer,size*3);
                                if (output)
                                        (void)fwrite((void *)buffer,size,3,dest);
                        }
                        /* Table Based Image Data */
                        pos=ftell(src);
                        (void)fread((void *)buffer,1,1,src);
                        if (verbose && debug) {
                                (void)fprintf(stderr,"Table Based Image Data:\n");
                                (void)fprintf(stderr,"\tLZW Minimum Code Size: 0x%02x\n",buffer[0]);
                        }
                        if (debug)
                                dump(pos,buffer,1);
                        if (output)
                                (void)fwrite((void *)buffer,1,1,dest);
                        transdata(src,dest);
                        break;
                case 0x21:      /* Extension */
                        (void)fread((void *)(buffer+1),1,1,src);
                        switch (buffer[1]) {
                        case 0xfe:      /* Comment Extension */
                                if (verbose)
                                {
                                        (void)fprintf(stderr,"Comment: ");
                                        dumpcomment(src);
                                        (void)fprintf(stderr,"\n");
                                }
                                if (debug)
                                        dump(pos,buffer,2);
                                if (skipcomment)
                                        skipdata(src);
                                else {
                                        if (output)
                                                (void)fwrite((void *)buffer,2,1,dest);
                                        transdata(src,dest);
                                }
                                break;
                        case 0x01:      /* Plain Text Extension */
                        case 0xf9:      /* Graphic Control Extension */
                        case 0xff:      /* Application Extension */
                        default:
                                if (verbose && debug)
                                        (void)fprintf(stderr,"Extension type: 0x%02x\n",buffer[1]);
                                if (debug)
                                        dump(pos,buffer,2);
                                if (output)
                                        (void)fwrite((void *)buffer,2,1,dest);
                                transblock(src,dest);
                                transdata(src,dest);
                                break;
                        }
                        break;
                case 0x3b:      /* Trailer (write comment just before here) */
                        if (verbose && debug)
                                (void)fprintf(stderr,"Trailer %o\n", pos);
                        if (debug)
                                dump(pos,buffer,1);
                        if (global_comment && *global_comment && output) {
                                (void)fputs("\041\376",dest);
                                writedata(dest,(unsigned char *)global_comment,strlen(global_comment));
                        }
                        if (output)
                                (void)fwrite((void *)buffer,1,1,dest);
                        break;
                default:
                        (void)fprintf(stderr,"0x%08lx: Error, unknown block 0x%02x!\n",ftell(src)-1,buffer[0]);
                        if (debug)
                                dump(pos,buffer,1);
                        return(1);
                }
        } while (buffer[0]!=0x3b&&!feof(src));
        return(buffer[0]==0x3b?SUCCESS:FAILURE);
}


/****************************************************************************/
extern int validate_gif_structure( const char * original_filename )
{
FILE    * infile;
int       rc;

    if ((infile = fopen(original_filename, READ_BINARY)) == NULL) {
        fprintf(stderr, "can't open gif image '%s'\n", original_filename);
        return( 1 );
        }
    output      = FALSE;
    verbose     = FALSE;
    debug       = FALSE;
    skipcomment = FALSE;
    rc = giftrans( infile, NULL );
    fclose( infile );
    return( rc );
}


/****************************************************************************/
extern void * get_gif_info( const char * original_filename )
{
FILE    * infile;

    if ((infile = fopen(original_filename, READ_BINARY)) == NULL) {
        fprintf(stderr, "can't open gif image '%s'\n", original_filename);
        return( NULL );
        }

    output      = FALSE;
    verbose     = TRUE;
    debug       = FALSE;
    skipcomment = FALSE;
    giftrans( infile, NULL );
    return( NULL );
}


/*****************************************************************************
    Modify the file in place, but be paranoid and safe about it.
    It's worth a few extra CPU cycles to make sure we never
    destory an original image:

    1) Validate the input file.
    2) Open a temporary file in same directory (filenameXX).
    3) Copy the data, writing a new comment block.
    4) Sync everything to disc.
    5) Validate the temporary file.
    6) Move the temporary file over the original.
*/
extern int set_gif_comment( const char * original_filename, char * comment )
{
int         i;
int         rc;
char *      temp_filename;
int         temp_filename_length;
struct stat statbuf;
FILE *      infile;
FILE *      outfile;


  /*
   * Make sure we're dealing with a proper input file.  Safety first!
   */
  if( validate_gif_structure( original_filename ) ) {
    fprintf(stderr, "error validating gif image '%s'\n", original_filename);
    return(ERROR_NOT_A_JPEG);
    }

  /* Get a unique temporary file in the same directory.  Hopefully 
   * if things go wrong, this file will still be left for recovery purposes.
   *
   * NB: I hate these stupid unsafe string functions in C...
   */
  outfile = NULL;
  temp_filename_length = strlen( original_filename) + 4;
  temp_filename = (char *)calloc( temp_filename_length, 1 );
  for( i=0; i<10; i++ ) {
    snprintf( temp_filename, temp_filename_length, "%s%d", original_filename, i );
    if( (stat( temp_filename, &statbuf )) && (outfile = fopen(temp_filename, WRITE_BINARY)) ) {
        //fprintf(stderr, "opened temporary file '%s'\n", temp_filename);
        break;
        }
    }
  if( !outfile ) {
    fprintf(stderr, "failed opening temporary file '%s'\n", temp_filename);
    return(ERROR_TEMP_FILE);
    }

  if ((infile = fopen(original_filename, READ_BINARY)) == NULL) {
    fprintf(stderr, "can't open gif image '%s'\n", original_filename);
    return(ERROR_NOT_A_JPEG);
    }
  /* Let's do it */
  output      = TRUE;
  verbose     = FALSE;
  debug       = FALSE;
  skipcomment = TRUE;
  global_comment = comment;
  rc = giftrans( infile, outfile );
  fclose( infile );
  fsync( fileno( outfile) );    /* Flush it to disk.  IMPORTANT!! */
                                /* We really should also flush the directory */

  /* If everything is OK, and if the new file validates, move
     it over the top of the original */
  if ( fclose( outfile ) || validate_gif_structure( temp_filename ) ) {
    fprintf(stderr, "error in temporary file '%s'\n", temp_filename);
    return(ERROR_TEMP_FILE);
    }
  if( rc ) {
    unlink( temp_filename );
    return( rc );
    } 
  if( rename( temp_filename, original_filename ) ) {
    fprintf(stderr, "error renaming '%s' to '%s'\n", temp_filename, original_filename);
    return(ERROR_TEMP_FILE);
    }

  return(0);
}


/*****************************************************************************/
#ifdef  STANDALONE_COMPILE
int
main (int argc, char **argv)
{
    char * progname;
    char * filename;
    char * comment;
    FILE * fp;
    int    error;

    /* Process command line arguments... */
    progname = argv[0];
    if (progname == NULL || progname[0] == 0)
        progname = "gif-info";  /* in case C library doesn't provide it */
    if( argc < 2 || argc > 3) {
        fprintf(stderr, "Usage: %s <filename> [\"<comment>\"]\nReads gif image info or sets comment.\n", progname);
        return(5);
        }
    filename = argv[1];
    comment  = argv[2];


    /* Check if file is readable... */
    if ((fp = fopen(filename, READ_BINARY)) == NULL) {
      fprintf(stderr, "Error: Can't open file '%s'\n", filename);
      return(5);
      }
    fclose(fp);

    /* Check if we really have a commentable image file here... */
    if( validate_gif_structure( filename ) ) {
      fprintf(stderr, "Error: error parsing file '%s' as a gif image\n", filename);
      return(5);
      }

    if( argc == 2 ) {
        get_gif_info( filename );
        }
    else {
        set_gif_comment( filename, comment );
    }

    return( 0 );
}
#endif

