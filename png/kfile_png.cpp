/* This file is part of the KDE project
 * Copyright (C) 2001, 2002 Rolf Magnus <ramagnus@kde.org>
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
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 *  $Id$
 */

#include <stdlib.h>
#include "kfile_png.h"

#include <kurl.h>
#include <kprocess.h>
#include <klocale.h>
#include <kgenericfactory.h>
#include <kdebug.h>

#include <qcstring.h>
#include <qfile.h>
#include <qdatetime.h>
#include <qdict.h>
#include <qvalidator.h>

// some defines to make it easier
// don't tell me anything about preprocessor usage :)
#define CHUNK_SIZE(data, index) ((data[index  ]<<24) + (data[index+1]<<16) + \
                                 (data[index+2]<< 8) +  data[index+3])
#define CHUNK_TYPE(data, index)  &data[index+4]
#define CHUNK_HEADER_SIZE 12
#define CHUNK_DATA(data, index, offset) data[8+index+offset]

// known translations for common png keys
static const char* knownTranslations[] = {
  I18N_NOOP("Title"),
  I18N_NOOP("Author"),
  I18N_NOOP("Description"),
  I18N_NOOP("Copyright"),
  I18N_NOOP("Creation Time"),
  I18N_NOOP("Software"),
  I18N_NOOP("Disclaimer"),
  I18N_NOOP("Warning"),
  I18N_NOOP("Source"),
  I18N_NOOP("Comment")
};    

// and for the colors
static const char* colors[] = {
  I18N_NOOP("Grayscale"),
  I18N_NOOP("Unknown"),
  I18N_NOOP("RGB"),
  I18N_NOOP("Palette"),
  I18N_NOOP("Grayscale/Alpha"),
  I18N_NOOP("RGB/Alpha")
};

  // and compressions  
char* compressions[] = 
{
  I18N_NOOP("deflate")
};

typedef KGenericFactory<KPngPlugin> PngFactory;

K_EXPORT_COMPONENT_FACTORY(kfile_png, PngFactory("kfile_png"));

KPngPlugin::KPngPlugin(QObject *parent, const char *name,
                       const QStringList &preferredItems)
    : KFilePlugin(parent, name, preferredItems)
{
    kdDebug(7034) << "png plugin\n";

    // set up our mime type
    KFileMimeTypeInfo* info = addMimeTypeInfo( "image/png" );

    KFileMimeTypeInfo::GroupInfo* group = 0;
    KFileMimeTypeInfo::ItemInfo* item = 0;

    // comment group
    group = addGroupInfo(info, "Comment", i18n("Comment"));
    addVariableInfo(group, QVariant::String, 0);

    // technical group
    group = addGroupInfo(info, "Technical", i18n("Technical details"));

    item = addItemInfo(group, "Resolution", i18n("Resolution"), QVariant::Size);
    setSuffix(item, i18n("pixels"));

    item = addItemInfo(group, "Bitdepth", i18n("Bitdepth"), QVariant::Int);
    setSuffix(item, i18n("bpp"));

    addItemInfo(group, "Color mode", i18n("Color mode"), QVariant::String);
    addItemInfo(group, "Compression", i18n("Compression"), QVariant::String);
}

bool KPngPlugin::readInfo( KFileMetaInfo& info, uint what)
{
    QFile f(info.path());
    f.open(IO_ReadOnly);

    bool readComments = false;
    if (what & (KFileMetaInfo::Fastest |
                KFileMetaInfo::DontCare |
                KFileMetaInfo::ContentInfo)) readComments = true;

    unsigned char *data = (unsigned char*) malloc(f.size()+1);
    f.readBlock((char*)data, f.size());
    data[f.size()]='\n';
  
    // find the start
    if ((data[0] == 137) && (data[1] == 80) && (data[2] == 78) && (data[3] == 71)
        && (data[4] ==  13) && (data[5] == 10) && (data[6] == 26) && (data[7] == 10))
    {
        // ok
            // the IHDR chunk should be the first
        if (!strncmp((char*)&data[12], "IHDR", 4))
        {
            // we found it
            unsigned long x,y;
            x = (data[16] << 24) + (data[17] << 16) + (data[18] << 8) + data[19];
            y = (data[20] << 24) + (data[21] << 16) + (data[22] << 8) + data[23];

            int type = data[25];
            int bpp = data[24];

            kdDebug(7034) << "resolution " << x << "*" << y << endl;

            switch (type)
            {
                case 0: break;           // Grayscale
                case 2: bpp *= 3; break; // RGB
                case 3: break;           // palette
                case 4: bpp *= 2; break; // grayscale w. alpha
                case 5: bpp *= 4; break; // RGBA

                default: // we don't get any sensible value here
                    bpp = 0;
            }

            KFileMetaInfoGroup techgroup = appendGroup(info, "Technical");

            appendItem(techgroup, "Resolution", QSize(x, y));
            appendItem(techgroup, "Bitdepth", bpp);
            appendItem(techgroup, "Color mode",
                            (type < sizeof(colors)/sizeof(colors[0])) ? colors[data[25]] : i18n("Unknown"));
            appendItem(techgroup, "Compression",
                            (data[26] < sizeof(compressions)/sizeof(compressions[0])) ? compressions[data[26]] : i18n("Unknown"));
        }

        // look for a tEXt chunk
        if (readComments)
        {
            int index = 8;
            index += CHUNK_SIZE(data, index) + CHUNK_HEADER_SIZE;
            KFileMetaInfoGroup commentGroup = appendGroup(info, "Comment");

            while(index<f.size()-12)
            {
                while (strncmp((char*)CHUNK_TYPE(data,index), "tEXt", 4)) 
                {
                    if (!strncmp((char*)CHUNK_TYPE(data,index), "IEND", 4))
                    {
                        free(data);
                        return false;;
                    }

                    index += CHUNK_SIZE(data, index) + CHUNK_HEADER_SIZE;
                }

                if (index<f.size())
                {
                    // we found a tEXt field
                    kdDebug(7034) << "We found a tEXt field\n";
                    // get the key, it's a null terminated string at the chunk start
                    unsigned char* key = &CHUNK_DATA(data,index,0);

                    int keysize = strlen((char*)key);

                    // the text comes after the key, but isn't null terminated
                    unsigned char* text = &CHUNK_DATA(data,index, keysize+1);
                    int textsize = CHUNK_SIZE(data, index)-keysize-1;
                    QByteArray arr(textsize);
                    arr = QByteArray(textsize).duplicate((const char*)text, textsize);

                    appendItem(commentGroup, QString(reinterpret_cast<char*>(key)), QVariant(QString(arr)));

                    kdDebug(7034) << "adding " << key << " / " << QString(arr) << endl;

                    index += CHUNK_SIZE(data, index) + CHUNK_HEADER_SIZE;
                }
            }
        }
    }
    free(data);
    return true;
}

#include "kfile_png.moc"
