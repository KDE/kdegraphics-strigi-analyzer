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
#include <kprocess.h>

#include <qcstring.h>
#include <qfile.h>
#include <qdatetime.h>
#include <qdict.h>
#include <qvalidator.h>

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

    addItemInfo(group, "CreationDate", i18n("Created"), QVariant::DateTime);
    addItemInfo(group, "ModDate", i18n("Modified"), QVariant::DateTime);
    addItemInfo(group, "Pages", i18n("Pages"), QVariant::Int);
    addItemInfo(group, "Encrypted", i18n("Encrypted"), QVariant::Bool);
    addVariableInfo(group, QVariant::String, 0);
}

bool KPdfPlugin::readInfo( KFileMetaInfo& info, uint /* what */)
{
    KProcess p;
    p << "pdfinfo" << info.path();
    p.setEnvironment("LC_TIME", "C");

    mInfo = info;

    QObject::connect(&p, SIGNAL(receivedStdout(KProcess*, char*, int)),
                     this, SLOT(slotReceivedStdout(KProcess*, char*, int)));

    if (!p.start(KProcess::Block, KProcess::Stdout))
    {
        kdDebug(7034) << "error executing subprocess\n";
        return false;
    }
    kdDebug(7034) << "subprocess finished\n";

    return true;
}

QDateTime KPdfPlugin::pdfDate(const QString& s) const
{
    QRegExp rePdfDate("^([0-9]{4})([0-9]{2})?([0-9]{2})?([0-9]{2})?([0-9]{2})?"
                    "([0-9]{2})?(\\+|-|Z)?(?:([0-9]{2})'([0-9]{2})')?$");
    QDateTime dt;
    if (rePdfDate.search(s) > -1)
    {
        // pdfinfo < 1.00 returns date in YYYYMMDDHHmmSSOHH'mm'
        int year = rePdfDate.cap(1).toInt();
        int month = rePdfDate.cap(2).toInt();
        int day = rePdfDate.cap(3).toInt();
        QDate d = QDate(year, month, day);
        int hour = rePdfDate.cap(4).toInt();
        int min = rePdfDate.cap(5).toInt();
        int sec = rePdfDate.cap(6).toInt();
        QTime t = QTime(hour, min, sec);
        dt.setDate(d);
        dt.setTime(t);
        QString zone = rePdfDate.cap(7);
        if (!zone.isEmpty())
        {
            // Convert to UTC
            int diff = rePdfDate.cap(8).toInt() * 3600;
            diff += rePdfDate.cap(9).toInt() * 60;
            if (zone == "+")
                diff *= -1;
            dt = dt.addSecs(diff);
        }
    }
    else
    {
        // pdfinfo >= 1.00 returns date in C (text) format
        dt = QDateTime::fromString(s);
    }
    return dt;
}

void KPdfPlugin::slotReceivedStdout(KProcess*, char* buffer, int buflen)
{
    kdDebug(7034) << "received stdout from child process\n";

    // just replace the last \n with a 0
    buffer[buflen-1] = '\0';
    QString s(buffer);
    kdDebug() << s << endl;
    QStringList l = QStringList::split("\n", s);
    KFileMetaInfoGroup generalGroup = appendGroup(mInfo, "General");

    QStringList::Iterator it = l.begin();
    for (; it != l.end(); ++it ) {
        kdDebug() << *it << endl;

        if ((*it).startsWith("CreationDate"))
        {
            QDateTime dt = pdfDate((*it).mid(13).stripWhiteSpace());
            if (dt.isValid())
                appendItem(generalGroup, "CreationDate", QVariant(dt));
        }
        else if ((*it).startsWith("ModDate"))
        {
            QDateTime dt = pdfDate((*it).mid(8).stripWhiteSpace());
            if (dt.isValid())
                appendItem(generalGroup, "ModDate", QVariant(dt));
        }
        else if ((*it).startsWith("Pages"))
        {
            appendItem(generalGroup, "Pages", QVariant((*it).mid(7).stripWhiteSpace().toInt()));
        }
        else if ((*it).startsWith("Encrypted"))
        {
            bool b = ((*it).mid(10).stripWhiteSpace() == "yes") ? true : false;
            appendItem(generalGroup, "Encrypted", QVariant(b, 42));
        }
        else
        {
            QString key( (*it).left( (*it).find(":") ) );
            QString value( (*it).mid((*it).find(":")+1).stripWhiteSpace() );

            appendItem(generalGroup, i18n(key.utf8()), QVariant(i18n(value.utf8())));
        }
    }
}

//this allows for translations from pdfinfo, do not remove or change!
#if 0
I18N_NOOP("Title")
I18N_NOOP("Creator")
I18N_NOOP("Producer")
I18N_NOOP("Linearized")
I18N_NOOP("Page size")
I18N_NOOP("yes")
I18N_NOOP("no")
I18N_NOOP("File size")
I18N_NOOP("Optimized")
I18N_NOOP("PDF version")
I18N_NOOP("Tagged")
#endif

#include "kfile_pdf.moc"
