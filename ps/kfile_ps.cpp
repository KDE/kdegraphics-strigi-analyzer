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

K_EXPORT_COMPONENT_FACTORY(kfile_ps, PSFactory("kfile_ps"));

KPSPlugin::KPSPlugin(QObject *parent, const char *name,
                       const QStringList &preferredItems) : 
    KFilePlugin( parent, name, preferredItems )
{
    kdDebug(7034) << "ps plugin\n";
}

bool KPSPlugin::readInfo( KFileMetaInfo::Internal& info )
{
    _info = info;
    _keys.clear();
    _dsc = new KDSC;
    _endComments = false;
    
    _dsc->setCommentHandler( this );
    
    FILE* fp = fopen( QFile::encodeName( info.path() ), "r" );
    char buf[4096];
    int count;
    while( ( count = fread( buf, sizeof(char), 4096, fp ) ) != 0 
        && !_endComments )
    {
	_dsc->scanData( buf, count );
    }
    fclose( fp );

    delete _dsc;
    _dsc = 0;
    
    info.setSupportedKeys( _keys );
    info.setPreferredKeys( m_preferred );
    info.setSupportsVariableKeys( false );
    return true;
}  

void KPSPlugin::comment( Name name )
{
    switch( name )
    {
    case Title:
	_keys << "Title";
        _info.insert( KFileMetaInfoItem( "Title", 
	                                 i18n( "Title" ),
                                         QVariant( _dsc->dsc_title() ) ) );
	break;
    case Creator: 
	_keys << "Creator"; 
        _info.insert( KFileMetaInfoItem( "Creator", 
	                                 i18n( "Creator" ),
                                         QVariant( _dsc->dsc_creator() ) ) );
	break;
    case CreationDate: 
	_keys << "CreationDate"; 
        _info.insert( KFileMetaInfoItem( "CreationDate", 
	                                 i18n( "Creation date" ), 
	                                 QVariant( _dsc->dsc_date() ) ) );
	break;
    case For: 
	_keys << "For"; 
        _info.insert( KFileMetaInfoItem( "For", 
	                                 i18n( "For" ),
	                                 QVariant( _dsc->dsc_for() ) ) );
	break;
    case Pages: 
	_keys << "Pages"; 
        _info.insert( KFileMetaInfoItem( "Pages", 
	                                 i18n( "Pages" ),
	                                 QVariant( _dsc->page_pages() ) ) );
	break;
    case EndComments: _endComments = true;
    default: ; // Ignore
    }
}

