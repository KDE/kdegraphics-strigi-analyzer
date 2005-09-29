/* This file is part of the KDE project
 * Copyright (C) 2002 Shane Wright <me@shanewright.co.uk>
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

#include <config.h>
#include "kfile_bmp.h"

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
#endif

typedef KGenericFactory<KBmpPlugin> BmpFactory;

K_EXPORT_COMPONENT_FACTORY(kfile_bmp, BmpFactory( "kfile_bmp" ))

KBmpPlugin::KBmpPlugin(QObject *parent, const char *name,
                       const QStringList &args)

    : KFilePlugin(parent, name, args)
{
    KFileMimeTypeInfo* info = addMimeTypeInfo( "image/x-bmp" );

    KFileMimeTypeInfo::GroupInfo* group = 0L;

    group = addGroupInfo(info, "Technical", i18n("Technical Details"));

    KFileMimeTypeInfo::ItemInfo* item;

    item = addItemInfo(group, "Type", i18n("Type"), QVariant::String);

    item = addItemInfo(group, "Dimensions", i18n("Dimensions"), QVariant::Size);
    setHint( item, KFileMimeTypeInfo::Size );
    setUnit(item, KFileMimeTypeInfo::Pixels);

    item = addItemInfo(group, "BitDepth", i18n("Bit Depth"), QVariant::Int);
    setUnit(item, KFileMimeTypeInfo::BitsPerPixel);

    item = addItemInfo(group, "Compression", i18n("Compression"), QVariant::String);

}


bool KBmpPlugin::readInfo( KFileMetaInfo& info, uint what)
{
    const char * bmptype_bm = "BM";
    const char * bmptype_ba = "BA";
    const char * bmptype_ci = "CI";
    const char * bmptype_cp = "CP";
    const char * bmptype_ic = "IC";
    const char * bmptype_pt = "PT";

    QFile file(info.path());

    if (!file.open(IO_ReadOnly))
    {
        kdDebug(7034) << "Couldn't open " << QFile::encodeName(info.path()) << endl;
        return false;
    }

    QDataStream dstream(&file);

    // BMP files are little-endian
    dstream.setByteOrder(QDataStream::LittleEndian);

    // create this now because we output image type early on
    KFileMetaInfoGroup group = appendGroup(info, "Technical");


    // read the beginning of the file and make sure it looks ok
    unsigned char * bmp_id = (unsigned char *) malloc(2);
    file.readBlock((char *) bmp_id, 2);

    if (memcmp(bmp_id, bmptype_bm, 2) == 0) {
        appendItem(group, "Type", i18n("Windows Bitmap"));
    } else if (memcmp(bmp_id, bmptype_ba, 2) == 0) {
        appendItem(group, "Type", i18n("OS/2 Bitmap Array"));
    } else if (memcmp(bmp_id, bmptype_ci, 2) == 0) {
        appendItem(group, "Type", i18n("OS/2 Color Icon"));
    } else if (memcmp(bmp_id, bmptype_cp, 2) == 0) {
        appendItem(group, "Type", i18n("OS/2 Color Pointer"));
    } else if (memcmp(bmp_id, bmptype_ic, 2) == 0) {
        appendItem(group, "Type", i18n("OS/2 Icon"));
    } else if (memcmp(bmp_id, bmptype_pt, 2) == 0) {
        appendItem(group, "Type", i18n("OS/2 Pointer"));
    } else {
        return false;
    }

    free(bmp_id);


    // read the next bits, we ignore them, but anyways...
    uint32_t bmp_size;
    uint16_t bmp_reserved1;
    uint16_t bmp_reserved2;
    uint32_t bmp_offbits;

    dstream >> bmp_size;
    dstream >> bmp_reserved1;
    dstream >> bmp_reserved2;
    dstream >> bmp_offbits;


    // we should now be at the file info structure
    uint32_t bmpi_size;
    uint32_t bmpi_width;
    uint32_t bmpi_height;
    uint16_t bmpi_planes;
    uint16_t bmpi_bitcount;
    uint32_t bmpi_compression;

    dstream >> bmpi_size;
    dstream >> bmpi_width;
    dstream >> bmpi_height;
    dstream >> bmpi_planes;
    dstream >> bmpi_bitcount;
    dstream >> bmpi_compression;


    // output the useful bits
    appendItem(group, "Dimensions", QSize(bmpi_width, bmpi_height));
    appendItem(group, "BitDepth", bmpi_bitcount);

    switch (bmpi_compression) {
    case 0 :
        appendItem(group, "Compression", i18n("None"));
        break;
    case 1 :
        appendItem(group, "Compression", i18n("RLE 8bit/pixel"));
        break;
    case 2 :
        appendItem(group, "Compression", i18n("RLE 4bit/pixel"));
        break;
    case 3 :
        appendItem(group, "Compression", i18n("Bitfields"));
        break;
    default :
        appendItem(group, "Compression", i18n("Unknown"));
    }

    return true;
}

#include "kfile_bmp.moc"
