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
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 *  $Id$
 */

#include "kfile_pdf.h"

#include <kgenericfactory.h>
#include <kdebug.h>

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
    addItemInfo(group, "Protected", i18n("Protected"), QVariant::String);
    addItemInfo(group, "Linearized", i18n("Linearized"), QVariant::String);
    addItemInfo(group, "Version", i18n("Version"), QVariant::String);
}

bool KPdfPlugin::readInfo( KFileMetaInfo& info, uint /* what */)
{
    Poppler::Document *doc = Poppler::Document::load(info.path());
    if (!doc || doc->isLocked())
    {
        delete doc;
        return false;
    }

    KFileMetaInfoGroup generalGroup = appendGroup(info, "General");

    appendItem(generalGroup, "Title", doc->getInfo("Title") );
    appendItem(generalGroup, "Subject", doc->getInfo("Subject") );
    appendItem(generalGroup, "Author", doc->getInfo("Author") );
    appendItem(generalGroup, "Keywords", doc->getInfo("Keywords") );
    appendItem(generalGroup, "Creator", doc->getInfo("Creator") );
    appendItem(generalGroup, "Producer", doc->getInfo("Producer") );

    appendItem(generalGroup, "CreationDate", doc->getDate("CreationDate") );
    appendItem(generalGroup, "ModificationDate", doc->getDate("ModDate") );
    appendItem(generalGroup, "Pages", doc->getNumPages() );
    
    QString enc;
    if (doc->isEncrypted())
    {
    	enc = i18n("Yes (Can Print:%1 Can Copy:%2 Can Change:%3 Can Add notes:%4)")
    	.arg(doc->okToPrint() ? i18n("Yes") : i18n("No"))
    	.arg(doc->okToCopy() ? i18n("Yes") : i18n("No"))
    	.arg(doc->okToChange() ? i18n("Yes") : i18n("No"))
    	.arg(doc->okToAddNotes() ? i18n("Yes") : i18n("No"));
    }
    else enc = i18n("No");
    
    appendItem(generalGroup, "Protected", enc );
    appendItem(generalGroup, "Linearized", doc->isLinearized() ? i18n("Yes") : i18n("No") );
    QString versionString = QString("%1").arg( doc->getPDFVersion(), 0, 'f', 1 );
    appendItem(generalGroup, "Version", versionString );

    delete doc;

    return true;
}

#include "kfile_pdf.moc"

