/*
 * This file is part of the KDE project, added by Bryce Nesbitt
 *
 * This is a plugin for Konqeror/KFile which processes 'extra' information
 * contained in a .gif image file (In particular the comment, and resolution).
 *
 **************************************
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

#include <stdlib.h>
#include "kfile_gif.h"

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
#include <qimage.h>

typedef KGenericFactory<KGifPlugin> GifFactory;

K_EXPORT_COMPONENT_FACTORY(kfile_gif, GifFactory("kfile_gif"))

KGifPlugin::KGifPlugin(QObject *parent, const char *name,
                       const QStringList &args)
    : KFilePlugin(parent, name, args)
{
    kdDebug(7034) << "gif KFileMetaInfo plugin\n";

    KFileMimeTypeInfo* info = addMimeTypeInfo( "image/gif" );

    KFileMimeTypeInfo::GroupInfo* group = 0L;

    group = addGroupInfo(info, "General", i18n("General"));

    KFileMimeTypeInfo::ItemInfo* item;

    item = addItemInfo(group, "Version", i18n("Version"), QVariant::String);

    item = addItemInfo( group, "Dimensions", i18n("Dimensions"), QVariant::Size );
    setHint( item, KFileMimeTypeInfo::Size );
    setUnit( item, KFileMimeTypeInfo::Pixels );

    item = addItemInfo(group, "BitDepth", i18n("Bit Depth"), QVariant::Int);
    setUnit(item, KFileMimeTypeInfo::BitsPerPixel);

}

bool KGifPlugin::readInfo( KFileMetaInfo& info, uint what )
{
    Q_UNUSED( what );

    kdDebug(7034) << "gif KFileMetaInfo readInfo\n";

    QFile file(info.path());

    if (!file.open(IO_ReadOnly)) {
	kdDebug(7034) << "Couldn't open " << QFile::encodeName(info.path()) << endl;
	return false;
    }

    QDataStream fstream(&file);

    bool isGIF87a = false;
    char magic[7];
    Q_UINT16 width = 0;
    Q_UINT16 height = 0;
    Q_UINT8 miscInfo = 0;

    fstream.readRawBytes( magic, 6 ); 
    magic[6] = 0x00; // null terminate

    // I have special requirements...
    fstream.setByteOrder( QDataStream::LittleEndian );
    fstream >> width;
    fstream >> height;
    fstream >> miscInfo;

    KFileMetaInfoGroup group = appendGroup(info, "General");

    if ( 0 == strncmp( magic, "GIF89a", 6 ) ) {
	appendItem( group, "Version", i18n("GIF Version 89a") );
    } else if ( 0 == strncmp( magic, "GIF87a", 6 ) ) {
	appendItem( group, "Version", i18n("GIF Version 87a") );
	isGIF87a = true;
    } else {
	appendItem( group, "Version", i18n("Unknown") );
    }

    appendItem( group, "Dimensions", QSize(width, height) );

    if ( isGIF87a ) {
	appendItem( group, "BitDepth", ( (miscInfo & 0x07) + 1) );
    }

    file.close();

    return true;
}

#include "kfile_gif.moc"
