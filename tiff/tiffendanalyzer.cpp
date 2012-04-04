/*
 * Copyright (C) 2009 Pino Toscano <pino@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include <strigi/analysisresult.h>
#include <strigi/analyzerplugin.h>
#include <strigi/fieldtypes.h>
#include <strigi/streamendanalyzer.h>

#include <tiff.h>
#include <tiffio.h>

#include <cstring>
#include <time.h>

using namespace Strigi;

namespace {

tsize_t
strigi_tiffReadProc(thandle_t handle, tdata_t buf, tsize_t size) {
    InputStream* stream = static_cast<InputStream*>(handle);
    const char* data = 0;
    int32_t read = stream->read(data, size, size);
    std::memcpy(static_cast<char*>(buf), data, read);
    return read;
}

tsize_t
strigi_tiffWriteProc(thandle_t, tdata_t, tsize_t) {
    return 0;
}

toff_t
strigi_tiffSeekProc(thandle_t handle, toff_t offset, int whence) {
    InputStream* stream = static_cast<InputStream*>(handle);
    switch (whence) {
    case SEEK_SET:
        stream->reset(offset);
        break;
    case SEEK_CUR:
        stream->skip(offset);
        break;
    case SEEK_END:
        stream->reset(stream->size() + offset);
        break;
    }
    return stream->position();
}

int
strigi_tiffCloseProc(thandle_t) {
    return 0;
}

toff_t
strigi_tiffSizeProc(thandle_t handle) {
    InputStream* stream = static_cast<InputStream*>(handle);
    return stream->size();
}

int
strigi_tiffMapProc(thandle_t, tdata_t*, toff_t*) {
    return 0;
}

void
strigi_tiffUnmapProc(thandle_t, tdata_t, toff_t) {
}

void
readTiffTagString(TIFF* tiff, ttag_t tag, AnalysisResult& ar, const RegisteredField* field) {
    char* buffer = 0;
    TIFFGetField(tiff, tag, &buffer);
    if (buffer) {
        ar.addValue(field, std::string(buffer));
    }
}

void
readTiffTagUint32(TIFF* tiff, ttag_t tag, AnalysisResult& ar, const RegisteredField* field) {
    uint32 value = 0;
    TIFFGetField(tiff, tag, &value);
    ar.addValue(field, value);
}

void
readTiffTagUint16(TIFF* tiff, ttag_t tag, AnalysisResult& ar, const RegisteredField* field) {
    uint16 value = 0;
    TIFFGetField(tiff, tag, &value);
    ar.addValue(field, value);
}

void
readTiffTagDateTime(TIFF* tiff, ttag_t tag, AnalysisResult& ar, const RegisteredField* field) {
    struct tm dt;
    char* buffer = 0;
    TIFFGetField(tiff, tag, &buffer);
    // the tiff datetime string format is as follows: "2005:06:03 17:13:33"
    if (buffer && (6 == sscanf(buffer, "%d:%d:%d %d:%d:%d",
            &dt.tm_year, &dt.tm_mon, &dt.tm_mday, &dt.tm_hour, &dt.tm_min, &dt.tm_sec)) )
        ar.addValue(field, uint32_t(mktime(&dt)));
}

}

class TiffEndAnalyzerFactory;

class TiffEndAnalyzer : public StreamEndAnalyzer {
private:
    AnalysisResult* result;
    const TiffEndAnalyzerFactory* factory;
public:
    TiffEndAnalyzer(const TiffEndAnalyzerFactory* f) :factory(f) {}
    ~TiffEndAnalyzer() {}
    const char* name() const {
        return "TiffEndAnalyzer";
    }
    bool checkHeader(const char* header, int32_t headersize) const;
    signed char analyze(AnalysisResult& idx, InputStream* in);
};


class TiffEndAnalyzerFactory : public StreamEndAnalyzerFactory {
friend class TiffEndAnalyzer;
private:
    StreamEndAnalyzer* newInstance() const {
        return new TiffEndAnalyzer(this);
    }
    const char* name() const {
        return "TiffEndAnalyzer";
    }
    void registerFields(FieldRegister& r);

    const RegisteredField* widthField;
    const RegisteredField* heightField;
    const RegisteredField* copyrightField;
    const RegisteredField* descriptionField;
    const RegisteredField* samplesPerPixelField;
    const RegisteredField* softwareField;
    const RegisteredField* artistField;
    const RegisteredField* dateTimeField;
    const RegisteredField* bitsPerSampleField;
    const RegisteredField* xResolutionField;
    const RegisteredField* yResolutionField;
    const RegisteredField* typeField;
};

#define NS_NFO "http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#"
#define NS_NIE "http://www.semanticdesktop.org/ontologies/2007/01/19/nie#"
#define NS_NCO "http://www.semanticdesktop.org/ontologies/2007/03/22/nco#"
// #define NS_NEXIF "http://www.semanticdesktop.org/ontologies/2007/05/10/nexif#"

void
TiffEndAnalyzerFactory::registerFields(FieldRegister& r) {
    widthField = r.registerField(NS_NFO "width");
    heightField = r.registerField(NS_NFO "height");
    copyrightField = r.registerField(NS_NIE "copyright");
    descriptionField = r.registerField(NS_NIE "description");
    // there is no appropriate field in NS_NFO/NS_NIE ontologies
    // samplesPerPixelField = r.registerField(NS_NEXIF "samplesPerPixel");
    softwareField = r.registerField(NS_NIE "generator");
    artistField = r.registerField(NS_NCO "creator");
    dateTimeField = r.registerField(NS_NIE "contentCreated");
    bitsPerSampleField = r.registerField(NS_NFO "colorDepth");
    xResolutionField = r.registerField(NS_NFO "horizontalResolution");
    yResolutionField = r.registerField(NS_NFO "verticalResolution");
    typeField = r.typeField;

    addField(widthField);
    addField(heightField);
    addField(copyrightField);
    addField(descriptionField);
    addField(softwareField);
    addField(artistField);
    addField(dateTimeField);
    addField(bitsPerSampleField);
    addField(xResolutionField);
    addField(yResolutionField);
    addField(typeField);
}

#undef NS_NFO
#undef NS_NIE
#undef NS_NEXIF

bool
TiffEndAnalyzer::checkHeader(const char* header, int32_t headersize) const {
    static const unsigned char tiffmagic_le[] = { 0x49, 0x49, 0x2A, 0x00 };
    static const unsigned char tiffmagic_be[] = { 0x4D, 0x4D, 0x00, 0x2A };

    return headersize >= 4 &&
           (std::memcmp(header, tiffmagic_le, 4) == 0 || std::memcmp(header, tiffmagic_be, 4) == 0);
}

signed char
TiffEndAnalyzer::analyze(AnalysisResult& ar, InputStream* in) {
    const std::string fileName = ar.fileName();
    TIFF* tiff = TIFFClientOpen(fileName.c_str(), "r", in,
                  strigi_tiffReadProc, strigi_tiffWriteProc, strigi_tiffSeekProc,
                  strigi_tiffCloseProc, strigi_tiffSizeProc,
                  strigi_tiffMapProc, strigi_tiffUnmapProc);
    if (!tiff) {
        return -1;
    }

    ar.addValue(factory->typeField, "http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#RasterImage");

    // simple fields
    readTiffTagUint32(tiff, TIFFTAG_IMAGEWIDTH, ar, factory->widthField);
    readTiffTagUint32(tiff, TIFFTAG_IMAGELENGTH, ar, factory->heightField);
    readTiffTagString(tiff, TIFFTAG_COPYRIGHT, ar, factory->copyrightField);
    readTiffTagString(tiff, TIFFTAG_IMAGEDESCRIPTION, ar, factory->descriptionField);
    // readTiffTagUint16(tiff, TIFFTAG_SAMPLESPERPIXEL, ar, factory->samplesPerPixelField);
    readTiffTagString(tiff, TIFFTAG_SOFTWARE, ar, factory->softwareField);
    readTiffTagString(tiff, TIFFTAG_ARTIST, ar, factory->artistField);
    readTiffTagDateTime(tiff, TIFFTAG_DATETIME, ar, factory->dateTimeField);
    readTiffTagUint16(tiff, TIFFTAG_BITSPERSAMPLE, ar, factory->bitsPerSampleField);

    // X and Y resolutions
    float xResolution = 0, yResolution = 0;
    TIFFGetField(tiff, TIFFTAG_XRESOLUTION, &xResolution);
    TIFFGetField(tiff, TIFFTAG_YRESOLUTION, &yResolution);
    uint16 resUnit = 0;
    TIFFGetFieldDefaulted(tiff, TIFFTAG_RESOLUTIONUNIT, &resUnit);
    switch (resUnit) {
    case RESUNIT_CENTIMETER:
        xResolution *= 2.54;
        yResolution *= 2.54;
        break;
    case RESUNIT_NONE:
        xResolution = 0;
        yResolution = 0;
        break;
    }
    if (xResolution > 0 && xResolution > 0) {
        ar.addValue(factory->xResolutionField, int(xResolution));
        ar.addValue(factory->yResolutionField, int(yResolution));
    }

    TIFFClose(tiff);

    return 0;
}

class Factory : public AnalyzerFactoryFactory {
public:
    std::list<StreamEndAnalyzerFactory*>
    streamEndAnalyzerFactories() const {
        std::list<StreamEndAnalyzerFactory*> af;
        af.push_back(new TiffEndAnalyzerFactory());
        return af;
    }
};

STRIGI_ANALYZER_FACTORY(Factory)
