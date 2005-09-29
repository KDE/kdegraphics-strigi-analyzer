/*
 * setcomment.cpp
 *
 * Copyright 2002 Bryce Nesbitt
 *
 * Based on wrjpgcom.c, Copyright (C) 1994-1997, Thomas G. Lane.
 * Part of the Independent JPEG Group's software release 6b of 27-Mar-1998
 *
 * This file contains a very simple stand-alone application that inserts
 * user-supplied text as a COM (comment) marker in a JPEG/JFIF file.
 * This may be useful as an example of the minimum logic needed to parse
 * JPEG markers.
 *
 * There can be an arbitrary number of COM blocks in each jpeg file, with
 * up to 64K of data each.  We, however, write just one COM and blow away
 * the rest.
 *
 *****************
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation version 2.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

#undef  STANDALONE_COMPILE

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include "config.h"

extern int safe_copy_and_modify( const char * original_filename, const char * comment );
    
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

#define WARNING_GARBAGE     1   /* Original file had some unspecified content */
#define ERROR_NOT_A_JPEG    5   /* Original file not a proper jpeg (must be 1st) */
#define ERROR_TEMP_FILE     6   /* Problem writing temporay file */
#define ERROR_SCREWUP       7   /* Original file is now damaged.  Ooops. */
#define ERROR_PREMATURE_EOF 8   /* Unexpected end of file */
#define ERROR_BAD_MARKER    9   /* Marker with illegal length */
#define ERROR_MARKER_ORDER  10  /* File seems to be mixed up */

static int          global_error;   /* global error flag.  Once set, we're dead. */

/****************************************************************************/
/*
 * These macros are used to read the input file and write the output file.
 * To reuse this code in another application, you might need to change these.
 */
static FILE * infile;           /* input JPEG file */

/* Return next input byte, or EOF if no more */
#define NEXTBYTE()  getc(infile)

static FILE * outfile;          /* output JPEG file */

/* Emit an output byte */
#define PUTBYTE(x)  putc((x), outfile)


/****************************************************************************/
/* Read one byte, testing for EOF */
static int
read_1_byte (void)
{
  int c;

  c = NEXTBYTE();
  if (c == EOF) {
    global_error = ERROR_PREMATURE_EOF;
    }
  return c;
}

/* Read 2 bytes, convert to unsigned int */
/* All 2-byte quantities in JPEG markers are MSB first */
static unsigned int
read_2_bytes (void)
{
  int c1, c2;

  c1 = NEXTBYTE();
  if (c1 == EOF)
    global_error = ERROR_PREMATURE_EOF;
  c2 = NEXTBYTE();
  if (c2 == EOF)
    global_error = ERROR_PREMATURE_EOF;
  return (((unsigned int) c1) << 8) + ((unsigned int) c2);
}


/****************************************************************************/
/* Routines to write data to output file */
static void
write_1_byte (int c)
{
  PUTBYTE(c);
}

static void
write_2_bytes (unsigned int val)
{
  PUTBYTE((val >> 8) & 0xFF);
  PUTBYTE(val & 0xFF);
}

static void
write_marker (int marker)
{
  PUTBYTE(0xFF);
  PUTBYTE(marker);
}

static void
copy_rest_of_file (void)
{
  int c;

  while ((c = NEXTBYTE()) != EOF)
    PUTBYTE(c);
}


/****************************************************************************/
/*
 * JPEG markers consist of one or more 0xFF bytes, followed by a marker
 * code byte (which is not an FF).  Here are the marker codes of interest
 * in this program.  (See jdmarker.c for a more complete list.)
 */

