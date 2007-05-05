/* This file is part of the KDE project
 * Copyright (C) 2002 Frank Pieczynski
 * Copyright (C) 2007 Jos van den Oever <jos@vandenoever.info>
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

/*
 Include the strigi specific headers.
*/
#define STRIGI_IMPORT_API
#include <strigi/analyzerplugin.h>
#include <strigi/streamendanalyzer.h>
#include <strigi/analysisresult.h>
#include <strigi/fieldtypes.h>
#include <KUrl>
#include <QDateTime>
#include "exif.h"

using namespace Strigi;
using namespace std;

/*
 Declare the factory.
*/
class JpegEndAnalyzerFactory;

/*
Define a class that inherits from StreamEndAnalyzer.
The only function we really need to implement is connectInputStream()
*/
class STRIGI_PLUGIN_API JpegEndAnalyzer : public StreamEndAnalyzer {
private:
    AnalysisResult* result;
    const JpegEndAnalyzerFactory* factory;

    QDateTime parseDateTime(const QString& string);
public:
    JpegEndAnalyzer(const JpegEndAnalyzerFactory* f) :factory(f) {}
    ~JpegEndAnalyzer() {}
    const char* name() const {
        return "JpegEndAnalyzer";
    }
    bool checkHeader(const char* header, int32_t headersize) const;
    char analyze(AnalysisResult& idx, ::InputStream* in);
};

/*
 Define a factory class the provides information about the fields that an
 analyzer can extract. This has a function similar to KFilePlugin::addItemInfo.
*/
class STRIGI_PLUGIN_API JpegEndAnalyzerFactory : public StreamEndAnalyzerFactory {
friend class JpegEndAnalyzer;
private:
    /* This is why this class is a factory. */
    StreamEndAnalyzer* newInstance() const {
        return new JpegEndAnalyzer(this);
    }
    const char* name() const {
        return "JpegEndAnalyzer";
    }
    void registerFields(FieldRegister& );

    /* define static fields that contain the field names. */
    static const string commentFieldName;
    static const string manufacturerFieldName;
    static const string modelFieldName;
    static const string creationDateFieldName;
    static const string widthFieldName;
    static const string heightFieldName;
    static const string orientationFieldName;
    static const string colorModeFieldName;
    static const string flashUsedFieldName;
    static const string focalLengthFieldName;
    static const string _35mmEquivalentFieldName;
    static const string ccdWidthFieldName;
    static const string exposureTimeFieldName;
    static const string apertureFieldName;
    static const string focusDistFieldName;
    static const string exposureBiasFieldName;
    static const string whiteBalanceFieldName;
    static const string meteringModeFieldName;
    static const string exposureFieldName;
    static const string isoEquivFieldName;
    static const string jpegQualityFieldName;
    static const string userCommentFieldName;
    static const string jpegProcessFieldName;
    static const string thumbnailFieldName;

    /* The RegisteredField instances are used to index specific fields quickly.
       We pass a pointer to the instance instead of a string.
    */
    const RegisteredField* commentField;
    const RegisteredField* manufacturerField;
    const RegisteredField* modelField;
    const RegisteredField* creationDateField;
    const RegisteredField* widthField;
    const RegisteredField* heightField;
    const RegisteredField* orientationField;
    const RegisteredField* colorModeField;
    const RegisteredField* flashUsedField;
    const RegisteredField* focalLengthField;
    const RegisteredField* _35mmEquivalentField;
    const RegisteredField* ccdWidthField;
    const RegisteredField* exposureTimeField;
    const RegisteredField* apertureField;
    const RegisteredField* focusDistField;
    const RegisteredField* exposureBiasField;
    const RegisteredField* whiteBalanceField;
    const RegisteredField* meteringModeField;
    const RegisteredField* exposureField;
    const RegisteredField* isoEquivField;
    const RegisteredField* jpegQualityField;
    const RegisteredField* userCommentField;
    const RegisteredField* jpegProcessField;
    const RegisteredField* thumbnailField;
};

