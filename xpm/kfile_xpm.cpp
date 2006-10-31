/***************************************************************************
 *   Copyright (C) 2004 by Martin Koller                                   *
 *   m.koller@surfeu.at                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.         *
 ***************************************************************************/

#include <config.h>
#include <qimage.h>
#include "kfile_xpm.h"

#include <kgenericfactory.h>

//--------------------------------------------------------------------------------

typedef KGenericFactory<xpmPlugin> xpmFactory;

K_EXPORT_COMPONENT_FACTORY(kfile_xpm, xpmFactory( "kfile_xpm" ))

//--------------------------------------------------------------------------------

xpmPlugin::xpmPlugin(QObject *parent, const char *name, const QStringList &args)
  : KFilePlugin(parent, name, args)
{
  KFileMimeTypeInfo* info = addMimeTypeInfo( "image/x-xpm" );

  // our new group
  KFileMimeTypeInfo::GroupInfo* group = 0;
  group = addGroupInfo(info, "xpmInfo", i18n("X PixMap File Information"));

  KFileMimeTypeInfo::ItemInfo* item;

  // our new items in the group
  item = addItemInfo(group, "Dimension", i18n("Dimension"), QVariant::Size);
  setHint(item, KFileMimeTypeInfo::Size);
  setUnit(item, KFileMimeTypeInfo::Pixels);

  item = addItemInfo(group, "BitDepth", i18n("Bit Depth"), QVariant::Int);
  setUnit(item, KFileMimeTypeInfo::BitsPerPixel);
}

//--------------------------------------------------------------------------------

bool xpmPlugin::readInfo(KFileMetaInfo& info, uint /*what*/)
{
  QImage pix;

  if ( ! pix.load(info.path(), "XPM") ) return false;

  KFileMetaInfoGroup group = appendGroup(info, "xpmInfo");

  appendItem(group, "Dimension", pix.size());
  appendItem(group, "BitDepth", pix.depth());

  return true;
}

#include "kfile_xpm.moc"
