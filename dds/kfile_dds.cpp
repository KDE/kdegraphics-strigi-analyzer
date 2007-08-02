/* This file is part of the KDE project
 * Copyright (C) 2002 Ignacio Castaño <castano@ludicon.com>
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
#include "kfile_dds.h"

#include <k3process.h>
#include <klocale.h>
#include <kgenericfactory.h>
#include <kstringvalidator.h>
#include <kdebug.h>

#include <q3dict.h>
#include <qvalidator.h>
#include <q3cstring.h>
#include <qfile.h>
#include <qdatetime.h>
#include <QSize>

typedef KGenericFactory<KDdsPlugin> DdsFactory;

typedef quint32 uint;
typedef quint16 ushort;
typedef quint8 uchar;

namespace {	// Private.

#if !defined(MAKEFOURCC)
#	define MAKEFOURCC(ch0, ch1, ch2, ch3) \
		(uint(uchar(ch0)) | (uint(uchar(ch1)) << 8) | \
		(uint(uchar(ch2)) << 16) | (uint(uchar(ch3)) << 24 ))
#endif

	static const uint FOURCC_DDS = MAKEFOURCC('D', 'D', 'S', ' ');
	static const uint FOURCC_DXT1 = MAKEFOURCC('D', 'X', 'T', '1');
	static const uint FOURCC_DXT2 = MAKEFOURCC('D', 'X', 'T', '2');
	static const uint FOURCC_DXT3 = MAKEFOURCC('D', 'X', 'T', '3');
	static const uint FOURCC_DXT4 = MAKEFOURCC('D', 'X', 'T', '4');
	static const uint FOURCC_DXT5 = MAKEFOURCC('D', 'X', 'T', '5');
	static const uint FOURCC_RXGB = MAKEFOURCC('R', 'X', 'G', 'B');

	static const uint DDSD_CAPS = 0x00000001l;
	static const uint DDSD_PIXELFORMAT = 0x00001000l;
	static const uint DDSD_WIDTH = 0x00000004l;
	static const uint DDSD_HEIGHT = 0x00000002l;
	static const uint DDSD_PITCH = 0x00000008l;

	static const uint DDSCAPS_TEXTURE = 0x00001000l;
	static const uint DDSCAPS2_VOLUME = 0x00200000l;
	static const uint DDSCAPS2_CUBEMAP = 0x00000200l;

	static const uint DDPF_RGB = 0x00000040l;
 	static const uint DDPF_FOURCC = 0x00000004l;
 	static const uint DDPF_ALPHAPIXELS = 0x00000001l;

	enum DDSType {
		DDS_A8R8G8B8 = 0,
		DDS_A1R5G5B5 = 1,
		DDS_A4R4G4B4 = 2,
		DDS_R8G8B8 = 3,
		DDS_R5G6B5 = 4,
		DDS_DXT1 = 5,
		DDS_DXT2 = 6,
		DDS_DXT3 = 7,
		DDS_DXT4 = 8,
		DDS_DXT5 = 9,
		DDS_RXGB = 10,
		DDS_UNKNOWN
	};


	struct DDSPixelFormat {
		uint size;
		uint flags;
		uint fourcc;
		uint bitcount;
		uint rmask;
		uint gmask;
		uint bmask;
		uint amask;
	};

	QDataStream & operator>> ( QDataStream & s, DDSPixelFormat & pf )
	{
		s >> pf.size;
		s >> pf.flags;
		s >> pf.fourcc;
		s >> pf.bitcount;
		s >> pf.rmask;
		s >> pf.gmask;
		s >> pf.bmask;
		s >> pf.amask;
		return s;
	}

	struct DDSCaps {
		uint caps1;
		uint caps2;
		uint caps3;
		uint caps4;
	};

	QDataStream & operator>> ( QDataStream & s, DDSCaps & caps )
	{
		s >> caps.caps1;
		s >> caps.caps2;
		s >> caps.caps3;
		s >> caps.caps4;
		return s;
	}

	struct DDSHeader {
		uint size;
		uint flags;
		uint height;
		uint width;
		uint pitch;
		uint depth;
		uint mipmapcount;
		uint reserved[11];
		DDSPixelFormat pf;
		DDSCaps caps;
		uint notused;
	};

	QDataStream & operator>> ( QDataStream & s, DDSHeader & header )
	{
		s >> header.size;
		s >> header.flags;
		s >> header.height;
		s >> header.width;
		s >> header.pitch;
		s >> header.depth;
		s >> header.mipmapcount;
		for( int i = 0; i < 11; i++ ) {
			s >> header.reserved[i];
		}
		s >> header.pf;
		s >> header.caps;
		s >> header.notused;
		return s;
	}

	static bool IsValid( const DDSHeader & header )
	{
		if( header.size != 124 ) {
			return false;
		}
		const uint required = (DDSD_WIDTH|DDSD_HEIGHT|DDSD_PIXELFORMAT);
		if( (header.flags & required) != required ) {
			return false;
		}
		if( header.pf.size != 32 ) {
			return false;
		}
		if( !(header.caps.caps1 & DDSCAPS_TEXTURE) ) {
			return false;
		}
		return true;
	}
	
} // namespace



K_EXPORT_COMPONENT_FACTORY(kfile_dds, DdsFactory( "kfile_dds" ))

// Constructor, init mime type info.
KDdsPlugin::KDdsPlugin(QObject *parent, const QStringList &args) : 
	KFilePlugin(parent, args)
{
    KFileMimeTypeInfo * info = addMimeTypeInfo( "image/x-dds" );

    KFileMimeTypeInfo::GroupInfo * group = 0L;

    group = addGroupInfo(info, "Technical", i18n("Technical Details"));

    KFileMimeTypeInfo::ItemInfo * item;

    item = addItemInfo(group, "Dimensions", i18n("Dimensions"), QVariant::Size);
    setHint(item, KFileMimeTypeInfo::Size);
    setUnit(item, KFileMimeTypeInfo::Pixels);

	item = addItemInfo(group, "Depth", i18n("Depth"), QVariant::Int);
	setUnit(item, KFileMimeTypeInfo::Pixels);
	
    item = addItemInfo(group, "BitDepth", i18n("Bit Depth"), QVariant::Int);
    setUnit(item, KFileMimeTypeInfo::BitsPerPixel);

	addItemInfo(group, "MipmapCount", i18n("Mipmap Count"), QVariant::Int);
	
	addItemInfo(group, "Type", i18n("Type"), QVariant::String);
    addItemInfo(group, "ColorMode", i18n("Color Mode"), QVariant::String);
    addItemInfo(group, "Compression", i18n("Compression"), QVariant::String);
}

// Read mime type info.
bool KDdsPlugin::readInfo( KFileMetaInfo& info, uint /*what*/)
{
	QFile file(info.path());

	if (!file.open(QIODevice::ReadOnly)) {
		kDebug(7034) << "Couldn't open " << QFile::encodeName(info.path());
		return false;
	}

	QDataStream s(&file);
	s.setByteOrder(QDataStream::LittleEndian);

	// Validate header.
	uint fourcc;
	s >> fourcc;
	if( fourcc != FOURCC_DDS ) {
		kDebug(7034) << QFile::encodeName(info.path()) << " is not a DDS file.";
		return false;
	}

	// Read image header.
	DDSHeader header;
	s >> header;

	// Check image file format.
	if( s.atEnd() || !IsValid( header ) ) {
		kDebug(7034) << QFile::encodeName(info.path()) << " is not a valid DDS file.";
		return false;
	}

	// Set file info.
	KFileMetaInfoGroup group = appendGroup(info, "Technical");
 	appendItem(group, "Dimensions", QSize(header.width, header.height));
	appendItem(group, "MipmapCount", header.mipmapcount);
	
	// Set file type.
	if( header.caps.caps2 & DDSCAPS2_CUBEMAP ) {
		appendItem(group, "Type", i18n("Cube Map Texture"));
	}
	else if( header.caps.caps2 & DDSCAPS2_VOLUME ) {
		appendItem(group, "Type", i18n("Volume Texture"));
		appendItem(group, "Depth", header.depth);
	}
	else {
		appendItem(group, "Type", i18n("2D Texture"));
	}

	// Set file color depth and compression.
	if( header.pf.flags & DDPF_RGB ) {
		appendItem(group, "BitDepth", header.pf.bitcount);
		appendItem(group, "Compression", i18n("Uncompressed"));
		if( header.pf.flags & DDPF_ALPHAPIXELS ) {
			appendItem(group, "ColorMode", "RGB/Alpha");
		}
		else {
			appendItem(group, "ColorMode", "RGB");
		}
	}
	else if( header.pf.flags & DDPF_FOURCC ) {
		switch( header.pf.fourcc ) {
			case FOURCC_DXT1:
				appendItem(group, "BitDepth", 4);
				appendItem(group, "Compression", "DXT1");
				appendItem(group, "ColorMode", "RGB");
				break;
			case FOURCC_DXT2:
				appendItem(group, "BitDepth", 16);
				appendItem(group, "Compression", "DXT2");
				appendItem(group, "ColorMode", "RGB/Alpha");
				break;
			case FOURCC_DXT3:
				appendItem(group, "BitDepth", 16);
				appendItem(group, "Compression", "DXT3");
				appendItem(group, "ColorMode", "RGB/Alpha");
				break;
			case FOURCC_DXT4:
				appendItem(group, "BitDepth", 16);
				appendItem(group, "Compression", "DXT4");
				appendItem(group, "ColorMode", "RGB/Alpha");
				break;
			case FOURCC_DXT5:
				appendItem(group, "BitDepth", 16);
				appendItem(group, "Compression", "DXT5");
				appendItem(group, "ColorMode", "RGB/Alpha");
				break;
			case FOURCC_RXGB:
				appendItem(group, "BitDepth", 16);
				appendItem(group, "Compression", "RXGB");
				appendItem(group, "ColorMode", "RGB");
				break;
			default:
				appendItem(group, "Compression", "Unknown");
				break;
		}
	}
	else {
		appendItem(group, "Compression", "Unknown");
	}

    return true;
}

#include "kfile_dds.moc"

