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
#include <kprocess.h>
#include <klocale.h>
#include <kgenericfactory.h>
#include <kdebug.h>
#include <kprocess.h>

#include <qcstring.h>
#include <qfile.h>
#include <qdatetime.h>
#include <qdict.h>
#include <qvalidator.h>

typedef KGenericFactory<KPdfPlugin> PdfFactory;

K_EXPORT_COMPONENT_FACTORY(kfile_pdf, PdfFactory("kfile_pdf"));

KPdfPlugin::KPdfPlugin(QObject *parent, const char *name,
                       const QStringList &preferredItems)
    : KFilePlugin(parent, name, preferredItems)
{
    kdDebug(7034) << "pdf plugin\n";
}

bool KPdfPlugin::readInfo( KFileMetaInfo::Internal& info, int )
{
    KProcess p;
    p << "pdfinfo" << info.path();
    
    m_info = info;
    
    QObject::connect(&p, SIGNAL(receivedStdout(KProcess*, char*, int)),
                     this, SLOT(slotReceivedStdout(KProcess*, char*, int)));

    if (!p.start(KProcess::Block, KProcess::Stdout))
    {
        kdDebug(7034) << "error executing subprocess\n";
        return false;
    }
    kdDebug(7034) << "subprocess finished\n";
    
    info.setPreferredKeys(m_preferred);
    info.setSupportsVariableKeys(false);
    return true;
}  


void KPdfPlugin::slotReceivedStdout(KProcess*, char* buffer, int buflen)
{
    kdDebug(7034) << "received stdout from child process\n";
    // just replace the last \n with a 0
    buffer[buflen-1] = '\0';
    QString s(buffer);
    kdDebug() << s << endl;
    QStringList l = QStringList::split("\n", s);
    
    QStringList keys;
    
    QStringList::Iterator it = l.begin();
    for (; it != l.end(); ++it ) {
        kdDebug() << *it << endl;

        if ((*it).startsWith("CreationDate"))
        {
            m_info.insert(KFileMetaInfoItem("Created", i18n("Created"),
                           QVariant((*it).mid(13).stripWhiteSpace())));
        }
        else if ((*it).startsWith("ModDate"))
        {
            m_info.insert(KFileMetaInfoItem("Modified", i18n("Modified"),
                           QVariant((*it).mid(8).stripWhiteSpace())));
        }
        else if ((*it).startsWith("Pages:"))
        {
            m_info.insert(KFileMetaInfoItem("Pages", i18n("Pages"),
                           QVariant((*it).mid(7).stripWhiteSpace().toInt())));
        }
        else if ((*it).startsWith("Encrypted:"))
        {
            bool b = ((*it).mid(10).stripWhiteSpace() == "yes") ? true : false;
            m_info.insert(KFileMetaInfoItem("Encrypted", i18n("Encrypted"),
                           QVariant(b, 42)));
        }
        else
        {
            QString key( (*it).left( (*it).find(":") ) );
            QString value( (*it).mid((*it).find(":")+1).stripWhiteSpace() );

            m_info.insert(KFileMetaInfoItem(key, i18n(key.utf8()),
                                            QVariant(value)));
        }
    }

    keys << "Created" << "Modified" << "Pages" << "Encrypted";
    m_info.setSupportedKeys(keys);
}

#include "kfile_pdf.moc"
