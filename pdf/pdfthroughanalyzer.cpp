/* This file is part of the KDE project
*  Copyright (C) 2001, 2002 Rolf Magnus <ramagnus@kde.org>
 * Copyright (C) 2007 Matthias Lechner
 * Copyright (C) 2007 Albert Astals Cid <aacid@kde.org>
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

#define STRIGI_IMPORT_API
#include <strigi/streamthroughanalyzer.h>
#include <strigi/analyzerplugin.h>
#include <strigi/fieldtypes.h>
#include <strigi/analysisresult.h>

#include <klocalizedstring.h>

// poppler includes
#define UNSTABLE_POPPLER_QT4
#include <poppler-qt4.h>

class PDFThroughAnalyzerFactory;

class PDFThroughAnalyzer : public Strigi::StreamThroughAnalyzer {
    private:
        const PDFThroughAnalyzerFactory* factory;
        Strigi::AnalysisResult* idx;

    public:
        PDFThroughAnalyzer( const PDFThroughAnalyzerFactory* f ) : factory( f ) {}

        void setIndexable( Strigi::AnalysisResult *i ) {
            idx = i;
        }
        Strigi::InputStream* connectInputStream( Strigi::InputStream *in );
        bool isReadyWithStream() { return true; }
};

class PDFThroughAnalyzerFactory : public Strigi::StreamThroughAnalyzerFactory {
friend class PDFThroughAnalyzer;
private:
    const char* name() const {
        return "PDFThroughAnalyzer";
    }
    Strigi::StreamThroughAnalyzer* newInstance() const {
        return new PDFThroughAnalyzer(this);
    }
    void registerFields( Strigi::FieldRegister& );
    
    static const std::string titleFieldName;
    static const std::string subjectFieldName;
    static const std::string authorFieldName;
    static const std::string keywordsFieldName;
    static const std::string creatorFieldName;
    static const std::string producerFieldName;
    static const std::string creationDateFieldName;
    static const std::string modificationDateFieldName;
    static const std::string pagesFieldName;
    static const std::string protectedFieldName;
    static const std::string linearizedFieldName;
    static const std::string versionFieldName;
public:
    const Strigi::RegisteredField* titleField;
    const Strigi::RegisteredField* subjectField;
    const Strigi::RegisteredField* authorField;
    const Strigi::RegisteredField* keywordsField;
    const Strigi::RegisteredField* creatorField;
    const Strigi::RegisteredField* producerField;
    const Strigi::RegisteredField* creationDateField;
    const Strigi::RegisteredField* modificationDateField;
    const Strigi::RegisteredField* pagesField;
    const Strigi::RegisteredField* protectedField;
    const Strigi::RegisteredField* linearizedField;
    const Strigi::RegisteredField* versionField;
};

const std::string PDFThroughAnalyzerFactory::titleFieldName( "content.title" );
const std::string PDFThroughAnalyzerFactory::subjectFieldName( "content.subject" );
const std::string PDFThroughAnalyzerFactory::authorFieldName( "content.author" );
const std::string PDFThroughAnalyzerFactory::keywordsFieldName( "content.keywords" );
const std::string PDFThroughAnalyzerFactory::creatorFieldName( "creator" );
const std::string PDFThroughAnalyzerFactory::producerFieldName( "producer" );
const std::string PDFThroughAnalyzerFactory::creationDateFieldName( "content.creation_time" );
const std::string PDFThroughAnalyzerFactory::modificationDateFieldName( "content.last_modified_time" );
const std::string PDFThroughAnalyzerFactory::pagesFieldName( "document.stats.page_count" );
const std::string PDFThroughAnalyzerFactory::protectedFieldName( "content.comment" );
const std::string PDFThroughAnalyzerFactory::linearizedFieldName( "linearized" );
const std::string PDFThroughAnalyzerFactory::versionFieldName( "content.version" );

void PDFThroughAnalyzerFactory::registerFields( Strigi::FieldRegister& reg ) {
    titleField = reg.registerField( titleFieldName, Strigi::FieldRegister::stringType, 1, 0 );
    subjectField = reg.registerField( subjectFieldName, Strigi::FieldRegister::stringType, 1, 0 );
    authorField = reg.registerField( authorFieldName, Strigi::FieldRegister::stringType, 1, 0 );
    keywordsField = reg.registerField( keywordsFieldName, Strigi::FieldRegister::stringType, 1, 0 );
    creatorField = reg.registerField( creatorFieldName, Strigi::FieldRegister::stringType, 1, 0 );
    producerField = reg.registerField( producerFieldName, Strigi::FieldRegister::stringType, 1, 0 );
    creationDateField = reg.registerField( creationDateFieldName, Strigi::FieldRegister::integerType, 1, 0 );
    modificationDateField = reg.registerField( modificationDateFieldName, Strigi::FieldRegister::integerType, 1, 0 );
    pagesField = reg.registerField( pagesFieldName, Strigi::FieldRegister::integerType, 1, 0 );
    protectedField = reg.registerField( protectedFieldName, Strigi::FieldRegister::stringType, 1, 0 );
    linearizedField = reg.registerField( linearizedFieldName, Strigi::FieldRegister::stringType, 1, 0 );
    versionField = reg.registerField( versionFieldName, Strigi::FieldRegister::stringType, 1, 0 );
}

Strigi::InputStream* PDFThroughAnalyzer::connectInputStream( Strigi::InputStream* in ) {
   if( !in )
        return in;

    if( in->size() == -1 )
        return in;

    // read from stream and store the results in memory
    const char *c;
    int32_t nread = in->read(c, in->size(), in->size() );
    in->reset( 0 );
    if( nread == -2 )
        return in;

    QByteArray ba(c, in->size());
    Poppler::Document *pdfDocument = Poppler::Document::loadFromData(ba);

    if (!pdfDocument || pdfDocument->isLocked())
    {
        delete pdfDocument;
        return in;
    }

    QString enc;
    if( pdfDocument->isEncrypted() ) {
        enc = i18n( "Yes (Can Print:%1 Can Copy:%2 Can Change:%3 Can Add notes:%4)",
        pdfDocument->okToPrint() ? i18n("Yes") : i18n("No"),
        pdfDocument->okToCopy() ? i18n("Yes") : i18n("No"),
        pdfDocument->okToChange() ? i18n("Yes") : i18n("No"),
        pdfDocument->okToAddNotes() ? i18n("Yes") : i18n("No"));
    } else
        enc = i18n( "No" );

    idx->addValue( factory->protectedField, enc.toUtf8().constData() );


    idx->addValue( factory->titleField, pdfDocument->info( "Title" ).toUtf8().constData() );
    idx->addValue( factory->subjectField, pdfDocument->info( "Subject" ).toUtf8().constData() );
    idx->addValue( factory->authorField, pdfDocument->info( "Author" ).toUtf8().constData() );
    idx->addValue( factory->keywordsField, pdfDocument->info( "Keywords" ).toUtf8().constData() );
    idx->addValue( factory->creatorField, pdfDocument->info( "Creator" ).toUtf8().constData() );
    idx->addValue( factory->producerField, pdfDocument->info( "Producer" ).toUtf8().constData() );

    idx->addValue( factory->creationDateField, pdfDocument->date( "CreationDate" ).toTime_t() );
    idx->addValue( factory->modificationDateField, pdfDocument->date( "ModDate").toTime_t() );

    idx->addValue( factory->pagesField, pdfDocument->numPages() );

    QString linearized;
    if( pdfDocument->isLinearized() )
        linearized = i18n( "Yes" );
    else
        linearized = i18n( "No" );

    idx->addValue( factory->linearizedField, linearized.toUtf8().constData() );

    idx->addValue( factory->versionField, pdfDocument->pdfVersion() );

    delete pdfDocument;

    return in;
}

class Factory : public Strigi::AnalyzerFactoryFactory {
public:
    std::list<Strigi::StreamThroughAnalyzerFactory*>
    streamThroughAnalyzerFactories() const {
        std::list<Strigi::StreamThroughAnalyzerFactory*> af;
        af.push_back(new PDFThroughAnalyzerFactory());
        return af;
    }
};

STRIGI_ANALYZER_FACTORY(Factory) 
