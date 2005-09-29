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
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

#include "kfile_pcx.h"

#include <kgenericfactory.h>
#include <kdebug.h>

#include <qdatastream.h>
#include <qfile.h>

typedef KGenericFactory<KPcxPlugin> PcxFactory;

K_EXPORT_COMPONENT_FACTORY(kfile_pcx, PcxFactory("kfile_pcx"))

QDataStream &operator>>( QDataStream &s, PALETTE &pal )
{
  for (  int i=0; i<16; ++i )
    s >> pal.p[ i ].r >> pal.p[ i ].g >> pal.p[ i ].b;

  return s;
}

QDataStream &operator>>( QDataStream &s, PCXHEADER &ph )
{
  s >> ph.Manufacturer;
  s >> ph.Version;
  s >> ph.Encoding;
  s >> ph.Bpp;
  s >> ph.XMin >> ph.YMin >> ph.XMax >> ph.YMax;
  s >> ph.HDpi >> ph.YDpi;
  s >> ph.Palette;
  s >> ph.Reserved;
  s >> ph.NPlanes;
  s >> ph.BytesPerLine;
  s >> ph.PaletteInfo;
  s >> ph.HScreenSize;
  s >> ph.VScreenSize;

  return s;
}

KPcxPlugin::KPcxPlugin( QObject *parent, const char *name,
        const QStringList &args ) : KFilePlugin( parent, name, args )
{
  kdDebug(7034) << "PCX file meta info plugin" << endl;
  KFileMimeTypeInfo* info = addMimeTypeInfo( "image/x-pcx" );

  KFileMimeTypeInfo::GroupInfo* group =
  addGroupInfo( info, "General", i18n( "General" ) );

  KFileMimeTypeInfo::ItemInfo* item;
  item = addItemInfo( group, "Dimensions", i18n( "Dimensions" ),
      QVariant::Size );
  setHint( item, KFileMimeTypeInfo::Size );
  setUnit( item, KFileMimeTypeInfo::Pixels );
  item = addItemInfo( group, "BitDepth", i18n( "Bit Depth" ),
      QVariant::Int );
  setUnit( item, KFileMimeTypeInfo::BitsPerPixel );
  item = addItemInfo( group, "Resolution", i18n( "Resolution" ),
      QVariant::Size );
  setUnit( item, KFileMimeTypeInfo::DotsPerInch );
  item = addItemInfo( group, "Compression", i18n( "Compression" ),
      QVariant::String );
}

bool KPcxPlugin::readInfo( KFileMetaInfo& info, uint )
{
  if ( info.path().isEmpty() )
    return false;

  struct PCXHEADER header;

  QFile f( info.path() );
  if ( !f.open( IO_ReadOnly ) )
    return false;

  QDataStream s( &f );
  s.setByteOrder(  QDataStream::LittleEndian );

  s >> header;

  int w = (  header.XMax-header.XMin ) + 1;
  int h = (  header.YMax-header.YMin ) + 1;
  int bpp = header.Bpp*header.NPlanes;

  KFileMetaInfoGroup group = appendGroup( info, "General" );

  appendItem( group, "Dimensions", QSize( w, h ) );
  appendItem( group, "BitDepth", bpp );
  appendItem( group, "Resolution", QSize( header.HDpi, header.YDpi ) );
  if ( header.Encoding == 1 )
    appendItem( group, "Compression", i18n( "Yes (RLE)" ) );
  else
    appendItem( group, "Compression", i18n( "None" ) );

  f.close();

  return true;
}

#include "kfile_pcx.moc"

/* vim: et sw=2 ts=2
*/

