/* This file is part of the KDE project
 * Copyright (C) 2002 Wilco Greven <greven@kde.org>
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

#include "kfile_ps.h"

#include <qfile.h>

#include <klocale.h>
#include <kgenericfactory.h>
#include <kdebug.h>

typedef KGenericFactory<KPSPlugin> PSFactory;

K_EXPORT_COMPONENT_FACTORY(kfile_ps, PSFactory("kfile_ps"))

KPSPlugin::KPSPlugin(QObject *parent, const char *name,
                       const QStringList &preferredItems) : 
    KFilePlugin( parent, name, preferredItems )
{
    kdDebug(7034) << "ps plugin\n";
    
    // set up our mimetype
    KFileMimeTypeInfo* info = addMimeTypeInfo( "application/postscript" );

    // general group
    KFileMimeTypeInfo::GroupInfo* group = addGroupInfo(info, "General", i18n("General"));
    addItemInfo(group, "Title", i18n("Title"), QVariant::String);
    addItemInfo(group, "Creator", i18n("Creator"), QVariant::String);
    addItemInfo(group, "CreationDate", i18n("Creation date"), QVariant::String);
    addItemInfo(group, "For", i18n("For"), QVariant::String);
    addItemInfo(group, "Pages", i18n("Pages"), QVariant::UInt);
}

bool KPSPlugin::readInfo( KFileMetaInfo& info, uint /* what */)
{
    _info = info;
    _group = appendGroup(info, "General");
    _dsc = new KDSC;
    _endComments = false;
    _setData = false;
    _dsc->setCommentHandler( this );

    FILE* fp = fopen( QFile::encodeName( info.path() ), "r" );
    if( fp == 0 )
        return false;
    
    char buf[4096];
    int count;
    while( ( count = fread( buf, sizeof(char), 4096, fp ) ) != 0
        && !_endComments )
    {
        if ( !_dsc->scanData( buf, count ) ) break;
    }
    fclose( fp );

    delete _dsc;
    _dsc = 0;
    
    return _setData;
}  

void KPSPlugin::comment( Name name )
{
    int pages;

    switch( name )
    {
    case Title:
        appendItem(_group, "Title", _dsc->dsc_title());
        _setData = true;
    break;
    case Creator:
        appendItem(_group, "Creator", _dsc->dsc_creator());
        _setData = true;
    break;
    case CreationDate:
        appendItem(_group, "CreationDate", _dsc->dsc_date());
        _setData = true;
    break;
    case For:
        appendItem(_group, "For", _dsc->dsc_for());
        _setData = true;
    break;
    case Pages:
        pages = _dsc->page_pages();
        if (pages)
        {
            appendItem(_group, "Pages", pages);
            _setData = true;
        }
    break;
    case EndComments: _endComments = true;
    default: ; // Ignore
    }
}

#include "kfile_ps.moc"
