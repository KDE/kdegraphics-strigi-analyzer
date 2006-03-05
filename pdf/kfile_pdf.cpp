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

KPdfPlugin::KPdfPlugin(QObject *parent, const char *, const QStringList &preferredItems)
    : KFilePlugin(parent, preferredItems)
{
    kDebug(7034) << "pdf plugin\n";

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
    m_doc = Poppler::Document::load(info.path());
    if (!m_doc || m_doc->isLocked())
    {
        delete m_doc;
        return false;
    }

    KFileMetaInfoGroup generalGroup = appendGroup(info, "General");

    appendItem(generalGroup, "Title", m_doc->info("Title") );
    appendItem(generalGroup, "Subject", m_doc->info("Subject") );
    appendItem(generalGroup, "Author", m_doc->info("Author") );
    appendItem(generalGroup, "Keywords", m_doc->info("Keywords") );
    appendItem(generalGroup, "Creator", m_doc->info("Creator") );
    appendItem(generalGroup, "Producer", m_doc->info("Producer") );

    appendItem(generalGroup, "CreationDate", m_doc->date("CreationDate") );
    appendItem(generalGroup, "ModificationDate", m_doc->date("ModDate") );
    appendItem(generalGroup, "Pages", m_doc->numPages() );
    
    QString enc;
    if (m_doc->isEncrypted())
    {
    	enc = i18n("Yes (Can Print:%1 Can Copy:%2 Can Change:%3 Can Add notes:%4)")
    	.arg(m_doc->okToPrint() ? i18n("Yes") : i18n("No"))
    	.arg(m_doc->okToCopy() ? i18n("Yes") : i18n("No"))
    	.arg(m_doc->okToChange() ? i18n("Yes") : i18n("No"))
    	.arg(m_doc->okToAddNotes() ? i18n("Yes") : i18n("No"));
    }
    else enc = i18n("No");
    
    appendItem(generalGroup, "Protected", enc );
    appendItem(generalGroup, "Linearized", m_doc->isLinearized() ? i18n("Yes") : i18n("No") );
    QString versionString = QString("%1").arg( m_doc->pdfVersion(), 0, 'f', 1 );
    appendItem(generalGroup, "Version", versionString );

    return true;
}

#include "kfile_pdf.moc"

