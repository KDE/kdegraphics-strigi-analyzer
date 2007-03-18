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
#include <strigi/jstreamsconfig.h>
#include <strigi/analyzerplugin.h>
#include <strigi/streamendanalyzer.h>
#include <strigi/analysisresult.h>
#include <strigi/cnstr.h>
#include <strigi/fieldtypes.h>
#include <KUrl>
#include <QDateTime>
#include "exif.h"

using namespace jstreams;
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
    const char* getName() const {
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
    const char* getName() const {
        return "JpegEndAnalyzer";
    }
    void registerFields(FieldRegister& );

    /* define static fields that contain the field names. cnstr is a string
       class that can efficiently store and compare constant strings. */
    static const cnstr commentFieldName;
    static const cnstr manufacturerFieldName;
    static const cnstr modelFieldName;
    static const cnstr creationDateFieldName;
    static const cnstr widthFieldName;
    static const cnstr heightFieldName;
    static const cnstr orientationFieldName;
    static const cnstr colorModeFieldName;
    static const cnstr flashUsedFieldName;
    static const cnstr focalLengthFieldName;
    static const cnstr _35mmEquivalentFieldName;
    static const cnstr ccdWidthFieldName;
    static const cnstr exposureTimeFieldName;
    static const cnstr apertureFieldName;
    static const cnstr focusDistFieldName;
    static const cnstr exposureBiasFieldName;
    static const cnstr whiteBalanceFieldName;
    static const cnstr meteringModeFieldName;
    static const cnstr exposureFieldName;
    static const cnstr isoEquivFieldName;
    static const cnstr jpegQualityFieldName;
    static const cnstr userCommentFieldName;
    static const cnstr jpegProcessFieldName;
    static const cnstr thumbnailFieldName;

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

const cnstr JpegEndAnalyzerFactory::commentFieldName("jpegcomment");
const cnstr JpegEndAnalyzerFactory::manufacturerFieldName("manufacturer");
const cnstr JpegEndAnalyzerFactory::modelFieldName("model");
const cnstr JpegEndAnalyzerFactory::creationDateFieldName("creationDate");
const cnstr JpegEndAnalyzerFactory::widthFieldName("width");
const cnstr JpegEndAnalyzerFactory::heightFieldName("height");
const cnstr JpegEndAnalyzerFactory::orientationFieldName("orientation");
const cnstr JpegEndAnalyzerFactory::colorModeFieldName("colorMode");
const cnstr JpegEndAnalyzerFactory::flashUsedFieldName("flashUsed");
const cnstr JpegEndAnalyzerFactory::focalLengthFieldName("focalLength");
const cnstr JpegEndAnalyzerFactory::_35mmEquivalentFieldName("_35mmEquivalent");
const cnstr JpegEndAnalyzerFactory::ccdWidthFieldName("ccdWidth");
const cnstr JpegEndAnalyzerFactory::exposureTimeFieldName("exposureTime");
const cnstr JpegEndAnalyzerFactory::apertureFieldName("aperture");
const cnstr JpegEndAnalyzerFactory::focusDistFieldName("focusDist");
const cnstr JpegEndAnalyzerFactory::exposureBiasFieldName("exposureBias");
const cnstr JpegEndAnalyzerFactory::whiteBalanceFieldName("whiteBalance");
const cnstr JpegEndAnalyzerFactory::meteringModeFieldName("meteringMode");
const cnstr JpegEndAnalyzerFactory::exposureFieldName("exposure");
const cnstr JpegEndAnalyzerFactory::isoEquivFieldName("isoEquiv");
const cnstr JpegEndAnalyzerFactory::jpegQualityFieldName("jpegQuality");
const cnstr JpegEndAnalyzerFactory::userCommentFieldName("userComment");
const cnstr JpegEndAnalyzerFactory::jpegProcessFieldName("jpegProcess");
const cnstr JpegEndAnalyzerFactory::thumbnailFieldName("thumbnail");

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
        ar.setField(factory->commentField, tag);
    }

    tag = (const char*)ImageInfo.getCameraMake().toUtf8();
    if (tag.length()) {
        ar.setField(factory->manufacturerField, tag);
    }

    tag = (const char*)ImageInfo.getCameraModel().toUtf8();
    if (tag.length()) {
        ar.setField(factory->modelField, tag);
    }

    QString qtag = ImageInfo.getDateTime();
    if (qtag.length()) {
        QDateTime dt = parseDateTime(qtag);
        if ( dt.isValid() ) {
            ar.setField(factory->creationDateField, dt.toTime_t());
        }
    }

    ar.setField(factory->widthField, (uint32_t)ImageInfo.getWidth());
    ar.setField(factory->heightField, (uint32_t)ImageInfo.getHeight());

    if (ImageInfo.getOrientation()) {
        ar.setField(factory->orientationField,
            (uint32_t)ImageInfo.getOrientation());
    }

    ar.setField(factory->colorModeField, ImageInfo.getIsColor()
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
        ar.setField(factory->flashUsedField, flash);
    }

    if (ImageInfo.getFocalLength()) {
        ar.setField(factory->focalLengthField, ImageInfo.getFocalLength());

        if (ImageInfo.getCCDWidth()){
            ar.setField(factory->_35mmEquivalentField,
                        (int)(ImageInfo.getFocalLength()/ImageInfo.getCCDWidth()*35 + 0.5) );
	}
    }

    if (ImageInfo.getCCDWidth()) {
        ar.setField(factory->ccdWidthField, ImageInfo.getCCDWidth());
    }

    if (ImageInfo.getExposureTime()) {
        tag = (const char*)
            QString().sprintf("%6.3f", ImageInfo.getExposureTime()).toUtf8();
        float exposureTime = ImageInfo.getExposureTime();
	if (exposureTime > 0 && exposureTime <= 0.5){
            tag += (const char*)
                QString().sprintf(" (1/%d)", (int)(0.5 + 1/exposureTime) ).toUtf8();
	}
        ar.setField(factory->exposureTimeField, tag);
    }

    if (ImageInfo.getApertureFNumber()) {
        ar.setField(factory->apertureField, ImageInfo.getApertureFNumber());
    }

    if (ImageInfo.getDistance()) {
        if (ImageInfo.getDistance() < 0) {
	    tag = "Infinite";
        } else {
	    tag = (const char*)QString()
                .sprintf("%5.2fm",(double)ImageInfo.getDistance()).toUtf8();
        }
        ar.setField(factory->focusDistField, tag);
    }

    if (ImageInfo.getExposureBias()) {
        ar.setField(factory->exposureBiasField, ImageInfo.getExposureBias());
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
        ar.setField(factory->whiteBalanceField, tag);
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
        ar.setField(factory->meteringModeField, tag);
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
        ar.setField(factory->exposureField, tag);
    }

    if (ImageInfo.getISOequivalent()){
	ar.setField(factory->isoEquivField, (int)ImageInfo.getISOequivalent());
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
        ar.setField(factory->jpegQualityField, tag);
    }

    tag = (const char*)ImageInfo.getUserComment().toUtf8();
    if (tag.length()) {
        ar.setField(factory->commentField, tag);
    }

    int a;
    for (a = 0; ; a++){
        if (ProcessTable[a].Tag == ImageInfo.getProcess() || ProcessTable[a].Tag == 0) {
            ar.setField(factory->jpegProcessField, ProcessTable[a].Desc);
            break;
        }
    }

    if (!ImageInfo.isNullThumbnail()) {
        QByteArray ba;
        QDataStream ds(&ba, QIODevice::WriteOnly);
        ds << ImageInfo.getThumbnail();
        ar.setField(factory->thumbnailField, ba.data(), ba.size());
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
