/* This file is part of the KDE project
 * Copyright (C) 2002 Shane Wright <me@shanewright.co.uk>
 * Copyright (C) 2007 Matthias Lechner <matthias@lmme.de>
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

#include <QByteArray>
#include <QDataStream>

using namespace Strigi;
using namespace std;

class IcoThroughAnalyzerFactory;
class IcoThroughAnalyzer : public StreamThroughAnalyzer {
    private:
        const IcoThroughAnalyzerFactory* factory;
        AnalysisResult* idx;

        void setIndexable( AnalysisResult *i ) {
            idx = i;
        }
        InputStream* connectInputStream( InputStream *in );
        bool isReadyWithStream() { return true; }
        const char* name() const {
          return "IcoThroughAnalyzer";
        }

    public:
        IcoThroughAnalyzer( const IcoThroughAnalyzerFactory* f ) : factory( f ) {}
};

class IcoThroughAnalyzerFactory : public StreamThroughAnalyzerFactory {
private:
    const char* name() const {
        return "IcoThroughAnalyzer";
    }
    StreamThroughAnalyzer* newInstance() const {
        return new IcoThroughAnalyzer(this);
    }
    void registerFields( FieldRegister& );

    static const string numberFieldName;
    static const string widthFieldName;
    static const string heightFieldName;
    static const string bitsPerPixelFieldName;
    static const string colorCountFieldName;
public:
    const RegisteredField* numberField;
    const RegisteredField* widthField;
    const RegisteredField* heightField;
    const RegisteredField* bitsPerPixelField;
    const RegisteredField* colorCountField;
};

const string IcoThroughAnalyzerFactory::numberFieldName( "document.stats.image_count" );
const string IcoThroughAnalyzerFactory::widthFieldName( "image.width" );
const string IcoThroughAnalyzerFactory::heightFieldName( "image.height" );
const string IcoThroughAnalyzerFactory::bitsPerPixelFieldName( "image.color_depth" );
const string IcoThroughAnalyzerFactory::colorCountFieldName( "image.color_count" );

void IcoThroughAnalyzerFactory::registerFields( FieldRegister& reg ) {
    numberField = reg.registerField( numberFieldName, FieldRegister::integerType, 1, 0 );
    widthField = reg.registerField( widthFieldName, FieldRegister::integerType, 1, 0 );
    heightField = reg.registerField( heightFieldName, FieldRegister::integerType, 1, 0 );
    bitsPerPixelField = reg.registerField( bitsPerPixelFieldName, FieldRegister::integerType, 1, 0 );
    colorCountField = reg.registerField( colorCountFieldName, FieldRegister::integerType, 1, 0 );

}

InputStream* IcoThroughAnalyzer::connectInputStream( InputStream* in ) {
    if( !in )
        return in;

    const char *c;
    int32_t nread = in->read( c, in->size(), in->size() );
    in->reset( 0 );
    if( nread == -2 )
        return in;

    QByteArray buffer( c, in->size() );
    QDataStream dstream( buffer );

    // ICO files are little-endian
    dstream.setByteOrder( QDataStream::LittleEndian );

    // read the beginning of the stream and make sure it looks ok
    uint16_t ico_reserved;
    uint16_t ico_type;
    uint16_t ico_count;

    dstream >> ico_reserved;
    dstream >> ico_type;
    dstream >> ico_count;

    if ((ico_reserved != 0) || (ico_type != 1) || (ico_count < 1))
        return in;


    // now loop through each of the icon entries
    uint8_t icoe_width;
    uint8_t icoe_height;
    uint8_t icoe_colorcount;
    uint8_t icoe_reserved;
    uint16_t icoe_planes;
    uint16_t icoe_bitcount;
    uint32_t icoe_bytesinres;
    uint32_t icoe_imageoffset;

    // read the data on the 1st icon
    dstream >> icoe_width;
    dstream >> icoe_height;
    dstream >> icoe_colorcount;
    dstream >> icoe_reserved;
    dstream >> icoe_planes;
    dstream >> icoe_bitcount;
    dstream >> icoe_bytesinres;
    dstream >> icoe_imageoffset;

    idx->addValue( factory->numberField, ico_count );

    idx->addValue( factory->widthField, icoe_width );
    idx->addValue( factory->heightField, icoe_height );

    if (icoe_bitcount > 0)
        idx->addValue( factory->bitsPerPixelField, icoe_bitcount );

    if (icoe_colorcount > 0)
        idx->addValue( factory->colorCountField, icoe_colorcount );
    else if (icoe_bitcount > 0)
        idx->addValue( factory->colorCountField, 2 ^ icoe_bitcount );

    return in;
}

class Factory : public AnalyzerFactoryFactory {
public:
    std::list<StreamThroughAnalyzerFactory*>
    getStreamThroughAnalyzerFactories() const {
        std::list<StreamThroughAnalyzerFactory*> af;
        af.push_back(new IcoThroughAnalyzerFactory());
        return af;
    }
};

STRIGI_ANALYZER_FACTORY(Factory) 
