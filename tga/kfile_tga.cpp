/* This file is part of the KDE project
 * Copyright (C) 2002 Shane Wright <me@shanewright.co.uk>
 *
 * This program is free softtgare; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Softtgare Foundation version 2.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied tgarranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Softtgare Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 */

#include <config.h>
#include "kfile_tga.h"

#include <kprocess.h>
#include <klocale.h>
#include <kgenericfactory.h>
#include <kstringvalidator.h>
#include <kdebug.h>

#include <qdict.h>
#include <qvalidator.h>
#include <qcstring.h>
#include <qfile.h>
#include <qdatetime.h>

#if !defined(__osf__)
#include <inttypes.h>
#else
typedef unsigned long uint32_t;
typedef unsigned short uint16_t;
typedef unsigned char uint8_t;
#endif

typedef KGenericFactory<KTgaPlugin> TgaFactory;

K_EXPORT_COMPONENT_FACTORY(kfile_tga, TgaFactory( "kfile_tga" ));

KTgaPlugin::KTgaPlugin(QObject *parent, const char *name,
                       const QStringList &args)
    
    : KFilePlugin(parent, name, args)
{
    KFileMimeTypeInfo* info = addMimeTypeInfo( "image/x-targa" );

    KFileMimeTypeInfo::GroupInfo* group = 0L;

    group = addGroupInfo(info, "Technical", "Technical Details");

    KFileMimeTypeInfo::ItemInfo* item;

    item = addItemInfo(group, "Resolution", i18n("Resolution"), QVariant::Size);

    item = addItemInfo(group, "Bitdepth", i18n("Bitdepth"), QVariant::Int);
    setSuffix(item, "bpp");

    item = addItemInfo(group, "Color mode", i18n("Color mode"), QVariant::String);
    item = addItemInfo(group, "Compression", i18n("Compression"), QVariant::String);

}

bool KTgaPlugin::readInfo( KFileMetaInfo& info, uint what)
{


    QFile file(info.path());

    if (!file.open(IO_ReadOnly))
    {
        kdDebug(7034) << "Couldn't open " << QFile::encodeName(info.path()) << endl;
        return false;
    }

    QDataStream dstream(&file);

    // TGA files are little-endian
    dstream.setByteOrder(QDataStream::LittleEndian);

    // the vars for the image data 
    uint8_t  tga_idlength;
    uint8_t  tga_colormaptype;
    uint8_t  tga_imagetype;
    uint16_t tga_colormap_fei;
    uint16_t tga_colormap_length;
    uint8_t  tga_colormap_entrysize;
    uint16_t tga_imagespec_origin_x;
    uint16_t tga_imagespec_origin_y;
    uint16_t tga_imagespec_width;
    uint16_t tga_imagespec_height;
    uint8_t  tga_imagespec_depth;
    uint8_t  tga_imagespec_descriptor;

    // read the image data
    dstream >> tga_idlength;
    dstream >> tga_colormaptype;
    dstream >> tga_imagetype;
    dstream >> tga_colormap_fei;
    dstream >> tga_colormap_length;
    dstream >> tga_colormap_entrysize;
    dstream >> tga_imagespec_origin_x;
    dstream >> tga_imagespec_origin_y;
    dstream >> tga_imagespec_width;
    dstream >> tga_imagespec_height;
    dstream >> tga_imagespec_depth;
    dstream >> tga_imagespec_descriptor;

    // output the useful bits
    KFileMetaInfoGroup group = appendGroup(info, "Technical");
    appendItem(group, "Resolution", QSize(tga_imagespec_width, tga_imagespec_height));
    appendItem(group, "Bitdepth", tga_imagespec_depth);

    switch (tga_imagetype) {
    case 1 : 
    case 9 :
    case 32 :
        appendItem(group, "Color mode", i18n("Color-mapped"));
        break;
    case 2 :
    case 10 :
    case 33 :
        appendItem(group, "Color mode", i18n("RGB"));
        break;
    case 3 :
    case 11 :
        appendItem(group, "Color mode", i18n("Black and white"));
        break;
    default :
        appendItem(group, "Color mode", i18n("Unknown"));
    }

    switch (tga_imagetype) {
    case 1 :
    case 2 :
    case 3 :
        appendItem(group, "Compression", i18n("Uncompressed"));
        break;
    case 9 :
    case 10 :
    case 11 :
        appendItem(group, "Compression", i18n("Runlength encoded"));
        break;
    case 32 :
        appendItem(group, "Compression", i18n("Huffman, Delta & RLE"));
        break;
    case 33 :
        appendItem(group, "Compression", i18n("Huffman, Delta, RLE (4-pass quadtree)"));
        break;
    default :
        appendItem(group, "Compression", i18n("Unknown"));
    };

    return true;
}

#include "kfile_tga.moc"
