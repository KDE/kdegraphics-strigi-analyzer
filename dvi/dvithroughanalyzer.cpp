/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2002 Matthias Witzgall <witzi@gmx.net>
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
#define STRIGI_IMPORT_API
#include <strigi/jstreamsconfig.h>
#include <strigi/analyzerplugin.h>
#include <strigi/streamthroughanalyzer.h>
#include <strigi/indexable.h>
#include <strigi/cnstr.h>
#include <strigi/fieldtypes.h>

using namespace jstreams;
using namespace std;

class DviThroughAnalyzerFactory;
class STRIGI_PLUGIN_API DviThroughAnalyzer : public StreamThroughAnalyzer {
private:
    Indexable* indexable;
    const DviThroughAnalyzerFactory* factory;
public:
    DviThroughAnalyzer(const DviThroughAnalyzerFactory* f) :factory(f) {}
    ~DviThroughAnalyzer() {}
    void setIndexable(jstreams::Indexable* i) { indexable = i; }
    InputStream *connectInputStream(InputStream *in);
    /* we only read the header so we are ready immediately */
    bool isReadyWithStream() { return true; }
};

class STRIGI_PLUGIN_API DviThroughAnalyzerFactory : public StreamThroughAnalyzerFactory {
friend class DviThroughAnalyzer;
private:
    const char* getName() const {
        return "DviThroughAnalyzer";
    }
    StreamThroughAnalyzer* newInstance() const {
        return new DviThroughAnalyzer(this);
    }
    void registerFields(FieldRegister& );
    static const cnstr commentFieldName;
    static const cnstr pagesFieldName;
    const RegisteredField* commentField;
    const RegisteredField* pagesField;
};

const cnstr DviThroughAnalyzerFactory::commentFieldName("comment");
const cnstr DviThroughAnalyzerFactory::pagesFieldName("pages");

void
DviThroughAnalyzerFactory::registerFields(FieldRegister& r) {
    commentField = r.registerField(commentFieldName, FieldRegister::stringType,
        1, 0);
    pagesField = r.registerField(pagesFieldName, FieldRegister::integerType,
        1, 0);
}

InputStream*
DviThroughAnalyzer::connectInputStream(InputStream* in) {
    // read the header
    const char* c;
    int32_t nread = in->read(c, 270, 270);
    in->reset(0);
    if (nread < 270) {
        return in;
    }
    // check the magic bytes (remember: all files pass through here)
    const unsigned char* buffer = (const unsigned char*)c;
    if (buffer[0] != 247  || buffer[1] != 2) {
        // this file is not a DVI file
        return in;
    }
    unsigned char bufferLength = buffer[14];
    string comment((const char*)buffer+15, bufferLength);
    indexable->setField(factory->commentField, comment);

    // TODO: extract the number of pages
    // this is tricky because we need to get the data from the end of the stream
    // a general purpose event driven stream implementation is required for that

    return in;
}

class Factory : public AnalyzerFactoryFactory {
public:
    list<StreamThroughAnalyzerFactory*>
    getStreamThroughAnalyzerFactories() const {
        list<StreamThroughAnalyzerFactory*> af;
        af.push_back(new DviThroughAnalyzerFactory());
        return af;
    }
};

STRIGI_ANALYZER_FACTORY(Factory)
