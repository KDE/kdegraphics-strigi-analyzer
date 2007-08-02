/* This file is part of the KDE project
 * Copyright (C) 2002 Nadeem Hasan <nhasan@kde.org>
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

#include "kfile_tiff.h"

#include <kgenericfactory.h>
#include <kdebug.h>

#include <qstringlist.h>
#include <qfile.h>
#include <qdatetime.h>
#include <qregexp.h>
#include <QSize>
#include <tiff.h>
#include <tiffio.h>

typedef KGenericFactory<KTiffPlugin> TiffFactory;

K_EXPORT_COMPONENT_FACTORY(kfile_tiff, TiffFactory("kfile_tiff"))

KTiffPlugin::KTiffPlugin(QObject *parent, 
        const QStringList &args) : KFilePlugin(parent, args)
{
    kDebug(7034) << "TIFF file meta info plugin";
    KFileMimeTypeInfo* info = addMimeTypeInfo( "image/tiff" );

    KFileMimeTypeInfo::GroupInfo* group =
            addGroupInfo(info, "General", i18n("General"));

    KFileMimeTypeInfo::ItemInfo* item;
    item = addItemInfo(group, "Description", i18n("Description"),
            QVariant::String);
    setHint(item, KFileMimeTypeInfo::Description);
    item = addItemInfo(group, "Copyright", i18n("Copyright"),
            QVariant::String);
    item = addItemInfo(group, "ColorMode", i18n("Color Mode"),
            QVariant::String);
    item = addItemInfo(group, "Dimensions", i18n("Dimensions"),
            QVariant::Size);
    setHint(item, KFileMimeTypeInfo::Size);
    setUnit(item, KFileMimeTypeInfo::Pixels);
    item = addItemInfo(group, "Resolution", i18n("Resolution"),
            QVariant::Size);
    setUnit(item, KFileMimeTypeInfo::DotsPerInch);
    item = addItemInfo(group, "BitDepth", i18n("Bit Depth"),
            QVariant::Int);
    setUnit(item, KFileMimeTypeInfo::BitsPerPixel);
    item = addItemInfo(group, "Compression", i18n("Compression"),
            QVariant::String);
    item = addItemInfo(group, "Software", i18n("Software"),
            QVariant::String);
    item = addItemInfo(group, "DateTime", i18n("Date/Time"),
            QVariant::DateTime);
    item = addItemInfo(group, "Artist", i18n("Artist"),
            QVariant::String);
    setHint(item, KFileMimeTypeInfo::Author);
    item = addItemInfo(group, "FaxPages", i18n("Fax Pages"),
            QVariant::Int);

    group = addGroupInfo(info, "Scanner", i18n("Scanner"));

    item = addItemInfo(group, "Make", i18n("Make"), QVariant::String);
    item = addItemInfo(group, "Model", i18n("Model"), QVariant::String);

    m_colorMode.setAutoDelete(true);
    m_imageCompression.setAutoDelete(true);

    m_colorMode.insert(PHOTOMETRIC_MINISWHITE,
                new QString(I18N_NOOP("Monochrome")));
    m_colorMode.insert(PHOTOMETRIC_MINISBLACK,
                new QString(I18N_NOOP("Monochrome")));
    m_colorMode.insert(PHOTOMETRIC_RGB,
                new QString(I18N_NOOP("RGB")));
    m_colorMode.insert(PHOTOMETRIC_PALETTE,
                new QString(I18N_NOOP("Palette color")));
    m_colorMode.insert(PHOTOMETRIC_MASK,
                new QString(I18N_NOOP("Transparency mask")));
    m_colorMode.insert(PHOTOMETRIC_SEPARATED,
                new QString(I18N_NOOP("Color separations")));
    m_colorMode.insert(PHOTOMETRIC_YCBCR,
                new QString(I18N_NOOP("YCbCr")));
    m_colorMode.insert(PHOTOMETRIC_CIELAB,
                new QString(I18N_NOOP("CIE Lab")));
#ifdef PHOTOMETRIC_ITULAB
    m_colorMode.insert(PHOTOMETRIC_ITULAB,
                new QString(I18N_NOOP("ITU Lab")));
#endif
    m_colorMode.insert(PHOTOMETRIC_LOGL,
                new QString(I18N_NOOP("LOGL")));
    m_colorMode.insert(PHOTOMETRIC_LOGLUV,
                new QString(I18N_NOOP("LOGLUV")));

    m_imageCompression.insert(COMPRESSION_NONE,
                new QString(I18N_NOOP("None")));
    m_imageCompression.insert(COMPRESSION_CCITTRLE,
                new QString(I18N_NOOP("RLE")));
    m_imageCompression.insert(COMPRESSION_CCITTFAX3,
                new QString(I18N_NOOP("G3 Fax")));
    m_imageCompression.insert(COMPRESSION_CCITTFAX4,
                new QString(I18N_NOOP("G4 Fax")));
    m_imageCompression.insert(COMPRESSION_LZW,
                new QString(I18N_NOOP("LZW")));
    m_imageCompression.insert(COMPRESSION_OJPEG,
                new QString(I18N_NOOP("JPEG")));
    m_imageCompression.insert(COMPRESSION_JPEG,
                new QString(I18N_NOOP("JPEG DCT")));
#ifdef COMPRESSION_ADOBE_DEFLATE
    m_imageCompression.insert(COMPRESSION_ADOBE_DEFLATE,
                new QString(I18N_NOOP("Adobe Deflate")));
#endif
    m_imageCompression.insert(COMPRESSION_NEXT,
                new QString(I18N_NOOP("NeXT 2-bit RLE")));
    m_imageCompression.insert(COMPRESSION_CCITTRLEW,
                new QString(I18N_NOOP("RLE Word")));
    m_imageCompression.insert(COMPRESSION_PACKBITS,
                new QString(I18N_NOOP("Packbits")));
    m_imageCompression.insert(COMPRESSION_THUNDERSCAN,
                new QString(I18N_NOOP("Thunderscan RLE")));
    m_imageCompression.insert(COMPRESSION_IT8CTPAD,
                new QString(I18N_NOOP("IT8 CT w/padding")));
    m_imageCompression.insert(COMPRESSION_IT8LW,
                new QString(I18N_NOOP("IT8 linework RLE")));
    m_imageCompression.insert(COMPRESSION_IT8MP,
                new QString(I18N_NOOP("IT8 monochrome")));
    m_imageCompression.insert(COMPRESSION_IT8BL,
                new QString(I18N_NOOP("IT8 binary lineart")));
    m_imageCompression.insert(COMPRESSION_PIXARFILM,
                new QString(I18N_NOOP("Pixar 10-bit LZW")));
    m_imageCompression.insert(COMPRESSION_PIXARLOG,
                new QString(I18N_NOOP("Pixar 11-bit ZIP")));
    m_imageCompression.insert(COMPRESSION_DEFLATE,
                new QString(I18N_NOOP("Pixar deflate")));
    m_imageCompression.insert(COMPRESSION_DCS,
                new QString(I18N_NOOP("Kodak DCS")));
    m_imageCompression.insert(COMPRESSION_JBIG,
                new QString(I18N_NOOP("ISO JBIG")));
    m_imageCompression.insert(COMPRESSION_SGILOG,
                new QString(I18N_NOOP("SGI log luminance RLE")));
    m_imageCompression.insert(COMPRESSION_SGILOG24,
                new QString(I18N_NOOP("SGI log 24-bit packed")));
}

QDateTime KTiffPlugin::tiffDate(const QString& s) const
{
    QDateTime dt;
    QRegExp rxDate("^([0-9]{4}):([0-9]{2}):([0-9]{2})\\s"
                   "([0-9]{2}):([0-9]{2}):([0-9]{2})$");

    if (rxDate.indexIn(s) != -1)
    {
        int year = rxDate.cap(1).toInt();
        int month = rxDate.cap(2).toInt();
        int day = rxDate.cap(3).toInt();
        int hour = rxDate.cap(4).toInt();
        int min = rxDate.cap(5).toInt();
        int sec = rxDate.cap(6).toInt();

        QDate d = QDate(year, month, day);
        QTime t = QTime(hour, min, sec);

        if (d.isValid() && t.isValid())
        {
            dt.setDate(d);
            dt.setTime(t);
        }
    }

    return dt;
}

bool KTiffPlugin::readInfo(KFileMetaInfo& info, uint)
{
    TIFF *tiff = TIFFOpen(QFile::encodeName(info.path()), "r");
    if (!tiff)
        return false;

    uint32 imageLength=0, imageWidth=0;
    uint16 bitsPerSample=0, imageCompression=0, colorMode=0, samplesPerPixel=0,
        imageAlpha=0, imageResUnit=0, dummy=0, faxPages=0;
    float imageXResolution=0, imageYResolution=0;
    char *description=0, *copyright=0, *software=0, *datetime=0, *artist=0,
        *scannerMake=0, *scannerModel=0;

    TIFFGetField(tiff, TIFFTAG_IMAGELENGTH, &imageLength);
    TIFFGetField(tiff, TIFFTAG_IMAGEWIDTH, &imageWidth);
    TIFFGetFieldDefaulted(tiff, TIFFTAG_BITSPERSAMPLE, &bitsPerSample);
    TIFFGetFieldDefaulted(tiff, TIFFTAG_SAMPLESPERPIXEL, &samplesPerPixel);
    TIFFGetField(tiff, TIFFTAG_PHOTOMETRIC, &colorMode);
    TIFFGetFieldDefaulted(tiff, TIFFTAG_COMPRESSION, &imageCompression);
    TIFFGetField(tiff, TIFFTAG_MATTEING, &imageAlpha);
    TIFFGetField(tiff, TIFFTAG_XRESOLUTION, &imageXResolution);
    TIFFGetField(tiff, TIFFTAG_YRESOLUTION, &imageYResolution);
    TIFFGetFieldDefaulted(tiff, TIFFTAG_RESOLUTIONUNIT, &imageResUnit);
    TIFFGetField(tiff, TIFFTAG_IMAGEDESCRIPTION, &description);
    TIFFGetField(tiff, TIFFTAG_SOFTWARE, &software);
    TIFFGetField(tiff, TIFFTAG_COPYRIGHT, &copyright);
    TIFFGetField(tiff, TIFFTAG_DATETIME, &datetime);
    TIFFGetField(tiff, TIFFTAG_ARTIST, &artist);
    TIFFGetField(tiff, TIFFTAG_PAGENUMBER, &dummy, &faxPages);
    TIFFGetField(tiff, TIFFTAG_MAKE, &scannerMake);
    TIFFGetField(tiff, TIFFTAG_MODEL, &scannerModel);

    kDebug(7034) << "Description: " << description;
    kDebug(7034) << "Width: " << imageWidth;
    kDebug(7034) << "Height: " << imageLength;
    kDebug(7034) << "BitDepth: " << bitsPerSample;
    kDebug(7034) << "ColorMode: " << colorMode;
    kDebug(7034) << "Compression: " << imageCompression;
    kDebug(7034) << "SamplesPerPixel: " << samplesPerPixel;
    kDebug(7034) << "ImageAlpha: " << imageAlpha;
    kDebug(7034) << "XResolution: " << imageXResolution;
    kDebug(7034) << "YResolution: " << imageYResolution;
    kDebug(7034) << "ResolutionUnit: " << imageResUnit;
    kDebug(7034) << "FaxPages: " << faxPages;
    kDebug(7034) << "DateTime: " << datetime;
    kDebug(7034) << "Copyright: " << copyright;
    kDebug(7034) << "Software: " <<  software;
    kDebug(7034) << "Artist: " << artist;
    kDebug(7034) << "Make: " << scannerMake;
    kDebug(7034) << "Model: " << scannerModel;

    if (imageResUnit == RESUNIT_CENTIMETER)
    {
        imageXResolution *= 2.54;
        imageYResolution *= 2.54;
    }
    else if (imageResUnit == RESUNIT_NONE)
    {
        imageXResolution = 0;
        imageYResolution = 0;
    }

    int imageBpp = bitsPerSample*samplesPerPixel;
    if (imageAlpha && colorMode==PHOTOMETRIC_RGB)
        m_colorMode.replace(PHOTOMETRIC_RGB, new QString(I18N_NOOP("RGBA")));

    KFileMetaInfoGroup group = appendGroup(info, "General");
    if (description)
        appendItem(group, "Description", QString(description));
    appendItem(group, "Dimensions", QSize(imageWidth, imageLength));
    appendItem(group, "BitDepth", imageBpp);
    if (imageXResolution>0 && imageYResolution>0)
        appendItem(group, "Resolution", QSize(
                static_cast<int>(imageXResolution),
                static_cast<int>(imageYResolution)));
    if (m_colorMode[colorMode])
        appendItem(group, "ColorMode", *m_colorMode[colorMode]);
    if (m_imageCompression[imageCompression])
        appendItem(group, "Compression", *m_imageCompression[imageCompression]);
    if (datetime)
    {
        QDateTime dt = tiffDate(QString(datetime));
        if (dt.isValid())
            appendItem(group, "DateTime", dt);
    }
    if (copyright)
        appendItem(group, "Copyright", QString(copyright));
    if (software)
        appendItem(group, "Software", QString(software));
    if (artist)
        appendItem(group, "Artist", QString(artist));

    if (faxPages>0 && (imageCompression==COMPRESSION_CCITTFAX3 ||
            imageCompression==COMPRESSION_CCITTFAX4))
    {
        appendItem(group, "FaxPages", faxPages);
    }

    if (scannerMake || scannerModel)
    {
        group = appendGroup(info, "Scanner");
        if (scannerMake)
            appendItem(group, "Make", QString(scannerMake));
        if (scannerModel)
            appendItem(group, "Model", QString(scannerModel));
    }

    TIFFClose(tiff);

    return true;
}

#include "kfile_tiff.moc"
