/*
** Minimal GIF parser, for use in extracting and setting metadata.
** Modified for KDE by Bryce Nesbitt
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
** The GIF file format is documented in
**      ftp://ftp.uu.net/doc/literary/obi/Standards/Graphics/Formats/gif89a.doc.Z
**
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
** Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
**
*/

#define  STANDALONE_COMPILE

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#ifndef FALSE
#define FALSE   (0)             /* This is the naked Truth */
#define TRUE    (1)             /* and this is the Light */
#endif

#define SUCCESS (0)
#define FAILURE (1)

static char     skipcomment,list,verbose,output,debug;
static char     *comment;
static long int pos;

static char     true[] = "True";
static char     false[] = "False";


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
                if (isprint(buffer[i]) || buffer[i]=='\r' || buffer[i]=='\n')
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
                (void)fprintf(stderr,"Width x Height: %dx%d pixels\n",readword(lsd), readword(lsd+2));
                //(void)fprintf(stderr,"Global Color Table Flag: %s\n",readflag(lsd[4]&0x80));
                (void)fprintf(stderr,"Color Depth: %d bits\n",(lsd[4]&0x70>>4)+1);
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
                                /*
                                :TODO:
                                Count comment size
                                Allocate big enough buffer
                                Read comment
                                */
                                if (verbose)
                                {
                                        (void)fprintf(stderr,"Comment Extension:\n");
                                        dumpcomment(src);
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
                        if (comment && *comment && output) {
                                (void)fprintf(stderr,"Writing comment %s\n", comment);
                                (void)fputs("\041\376",dest);
                                writedata(dest,(unsigned char *)comment,strlen(comment));
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

#ifdef  STANDALONE_COMPILE
int main(argc,argv)
int     argc;
char    *argv[];
{
int     rc;

        output      = TRUE;
        verbose     = TRUE;
        debug       = FALSE;
        skipcomment = FALSE;
        rc = giftrans(stdin, stdout);
        (void)fprintf(stderr,"\nReturn code %d\n", rc );
}
#endif
