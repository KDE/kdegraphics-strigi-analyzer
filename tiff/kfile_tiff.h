/* This file is part of the KDE project
 * Copyright (C) 2002 Nadeem Hasan <nhasan@kde.org>
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
 */

#ifndef __KFILE_TXT_H_
#define __KFILE_TXT_H_

#include <kfilemetainfo.h>
#include <kurl.h>

#include <qintdict.h>

class QStringList;

class KTiffPlugin: public KFilePlugin
{
    Q_OBJECT

public:
    KTiffPlugin(QObject *parent, const char *name, const QStringList& args);
    virtual bool readInfo(KFileMetaInfo& info, uint what);

private:
    QDateTime tiffDate(const QString&) const;

    QIntDict<QString> m_colorMode;
    QIntDict<QString> m_imageCompression;
};

#endif
