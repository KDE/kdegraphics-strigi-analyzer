/* This file is part of the KDE project
 * Copyright (C) 2002 Frank Pieczynski
 * based on the jhead tool of Matthias Wandel (see below)
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


#include <stdlib.h>
#include "kfile_jpeg.h"

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

#include "exif.h"

typedef KGenericFactory<KJpegPlugin> JpegFactory;

K_EXPORT_COMPONENT_FACTORY(kfile_jpeg, JpegFactory("kfile_jpeg"));

KJpegPlugin::KJpegPlugin(QObject *parent, const char *name,
                       const QStringList &preferredItems)
    : KFilePlugin(parent, name, preferredItems)
{
    kdDebug(7034) << "jpeg plugin\n";
}

QValidator* KJpegPlugin::createValidator(const KFileMetaInfoItem& item,
                                        QObject *parent,
                                        const char *name ) const
{
    if (item.isEditable())
        return new QRegExpValidator(QRegExp(".*"), parent, name);
    else
        return 0L;
}

bool KJpegPlugin::writeInfo( const KFileMetaInfo::Internal& info ) const
{
    QString comment = info["Comment"].value().toString();
    QString path    = info.path();

    kdDebug(7034) << "exif writeInfo: " << info.path() << " \"" << comment << "\"\n";

    /*
        Do a strictly safe insertion of the comment:

        Scan original to verify it's a proper jpeg
        Open a unique temporary file in this directory
        Write temporary, replacing all COM blocks with this one.
        Scan temporary, to verify it's a proper jpeg
        Rename original to another unique name
        Rename temporary to original
        Unlink original
    */
    /*
        The jpeg standard does not regulate the contents of the COM block.
        I'm assuming the best thing to do here is write as unicode utf-8,
        which is fully backwards compatible with readers expecting ascii.
        Readers expecting a national character set are out of luck...
    */
    if( safe_copy_and_modify( path.latin1(), comment.utf8() ) ) {
            return false;
        }
    return true;
}

