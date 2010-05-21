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

/*
 Include the strigi specific headers.
*/
#define STRIGI_IMPORT_API
#include <strigi/analyzerplugin.h>
#include <strigi/streamendanalyzer.h>
#include <strigi/analysisresult.h>
#include <strigi/fieldtypes.h>

using namespace Strigi;
using namespace std;

/*
 Declare the factory.
*/
class DviEndAnalyzerFactory;

/*
Define a class that inherits from StreamEndAnalyzer.
The only function we really need to implement is connectInputStream()
*/
class STRIGI_PLUGIN_API DviEndAnalyzer : public StreamEndAnalyzer {
private:
    const DviEndAnalyzerFactory* factory;
    const char* name() const {
        return "DviEndAnalyzer";
    }
public:
    DviEndAnalyzer(const DviEndAnalyzerFactory* f) :factory(f) {}
    ~DviEndAnalyzer() {}
    bool checkHeader(const char *header, int32_t headersize) const;
    signed char analyze(Strigi::AnalysisResult &idx, InputStream *in);
};

/*
 Define a factory class the provides information about the fields that an
 analyzer can extract. This has a function similar to KFilePlugin::addItemInfo.
*/
class STRIGI_PLUGIN_API DviEndAnalyzerFactory : public StreamEndAnalyzerFactory {
friend class DviEndAnalyzer;
private:
    const char* name() const {
        return "DviEndAnalyzer";
    }
    /* This is why this class is a factory. */
    StreamEndAnalyzer* newInstance() const {
        return new DviEndAnalyzer(this);
    }
    void registerFields(FieldRegister& );

    /* The RegisteredField instances are used to index specific fields quickly.
       We pass a pointer to the instance instead of a string.
    */
    const RegisteredField* commentField;
    const RegisteredField* pagesField;
};

/*
 Register the field names so that the StreamIndexer knows which analyzer
 provides what information.
*/
void
DviEndAnalyzerFactory::registerFields(FieldRegister& r) {
    commentField = r.registerField("http://www.semanticdesktop.org/ontologies/2007/01/19/nie#comment");
    pagesField = r.registerField("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#pageCount");
}

bool
DviEndAnalyzer::checkHeader(const char *header, int32_t headersize) const {
    if (headersize < 270) {
        return false;
    }
    // check the magic bytes (remember: all files pass through here)
    const unsigned char* buffer = (const unsigned char*)header;
    if (buffer[0] != 247  || buffer[1] != 2) {
        // this file is not a DVI file
        return false;
    }
    return true;
}

signed char
DviEndAnalyzer::analyze(AnalysisResult &idx, InputStream *in) {
    // read the header
    const char* c;
    int32_t nread = in->read(c, 270, 270);
    if (nread != 270) return -1;
    const unsigned char* buffer = (const unsigned char*)c;
    unsigned char bufferLength = buffer[14];
    string comment((const char*)buffer+15, bufferLength);
    idx.addValue(factory->commentField, comment);

    // now get total number of pages
    const int64_t size = in->size();
    if (size < 0) return 0; // the size is unknown, so reading cannot continue
                            // at the end, this is not an error
    if (in->reset(size - 13) != size - 13) return -1;
    nread = in->read(c, 13, 13);
    if (nread != 13) {
        return -1;
    }

    int i = 12; // reset running index i
    buffer = (const unsigned char*)c;
    while (i >= 4 && buffer[i] == 223) {
        --i;
    } // skip all trailing bytes

    if (i <= 4 || (buffer[i] != 2) || (i > 8) || (i < 5)) {
        // wrong file formatx
        return -1;
    }

    // now we know the position of the pointer to the beginning of the postamble and we can read it
    uint32_t ptr = buffer[i - 4];
    ptr = (ptr << 8) | buffer[i - 3];
    ptr = (ptr << 8) | buffer[i - 2];
    ptr = (ptr << 8) | buffer[i - 1];

    // bytes for total number of pages have a offset of 27 to the beginning of the postamble
    if (in->reset(ptr + 27) != ptr + 27) return -1;

    // now read total number of pages from file
    nread = in->read(c, 2, 2);
    if (nread != 2) {
        // read error (3)
        return -1;
    }

    buffer = (const unsigned char*)c;
    uint16_t pages = buffer[0];
    pages = (pages << 8) | buffer[1];

    idx.addValue(factory->pagesField, pages);

    return 0;
}

/*
 For plugins, we need to have a way to find out which plugins are defined in a
 plugin. One instance of AnalyzerFactoryFactory per plugin profides this
 information.
*/
class Factory : public AnalyzerFactoryFactory {
public:
    list<StreamEndAnalyzerFactory*>
    streamEndAnalyzerFactories() const {
        list<StreamEndAnalyzerFactory*> af;
        af.push_back(new DviEndAnalyzerFactory());
        return af;
    }
};

/*
 Register the AnalyzerFactoryFactory
*/
STRIGI_ANALYZER_FACTORY(Factory)
