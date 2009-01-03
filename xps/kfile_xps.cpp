/* This file is part of the KDE project
 * Copyright (C) 2006 Brad Hards <bradh@frogmouth.net>
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
 */

#include "kfile_xps.h"

#include <kgenericfactory.h>
#include <kdebug.h>
#include <kzip.h>

#include <QDomDocument>
#include <QImage>

typedef KGenericFactory<KXpsPlugin> XpsFactory;

K_EXPORT_COMPONENT_FACTORY(kfile_xps, XpsFactory("kfile_xps"))

KXpsPlugin::KXpsPlugin(QObject *parent, const QStringList &preferredItems)
    : KFilePlugin(parent, preferredItems)
{
    // set up our mime type
    KFileMimeTypeInfo* info = addMimeTypeInfo( "application/vnd.ms-xpsdocument" );

    // general group
    KFileMimeTypeInfo::GroupInfo* group = addGroupInfo(info, "General", i18n("General"));

    KFileMimeTypeInfo::ItemInfo* item;

    item = addItemInfo(group, "Title", i18n("Title"), QVariant::String);
    setHint(item, KFileMimeTypeInfo::Name);
    item = addItemInfo(group, "Subject", i18n("Subject"), QVariant::String);
    item = addItemInfo(group, "Subject", i18n("Description"), QVariant::String);
    setHint(item, KFileMimeTypeInfo::Description);
    item = addItemInfo(group, "Author", i18n("Author"), QVariant::String);
    setHint(item, KFileMimeTypeInfo::Author);
    addItemInfo(group, "Keywords", i18n("Keywords"), QVariant::String);

    item = addItemInfo( group, "Thumbnail", i18n("Thumbnail"), QVariant::Image );
    setHint( item, KFileMimeTypeInfo::Thumbnail );

    item = addItemInfo( group, "ThumbnailDimensions",
                        i18n("Thumbnail Dimensions"), QVariant::Size );
    setHint( item, KFileMimeTypeInfo::Size );
    setUnit( item, KFileMimeTypeInfo::Pixels );

    addItemInfo(group, "CreationDate", i18n("Creation Date"), QVariant::DateTime);
    addItemInfo(group, "ModificationDate", i18n("Modified"), QVariant::DateTime);

/*
    addItemInfo(group, "Pages", i18n("Pages"), QVariant::Int);
*/
    addItemInfo(group, "Documents", i18n("Number of Documents"), QVariant::Int);
}


