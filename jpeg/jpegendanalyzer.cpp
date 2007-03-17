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
    static const cnstr creationTimeFieldName;
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
};

const cnstr JpegEndAnalyzerFactory::commentFieldName("comment");

/*
 Register the field names so that the StreamIndexer knows which analyzer
 provides what information.
*/
void
JpegEndAnalyzerFactory::registerFields(FieldRegister& r) {
    commentField = r.registerField(commentFieldName, FieldRegister::stringType,
        -1, 0);
}
bool
JpegEndAnalyzer::checkHeader(const char* header, int32_t headersize) const {
    // _very_ simple check to filter out the worst
    return headersize > 1 && header[0] == '%';
}
char
JpegEndAnalyzer::analyze(AnalysisResult& ar, ::InputStream*) {
    return -1;
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
