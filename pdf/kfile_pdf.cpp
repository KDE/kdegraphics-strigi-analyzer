/* This file is part of the KDE project
 * Copyright (C) 2001, 2002 Rolf Magnus <ramagnus@kde.org>
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
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 *  $Id$
 */

#include "kfile_pdf.h"

#include <kurl.h>
#include <kgenericfactory.h>
#include <kdebug.h>

#include <qcstring.h>
#include <qfile.h>
#include <qdatetime.h>
#include <qdict.h>
#include <qvalidator.h>

#include <goo/gmem.h>
#include <ErrorCodes.h>
#include <UnicodeMap.h>
#include "GlobalParams.h"

typedef KGenericFactory<KPdfPlugin> PdfFactory;

K_EXPORT_COMPONENT_FACTORY(kfile_pdf, PdfFactory("kfile_pdf"))

KPdfPlugin::KPdfPlugin(QObject *parent, const char *name, const QStringList &preferredItems)
    : KFilePlugin(parent, name, preferredItems)
{
    kdDebug(7034) << "pdf plugin\n";

    // set up our mime type
    KFileMimeTypeInfo* info = addMimeTypeInfo( "application/pdf" );

    // general group
    KFileMimeTypeInfo::GroupInfo* group = addGroupInfo(info, "General", i18n("General"));

    KFileMimeTypeInfo::ItemInfo* item;

    item = addItemInfo(group, "Title", i18n("Title"), QVariant::String);
    setHint(item, KFileMimeTypeInfo::Name);
    item = addItemInfo(group, "Subject", i18n("Subject"), QVariant::String);
    setHint(item, KFileMimeTypeInfo::Description);
    item = addItemInfo(group, "Author", i18n("Author"), QVariant::String);
    setHint(item, KFileMimeTypeInfo::Author);
    addItemInfo(group, "Keywords", i18n("Key Words"), QVariant::String);
    addItemInfo(group, "Creator", i18n("Creator"), QVariant::String);
    addItemInfo(group, "Producer", i18n("Producer"), QVariant::String);
    addItemInfo(group, "CreationDate", i18n("Creation Date"), QVariant::DateTime);
    addItemInfo(group, "ModificationDate", i18n("Modified"), QVariant::DateTime);
    addItemInfo(group, "Pages", i18n("Pages"), QVariant::Int);
    addItemInfo(group, "Encrypted", i18n("Encrypted"), QVariant::Bool);
    addItemInfo(group, "Linearized", i18n("Linearized"), QVariant::Bool);
    addItemInfo(group, "Version", i18n("Version"), QVariant::String);
}

/* borrowed from kpdf */
static QString unicodeToQString(Unicode* u, int len) {
    QString ret;
    ret.setLength(len);
    QChar* qch = (QChar*) ret.unicode();
    for (;len;--len)
      *qch++ = (QChar) *u++;
    return ret;
}

/* borrowed from kpdf */
QString KPdfPlugin::getDocumentInfo( const QString & data ) const
{
    // [Albert] Code adapted from pdfinfo.cc on xpdf
    Object info;
    if ( !m_doc )
        return i18n( "Unknown" );

    m_doc->getDocInfo( &info );
    if ( !info.isDict() )
        return i18n( "Unknown" );

    QString result;
    Object obj;
    GooString *s1;
    GBool isUnicode;
    Unicode u;
    int i;
    Dict *infoDict = info.getDict();

    if ( infoDict->lookup( (char*)data.latin1(), &obj )->isString() )
    {
        s1 = obj.getString();
        if ( ( s1->getChar(0) & 0xff ) == 0xfe && ( s1->getChar(1) & 0xff ) == 0xff )
        {
            isUnicode = gTrue;
            i = 2;
        }
        else
        {
            isUnicode = gFalse;
            i = 0;
        }
        while ( i < obj.getString()->getLength() )
        {
            if ( isUnicode )
            {
                u = ( ( s1->getChar(i) & 0xff ) << 8 ) | ( s1->getChar(i+1) & 0xff );
                i += 2;
            }
            else
            {
                u = s1->getChar(i) & 0xff;
                ++i;
            }
            result += unicodeToQString( &u, 1 );
        }
        obj.free();
        info.free();
        return result;
    }
    obj.free();
    info.free();
    return i18n( "Unknown" );
}

/* borrowed from kpdf */
QDateTime KPdfPlugin::getDocumentDate( const QString & data ) const
{
    // [Albert] Code adapted from pdfinfo.cc on xpdf
    if ( !m_doc )
        return QDateTime();

    Object info;
    m_doc->getDocInfo( &info );
    if ( !info.isDict() ) {
	info.free();
        return QDateTime();
    }

    Object obj;
    char *s;
    int year, mon, day, hour, min, sec;
    Dict *infoDict = info.getDict();
    QString result;

    if ( infoDict->lookup( (char*)data.latin1(), &obj )->isString() )
    {
        s = obj.getString()->getCString();
        if ( s[0] == 'D' && s[1] == ':' )
            s += 2;

        if ( sscanf( s, "%4d%2d%2d%2d%2d%2d", &year, &mon, &day, &hour, &min, &sec ) == 6 )
        {
            QDate d( year, mon, day );  //CHECK: it was mon-1, Jan->0 (??)
            QTime t( hour, min, sec );
            if ( d.isValid() && t.isValid() ) {
		obj.free();
		info.free();
		return QDateTime( d, t );
	    }
        }
    }
    obj.free();
    info.free();
    return QDateTime();
}

bool KPdfPlugin::readInfo( KFileMetaInfo& info, uint /* what */)
{
    GooString *filename_gooified = new GooString( info.path().latin1() );
    m_doc = new PDFDoc(filename_gooified, 0, 0);
    delete filename_gooified;

    KFileMetaInfoGroup generalGroup = appendGroup(info, "General");
    Object metaData;
    m_doc->getDocInfo(&metaData);

    appendItem(generalGroup, "Title", getDocumentInfo("Title") );
    appendItem(generalGroup, "Subject", getDocumentInfo("Subject") );
    appendItem(generalGroup, "Author", getDocumentInfo("Author") );
    appendItem(generalGroup, "Keywords", getDocumentInfo("Keywords") );
    appendItem(generalGroup, "Creator", getDocumentInfo("Creator") );
    appendItem(generalGroup, "Producer", getDocumentInfo("Producer") );

    appendItem(generalGroup, "CreationDate", getDocumentDate("CreationDate") );
    appendItem(generalGroup, "ModificationDate", getDocumentDate("ModDate") );
    appendItem(generalGroup, "Pages", m_doc->getNumPages() );
    appendItem(generalGroup, "Encrypted", m_doc->isEncrypted() );
    appendItem(generalGroup, "Linearized", m_doc->isLinearized() );
    QString versionString = QString("%1").arg( m_doc->getPDFVersion(), 0, 'f', 1 );
    appendItem(generalGroup, "Version", versionString );

    return true;
}

#include "kfile_pdf.moc"
