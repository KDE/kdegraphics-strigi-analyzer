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
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 */

#include "kfile_tiff.h"

#include <kurl.h>
#include <kgenericfactory.h>
#include <kdebug.h>

#include <qstringlist.h>
#include <qintdict.h>
#include <qfile.h>

#include <tiff.h>
#include <tiffio.h>

typedef KGenericFactory<KTiffPlugin> TiffFactory;

K_EXPORT_COMPONENT_FACTORY(kfile_tiff, TiffFactory("kfile_tiff"));

KTiffPlugin::KTiffPlugin(QObject *parent, const char *name,
        const QStringList &args) : KFilePlugin(parent, name, args)
{
    kdDebug(7034) << "TIFF file meta info plugin\n";
    KFileMimeTypeInfo* info = addMimeTypeInfo( "image/tiff" );

    KFileMimeTypeInfo::GroupInfo* group =
            addGroupInfo(info, "General", i18n("General"));

    KFileMimeTypeInfo::ItemInfo* item;
    item = addItemInfo(group, "Description", i18n("Description"), 
            QVariant::String);
    setHint(item, KFileMimeTypeInfo::Description);
    item = addItemInfo(group, "Copyright", i18n("Copyright"), 
            QVariant::String);
    item = addItemInfo(group, "ImageType", i18n("Image type"), 
            QVariant::String);
    item = addItemInfo(group, "Dimensions", i18n("Dimensions"), 
            QVariant::Size);
    setHint(item, KFileMimeTypeInfo::Size);
    setSuffix(item, i18n(" pixels"));
    item = addItemInfo(group, "Resolution", i18n("Resolution"), 
            QVariant::Size);
    setSuffix(item, i18n(" dpi"));
    item = addItemInfo(group, "BitDepth", i18n("Bit depth"), 
            QVariant::Int);
    setSuffix(item, i18n(" bpp"));
    item = addItemInfo(group, "Compression", i18n("Compression"), 
            QVariant::String);
    item = addItemInfo(group, "Software", i18n("Software"), 
            QVariant::String);
    item = addItemInfo(group, "DateTime", i18n("Date/time"), 
            QVariant::String);
    item = addItemInfo(group, "Artist", i18n("Artist"), 
            QVariant::String);
    setHint(item, KFileMimeTypeInfo::Author);
    item = addItemInfo(group, "FaxPages", i18n("Fax pages"), 
            QVariant::Int);

    m_imageType.setAutoDelete(true);
    m_imageCompression.setAutoDelete(true);

    m_imageType.insert(PHOTOMETRIC_MINISWHITE, 
                new QString(I18N_NOOP("Monochrome")));
    m_imageType.insert(PHOTOMETRIC_MINISBLACK, 
                new QString(I18N_NOOP("Monochrome")));
    m_imageType.insert(PHOTOMETRIC_RGB, 
                new QString(I18N_NOOP("RGB")));
    m_imageType.insert(PHOTOMETRIC_PALETTE, 
                new QString(I18N_NOOP("Palette color")));
    m_imageType.insert(PHOTOMETRIC_MASK, 
                new QString(I18N_NOOP("Transparency mask")));
    m_imageType.insert(PHOTOMETRIC_SEPARATED, 
                new QString(I18N_NOOP("Color separation")));
    m_imageType.insert(PHOTOMETRIC_YCBCR, 
                new QString(I18N_NOOP("YCbCr")));
    m_imageType.insert(PHOTOMETRIC_CIELAB, 
                new QString(I18N_NOOP("CIE Lab")));
#ifdef PHOTOMETRIC_ITULAB
    m_imageType.insert(PHOTOMETRIC_ITULAB, 
                new QString(I18N_NOOP("ITU Lab")));
#endif
    m_imageType.insert(PHOTOMETRIC_LOGL, 
                new QString(I18N_NOOP("LOGL")));
    m_imageType.insert(PHOTOMETRIC_LOGLUV, 
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
    m_imageCompression.insert(COMPRESSION_ADOBE_DEFLATE, 
                new QString(I18N_NOOP("Adobe Deflate")));
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

bool KTiffPlugin::readInfo(KFileMetaInfo& info, uint)
{
    TIFF *tiff = TIFFOpen(QFile::encodeName(info.path()), "r");
    if (!tiff)
        return false;

    uint32 imageLength=0, imageWidth=0;
    uint16 bitsPerSample=0, imageCompression=0, imageType=0, samplesPerPixel=0,
           imageAlpha=0, imageResUnit=0, dummy=0, faxPages=0; 
    float imageXResolution=0, imageYResolution=0;
    char *description=0, *copyright=0, *software=0, *datetime=0, *artist=0;

    TIFFGetField(tiff, TIFFTAG_IMAGELENGTH, &imageLength);
    TIFFGetField(tiff, TIFFTAG_IMAGEWIDTH, &imageWidth);
    TIFFGetFieldDefaulted(tiff, TIFFTAG_BITSPERSAMPLE, &bitsPerSample);
    TIFFGetFieldDefaulted(tiff, TIFFTAG_SAMPLESPERPIXEL, &samplesPerPixel);
    TIFFGetField(tiff, TIFFTAG_PHOTOMETRIC, &imageType);
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

    kdDebug(7034) << "Description: " << (void *)description << " -> " << description << endl;
    kdDebug(7034) << "Width: " << imageWidth << endl;
    kdDebug(7034) << "Height: " << imageLength << endl;
    kdDebug(7034) << "BitDepth: " << bitsPerSample << endl;
    kdDebug(7034) << "ImageType: " << imageType << endl;
    kdDebug(7034) << "Compression: " << imageCompression << endl;
    kdDebug(7034) << "SamplesPerPixel: " << samplesPerPixel << endl;
    kdDebug(7034) << "ImageAlpha: " << imageAlpha << endl;
    kdDebug(7034) << "XResolution: " << imageXResolution << endl;
    kdDebug(7034) << "YResolution: " << imageYResolution << endl;
    kdDebug(7034) << "ResolutionUnit: " << imageResUnit << endl;
    kdDebug(7034) << "FaxPages: " << faxPages << endl;
    kdDebug(7034) << "DateTime: " << datetime << endl;
    kdDebug(7034) << "Copyright: " << copyright << endl;
    kdDebug(7034) << "Software: " <<  software << endl;
    kdDebug(7034) << "Artist: " << artist << endl;

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
    if (imageAlpha && imageType==PHOTOMETRIC_RGB)
        m_imageType.replace(PHOTOMETRIC_RGB, new QString(I18N_NOOP("RGBA")));

    KFileMetaInfoGroup group = appendGroup(info, "General");
    if (description)
        appendItem(group, "Description", QString(description));
    appendItem(group, "Dimensions", QSize(imageWidth, imageLength));
    appendItem(group, "BitDepth", imageBpp);
    if (imageXResolution>0 && imageYResolution>0)
    {
        kdDebug(7034) << "Adding resolution..." << endl;
        appendItem(group, "Resolution", QSize(
                static_cast<int>(imageXResolution), 
                static_cast<int>(imageYResolution)));
    }
    if (m_imageType[imageType])
        appendItem(group, "ImageType", *m_imageType[imageType]);
    if (m_imageCompression[imageCompression])
        appendItem(group, "Compression", *m_imageCompression[imageCompression]);
    if (datetime)
        appendItem(group, "DateTime", QString(datetime));
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

    TIFFClose(tiff);

    return true;
}

#include "kfile_tiff.moc"