#define M_SOF0  0xC0            /* Start Of Frame N */
#define M_SOF1  0xC1            /* N indicates which compression process */
#define M_SOF2  0xC2            /* Only SOF0-SOF2 are now in common use */
#define M_SOF3  0xC3
#define M_SOF5  0xC5            /* NB: codes C4 and CC are NOT SOF markers */
#define M_SOF6  0xC6
#define M_SOF7  0xC7
#define M_SOF9  0xC9
#define M_SOF10 0xCA
#define M_SOF11 0xCB
#define M_SOF13 0xCD
#define M_SOF14 0xCE
#define M_SOF15 0xCF
#define M_SOI   0xD8            /* Start Of Image (beginning of datastream) */
#define M_EOI   0xD9            /* End Of Image (end of datastream) */
#define M_SOS   0xDA            /* Start Of Scan (begins compressed data) */
#define M_COM   0xFE            /* COMment */


/*
 * Find the next JPEG marker and return its marker code.
 * We expect at least one FF byte, possibly more if the compressor used FFs
 * to pad the file.  (Padding FFs will NOT be replicated in the output file.)
 * There could also be non-FF garbage between markers.  The treatment of such
 * garbage is unspecified; we choose to skip over it but emit a warning msg.
 * NB: this routine must not be used after seeing SOS marker, since it will
 * not deal correctly with FF/00 sequences in the compressed image data...
 */
static int
next_marker (void)
{
  int c;
  int discarded_bytes = 0;

  /* Find 0xFF byte; count and skip any non-FFs. */
  c = read_1_byte();
  while (c != 0xFF) {
    discarded_bytes++;
    c = read_1_byte();
  }
  /* Get marker code byte, swallowing any duplicate FF bytes.  Extra FFs
   * are legal as pad bytes, so don't count them in discarded_bytes.
   */
  do {
    c = read_1_byte();
  } while (c == 0xFF);

  if (discarded_bytes != 0) {
    global_error = WARNING_GARBAGE;
  }

  return c;
}


/*
 * Most types of marker are followed by a variable-length parameter segment.
 * This routine skips over the parameters for any marker we don't otherwise
 * want to process.
 * Note that we MUST skip the parameter segment explicitly in order not to
 * be fooled by 0xFF bytes that might appear within the parameter segment;
 * such bytes do NOT introduce new markers.
 */
static void
copy_variable (void)
/* Copy an unknown or uninteresting variable-length marker */
{
  unsigned int length;

  /* Get the marker parameter length count */
  length = read_2_bytes();
  write_2_bytes(length);
  /* Length includes itself, so must be at least 2 */
  if (length < 2) {
    global_error = ERROR_BAD_MARKER;
    length = 2;
    }
  length -= 2;
  /* Skip over the remaining bytes */
  while (length > 0) {
    write_1_byte(read_1_byte());
    length--;
  }
}

static void
skip_variable (void)
/* Skip over an unknown or uninteresting variable-length marker */
{
  unsigned int length;

  /* Get the marker parameter length count */
  length = read_2_bytes();
  /* Length includes itself, so must be at least 2 */
  if (length < 2) {
    global_error = ERROR_BAD_MARKER;
    length = 2;
    }
  length -= 2;
  /* Skip over the remaining bytes */
  while (length > 0) {
    (void) read_1_byte();
    length--;
  }
}


