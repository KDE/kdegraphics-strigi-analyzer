/* This file is part of the KDE project
 * Copyright (C) 2004 Melchior FRANZ <mfranz@kde.org>
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
 */

#include <config.h>
#include "kfile_rgb.h"

#include <qfile.h>

#include <kgenericfactory.h>
#include <kdebug.h>


typedef KGenericFactory<KRgbPlugin> RgbFactory;

K_EXPORT_COMPONENT_FACTORY(kfile_rgb, RgbFactory("kfile_rgb"))


KRgbPlugin::KRgbPlugin(QObject *parent, const char *name, const QStringList &args) :
	KFilePlugin(parent, name, args)
{
	KFileMimeTypeInfo* info = addMimeTypeInfo("image/x-rgb");

	KFileMimeTypeInfo::GroupInfo* group = 0;
	group = addGroupInfo(info, "Technical", i18n("Technical Details"));

	KFileMimeTypeInfo::ItemInfo* item;

	item = addItemInfo(group, "Dimensions", i18n("Dimensions"), QVariant::Size);
	setHint(item, KFileMimeTypeInfo::Size);
	setUnit(item, KFileMimeTypeInfo::Pixels);

	item = addItemInfo(group, "BitDepth", i18n("Bit Depth"), QVariant::Int);
	setUnit(item, KFileMimeTypeInfo::BitsPerPixel);

	item = addItemInfo(group, "ColorMode", i18n("Color mode"), QVariant::String);
	item = addItemInfo(group, "Compression", i18n("Compression"), QVariant::String);
	item = addItemInfo(group, "ImageName", i18n("Name"), QVariant::String);
}


bool KRgbPlugin::readInfo(KFileMetaInfo& info, uint /*what*/)
{
	QFile file(info.path());

	if (!file.open(IO_ReadOnly)) {
		kdDebug(7034) << "Couldn't open " << QFile::encodeName(info.path()) << endl;
		return false;
	}

	QDataStream dstream(&file);

	Q_UINT16 magic;
	Q_UINT8  storage;
	Q_UINT8  bpc;
	Q_UINT16 dimension;
	Q_UINT16 xsize;
	Q_UINT16 ysize;
	Q_UINT16 zsize;
	Q_UINT32 pixmin;
	Q_UINT32 pixmax;
	Q_UINT32 dummy;
	char     imagename[80];
	Q_UINT32 colormap;

	dstream >> magic;
	dstream >> storage;
	dstream >> bpc;
	dstream >> dimension;
	dstream >> xsize;
	dstream >> ysize;
	dstream >> zsize;
	dstream >> pixmin;
	dstream >> pixmax;
	dstream >> dummy;
	dstream.readRawBytes(imagename, 80);
	imagename[79] = '\0';
	dstream >> colormap;

	KFileMetaInfoGroup group = appendGroup(info, "Technical");

	if (dimension == 1)
		ysize = 1;
	appendItem(group, "Dimensions", QSize(xsize, ysize));
	appendItem(group, "BitDepth", zsize * 8 * bpc);

	if (dimension == 2 && zsize == 1)
		appendItem(group, "ColorMode", i18n("Grayscale"));
	else if (dimension == 3) {
		if (zsize == 2)
			appendItem(group, "ColorMode", i18n("Grayscale/Alpha"));
		else if (zsize == 3)
			appendItem(group, "ColorMode", i18n("RGB"));
		else if (zsize == 4)
			appendItem(group, "ColorMode", i18n("RGB/Alpha"));
	}

	if (!storage)
		appendItem(group, "Compression", i18n("Uncompressed"));
	else if (storage == 1)
		appendItem(group, "Compression", i18n("Runlength encoded"));
	else
		appendItem(group, "Compression", i18n("Unknown"));

	if (imagename[0])
		appendItem(group, "ImageName", imagename);
	return true;
}

#include "kfile_rgb.moc"
