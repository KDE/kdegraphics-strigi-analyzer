/* This file is part of the KDE project
*  Copyright (C) 2001, 2002 Rolf Magnus <ramagnus@kde.org>
 * Copyright (C) 2007 Matthias Lechner
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
#include <strigi/cnstr.h>

#include <klocalizedstring.h>

// poppler includes
#include <poppler/goo/GooString.h>
#include <poppler/PDFDoc.h>
#include <poppler/ErrorCodes.h>
#include <poppler/Dict.h>
#include <poppler/Stream.h>

// needed for date/time conversion
#include <ctime>

using namespace Strigi;
//using namespace std;

class PDFThroughAnalyzerFactory;
class PDFThroughAnalyzer : public StreamThroughAnalyzer {
    private:
        const PDFThroughAnalyzerFactory* factory;
        AnalysisResult* idx;

        void setIndexable( AnalysisResult *i ) {
            idx = i;
        }
        jstreams::InputStream* connectInputStream( jstreams::InputStream *in );
        bool isReadyWithStream() { return true; }

        PDFDoc *m_pdfDocument;

        time_t convertDate( std::string date );
        std::string info( const std::string& key );
    public:
        PDFThroughAnalyzer( const PDFThroughAnalyzerFactory* f ) : factory( f ) {}
};

class PDFThroughAnalyzerFactory : public StreamThroughAnalyzerFactory {
private:
    const char* getName() const {
        return "PDFThroughAnalyzer";
    }
    StreamThroughAnalyzer* newInstance() const {
        return new PDFThroughAnalyzer(this);
    }
    void registerFields( FieldRegister& );
    static const cnstr titleFieldName;
    static const cnstr subjectFieldName;
    static const cnstr authorFieldName;
    static const cnstr keywordsFieldName;
    static const cnstr creatorFieldName;
    static const cnstr producerFieldName;
    static const cnstr creationDateFieldName;
    static const cnstr modificationDateFieldName;
    static const cnstr pagesFieldName;
    static const cnstr protectedFieldName;
    static const cnstr linearizedFieldName;
    static const cnstr versionFieldName;
public:
    const RegisteredField* titleField;
    const RegisteredField* subjectField;
    const RegisteredField* authorField;
    const RegisteredField* keywordsField;
    const RegisteredField* creatorField;
    const RegisteredField* producerField;
    const RegisteredField* creationDateField;
    const RegisteredField* modificationDateField;
    const RegisteredField* pagesField;
    const RegisteredField* protectedField;
    const RegisteredField* linearizedField;
    const RegisteredField* versionField;
};

const cnstr PDFThroughAnalyzerFactory::titleFieldName( "title" );
const cnstr PDFThroughAnalyzerFactory::subjectFieldName( "subject" );
const cnstr PDFThroughAnalyzerFactory::authorFieldName( "author" );
const cnstr PDFThroughAnalyzerFactory::keywordsFieldName( "keywords" );
const cnstr PDFThroughAnalyzerFactory::creatorFieldName( "creator" );
const cnstr PDFThroughAnalyzerFactory::producerFieldName( "producer" );
const cnstr PDFThroughAnalyzerFactory::creationDateFieldName( "creationdate" );
const cnstr PDFThroughAnalyzerFactory::modificationDateFieldName( "modificationdate" );
const cnstr PDFThroughAnalyzerFactory::pagesFieldName( "pages" );
const cnstr PDFThroughAnalyzerFactory::protectedFieldName( "protected" );
const cnstr PDFThroughAnalyzerFactory::linearizedFieldName( "linearized" );
const cnstr PDFThroughAnalyzerFactory::versionFieldName( "version" );

void PDFThroughAnalyzerFactory::registerFields( FieldRegister& reg ) {
    titleField = reg.registerField( titleFieldName, FieldRegister::stringType, 1, 0 );
    subjectField = reg.registerField( subjectFieldName, FieldRegister::stringType, 1, 0 );
    authorField = reg.registerField( authorFieldName, FieldRegister::stringType, 1, 0 );
    keywordsField = reg.registerField( keywordsFieldName, FieldRegister::stringType, 1, 0 );
    creatorField = reg.registerField( creatorFieldName, FieldRegister::stringType, 1, 0 );
    producerField = reg.registerField( producerFieldName, FieldRegister::stringType, 1, 0 );
    creationDateField = reg.registerField( creationDateFieldName, FieldRegister::integerType, 1, 0 );
    modificationDateField = reg.registerField( modificationDateFieldName, FieldRegister::integerType, 1, 0 );
    pagesField = reg.registerField( pagesFieldName, FieldRegister::integerType, 1, 0 );
    protectedField = reg.registerField( protectedFieldName, FieldRegister::stringType, 1, 0 );
    linearizedField = reg.registerField( linearizedFieldName, FieldRegister::stringType, 1, 0 );
    versionField = reg.registerField( versionFieldName, FieldRegister::stringType, 1, 0 );
}

jstreams::InputStream* PDFThroughAnalyzer::connectInputStream( jstreams::InputStream* in ) {
    if( !in )
        return in;

    if( in->getSize() == -1 )
        return in;

    // read from stream and store the results in memory
    const char *c;
    int32_t nread = in->read(c, in->getSize(), in->getSize() );
    in->reset( 0 );
    if( nread == -2 )
        return in;

    // check if file is pdf
    if( in->getSize() > 4 ) {
        if( strncmp( c, "%PDF", 4 ) )
            return in;
    } else
        return in;

    // create a poppler MemStream out of the read stream
    char *buffer = (char*) c;
    Object obj;
    obj.initNull();
    MemStream *memStream = new MemStream( buffer, 0, in->getSize(), &obj );

    // load document
    m_pdfDocument = new PDFDoc( memStream, (GooString*) 0, (GooString*) 0, &obj );

    if( !(m_pdfDocument->isOk() || m_pdfDocument->getErrorCode() == errEncrypted) ) {
        // could not open pdf document
        delete m_pdfDocument;
        return in;
    }

    QString enc;
    if( m_pdfDocument->isEncrypted() ) {
        enc = i18n( "Yes (Can Print:%1 Can Copy:%2 Can Change:%3 Can Add notes:%4)",
        m_pdfDocument->okToPrint() ? i18n("Yes") : i18n("No"),
        m_pdfDocument->okToCopy() ? i18n("Yes") : i18n("No"),
        m_pdfDocument->okToChange() ? i18n("Yes") : i18n("No"),
        m_pdfDocument->okToAddNotes() ? i18n("Yes") : i18n("No"));
    } else
        enc = i18n( "No" );

    idx->setField( factory->protectedField, (const char*) enc.toUtf8() );


    idx->setField( factory->titleField, info( "Title" ) );
    idx->setField( factory->subjectField, info( "Subject" ) );
    idx->setField( factory->authorField, info( "Author" ) );
    idx->setField( factory->keywordsField, info( "Keywords" ) );
    idx->setField( factory->creatorField, info( "Creator" ) );
    idx->setField( factory->producerField, info( "Producer" ) );

    idx->setField( factory->creationDateField,
                   (uint) convertDate( info( "CreationDate" ) ) );
    idx->setField( factory->modificationDateField,
                   (uint) convertDate( info( "ModDate") ) );

    idx->setField( factory->pagesField, m_pdfDocument->getNumPages() );

    QString linearized;
    if( m_pdfDocument->isLinearized() )
        linearized = i18n( "Yes" );
    else
        linearized = i18n( "No" );

    idx->setField( factory->linearizedField, (const char*) linearized.toUtf8() );

    idx->setField( factory->versionField, m_pdfDocument->getPDFVersion() );

    return in;
}

std::string PDFThroughAnalyzer::info( const std::string& key ) {
    Object info;
    m_pdfDocument->getDocInfo( &info );
    if ( !info.isDict() )
        return std::string();

    Object obj;
    Dict *infoDict = info.getDict();
    std::string result;

    if ( infoDict->lookup( key.c_str(), &obj )->isString() ) {
        GooString *s1;
        GBool isUnicode;
        Unicode u;
        int i;

        s1 = obj.getString();
        if ( ( s1->getChar(0) & 0xff ) == 0xfe && ( s1->getChar(1) & 0xff ) == 0xff ) {
            isUnicode = gTrue;
            i = 2;
        } else {
            isUnicode = gFalse;
            i = 0;
        }

        while ( i < obj.getString()->getLength() ) {
            if ( isUnicode ) {
                u = ( ( s1->getChar(i) & 0xff ) << 8 ) | ( s1->getChar(i+1) & 0xff );
                i += 2;
            } else {
                u = s1->getChar(i) & 0xff;
                ++i;
            }
            result += (const char) u;
        }

        obj.free();
        info.free();
        return result;
    }

    obj.free();
    info.free();
    return std::string();
}

// adapted from poppler-document.cc from the qt4 bindings of poppler
time_t PDFThroughAnalyzer::convertDate( std::string date ) {
    int year;
    int mon = 1;
    int day = 1;
    int hour = 0;
    int min = 0;
    int sec = 0;
    char tz = 0x00;
    int tzHours = 0;
    int tzMins = 0;

    if ( date[0] == 'D' && date[1] == ':' ) {
        date.erase( 0, 2 );
    }

    if ( sscanf( date.c_str(), "%4d%2d%2d%2d%2d%2d%c%2d%*c%2d",
                 &year, &mon, &day, &hour, &min, &sec,
                 &tz, &tzHours, &tzMins ) > 0 ) {
        /* Workaround for y2k bug in Distiller 3 stolen from gpdf, hoping that it won't
         * be used after y2.2k */
        if ( year < 1930 && date.size() > 14) {
            int century, years_since_1900;
            if ( sscanf( date.c_str(), "%2d%3d%2d%2d%2d%2d%2d",
                         &century, &years_since_1900,
                         &mon, &day, &hour, &min, &sec) == 7 )
                year = century * 100 + years_since_1900;
            else
                return 0;
        }

        struct tm dateTime;
        memset( &dateTime, 0, sizeof( dateTime ) );

        dateTime.tm_year = year - 1900;
        dateTime.tm_mon = mon - 1;
        dateTime.tm_mday = day;
        dateTime.tm_hour = hour;
        dateTime.tm_min = min;
        dateTime.tm_sec = sec;
        dateTime.tm_isdst = -1;
        dateTime.tm_gmtoff = 0;

        if ( tz ) {
            // we have some form of timezone
            if ( '+' == tz ) {
                // local time is ahead of UTC
                dateTime.tm_gmtoff -= tzHours * 60 * 60;
                dateTime.tm_gmtoff -= tzMins * 60;
            } else if ( '-' == tz ) {
                // local time is behind UTC
                dateTime.tm_gmtoff += tzHours * 60 * 60;
                dateTime.tm_gmtoff += tzMins * 60;
            }
        }

        return mktime( &dateTime );
    }
    return 0;
}

class Factory : public AnalyzerFactoryFactory {
public:
    std::list<StreamThroughAnalyzerFactory*>
    getStreamThroughAnalyzerFactories() const {
        std::list<StreamThroughAnalyzerFactory*> af;
        af.push_back(new PDFThroughAnalyzerFactory());
        return af;
    }
};

STRIGI_ANALYZER_FACTORY(Factory) 
