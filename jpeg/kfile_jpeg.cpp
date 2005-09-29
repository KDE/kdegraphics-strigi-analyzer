/* This file is part of the KDE project
 * Copyright (C) 2002 Frank Pieczynski <pieczy@knuut.de>,
 *               2002 Carsten Pfeiffer <pfeiffer@kde.org>
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
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
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

#define EXIFGROUP "Jpeg EXIF Data"

typedef KGenericFactory<KJpegPlugin> JpegFactory;

K_EXPORT_COMPONENT_FACTORY(kfile_jpeg, JpegFactory("kfile_jpeg"))

KJpegPlugin::KJpegPlugin(QObject *parent, const char *name,
                       const QStringList &args )
    : KFilePlugin(parent, name, args)
{
  kdDebug(7034) << "jpeg plugin\n";

  //
  // define all possible meta info items
  //
  KFileMimeTypeInfo *info = addMimeTypeInfo("image/jpeg");
  KFileMimeTypeInfo::GroupInfo *exifGroup = addGroupInfo( info, EXIFGROUP,
                                                          i18n("JPEG Exif") );
  KFileMimeTypeInfo::ItemInfo* item;

  item = addItemInfo( exifGroup, "Comment", i18n("Comment"), QVariant::String);
  setAttributes( item,
                 KFileMimeTypeInfo::Modifiable |
                 KFileMimeTypeInfo::Addable |
                 KFileMimeTypeInfo::MultiLine );

  item = addItemInfo( exifGroup, "Manufacturer", i18n("Camera Manufacturer"),
                      QVariant::String );

  item = addItemInfo( exifGroup, "Model", i18n("Camera Model"),
                      QVariant::String );

  item = addItemInfo( exifGroup, "Date/time", i18n("Date/Time"),
                      QVariant::DateTime );

  item = addItemInfo( exifGroup, "CreationDate", i18n("Creation Date"),
                      QVariant::Date );

  item = addItemInfo( exifGroup, "CreationTime", i18n("Creation Time"),
                      QVariant::Time );

  item = addItemInfo( exifGroup, "Dimensions", i18n("Dimensions"),
                      QVariant::Size );
  setHint( item, KFileMimeTypeInfo::Size );
  setUnit( item, KFileMimeTypeInfo::Pixels );

  item = addItemInfo( exifGroup, "Orientation", i18n("Orientation"),
                      QVariant::Int );

  item = addItemInfo( exifGroup, "ColorMode", i18n("Color Mode"),
                      QVariant::String );

  item = addItemInfo( exifGroup, "Flash used", i18n("Flash Used"),
                      QVariant::String );
  item = addItemInfo( exifGroup, "Focal length", i18n("Focal Length"),
                      QVariant::String );
  setUnit( item, KFileMimeTypeInfo::Millimeters );

  item = addItemInfo( exifGroup, "35mm equivalent", i18n("35mm Equivalent"),
                      QVariant::Int );
  setUnit( item, KFileMimeTypeInfo::Millimeters );

  item = addItemInfo( exifGroup, "CCD width", i18n("CCD Width"),
                      QVariant::String );
  setUnit( item, KFileMimeTypeInfo::Millimeters );

  item = addItemInfo( exifGroup, "Exposure time", i18n("Exposure Time"),
                      QVariant::String );
  setHint( item, KFileMimeTypeInfo::Seconds );

  item = addItemInfo( exifGroup, "Aperture", i18n("Aperture"),
                      QVariant::String );

  item = addItemInfo( exifGroup, "Focus dist.", i18n("Focus Dist."),
                      QVariant::String );

  item = addItemInfo( exifGroup, "Exposure bias", i18n("Exposure Bias"),
                      QVariant::String );

  item = addItemInfo( exifGroup, "Whitebalance", i18n("Whitebalance"),
                      QVariant::String );

  item = addItemInfo( exifGroup, "Metering mode", i18n("Metering Mode"),
                      QVariant::String );

  item = addItemInfo( exifGroup, "Exposure", i18n("Exposure"),
                      QVariant::String );

  item = addItemInfo( exifGroup, "ISO equiv.", i18n("ISO Equiv."),
                      QVariant::String );

  item = addItemInfo( exifGroup, "JPEG quality", i18n("JPEG Quality"),
                      QVariant::String );

  item = addItemInfo( exifGroup, "User comment", i18n("User Comment"),
                      QVariant::String );
  setHint(item,  KFileMimeTypeInfo::Description);

  item = addItemInfo( exifGroup, "JPEG process", i18n("JPEG Process"),
                      QVariant::String );

  item = addItemInfo( exifGroup, "Thumbnail", i18n("Thumbnail"),
                      QVariant::Image );
  setHint( item, KFileMimeTypeInfo::Thumbnail );

//  ###
//   exifGroup.setSupportsVariableKeys(true);
}

QValidator* KJpegPlugin::createValidator(const KFileMetaInfoItem& /*item*/,
                                        QObject */*parent*/,
                                         const char */*name*/ ) const
{
    // no need to return a validator that validates everything as OK :)
//     if (item.isEditable())
//         return new QRegExpValidator(QRegExp(".*"), parent, name);
//     else
    return 0L;
}

