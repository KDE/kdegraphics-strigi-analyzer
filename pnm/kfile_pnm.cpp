/* This file is part of the KDE project
 * Copyright (C) 2003 Volker Krause <vkrause@kde.org>
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

#include "kfile_pnm.h"

#include <math.h>
#include <kgenericfactory.h>
#include <qfile.h>
#include <qtextstream.h>
#include <QSize>
static const char* formats[] = {
	I18N_NOOP("plain"),
	I18N_NOOP("raw")
};

typedef KGenericFactory<KPnmPlugin> PnmFactory;

K_EXPORT_COMPONENT_FACTORY(kfile_pnm, PnmFactory("kfile_pnm"))

KPnmPlugin::KPnmPlugin(QObject *parent, const QStringList &args) : KFilePlugin(parent, args) 
{
	makeMimeTypeInfo( "image/x-portable-bitmap" );
	makeMimeTypeInfo( "image/x-portable-graymap" );
	makeMimeTypeInfo( "image/x-portable-pixmap" );
}

void KPnmPlugin::makeMimeTypeInfo(const QString& mimetype)
{
	KFileMimeTypeInfo* info = addMimeTypeInfo( mimetype );

	KFileMimeTypeInfo::GroupInfo* group = 0;
	KFileMimeTypeInfo::ItemInfo* item = 0;

	group = addGroupInfo(info, "General", i18n("General"));

	addItemInfo(group, "Format", i18n("Format"), QVariant::String);

	item = addItemInfo(group, "Dimensions", i18n("Dimensions"), QVariant::Size);
	setUnit(item, KFileMimeTypeInfo::Pixels);

	item = addItemInfo(group, "BitDepth", i18n("Bit Depth"), QVariant::Int);
	setUnit(item, KFileMimeTypeInfo::BitsPerPixel);

	addItemInfo(group, "Comment", i18n("Comment"), QVariant::String);
}


bool KPnmPlugin::readInfo( KFileMetaInfo& info, uint /*what*/)
{
	QFile f(info.path());
	if(!f.open(QIODevice::ReadOnly))
		return false;
	if(f.size() <= 2)
		return false;
	QTextStream stream(&f);

	char c;
	stream >> c;
	if(c != 'P')
		return false;

	// image format and type
	int n;
	stream >> n;
	int format = (n - 1) / 3;
	if(format != 0 && format != 1)
		return false;
	int type = (n - 1) % 3;

	QString comments, buffer;
	while(!stream.atEnd()) {
		stream >> c;
		// comment
		if( c == '#') {
			buffer = stream.readLine();
			comments += buffer.trimmed();
			comments += '\n';
		}
		// image size
		// unfortunately Qt doesn't have some kind of push-back method for 
		// QTextStream, so we need to manually decode the first part of the 
		// image size.
		if( c >= '0' && c <= '9' ) {
			buffer = "";
			QChar tmp(c);
			while(!stream.atEnd() && tmp.isDigit()) {
				buffer += tmp;
				stream >> tmp;
			}
			break;
		}
	}

	// image size
	int x, y, max;
	x = buffer.toInt();
	stream >> y;
	stream >> max;

	// bit depth
	// PNM doesn't provide a conventional bit depth value, only the highest value
	// per pixel. To have comparable values with other formats, we convert it to
	// a normal bit depth value.
	int bpp = 1;
	if(type != 0) 
		bpp = (int)ceil(log((double)max) / log(2.0));
	if(type == 2) 
		bpp *= 3;

	KFileMetaInfoGroup group = appendGroup(info, "General");
	appendItem(group, "Format", formats[format]);
	appendItem(group, "Dimensions", QSize(x, y));
	if(!comments.isEmpty())
		appendItem(group, "Comment", comments.trimmed());
	appendItem(group, "BitDepth", bpp);

	f.close();
	return true;
}

#include "kfile_pnm.moc"
