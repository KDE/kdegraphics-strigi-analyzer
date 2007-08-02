// -*- C++;indent-tabs-mode: t; tab-width: 4; c-basic-offset: 4; -*-
/* This file is part of the KDE project
 * Copyright (C) 2003 <bradh@frogmouth.net>
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
 *  $Id$
 */

#include <ImfStandardAttributes.h>
#include <ImathBox.h>
#include <ImfInputFile.h>
#include <ImfBoxAttribute.h>
#include <ImfChannelListAttribute.h>
#include <ImfCompressionAttribute.h>
#include <ImfFloatAttribute.h>
#include <ImfIntAttribute.h>
#include <ImfLineOrderAttribute.h>
#include <ImfStringAttribute.h>
#include <ImfVecAttribute.h>
#include <ImfPreviewImage.h>
#include <ImfVersion.h>

#include <iostream>

#include <stdlib.h>
#include <string>

#include <kurl.h>
#include <k3process.h>
#include <klocale.h>
#include <kgenericfactory.h>
#include <kdebug.h>

#include <qfile.h>
#include <qdatetime.h>
#include <q3dict.h>
#include <qvalidator.h>
#include <qimage.h>


#include "kfile_exr.h"
using namespace Imf;

typedef KGenericFactory<KExrPlugin> ExrFactory;

K_EXPORT_COMPONENT_FACTORY(kfile_exr, ExrFactory("kfile_exr"))

KExrPlugin::KExrPlugin(QObject *parent, 
                       const QStringList &args)
    : KFilePlugin(parent, args)
{
    // set up our mime type
    KFileMimeTypeInfo* info = addMimeTypeInfo( "image/x-exr" );

    KFileMimeTypeInfo::GroupInfo* group = 0;
	KFileMimeTypeInfo::ItemInfo* item;

	// info group
    group = addGroupInfo( info, "Info", i18n("Information") );
    addItemInfo( group, "Version", i18n("Format Version"), QVariant::Int );
    addItemInfo( group, "Tiled image", i18n("Tiled Image"), QVariant::String );
	item = addItemInfo( group, "Dimensions", i18n("Dimensions"), QVariant::Size );
	setHint( item, KFileMimeTypeInfo::Size );
	setUnit( item, KFileMimeTypeInfo::Pixels );
	item = addItemInfo( group, "ThumbnailDimensions",
						i18n("Thumbnail Dimensions"), QVariant::Size );
	setHint( item, KFileMimeTypeInfo::Size );
	setUnit( item, KFileMimeTypeInfo::Pixels );
	addItemInfo( group, "Comment", i18n("Comment"), QVariant::String );
	item = addItemInfo( group, "Thumbnail", i18n("Thumbnail"), QVariant::Image );
	setHint( item, KFileMimeTypeInfo::Thumbnail );

	// standard attributes group
    group = addGroupInfo( info, "Standard", i18n("Standard Attributes") );
    addItemInfo( group, "Owner", i18n("Owner"), QVariant::String );
	addItemInfo( group, "Comments", i18n("Comments"), QVariant::String );
	addItemInfo( group, "Capture Date", i18n("Capture Date"), QVariant::String );
	item = addItemInfo( group, "UTC Offset", i18n("UTC Offset"), QVariant::String );
	item = addItemInfo( group, "Exposure time", i18n("Exposure Time"), QVariant::Double);
	setUnit( item, KFileMimeTypeInfo::Seconds );
	item = addItemInfo( group, "Focus", i18n("Focus"), QVariant::Double);
	setSuffix( item, i18nc("Metres", "m") );
	item = addItemInfo( group, "X Density", i18n("X Density"), QVariant::Double);
	setSuffix( item, i18nc("Pixels Per Inch", " ppi") );
	item = addItemInfo( group, "White luminance", i18n("White Luminance"), QVariant::Double);
	setSuffix( item, i18nc("Candelas per square metre", " Nits") );
	addItemInfo( group, "Longitude", i18n("Longitude"), QVariant::String );
	addItemInfo( group, "Latitude", i18n("Latitude"), QVariant::String );
	item = 	addItemInfo( group, "Altitude", i18n("Altitude"), QVariant::String );
	setSuffix( item, i18nc("Metres", "m") );
	addItemInfo( group, "ISO speed", i18n("ISO Speed"), QVariant::Double );
	addItemInfo( group, "Aperture", i18n("Aperture"), QVariant::Double );

    // channel group
    group = addGroupInfo( info, "Channel", i18n("Channels") );
    addItemInfo( group, "A", i18n("A"), QVariant::String );
    addItemInfo( group, "R", i18n("R"), QVariant::String );
    addItemInfo( group, "G", i18n("G"), QVariant::String );
    addItemInfo( group, "B", i18n("B"), QVariant::String );
    addItemInfo( group, "Z", i18n("Z"), QVariant::String );
    addItemInfo( group, "NX", i18n("NX"), QVariant::String );
    addItemInfo( group, "NY", i18n("NY"), QVariant::String );
    addItemInfo( group, "NZ", i18n("NZ"), QVariant::String );
    addItemInfo( group, "R", i18n("R"), QVariant::String );
    addItemInfo( group, "U", i18n("U"), QVariant::String );
    addItemInfo( group, "V", i18n("V"), QVariant::String );
    addItemInfo( group, "materialID", i18n("materialID"), QVariant::String );
    addItemInfo( group, "objectID", i18n("objectID"), QVariant::String );
    addItemInfo( group, "renderID", i18n("renderID"), QVariant::String );
    addItemInfo( group, "pixelCover", i18n("pixelCover"), QVariant::String );
    addItemInfo( group, "velX", i18n("velX"), QVariant::String );
    addItemInfo( group, "velY", i18n("velY"), QVariant::String );
    addItemInfo( group, "packedRGBA", i18n("packedRGBA"), QVariant::String );


    // technical group
    group = addGroupInfo( info, "Technical", i18n("Technical Details") );
    addItemInfo( group, "Compression", i18n("Compression"), QVariant::String );
    addItemInfo( group, "Line Order", i18n("Line Order"), QVariant::String );

    // 3dsMax group
	// This supports a special plugin for 3D Studio Max
    group = addGroupInfo( info, "3dsMax", i18n("3dsMax Details") );
    addItemInfo( group, "Local time", i18n("Local Time"), QVariant::String );
    addItemInfo( group, "System time", i18n("System Time"), QVariant::String );
    addItemInfo( group, "Plugin version", i18n("Plugin Version"), QVariant::String );
    addItemInfo( group, "EXR version", i18n("EXR Version"), QVariant::String );
    addItemInfo( group, "Computer name", i18n("Computer Name"), QVariant::String );
}

