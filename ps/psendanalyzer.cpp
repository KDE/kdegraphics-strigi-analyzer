/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2002 Wilco Greven <greven@kde.org>
 * Copyright (C) 2007 Jos van den Oever <jos@vandenoever.info>
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

/*
 Include the strigi specific headers.
*/
#define STRIGI_IMPORT_API
#include <strigi/analyzerplugin.h>
#include <strigi/streamendanalyzer.h>
#include <strigi/analysisresult.h>
#include <strigi/fieldtypes.h>
#include <QFile>
#include "dscparse_adapter.h"

using namespace Strigi;
using namespace std;

/*
 Declare the factory.
*/
class PsEndAnalyzerFactory;

/*
Define a class that inherits from StreamEndAnalyzer.
The only function we really need to implement is connectInputStream()
*/
class STRIGI_PLUGIN_API PsEndAnalyzer : public StreamEndAnalyzer,
           public KDSCCommentHandler {
private:
    AnalysisResult* result;
    const PsEndAnalyzerFactory* factory;

#warning KDSC must be linked, it must be moved into a shared library
// this line breaks linking
//    KDSC dsc;
    bool endComments;
    int setData;

    void comment( Name );
public:
    PsEndAnalyzer(const PsEndAnalyzerFactory* f) :factory(f) {}
    ~PsEndAnalyzer() {}
    const char* name() const {
        return "PsEndAnalyzer";
    }
    bool checkHeader(const char* header, int32_t headersize) const;
    char analyze(AnalysisResult& idx, ::InputStream* in);
};

/*
 Define a factory class the provides information about the fields that an
 analyzer can extract. This has a function similar to KFilePlugin::addItemInfo.
*/
class STRIGI_PLUGIN_API PsEndAnalyzerFactory : public StreamEndAnalyzerFactory {
friend class PsEndAnalyzer;
private:
    /* This is why this class is a factory. */
    StreamEndAnalyzer* newInstance() const {
        return new PsEndAnalyzer(this);
    }
    const char* name() const {
        return "PsEndAnalyzer";
    }
    void registerFields(FieldRegister& );

    /* define static fields that contain the field names. */
    static const string titleFieldName;
    static const string creatorFieldName;
    static const string createdateFieldName;
    static const string forFieldName;
    static const string pagesFieldName;

    /* The RegisteredField instances are used to index specific fields quickly.
       We pass a pointer to the instance instead of a string.
    */
    const RegisteredField* titleField;
    const RegisteredField* creatorField;
    const RegisteredField* createdateField;
    const RegisteredField* forField;
    const RegisteredField* pagesField;
};

const string PsEndAnalyzerFactory::titleFieldName("content.title");
const string PsEndAnalyzerFactory::creatorFieldName("creator");
const string PsEndAnalyzerFactory::createdateFieldName("content.creation_time");
const string PsEndAnalyzerFactory::forFieldName("for");
const string PsEndAnalyzerFactory::pagesFieldName("document.stats.page_count");

/*
 Register the field names so that the StreamIndexer knows which analyzer
 provides what information.
*/
void
PsEndAnalyzerFactory::registerFields(FieldRegister& r) {
    titleField = r.registerField(titleFieldName, FieldRegister::stringType,
        1, 0);
    creatorField = r.registerField(titleFieldName, FieldRegister::stringType,
        1, 0);
    createdateField = r.registerField(createdateFieldName,
        FieldRegister::stringType, 1, 0);
    forField = r.registerField(forFieldName, FieldRegister::stringType,
        1, 0);
    pagesField = r.registerField(pagesFieldName, FieldRegister::integerType,
        1, 0);
}
bool
PsEndAnalyzer::checkHeader(const char* header, int32_t headersize) const {
    // _very_ simple check to filter out the worst
    return headersize > 1 && header[0] == '%';
}

void
PsEndAnalyzer::comment(Name name) {
#warning KDSC must be linked, it must be moved into a shared library
// this block breaks linking
/*    switch( name )
    {
    case Title:
        result->setField(factory->titleField,
            (const char*)dsc.dsc_title().toUtf8());
	++setData;
    break;
    case Creator:
        result->setField(factory->creatorField,
            (const char*)dsc.dsc_creator().toUtf8());
	++setData;
    break;
    case CreationDate:
        result->setField(factory->createdateField,
            (const char*)dsc.dsc_date().toUtf8());
	++setData;
    break;
    case For:
        result->setField(factory->forField,
            (const char*)dsc.dsc_for().toUtf8());
	++setData;
    break;
    case Pages: {
        int pages = dsc.page_pages();
        if (pages) {
            result->setField(factory->pagesField, pages);
	    ++setData;
        }
    }
    break;
    
    // Right now we watch for 5 elements:
    //  Title, Creator, CreationDate, For, Pages
    //
    // If you add another one(s), please update the 5 in "_setData == 5" above
    //
    case EndComments: endComments = true;
    default: ; // Ignore
    }*/
}

char
PsEndAnalyzer::analyze(AnalysisResult& ar, ::InputStream*) {
    // we can only read real files (not streams) with this plugin
    if (ar.depth()) {
        return -1;
    }
    endComments = false;
    setData = 0;

#warning KDSC must be linked, it must be moved into a shared library
// this line breaks linking
//    dsc.setCommentHandler( this );
    QString path(QString::fromUtf8(ar.path().c_str()));
    FILE* fp = fopen( QFile::encodeName(path ), "r" );
    if (fp == 0) {
        return -1;
    }
    
    char buf[4096];
    int count;
    while( ( count = fread( buf, sizeof(char), sizeof( buf ), fp ) ) ) {
#warning KDSC must be linked, it must be moved into a shared library
// this line breaks linking
//	    if ( !dsc.scanData( buf, count ) ) break;
	    if ( endComments || setData == 5 ) break; // Change if new item scanned
    }
    fclose( fp );

    return (setData > 0) ?0 :-1;
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
        af.push_back(new PsEndAnalyzerFactory());
        return af;
    }
};

/*
 Register the AnalyzerFactoryFactory
*/
STRIGI_ANALYZER_FACTORY(Factory)