bool KXpsPlugin::readInfo( KFileMetaInfo& info, uint /* what */)
{
    KFileMetaInfoGroup generalGroup = appendGroup(info, "General");

    KZip *xpsArchive = new KZip( info.path() );
    if ( xpsArchive->open( IO_ReadOnly ) == true ) {
        // kdDebug(7115) << "Successful open of " << xpsArchive->fileName() << endl;
    } else {
        kDebug(7115) << "Could not open XPS archive: " << xpsArchive->fileName();
	delete xpsArchive;
	return false;
    }

    const KZipFileEntry* relFile = static_cast<const KZipFileEntry *>(xpsArchive->directory()->entry("_rels/.rels"));

    if ( !relFile ) {
        delete xpsArchive;	
        // this might occur if we can't read the zip directory, or it doesn't have the relationships entry
        return false;
    }


    if ( relFile->name().isEmpty() ) {
	delete xpsArchive;
        // this might occur if we can't read the zip directory, or it doesn't have the relationships entry
        return false;
    }

    QIODevice* relDevice = relFile->createDevice();

    QDomDocument relDom;
    QString errMsg;
    int errLine, errCol;
    if ( relDom.setContent( relDevice, true, &errMsg, &errLine, &errCol ) == false ) {
        // parse error
        kDebug(7115) << "Could not parse relationship document: " << errMsg << " : "
		     << errLine << " : " << errCol << endl;
        delete relDevice;
	delete xpsArchive;
	return false;
    }

    QDomElement docElem = relDom.documentElement();
    
    QString thumbFileName;
    QString fixedRepresentationFileName;
    QString metadataFileName;

    QDomNode n = docElem.firstChild();
    while( !n.isNull() ) {
        QDomElement e = n.toElement();
	if( !e.isNull() ) {
	    if ("http://schemas.openxmlformats.org/package/2006/relationships/metadata/thumbnail" == e.attribute("Type") ) {
	        thumbFileName = e.attribute("Target");
	    } else if ("http://schemas.microsoft.com/xps/2005/06/fixedrepresentation" == e.attribute("Type") ) {
	        fixedRepresentationFileName = e.attribute("Target");
	    } else if ("http://schemas.openxmlformats.org/package/2006/relationships/metadata/core-properties" == e.attribute("Type") ) {
	        metadataFileName = e.attribute("Target");
	    }
	}
	n = n.nextSibling();
    }

    delete relDevice;

    if ( fixedRepresentationFileName.isEmpty() ) {
	delete xpsArchive;
        // FixedRepresentation is a required part of the XPS document
        return false;
    }

    const KZipFileEntry* fixedRepFile = static_cast<const KZipFileEntry *>(xpsArchive->directory()->entry( fixedRepresentationFileName ));

    QIODevice* fixedRepDevice = fixedRepFile->createDevice();

    QDomDocument fixedRepDom;
    if ( fixedRepDom.setContent( fixedRepDevice, true, &errMsg, &errLine, &errCol ) == false ) {
        // parse error
        kDebug(7115) << "Could not parse Fixed Representation document: " << errMsg << " : "
		     << errLine << " : " << errCol << endl;
        delete fixedRepDevice;
	delete xpsArchive;
	return false;
    }

    docElem = fixedRepDom.documentElement();

    QString firstDocumentFileName;
    int numDocuments = 0; // the number of Documents in this FixedDocumentSequence

    n = docElem.firstChild();
    while( !n.isNull() ) {
        QDomElement e = n.toElement();
	if( !e.isNull() ) {
	    if (e.tagName() == "DocumentReference") {
	        if (firstDocumentFileName.isEmpty()) {
		    // we don't already have a filename, so take this one
		    firstDocumentFileName = e.attribute("Source");
		}
		numDocuments++;
	    }
	}
	n = n.nextSibling();
    }

    delete fixedRepDevice;

#if 0
    // This stuff is used for detailed parsing - not really required

    // no document? bail out here.
    if ( firstDocumentFileName.isEmpty() ) {
        return false;
    }

    KZipFileEntry* firstDocumentFile = static_cast<const KZipFileEntry *>(xpsArchive->directory()->entry( firstDocumentFileName ));

    QIODevice* firstDocumentDevice = firstDocumentFile->device();    

    QDomDocument firstDocumentDom;
    if ( firstDocumentDom.setContent( firstDocumentDevice, true, &errMsg, &errLine, &errCol ) == false ) {
        // parse error
        kDebug(7115) << "Could not parse first document: " << errMsg << " : "
		     << errLine << " : " << errCol << endl;
	return false;
    }

    n = firstDocumentDom.documentElement().firstChild();

    while( !n.isNull() ) {
        QDomElement e = n.toElement();
	if( !e.isNull() ) {
	  kDebug(7155) << "DOcument: " << e.tagName() << " : " << e.text();
	}
	n = n.nextSibling();
    }
#endif

    if ( ! metadataFileName.isEmpty() ) {
        const KZipFileEntry* corepropsFile = static_cast<const KZipFileEntry *>(xpsArchive->directory()->entry(metadataFileName));
	kDebug(7115) << "metadata file name: " << metadataFileName;

	QDomDocument metadataDocumentDom;

	QIODevice *corepropsDevice = corepropsFile->createDevice();

	if ( metadataDocumentDom.setContent( corepropsDevice, true, &errMsg, &errLine, &errCol ) == false ) {
	    // parse error
	    kDebug(7115) << "Could not parse core properties (metadata) document: " << errMsg << " : "
			 << errLine << " : " << errCol << endl;
            delete corepropsDevice;
	    delete xpsArchive;
	    return false;
	}

	n = metadataDocumentDom.documentElement().firstChild(); // the <coreProperties> level
	while( !n.isNull() ) {
	    QDomElement e = n.toElement();
	    if( !e.isNull() ) {
		if (e.tagName() == "title") {
		    appendItem(generalGroup, "Title", e.text() );
		} else if (e.tagName() == "subject") {
		    appendItem(generalGroup, "Subject", e.text() );
		} else if (e.tagName() == "description") {
		    appendItem(generalGroup, "Description", e.text() );
		} else if (e.tagName() == "creator") {
		    appendItem(generalGroup, "Author", e.text() );
		} else if (e.tagName() == "created") {
		    appendItem(generalGroup, "CreationDate", QDateTime::fromString( e.text(), "yyyy-MM-ddThh:mm:ssZ" ) );
		} else if (e.tagName() == "modified") {
		    appendItem(generalGroup, "ModificationDate", QDateTime::fromString( e.text(), "yyyy-MM-ddThh:mm:ssZ" ) );
		} else if (e.tagName() == "keywords") {
		    appendItem(generalGroup, "Keywords", e.text() );
		} else {
		    kDebug(7155) << "unhandled metadata tag: " << e.tagName() << " : " << e.text();
		}
	    }
	    n = n.nextSibling();
	}

	delete corepropsDevice;
    }

    if ( ! thumbFileName.isEmpty() ) {
        const KZipFileEntry* thumbFile = static_cast<const KZipFileEntry *>(xpsArchive->directory()->entry(thumbFileName));

	QImage img;
	img.loadFromData(thumbFile->data());

	appendItem( generalGroup, "Thumbnail", img);
	appendItem( generalGroup, "ThumbnailDimensions", QSize( img.width(), img.height() ) );
    }

    appendItem(generalGroup, "Documents", numDocuments);

    delete xpsArchive;

    return true;
}

#include "kfile_xps.moc"

