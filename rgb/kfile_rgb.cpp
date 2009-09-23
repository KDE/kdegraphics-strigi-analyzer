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
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

#include <config.h>
#include "kfile_rgb.h"

#include <qfile.h>
#include <qvalidator.h>
#include <QSize>
#include <kdebug.h>
#include <kgenericfactory.h>


typedef KGenericFactory<KRgbPlugin> RgbFactory;

K_EXPORT_COMPONENT_FACTORY(kfile_rgb, RgbFactory("kfile_rgb"))


KRgbPlugin::KRgbPlugin(QObject *parent, const QStringList &args) :
	KFilePlugin(parent, args)
{
	KFileMimeTypeInfo* info = addMimeTypeInfo("image/x-rgb");

	KFileMimeTypeInfo::GroupInfo* group = 0;
	KFileMimeTypeInfo::ItemInfo* item;


	group = addGroupInfo(info, "Comment", i18n("Comment"));

	item = addItemInfo(group, "ImageName", i18n("Name"), QVariant::String);
	setAttributes(item, KFileMimeTypeInfo::Modifiable);
	setHint(item, KFileMimeTypeInfo::Description);


	group = addGroupInfo(info, "Technical", i18n("Technical Details"));

	item = addItemInfo(group, "Dimensions", i18n("Dimensions"), QVariant::Size);
	setHint(item, KFileMimeTypeInfo::Size);
	setUnit(item, KFileMimeTypeInfo::Pixels);

	item = addItemInfo(group, "BitDepth", i18n("Bit Depth"), QVariant::Int);
	setUnit(item, KFileMimeTypeInfo::BitsPerPixel);

	item = addItemInfo(group, "ColorMode", i18n("Color Mode"), QVariant::String);
	item = addItemInfo(group, "Compression", i18n("Compression"), QVariant::String);
	item = addItemInfo(group, "SharedRows",
			i18nc("percentage of avoided vertical redundancy (the higher the better)",
			"Shared Rows"), QVariant::String);

}


bool KRgbPlugin::readInfo(KFileMetaInfo& info, uint /*what*/)
{
	QFile file(info.path());

	if (!file.open(QIODevice::ReadOnly)) {
		kDebug(7034) << "Couldn't open " << QFile::encodeName(info.path());
		return false;
	}

	QDataStream dstream(&file);

	quint16 magic;
	quint8  storage;
	quint8  bpc;
	quint16 dimension;
	quint16 xsize;
	quint16 ysize;
	quint16 zsize;
	quint32 pixmin;
	quint32 pixmax;
	quint32 dummy;
	char     imagename[80];
	quint32 colormap;

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
	quint8 u8;
	for (uint i = 0; i < 404; i++)
		dstream >> u8;

	if (magic != 474)
		return false;

	KFileMetaInfoGroup group;

	group = appendGroup(info, "Technical");

	if (dimension == 1)
		ysize = 1;
	appendItem(group, "Dimensions", QSize(xsize, ysize));
	appendItem(group, "BitDepth", zsize * 8 * bpc);

	if (zsize == 1)
		appendItem(group, "ColorMode", i18n("Grayscale"));
	else if (zsize == 2)
		appendItem(group, "ColorMode", i18n("Grayscale/Alpha"));
	else if (zsize == 3)
		appendItem(group, "ColorMode", i18n("RGB"));
	else if (zsize == 4)
		appendItem(group, "ColorMode", i18n("RGB/Alpha"));

	if (!storage)
		appendItem(group, "Compression", i18nc("Compression", "Uncompressed"));
	else if (storage == 1) {
		long compressed = file.size() - 512;
		long verbatim = xsize * ysize * zsize;
		appendItem(group, "Compression", i18nc("Compression", "Runlength Encoded")
				+ QString(", %1%").arg(compressed * 100.0 / verbatim, 0, 'f', 1));

		long k;
		quint32 offs;
		QMap<quint32, uint> map;
		QMap<quint32, uint>::Iterator it;
		QMap<quint32, uint>::Iterator end = map.end();
		for (k = 0; k < (ysize * zsize); k++) {
			dstream >> offs;
			if ((it = map.find(offs)) != end)
				map.insert(offs, *it + 1);
			else
				map[offs] = 0;
		}
		for (k = 0, it = map.begin(); it != end; ++it)
			k += *it;

		if (k)
			appendItem(group, "SharedRows", QString("%1%").arg(k * 100.0
					/ (ysize * zsize), 0, 'f', 1));
		else
			appendItem(group, "SharedRows", i18nc("SharedRows", "None"));
	} else
		appendItem(group, "Compression", i18nc("Compression", "Unknown"));


	group = appendGroup(info, "Comment");
	appendItem(group, "ImageName", imagename);

	file.close();
	return true;
}


bool KRgbPlugin::writeInfo(const KFileMetaInfo& info) const
{
	QFile file(info.path());

	if (!file.open(QIODevice::WriteOnly)) {
		kDebug(7034) << "couldn't open " << QFile::encodeName(info.path());
		return false;
	}

	if (!file.seek(24)) {
		kDebug(7034) << "couldn't set offset";
		return false;
	}

	QDataStream dstream(&file);
	QString s = info["Comment"]["ImageName"].value().toString();
	s.truncate(79);

	int i;
	for (i = 0; i < s.length(); i++)
		dstream << quint8(s.toLatin1()[i]);
	for (; i < 80; i++)
		dstream << quint8(0);

	file.close();
	return true;
}


// restrict to 79 ASCII characters
QValidator* KRgbPlugin::createValidator(const QString&, const QString &,
		const QString &, QObject* parent, const char* name) const
{
	QRegExpValidator *val = new QRegExpValidator(QRegExp("[\x0020-\x007E]{79}"), parent);
	val->setObjectName(name);
	return val;
}


#include "kfile_rgb.moc"