static int
scan_JPEG_header (int keep_COM)
/*
 * Parse & copy the marker stream until SOFn or EOI is seen;
 * copy data to output, but discard COM markers unless keep_COM is true.
 */
{
  int c1, c2;
  int marker;

  /*
   * Read the initial marker, which should be SOI.
   * For a JFIF file, the first two bytes of the file should be literally
   * 0xFF M_SOI.  To be more general, we could use next_marker, but if the
   * input file weren't actually JPEG at all, next_marker might read the whole
   * file and then return a misleading error message...
   */
  c1 = NEXTBYTE();
  c2 = NEXTBYTE();
  if (c1 != 0xFF || c2 != M_SOI) {
    global_error = ERROR_NOT_A_JPEG;
    return EOF;
    }

  write_marker(M_SOI);

  /* Scan miscellaneous markers until we reach SOFn. */
  for (;;) {
    marker = next_marker();
    switch (marker) {
      /* Note that marker codes 0xC4, 0xC8, 0xCC are not, and must not be,
       * treated as SOFn.  C4 in particular is actually DHT.
       */
      case M_SOF0:                /* Baseline */
      case M_SOF1:                /* Extended sequential, Huffman */
      case M_SOF2:                /* Progressive, Huffman */
      case M_SOF3:                /* Lossless, Huffman */
      case M_SOF5:                /* Differential sequential, Huffman */
      case M_SOF6:                /* Differential progressive, Huffman */
      case M_SOF7:                /* Differential lossless, Huffman */
      case M_SOF9:                /* Extended sequential, arithmetic */
      case M_SOF10:               /* Progressive, arithmetic */
      case M_SOF11:               /* Lossless, arithmetic */
      case M_SOF13:               /* Differential sequential, arithmetic */
      case M_SOF14:               /* Differential progressive, arithmetic */
      case M_SOF15:               /* Differential lossless, arithmetic */
      return marker;

    case M_SOS:                 /* should not see compressed data before SOF */
      global_error = ERROR_MARKER_ORDER;
      break;

    case M_EOI:                 /* in case it's a tables-only JPEG stream */
      return marker;

    case M_COM:                 /* Existing COM: conditionally discard */
      if (keep_COM) {
        write_marker(marker);
        copy_variable();
      } else {
        skip_variable();
      }
      break;

    default:                    /* Anything else just gets copied */
      write_marker(marker);
      copy_variable();          /* we assume it has a parameter count... */
      break;
    }
  } /* end loop */
}


/****************************************************************************/
/*
    Verify we know how to set the comment on this type of file.

    TODO: The actual check!  This should verify
    the image size promised in the headers matches the file,
    and that all markers are properly formatted.
*/
static int validate_image_file( const char * filename )
{
int status = 1;
int c1, c2;

  if ( (infile = fopen(filename, READ_BINARY)) ) {
    c1 = NEXTBYTE();
    c2 = NEXTBYTE();
    if (c1 != 0xFF || c2 != M_SOI) 
      status = ERROR_NOT_A_JPEG;
    else 
      status = 0;
    fclose( infile ); 
  }
  return( status );
}