const string JpegEndAnalyzerFactory::commentFieldName("content.comment");
const string JpegEndAnalyzerFactory::manufacturerFieldName("photo.camera_manufacturer");
const string JpegEndAnalyzerFactory::modelFieldName("photo.camera_model");
const string JpegEndAnalyzerFactory::creationDateFieldName("content.creation_time");
const string JpegEndAnalyzerFactory::widthFieldName("image.width");
const string JpegEndAnalyzerFactory::heightFieldName("image.height");
const string JpegEndAnalyzerFactory::orientationFieldName("photo.orientation");
const string JpegEndAnalyzerFactory::colorModeFieldName("image.color_space");
const string JpegEndAnalyzerFactory::flashUsedFieldName("photo.flash_used");
const string JpegEndAnalyzerFactory::focalLengthFieldName("photo.focal_length");
const string JpegEndAnalyzerFactory::_35mmEquivalentFieldName("photo.35mm_equivalent");
const string JpegEndAnalyzerFactory::ccdWidthFieldName("photo.cdd_width");
const string JpegEndAnalyzerFactory::exposureTimeFieldName("photo.exposure_time");
const string JpegEndAnalyzerFactory::apertureFieldName("photo.aperture");
const string JpegEndAnalyzerFactory::focusDistFieldName("photo.focus_distance");
const string JpegEndAnalyzerFactory::exposureBiasFieldName("photo.exposure_bias");
const string JpegEndAnalyzerFactory::whiteBalanceFieldName("photo.white_balance");
const string JpegEndAnalyzerFactory::meteringModeFieldName("photo.metering_mode");
const string JpegEndAnalyzerFactory::exposureFieldName("photo.exposure_program");
const string JpegEndAnalyzerFactory::isoEquivFieldName("photo.iso_equivalent");
const string JpegEndAnalyzerFactory::jpegQualityFieldName("compressed.target_quality");
const string JpegEndAnalyzerFactory::userCommentFieldName("content.comment");
const string JpegEndAnalyzerFactory::jpegProcessFieldName("compressed.compression_algorithm");
const string JpegEndAnalyzerFactory::thumbnailFieldName("content.thumbnail");

/*
 Register the field names so that the StreamIndexer knows which analyzer
 provides what information.
*/
void
JpegEndAnalyzerFactory::registerFields(FieldRegister& r) {
    commentField = r.registerField(commentFieldName, FieldRegister::stringType,
        -1, 0);
    manufacturerField = r.registerField(manufacturerFieldName, FieldRegister::stringType,        -1, 0);
    modelField = r.registerField(modelFieldName, FieldRegister::stringType,        -1, 0);
    creationDateField = r.registerField(creationDateFieldName, FieldRegister::stringType,        -1, 0);
    widthField = r.registerField(widthFieldName, FieldRegister::stringType,        -1, 0);
    heightField = r.registerField(heightFieldName, FieldRegister::stringType,        -1, 0);
    orientationField = r.registerField(orientationFieldName, FieldRegister::stringType,        -1, 0);
    colorModeField = r.registerField(colorModeFieldName, FieldRegister::stringType,        -1, 0);
    flashUsedField = r.registerField(flashUsedFieldName, FieldRegister::stringType,        -1, 0);
    focalLengthField = r.registerField(focalLengthFieldName, FieldRegister::stringType,        -1, 0);
    _35mmEquivalentField = r.registerField(_35mmEquivalentFieldName, FieldRegister::stringType,        -1, 0);
    ccdWidthField = r.registerField(ccdWidthFieldName, FieldRegister::stringType,        -1, 0);
    exposureTimeField = r.registerField(exposureTimeFieldName, FieldRegister::stringType,        -1, 0);
    apertureField = r.registerField(apertureFieldName, FieldRegister::stringType,        -1, 0);
    focusDistField = r.registerField(focusDistFieldName, FieldRegister::stringType,        -1, 0);
    exposureBiasField = r.registerField(exposureBiasFieldName, FieldRegister::stringType,        -1, 0);
    whiteBalanceField = r.registerField(whiteBalanceFieldName, FieldRegister::stringType,        -1, 0);
    meteringModeField = r.registerField(meteringModeFieldName, FieldRegister::stringType,        -1, 0);
    exposureField = r.registerField(exposureFieldName, FieldRegister::stringType,        -1, 0);
    isoEquivField = r.registerField(isoEquivFieldName, FieldRegister::stringType,        -1, 0);
    jpegQualityField = r.registerField(jpegQualityFieldName, FieldRegister::stringType,        -1, 0);
    userCommentField = r.registerField(userCommentFieldName, FieldRegister::stringType,        -1, 0);
    jpegProcessField = r.registerField(jpegProcessFieldName, FieldRegister::stringType,        -1, 0);
    thumbnailField = r.registerField(thumbnailFieldName, FieldRegister::stringType,        -1, 0);
}