bool KJpegPlugin::readInfo( KFileMetaInfo::Internal& info )
{
    QString tag;

    ExifData ImageInfo;

    // parse the jpeg file now
    if (ImageInfo.scan(info.path()) == false) {
        kdDebug(7034) << "Not JPEG file!\n";
        return false;
    }

    // I insert a comment always, so that the user can edit it
    tag = ImageInfo.getComment();
    kdDebug(7034) << "exif inserting Comment: " << tag << "\n";
    info.insert(KFileMetaInfoItem("Comment", i18n("Comment"),
	QVariant(tag), true));

    tag = ImageInfo.getCameraMake();
    if (tag.length()) {
    	info.insert(KFileMetaInfoItem("Camera make", i18n("Camera make"),
	    QVariant(tag), false));
    }

    tag = ImageInfo.getCameraModel();
    if (tag.length()){
	info.insert(KFileMetaInfoItem("Camera model", i18n("Camera model"),
	    QVariant(tag), false));
    }

    tag = ImageInfo.getDateTime();
    if (tag.length()){
        info.insert(KFileMetaInfoItem("Date/Time", i18n("Date/Time"),
	    QVariant(tag), false));
    }

    info.insert(KFileMetaInfoItem("Resolution", i18n("Resolution"),
		QVariant(QString("%1 x %2").arg(ImageInfo.getWidth()).arg(ImageInfo.getHeight())), false,
		QString::null, i18n("pixels")));

    if (ImageInfo.getOrientation()){
	info.insert(KFileMetaInfoItem("Orientation", i18n("Orientation"),
	    QVariant(ImageInfo.getOrientation()), false));
    }

    if (ImageInfo.getIsColor() == 0){
    	info.insert(KFileMetaInfoItem("Color/bw", i18n("Color/bw"),
		QVariant(QString("Black and white")), false));
    }
    else {
    	info.insert(KFileMetaInfoItem("Color/bw", i18n("Color/bw"),
		QVariant(QString("Color")), false));
    }

    if (ImageInfo.getFlashUsed() >= 0){
        if (ImageInfo.getFlashUsed()) tag = i18n("Yes");
	else					tag = i18n("No");
	info.insert(KFileMetaInfoItem("Flash used", i18n("Flash used"),
	    QVariant(tag), false));
    }

    if (ImageInfo.getFocalLength()){
    	info.insert(KFileMetaInfoItem("Focal length", i18n("Focal length"),
		QVariant(QString().sprintf("%4.1f", (double)ImageInfo.getFocalLength())), false,
		QString::null, i18n("mm")));
	if (ImageInfo.getCCDWidth()){
    	    info.insert(KFileMetaInfoItem("35mm equivalent", i18n("35mm equivalent"),
			QVariant(QString().sprintf("%d",
			 (int)(ImageInfo.getFocalLength()/ImageInfo.getCCDWidth()*35 + 0.5))), false,
			QString::null, i18n("mm")));
	}
    }

    if (ImageInfo.getCCDWidth()){
	info.insert(KFileMetaInfoItem("CCD Width", i18n("CCD Width"),
		QVariant(QString().sprintf("%4.2f", (double)ImageInfo.getCCDWidth())), false,
		QString::null, i18n("mm")));
    }

    if (ImageInfo.getExposureTime()){
        tag=QString().sprintf("%6.3f", (double)ImageInfo.getExposureTime());
	if (ImageInfo.getExposureTime() <= 0.5){
		tag+=QString().sprintf(" (1/%d)", (int)(0.5 + 1/ImageInfo.getExposureTime()));
	}
	info.insert(KFileMetaInfoItem("Exposure time", i18n("Exposure time"),
		QVariant(tag), false,
		QString::null, i18n("sec")));
    }

    if (ImageInfo.getApertureFNumber()){
	info.insert(KFileMetaInfoItem("Aperture", i18n("Aperture"),
		QVariant(QString().sprintf("f/%3.1f",
		(double)ImageInfo.getApertureFNumber())), false));
    }

    if (ImageInfo.getDistance()){
        if (ImageInfo.getDistance() < 0){
	    tag=QString(i18n("Infinite"));
        }else{
	    tag=QString().sprintf("%5.2fm",(double)ImageInfo.getDistance());
        }
    	info.insert(KFileMetaInfoItem("Focus Dist.", i18n("Focus Dist."),
		QVariant(tag), false));
    }

    if (ImageInfo.getExposureBias()){
	info.insert(KFileMetaInfoItem("Exposure bias", i18n("Exposure bias"),
		QVariant(QString().sprintf("%4.2f", (double)ImageInfo.getExposureBias())), false));
    }

    if (ImageInfo.getWhitebalance() != -1){
        switch(ImageInfo.getWhitebalance()) {
	case 0:
	    tag=QString(i18n("unknown"));
	    break;
	case 1:
	    tag=QString(i18n("Daylight"));
	    break;
	case 2:
	    tag=QString(i18n("Fluorescent"));
	    break;
	case 3:
	    //tag=QString(i18n("incandescent"));
	    tag=QString(i18n("Tungsten"));
	    break;
	case 17:
	    tag=QString(i18n("Standard light A"));
	    break;
	case 18:
	    tag=QString(i18n("Standard light B"));
	    break;
	case 19:
	    tag=QString(i18n("Standard light C"));
	    break;
	case 20:
	    tag=QString(i18n("D55"));
	    break;
	case 21:
	    tag=QString(i18n("D65"));
	    break;
	case 22:
	    tag=QString(i18n("D75"));
	    break;
	case 255:
	    tag=QString(i18n("other"));
	    break;
	default:
            //23 to 254 = reserved
	    tag=QString(i18n("reserved"));
	}
  	info.insert(KFileMetaInfoItem("Whitebalance", i18n("Whitebalance"),
		QVariant(tag), false));
    }

    if (ImageInfo.getMeteringMode() != -1){
        switch(ImageInfo.getMeteringMode()) {
	case 0:
	    tag=QString(i18n("unknown"));
	    break;
	case 1:
	    tag=QString(i18n("Average"));
	    break;
	case 2:
	    tag=QString(i18n("CenterWeightedAverage"));
	    break;
	case 3:
	    tag=QString(i18n("Spot"));
	    break;
	case 4:
	    tag=QString(i18n("MultiSpot"));
	    break;
	case 5:
	    tag=QString(i18n("Pattern"));
	    break;
	case 6:
	    tag=QString(i18n("Partial"));
	    break;
	case 7:
	    tag=QString(i18n("reserved"));
	    break;
	case 255:
	    tag=QString(i18n("other"));
	    break;
	default:
	    // 7 to 254 = reserved
	    tag=QString(i18n("reserved"));
	}
	info.insert(KFileMetaInfoItem("Metering Mode", i18n("Metering Mode"),
		QVariant(tag), false));
    }

    if (ImageInfo.getExposureProgram()){
        switch(ImageInfo.getExposureProgram()) {
	case 0:
	    tag=QString(i18n("Not defined"));
	    break;
	case 1:
	    tag=QString(i18n("Manual"));
	    break;
	case 2:
	    tag=QString(i18n("Normal program"));
	    break;
	case 3:
	    tag=QString(i18n("Aperture priority"));
	    break;
	case 4:
	    tag=QString(i18n("Shutter priority"));
	    break;
	case 5:
	    tag=QString(i18n("Creative program\n(biased toward fast shutter speed)"));
	    break;
	case 6:
	    tag=QString(i18n("Action program\n(biased toward fast shutter speed)"));
	    break;
	case 7:
	    tag=QString(i18n("Portrait mode\n(for closeup photos with the background out of focus)"));
	    break;
	case 8:
	    tag=QString(i18n("Landscape mode\n(for landscape photos with the background in focus)"));
	    break;
	default:
	    // 9 to 255 = reserved
	    tag=QString(i18n("reserved"));
	}
	info.insert(KFileMetaInfoItem("Exposure", i18n("Exposure"),
		QVariant(tag), false));
    }

    if (ImageInfo.getISOequivalent()){
	info.insert(KFileMetaInfoItem("ISO equiv.", i18n("ISO equiv."),
		QVariant(QString().sprintf("%2d", (int)ImageInfo.getISOequivalent())), false));
    }

    if (ImageInfo.getCompressionLevel()){
	switch(ImageInfo.getCompressionLevel()) {
	case 1:
	    tag=QString(i18n("basic"));
            break;
	case 2:
	    tag=QString(i18n("normal"));
	    break;
        case 4:
	    tag=QString(i18n("fine"));
	    break;
	default:
	    tag=QString(i18n("unknown"));
	}
	info.insert(KFileMetaInfoItem("JPG Quality", i18n("JPG Quality"),
	    QVariant(tag), false));
    }

    tag = ImageInfo.getUserComment();
    if (tag.length()){
	info.insert(KFileMetaInfoItem("UserComment", i18n("UserComment"),
	    QVariant(tag), false));
    }

    int a;
    for (a=0;;a++){
        if (ProcessTable[a].Tag == ImageInfo.getProcess() || ProcessTable[a].Tag == 0){
    	    info.insert(KFileMetaInfoItem("JPEG Process", i18n("JPEG Process"),
		QVariant(QString().sprintf("%s", ProcessTable[a].Desc)), false));
            break;
        }
    }

    if (ImageInfo.isNullThumbnail() == false){
        info.insert(KFileMetaInfoItem("Thumbnail", i18n("Thumbnail"),
	    QVariant(ImageInfo.getThumbnail()), false));
    }

  QStringList supported;
  supported << "Comment" << "Resolution" << "Camera make" << "Camera model" << "Date/Time" <<
  "Color/bw" << "Flash used" << "Focal length" << "35mm equivalent" << "CCD Width" <<
   "Exposure time" << "Aperture" << "Focus Dist." << "ISO equiv." << "Exposure bias" <<
   "Whitebalance" << "Metering Mode" << "Exposure" << "JPG Quality" << "Orientation" <<
   "User Comment" << "JPEG Process" << "Thumbnail";
  info.setSupportedKeys(supported);
  info.setPreferredKeys(m_preferred);
  info.setSupportsVariableKeys(true);
  //DiscardData();
  return true;
}

#include "kfile_jpeg.moc"