/****************************************************************************/
/*
    Modify the file in place, but be paranoid and safe about it.
    It's worth a few extra CPU cycles to make sure we never
    destory an original image:
    1) Validate the input file.
    2) Open a temporary file.
    3) Copy the data, writing a new comment block.
    4) Validate the temporary file.
    5) Move the temporary file over the original.

    To be even more paranoid & safe we could:
    5) Rename the original to a different temporary name.
    6) Rename the temporary to the original.
    7) Delete the original.
*/
extern int safe_copy_and_modify( const char * original_filename, const char * comment )
{
char *       temp_filename;
int          temp_filename_length;
int          comment_length  = 0;
int          marker;
int          i;
struct stat  statbuf;

  global_error  = 0;

  /*
   * Make sure we're dealing with a proper input file.  Safety first!
   */
  if( validate_image_file( original_filename ) ) {
    fprintf(stderr, "error validating original file %s\n", original_filename);
    return(ERROR_NOT_A_JPEG);
    }

  /* Get a unique temporary file in the same directory.  Hopefully 
   * if things go wrong, this file will still be left for recovery purposes.
   *
   * NB: I hate these stupid string functions in C... the buffer length is too
   * hard to manage...
   */
  outfile = NULL;
  temp_filename_length = strlen( original_filename) + 4;
  temp_filename = (char *)calloc( temp_filename_length, 1 );
  for( i=0; i<10; i++ ) {
    snprintf( temp_filename, temp_filename_length, "%s%d", original_filename, i );
    if( stat( temp_filename, &statbuf ) ) {
        outfile = fopen(temp_filename, WRITE_BINARY);
        break;
        }
    }
  if( !outfile ) {
    fprintf(stderr, "failed opening temporary file %s\n", temp_filename);
    free(temp_filename);
    return(ERROR_TEMP_FILE);
    }


  /*
   * Let's rock and roll! 
   */
  if ((infile = fopen(original_filename, READ_BINARY)) == NULL) {
    fprintf(stderr, "can't open input file %s\n", original_filename);
    free(temp_filename);
    return(ERROR_NOT_A_JPEG);
    }
  /* Copy JPEG headers until SOFn marker;
   * we will insert the new comment marker just before SOFn.
   * This (a) causes the new comment to appear after, rather than before,
   * existing comments; and (b) ensures that comments come after any JFIF
   * or JFXX markers, as required by the JFIF specification.
   */
  marker = scan_JPEG_header(0);
  /* Insert the new COM marker, but only if nonempty text has been supplied */
  if (comment) {
    comment_length = strlen( comment );
    }
  if (comment_length > 0) {
    write_marker(M_COM);
    write_2_bytes(comment_length + 2);
    while (comment_length > 0) {
      write_1_byte(*comment++);
      comment_length--;
      }
    }
  /* Duplicate the remainder of the source file.
   * Note that any COM markers occurring after SOF will not be touched.
   *
   * :TODO: Discard COM markers occurring after SOF
   */
  write_marker(marker);
  copy_rest_of_file();
  fclose( infile );
  fsync( fileno( outfile) );    /* Make sure its really on disk first.  !!!VERY IMPORTANT!!! */
  if ( fclose( outfile ) ) {
    fprintf(stderr, "error in temporary file %s\n", temp_filename);
    free(temp_filename);
    return(ERROR_TEMP_FILE);
    }


  /*
   * Make sure we did it right.  We've already fsync()'ed the file.  Safety first!
   */
  if( validate_image_file( temp_filename ) ) {
    fprintf(stderr, "error in temporary file %s\n", temp_filename);
    free(temp_filename);
    return(ERROR_TEMP_FILE);
    }

  if( global_error >= ERROR_NOT_A_JPEG ) {
    fprintf(stderr, "error %d processing %s\n", global_error, original_filename);
    free(temp_filename);
    return(ERROR_NOT_A_JPEG);
    }

  if( rename( temp_filename, original_filename ) ) {
    fprintf(stderr, "error renaming %s to %s\n", temp_filename, original_filename);
    free(temp_filename);
    return(ERROR_TEMP_FILE);
    }
  free(temp_filename);

  return(0);
}


#ifdef  STANDALONE_COMPILE
int
main (int argc, char **argv)
{
    char * progname;       /* program name for error messages */
    char * filename;
    char * comment;
    FILE * fp;
    int    error;

    /* Process command line arguments... */
    progname = argv[0];
    if (progname == NULL || progname[0] == 0)
        progname = "writejpgcomment";  /* in case C library doesn't provide it */
    if( argc != 3) {
        fprintf(stderr, "Usage: %s <filename> \"<comment>\"\nOverwrites the comment in a image file with the given comment.\n", progname);
        return(5);
        }
    filename = argv[1];
    comment  = argv[2];


    /* Check if file is readable... */
    if ((fp = fopen(filename, READ_BINARY)) == NULL) {
      fprintf(stderr, "Error: Can't open file %s\n", filename);
      fclose(fp);
      return(5);
    }
    fclose(fp);

    /* Check if we really have a commentable image file here... */
    if( validate_image_file( filename ) ) {
      fprintf(stderr, "Error: file %s is not of a supported type\n", filename);
      return(5);
      }

    /* Lets do it... modify the comment in place */
    if ((error = safe_copy_and_modify( filename, comment ) )) {
        fprintf(stderr, "Error: %d setting jpg comment\n", error);
        return(10);
    }


    /* TODO: Read comment back out of jpg and display it */
    return( 0 );
}
#endif