bool
JpegEndAnalyzer::checkHeader(const char* header, int32_t headersize) const {
    static const unsigned char jpgmagic[]
        = {0xFF, 0xD8, 0xFF};
    return headersize >= 3 &&  memcmp(header, jpgmagic, 3) == 0;
}
char
JpegEndAnalyzer::analyze(AnalysisResult& ar, ::InputStream*) {
    const KUrl path( ar.path().c_str() );
    if ( path.isEmpty() ) // remote file
        return -1;

    ExifData ImageInfo;

    // parse the jpeg file now
    try {
        if ( !ImageInfo.scan(path.path()) ) {
            kDebug(7034) << "Not a JPEG file!\n";
            return false;
        }
    } catch (FatalError& e) { // malformed exif data?
        e.debug_print();
        return -1;
    }

    string tag = (const char*)ImageInfo.getComment().toUtf8();
    if ( tag.length() ) {
        ar.addValue(factory->commentField, tag);
    }

    tag = (const char*)ImageInfo.getCameraMake().toUtf8();
    if (tag.length()) {
        ar.addValue(factory->manufacturerField, tag);
    }

    tag = (const char*)ImageInfo.getCameraModel().toUtf8();
    if (tag.length()) {
        ar.addValue(factory->modelField, tag);
    }

    QString qtag = ImageInfo.getDateTime();
    if (qtag.length()) {
        QDateTime dt = parseDateTime(qtag);
        if ( dt.isValid() ) {
            ar.addValue(factory->creationDateField, dt.toTime_t());
        }
    }

    ar.addValue(factory->widthField, (uint32_t)ImageInfo.getWidth());
    ar.addValue(factory->heightField, (uint32_t)ImageInfo.getHeight());

    if (ImageInfo.getOrientation()) {
        ar.addValue(factory->orientationField,
            (uint32_t)ImageInfo.getOrientation());
    }

    ar.addValue(factory->colorModeField, ImageInfo.getIsColor()
        ?"Color" :"Black and white");

    int flashUsed = ImageInfo.getFlashUsed(); // -1, <set>
    if (flashUsed >= 0) {
	 string flash = "(unknown)";
         switch (flashUsed) {
         case 0: flash = "No";
             break;
         case 1:
         case 5:
         case 7:
             flash = "Fired";
             break;
         case 9:
         case 13:
         case 15:
             flash = "Fill Fired";
             break;
         case 16:
             flash = "Off";
             break;
         case 24:
             flash = "Auto Off";
             break;
         case 25:
         case 29:
         case 31:
             flash = "Auto Fired";
             break;
         case 32:
             flash = "Not Available";
             break;
         default:
             break;
        }
        ar.addValue(factory->flashUsedField, flash);
    }

    if (ImageInfo.getFocalLength()) {
        ar.addValue(factory->focalLengthField, ImageInfo.getFocalLength());

        if (ImageInfo.getCCDWidth()){
            ar.addValue(factory->_35mmEquivalentField,
                        (int)(ImageInfo.getFocalLength()/ImageInfo.getCCDWidth()*35 + 0.5) );
	}
    }

    if (ImageInfo.getCCDWidth()) {
        ar.addValue(factory->ccdWidthField, ImageInfo.getCCDWidth());
    }

    if (ImageInfo.getExposureTime()) {
        tag = (const char*)
            QString().sprintf("%6.3f", ImageInfo.getExposureTime()).toUtf8();
        float exposureTime = ImageInfo.getExposureTime();
	if (exposureTime > 0 && exposureTime <= 0.5){
            tag += (const char*)
                QString().sprintf(" (1/%d)", (int)(0.5 + 1/exposureTime) ).toUtf8();
	}
        ar.addValue(factory->exposureTimeField, tag);
    }

    if (ImageInfo.getApertureFNumber()) {
        ar.addValue(factory->apertureField, ImageInfo.getApertureFNumber());
    }

    if (ImageInfo.getDistance()) {
        if (ImageInfo.getDistance() < 0) {
	    tag = "Infinite";
        } else {
	    tag = (const char*)QString()
                .sprintf("%5.2fm",(double)ImageInfo.getDistance()).toUtf8();
        }
        ar.addValue(factory->focusDistField, tag);
    }

    if (ImageInfo.getExposureBias()) {
        ar.addValue(factory->exposureBiasField, ImageInfo.getExposureBias());
    }

    if (ImageInfo.getWhitebalance() != -1) {
        switch(ImageInfo.getWhitebalance()) {
	case 0:
	    tag = "Unknown";
	    break;
	case 1:
	    tag = "Daylight";
	    break;
	case 2:
	    tag = "Fluorescent";
	    break;
	case 3:
	    //tag = "incandescent";
	    tag = "Tungsten";
	    break;
	case 17:
	    tag = "Standard light A";
	    break;
	case 18:
	    tag = "Standard light B";
	    break;
	case 19:
	    tag = "Standard light C";
	    break;
	case 20:
	    tag = "D55";
	    break;
	case 21:
	    tag = "D65";
	    break;
	case 22:
	    tag = "D75";
	    break;
	case 255:
	    tag = "Other";
	    break;
	default:
            //23 to 254 = reserved
	    tag = "Unknown";
	}
        ar.addValue(factory->whiteBalanceField, tag);
    }

    if (ImageInfo.getMeteringMode() != -1) {
        switch(ImageInfo.getMeteringMode()) {
	case 0:
	    tag = "Unknown";
	    break;
	case 1:
	    tag = "Average";
	    break;
	case 2:
	    tag = "Center weighted average";
	    break;
	case 3:
	    tag = "Spot";
	    break;
	case 4:
	    tag = "MultiSpot";
	    break;
	case 5:
	    tag = "Pattern";
	    break;
	case 6:
	    tag = "Partial";
	    break;
	case 255:
	    tag = "Other";
	    break;
	default:
	    // 7 to 254 = reserved
	    tag = "Unknown";
	}
        ar.addValue(factory->meteringModeField, tag);
    }

    if (ImageInfo.getExposureProgram()){
        switch(ImageInfo.getExposureProgram()) {
	case 0:
	    tag = "Not defined";
	    break;
	case 1:
	    tag = "Manual";
	    break;
	case 2:
	    tag = "Normal program";
	    break;
	case 3:
	    tag = "Aperture priority";
	    break;
	case 4:
	    tag = "Shutter priority";
	    break;
	case 5:
	    tag = "Creative program\n(biased toward fast shutter speed)";
	    break;
	case 6:
	    tag = "Action program\n(biased toward fast shutter speed)";
	    break;
	case 7:
	    tag = "Portrait mode\n(for closeup photos with the background out of focus)";
	    break;
	case 8:
	    tag = "Landscape mode\n(for landscape photos with the background in focus)";
	    break;
	default:
	    // 9 to 255 = reserved
	    tag = "Unknown";
	}
        ar.addValue(factory->exposureField, tag);
    }

    if (ImageInfo.getISOequivalent()){
	ar.addValue(factory->isoEquivField, (int)ImageInfo.getISOequivalent());
    }

    if (ImageInfo.getCompressionLevel()){
	switch(ImageInfo.getCompressionLevel()) {
	case 1:
	    tag = "Basic";
            break;
	case 2:
	    tag = "Normal";
	    break;
        case 4:
	    tag = "Fine";
	    break;
	default:
	    tag = "Unknown";
	}
        ar.addValue(factory->jpegQualityField, tag);
    }

    tag = (const char*)ImageInfo.getUserComment().toUtf8();
    if (tag.length()) {
        ar.addValue(factory->commentField, tag);
    }

    int a;
    for (a = 0; ; a++){
        if (ProcessTable[a].Tag == ImageInfo.getProcess() || ProcessTable[a].Tag == 0) {
            ar.addValue(factory->jpegProcessField, ProcessTable[a].Desc);
            break;
        }
    }

    if (!ImageInfo.isNullThumbnail()) {
        QByteArray ba;
        QDataStream ds(&ba, QIODevice::WriteOnly);
        ds << ImageInfo.getThumbnail();
        ar.addValue(factory->thumbnailField, ba.data(), ba.size());
    }
    return 0;
}
// format of the string is:
// YYYY:MM:DD HH:MM:SS
QDateTime
JpegEndAnalyzer::parseDateTime(const QString& string) {
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
/*
 For plugins, we need to have a way to find out which plugins are defined in a
 plugin. One instance of AnalyzerFactoryFactory per plugin profides this
 information.
*/
class Factory : public AnalyzerFactoryFactory {
public:
    list<StreamEndAnalyzerFactory*>
    getStreamEndAnalyzerFactories() const {
        list<StreamEndAnalyzerFactory*> af;
        af.push_back(new JpegEndAnalyzerFactory());
        return af;
    }
};

/*
 Register the AnalyzerFactoryFactory
*/
STRIGI_ANALYZER_FACTORY(Factory)