QString doType( PixelType pt )
{
    switch (pt)
    {
	case UINT:
		return QString("32-bit unsigned integer");
		break;
	case HALF:
		return QString("16-bit floating-point");
		break;
	case FLOAT:
		return QString("32-bit floating-point");
		break;
    default:
		return QString();
		break;
    }
}

bool KExrPlugin::readInfo( KFileMetaInfo& info, uint what)
{
	try
	{
		InputFile in ( QFile::encodeName(info.path()) );
		const Header &h = in.header();

		KFileMetaInfoGroup infogroup = appendGroup(info, "Info");
		KFileMetaInfoGroup stdgroup = appendGroup(info, "Standard");
		KFileMetaInfoGroup channelgroup = appendGroup(info, "Channel");
		KFileMetaInfoGroup techgroup = appendGroup(info, "Technical");
		KFileMetaInfoGroup threedsmaxgroup = appendGroup(info, "3dsMax");

		appendItem( infogroup, "Version", getVersion(in.version()) );
		if (isTiled(in.version())) {
			appendItem( infogroup, "Tiled image", "yes" );
		} else {
			appendItem( infogroup, "Tiled image", "no" );
		}

		Imath::Box2i dw = h.dataWindow();
		appendItem( infogroup, "Dimensions", QSize( (dw.max.x - dw.min.x + 1 ),
													(dw.max.y - dw.min.y + 1 ) ) );

		if ( h.hasPreviewImage() ) {
			const PreviewImage &preview = in.header().previewImage();
			appendItem( infogroup, "ThumbnailDimensions", QSize(preview.width(), preview.height()) );
			QImage qpreview(preview.width(), preview.height(), 32, 0, QImage::BigEndian);
			for ( unsigned int y=0; y < preview.height(); y++ ) {
				for ( unsigned int x=0; x < preview.width(); x++ ) {
					const PreviewRgba &q = preview.pixels()[x+(y*preview.width())];
					qpreview.setPixel( x, y, qRgba(q.r, q.g, q.b, q.a) );
				}
			}
			appendItem( infogroup, "Thumbnail", qpreview);
		}

		const StringAttribute *commentSA = h.findTypedAttribute <StringAttribute> ("comment");
		if (commentSA) {
			std::string commentString = commentSA->value();
			QString qcommentString(commentString.data());
			qcommentString.resize(commentString.size());
			appendItem( infogroup, "Comment", qcommentString );
		}

		// Standard Attributes we are interested in and can
		// meaningfully represent.
		if ( hasComments(h) ) {
			std::string commentsString = comments(h);
			QString qcommentsString(commentsString.data());
			qcommentsString.resize(commentsString.size());
			appendItem( stdgroup, "Comments", qcommentsString );
		}
		if ( hasOwner(h) ) {
			std::string ownerString = owner(h);
			QString qownerString(ownerString.data());
			qownerString.resize(ownerString.size());
			appendItem( stdgroup, "Owner", qownerString );
		}
		if ( hasCapDate(h) ) {
			std::string capDateString = capDate(h);
			QString qcapDateString(capDateString.data());
			qcapDateString.resize(capDateString.size());
			appendItem( stdgroup, "Capture Date", qcapDateString );
		}
		if ( hasutcOffset(h) ) {
			QString UTCOffset;
			if (utcOffset(h)>0.0) {
				UTCOffset.append(QString("%1").arg(utcOffset(h)/3600, 0, 'f', 1));
				UTCOffset.append(" hours behind UTC");
			} else {
				UTCOffset.append(QString("%1").arg(-1.0*utcOffset(h)/3600, 0, 'f', 1));
				UTCOffset.append(" hours ahead of UTC");
			}
			appendItem( stdgroup, "UTC Offset", UTCOffset); 
		}
		if ( hasExpTime(h) ) {
			double exposureTime = expTime(h);
			appendItem( stdgroup, "Exposure time", exposureTime );
		}
		if ( hasFocus(h) ) {
			double focusDistance = focus(h);
			appendItem( stdgroup, "Focus", focusDistance );
		}
		if ( hasXDensity(h) ) {
			double XDensity = xDensity(h);
			appendItem( stdgroup, "X Density", XDensity );
		}
		if ( hasWhiteLuminance(h) ) {
			double WhiteLuminance = whiteLuminance(h);
			appendItem( stdgroup, "White luminance", WhiteLuminance );
		}
		if ( hasLongitude(h) ) {
			QString Longitude;
			if (longitude(h)<0.0) {
				Longitude.append(QString("%1").arg(-1.0*longitude(h),0,'f',3));
				Longitude.append(" deg West");
			} else {
				Longitude.append(QString("%1").arg(longitude(h),0,'f',3));
				Longitude.append(" deg East");
			}
			appendItem( stdgroup, "Longitude", Longitude);
		}
		if ( hasLatitude(h) ) {
			QString Latitude;
			if (latitude(h)<0.0) {
				Latitude.append(QString("%1").arg(-1.0*latitude(h),0,'f',3));
				Latitude.append(" deg South");
			} else {
				Latitude.append(QString("%1").arg(latitude(h),0,'f',3));
				Latitude.append(" deg North");
			}
			appendItem( stdgroup, "Latitude", Latitude );
		}
		if ( hasAltitude(h) ) {
			double Altitude = altitude(h);
			appendItem( stdgroup, "Altitude", QString("%1").arg(Altitude,0,'f',1) );
		}
		if ( hasIsoSpeed(h) ) {
			double IsoSpeed = isoSpeed(h);
			appendItem( stdgroup, "ISO speed", IsoSpeed );
		}
		if ( hasAperture(h) ) {
			double Aperture = aperture(h);
			appendItem( stdgroup, "Aperture", Aperture );
		}

	    for (Header::ConstIterator i = h.begin(); i != h.end(); ++i) {
			const Attribute *a = &i.attribute();
			
			if (const CompressionAttribute *ta = dynamic_cast <const CompressionAttribute *> (a)) {
				switch ( ta->value() )
					{
					case NO_COMPRESSION:
						appendItem( techgroup, "Compression", i18n("No compression"));
						break;
					case RLE_COMPRESSION:
						appendItem( techgroup, "Compression", i18n("Run Length Encoding"));
						break;
					case ZIPS_COMPRESSION:
						appendItem( techgroup, "Compression", i18n("zip, individual scanlines"));
						break;
					case ZIP_COMPRESSION:
						appendItem( techgroup, "Compression", i18n("zip, multi-scanline blocks"));
						break;
					case PIZ_COMPRESSION:
						appendItem( techgroup, "Compression", i18n("piz compression"));
						break;
					default:
						break;
					}
			} else if (const LineOrderAttribute *ta = dynamic_cast <const LineOrderAttribute *> (a)) {
				switch (ta->value())
					{
					case INCREASING_Y:
						appendItem( techgroup, "Line Order", i18n("increasing Y"));
						break;
					case DECREASING_Y:
						appendItem( techgroup, "Line Order", i18n("decreasing Y"));
						break;
					default:
						break;
					};
			} else if (const ChannelListAttribute *ta = dynamic_cast <const ChannelListAttribute *> (a)) {
				
			    for (ChannelList::ConstIterator i = ta->value().begin(); i != ta->value().end(); ++i)
					{
						appendItem( channelgroup, i.name(), doType(i.channel().type) );
					}
			}
		}

		// This section deals with some special case stuff for a 3D Studio Max
		// plugin from Splutterfish. The weird construction is an
		// attempt to to deal with class conversion. C++ string handling
		// without Qt is a pain...
		const StringAttribute *ver3DSM = h.findTypedAttribute <StringAttribute> ("version3dsMax");
		if (ver3DSM) {
			std::string ver3DSMstring = ver3DSM->value();
			QString qver3DSMstring(ver3DSMstring.data());
			qver3DSMstring.resize(ver3DSMstring.size());
			appendItem( threedsmaxgroup, "Plugin version", qver3DSMstring );
		}
		const StringAttribute *verEXR = h.findTypedAttribute <StringAttribute> ("versionEXR");
		if (verEXR) {
			std::string verEXRstring = verEXR->value();
			QString qverEXRstring(verEXRstring.data());
			qverEXRstring.resize(verEXRstring.size());
			appendItem( threedsmaxgroup, "EXR version", QString( verEXRstring.data() ) );
		}
		const StringAttribute *localTime = h.findTypedAttribute <StringAttribute> ("localTime");
		if (localTime) {
			std::string localTimeString = localTime->value();
			QString qlocalTimeString(localTimeString.data());
			qlocalTimeString.resize(localTimeString.size());
			appendItem( threedsmaxgroup, "Local time", qlocalTimeString );
		}
		const StringAttribute *systemTime = h.findTypedAttribute <StringAttribute> ("systemTime");
		if (systemTime) {
			std::string systemTimeString = systemTime->value();
			QString qsystemTimeString(systemTimeString.data());
			qsystemTimeString.resize(systemTimeString.size());
			appendItem( threedsmaxgroup, "System time", qsystemTimeString );
		}
		const StringAttribute *computerName = h.findTypedAttribute <StringAttribute> ("computerName");
		if (computerName) {
			std::string computerNameString = computerName->value();
			QString qcomputerNameString(computerNameString.data());
			qcomputerNameString.resize(computerNameString.size());
			appendItem( threedsmaxgroup, "Computer name", qcomputerNameString );
		}

		return true;
	}
	catch (const std::exception &e)
	{
		kDebug(0) << e.what();
	    return false;
	}
}

#include "kfile_exr.moc"