bool KJpegPlugin::writeInfo( const KFileMetaInfo& info ) const
{
    QString comment = info[EXIFGROUP].value("Comment").toString();
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
    if( safe_copy_and_modify( QFile::encodeName( path ), comment.utf8() ) ) {
            return false;
        }
    return true;
}

bool KJpegPlugin::readInfo( KFileMetaInfo& info, uint what )
{
    const QString path( info.path() );
    if ( path.isEmpty() ) // remote file
        return false;

    QString tag;
    ExifData ImageInfo;

    // parse the jpeg file now
    try {
        if ( !ImageInfo.scan(info.path()) ) {
            kdDebug(7034) << "Not a JPEG file!\n";
            return false;
        }
    }
    catch (FatalError& e) { // malformed exif data?
        kdDebug(7034) << "Exception caught while parsing Exif data of: " << info.path() << endl;
        e.debug_print();
        return false;
    }

    KFileMetaInfoGroup exifGroup = appendGroup( info, EXIFGROUP );

    tag = ImageInfo.getComment();
    if ( tag.length() ) {
        kdDebug(7034) << "exif inserting Comment: " << tag << "\n";
        appendItem( exifGroup, "Comment", tag );
    } else {
        appendItem( exifGroup, "Comment", tag );    // So user can add new comment
    }

    tag = ImageInfo.getCameraMake();
    if (tag.length())
    	appendItem( exifGroup, "Manufacturer", tag );

    tag = ImageInfo.getCameraModel();
    if (tag.length())
        appendItem( exifGroup, "Model", tag );

    tag = ImageInfo.getDateTime();
    if (tag.length()){
        QDateTime dt = parseDateTime( tag.stripWhiteSpace() );
        if ( dt.isValid() ) {
            appendItem( exifGroup, "Date/time", dt );
            appendItem( exifGroup, "CreationDate", dt.date() );
            appendItem( exifGroup, "CreationTime", dt.time() );
        }
    }

    appendItem( exifGroup,"Dimensions", QSize( ImageInfo.getWidth(),
                                               ImageInfo.getHeight() ) );

    if ( ImageInfo.getOrientation() )
        appendItem( exifGroup, "Orientation", ImageInfo.getOrientation() );

    appendItem( exifGroup, "ColorMode", ImageInfo.getIsColor() ?
                i18n("Color") : i18n("Black and white") );

    int flashUsed = ImageInfo.getFlashUsed(); // -1, <set>
    if ( flashUsed >= 0 ) {
	 QString flash = i18n("Flash", "(unknown)");
         switch ( flashUsed ) {
         case 0: flash = i18n("Flash", "No");
             break;
         case 1:
         case 5:
         case 7:
             flash = i18n("Flash", "Fired");
             break;
         case 9:
         case 13:
         case 15:
             flash = i18n( "Flash", "Fill Fired" );
             break;
         case 16:
             flash = i18n( "Flash", "Off" );
             break;
         case 24:
             flash = i18n( "Flash", "Auto Off" );
             break;
         case 25:
         case 29:
         case 31:
             flash = i18n( "Flash", "Auto Fired" );
             break;
         case 32:
             flash = i18n( "Flash", "Not Available" );
             break;
         default:
             break;
        }
        appendItem( exifGroup, "Flash used",
                    flash );
    }

    if (ImageInfo.getFocalLength()){
    	appendItem( exifGroup, "Focal length",
                    QString().sprintf("%4.1f", ImageInfo.getFocalLength()) );

        if (ImageInfo.getCCDWidth()){
            appendItem( exifGroup, "35mm equivalent",
                        (int)(ImageInfo.getFocalLength()/ImageInfo.getCCDWidth()*35 + 0.5) );
	}
    }

    if (ImageInfo.getCCDWidth()){
	appendItem( exifGroup, "CCD width",
                    QString().sprintf("%4.2f", ImageInfo.getCCDWidth()) );
    }

    if (ImageInfo.getExposureTime()){
        tag=QString().sprintf("%6.3f", ImageInfo.getExposureTime());
        float exposureTime = ImageInfo.getExposureTime();
	if (exposureTime > 0 && exposureTime <= 0.5){
            tag+=QString().sprintf(" (1/%d)", (int)(0.5 + 1/exposureTime) );
	}
	appendItem( exifGroup, "Exposure time", tag );
    }

    if (ImageInfo.getApertureFNumber()){
	appendItem( exifGroup, "Aperture",
                    QString().sprintf("f/%3.1f",
                                      (double)ImageInfo.getApertureFNumber()));
    }

    if (ImageInfo.getDistance()){
        if (ImageInfo.getDistance() < 0){
	    tag=i18n("Infinite");
        }else{
	    tag=QString().sprintf("%5.2fm",(double)ImageInfo.getDistance());
        }
    	appendItem( exifGroup, "Focus dist.", tag );
    }

    if (ImageInfo.getExposureBias()){
	appendItem( exifGroup, "Exposure bias",
                    QString().sprintf("%4.2f",
                                      (double)ImageInfo.getExposureBias()) );
    }

    if (ImageInfo.getWhitebalance() != -1){
        switch(ImageInfo.getWhitebalance()) {
	case 0:
	    tag=i18n("Unknown");
	    break;
	case 1:
	    tag=i18n("Daylight");
	    break;
	case 2:
	    tag=i18n("Fluorescent");
	    break;
	case 3:
	    //tag=i18n("incandescent");
	    tag=i18n("Tungsten");
	    break;
	case 17:
	    tag=i18n("Standard light A");
	    break;
	case 18:
	    tag=i18n("Standard light B");
	    break;
	case 19:
	    tag=i18n("Standard light C");
	    break;
	case 20:
	    tag=i18n("D55");
	    break;
	case 21:
	    tag=i18n("D65");
	    break;
	case 22:
	    tag=i18n("D75");
	    break;
	case 255:
	    tag=i18n("Other");
	    break;
	default:
            //23 to 254 = reserved
	    tag=i18n("Unknown");
	}
  	appendItem( exifGroup, "Whitebalance", tag );
    }

    if (ImageInfo.getMeteringMode() != -1){
        switch(ImageInfo.getMeteringMode()) {
	case 0:
	    tag=i18n("Unknown");
	    break;
	case 1:
	    tag=i18n("Average");
	    break;
	case 2:
	    tag=i18n("Center weighted average");
	    break;
	case 3:
	    tag=i18n("Spot");
	    break;
	case 4:
	    tag=i18n("MultiSpot");
	    break;
	case 5:
	    tag=i18n("Pattern");
	    break;
	case 6:
	    tag=i18n("Partial");
	    break;
	case 255:
	    tag=i18n("Other");
	    break;
	default:
	    // 7 to 254 = reserved
	    tag=i18n("Unknown");
	}
	appendItem( exifGroup, "Metering mode", tag );
    }

    if (ImageInfo.getExposureProgram()){
        switch(ImageInfo.getExposureProgram()) {
	case 0:
	    tag=i18n("Not defined");
	    break;
	case 1:
	    tag=i18n("Manual");
	    break;
	case 2:
	    tag=i18n("Normal program");
	    break;
	case 3:
	    tag=i18n("Aperture priority");
	    break;
	case 4:
	    tag=i18n("Shutter priority");
	    break;
	case 5:
	    tag=i18n("Creative program\n(biased toward fast shutter speed)");
	    break;
	case 6:
	    tag=i18n("Action program\n(biased toward fast shutter speed)");
	    break;
	case 7:
	    tag=i18n("Portrait mode\n(for closeup photos with the background out of focus)");
	    break;
	case 8:
	    tag=i18n("Landscape mode\n(for landscape photos with the background in focus)");
	    break;
	default:
	    // 9 to 255 = reserved
	    tag=i18n("Unknown");
	}
	appendItem( exifGroup, "Exposure", tag );
    }

    if (ImageInfo.getISOequivalent()){
	appendItem( exifGroup, "ISO equiv.",
                    QString().sprintf("%2d",
                                      (int)ImageInfo.getISOequivalent()) );
    }

    if (ImageInfo.getCompressionLevel()){
	switch(ImageInfo.getCompressionLevel()) {
	case 1:
	    tag=i18n("Basic");
            break;
	case 2:
	    tag=i18n("Normal");
	    break;
        case 4:
	    tag=i18n("Fine");
	    break;
	default:
	    tag=i18n("Unknown");
	}
	appendItem( exifGroup, "JPEG quality", tag );
    }

    tag = ImageInfo.getUserComment();
    if (tag.length()){
	appendItem( exifGroup, "EXIF comment", tag );
    }

    int a;
    for (a=0;;a++){
        if (ProcessTable[a].Tag == ImageInfo.getProcess() || ProcessTable[a].Tag == 0){
    	    appendItem( exifGroup, "JPEG process",
                        QString::fromUtf8( ProcessTable[a].Desc) );
            break;
        }
    }

    if ( what & KFileMetaInfo::Thumbnail && !ImageInfo.isNullThumbnail() ){
        appendItem( exifGroup, "Thumbnail", ImageInfo.getThumbnail() );
    }

  return true;
}

// format of the string is:
// YYYY:MM:DD HH:MM:SS
QDateTime KJpegPlugin::parseDateTime( const QString& string )
{
    QDateTime dt;
    if ( string.length() != 19 )
        return dt;

    QString year    = string.left( 4 );
    QString month   = string.mid( 5, 2 );
    QString day     = string.mid( 8, 2 );
    QString hour    = string.mid( 11, 2 );
    QString minute  = string.mid( 14, 2 );
    QString seconds = string.mid( 17, 2 );

    bool ok;
    bool allOk = true;
    int y  = year.toInt( &ok );
    allOk &= ok;

    int mo = month.toInt( &ok );
    allOk &= ok;

    int d  = day.toInt( &ok );
    allOk &= ok;

    int h  = hour.toInt( &ok );
    allOk &= ok;

    int mi = minute.toInt( &ok );
    allOk &= ok;

    int s  = seconds.toInt( &ok );
    allOk &= ok;

    if ( allOk ) {
        dt.setDate( QDate( y, mo, d ) );
        dt.setTime( QTime( h, mi, s ) );
    }

    return dt;
}

#include "kfile_jpeg.moc"
