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

#ifndef __KFILE_XPS_H__
#define __KFILE_XPS_H__

#include <kfilemetainfo.h>

class QStringList;

class KXpsPlugin: public KFilePlugin
{
Q_OBJECT
public:
    KXpsPlugin( QObject *parent, const QStringList& preferredItems );

    virtual bool readInfo(KFileMetaInfo& info, uint what);

private:
};

#endif
