/* This file is part of the KDE project
 * Copyright (C) 2002 Matthias Witzgall <witzi@gmx.net>
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


/* further information about the dvi file format could be downloaded from: http://www.rpi.edu/~sofkam/DVI/archive/standards/dvistd0.dvi */

#include "kfile_dvi.h"

#include <kgenericfactory.h>
#include <kdebug.h>
#include <klocale.h>
#include <kfilemetainfo.h>

#include <qstring.h>
#include <qvariant.h>
#include <qdatetime.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qregexp.h>


// preprocessormacro K_EXPORT_COMPONENT_FACTORY loads shared library 'kfile_dvi.so' dynamic if necessary
typedef KGenericFactory<KDviPlugin> DviFactory;
K_EXPORT_COMPONENT_FACTORY(kfile_dvi, DviFactory("kfile_dvi"))

KDviPlugin::KDviPlugin (QObject * parent, const QStringList & preferredItems)
  : KFilePlugin(parent, preferredItems)
{
  kDebug(7034) << "dvi plugin";
  
  // set up our mime type
  KFileMimeTypeInfo * info = this->addMimeTypeInfo("application/x-dvi");
  
  
  KFileMimeTypeInfo::GroupInfo * group = this->addGroupInfo(info, "General", "General");
  
  this->addItemInfo(group, "3_Created", i18n("Created"), QVariant::String);
  this->addItemInfo(group, "6_Comment", i18n("Comment"), QVariant::String);
  this->addItemInfo(group, "7_Pages", i18n("Pages"), QVariant::UInt);
}

bool KDviPlugin::readInfo (KFileMetaInfo & info, uint /* what (unused in this plugin) */)
{
  if ( info.path().isEmpty() )
    return false;
  KFileMetaInfoGroup GeneralGroup = appendGroup(info, "General");
  QFile f(info.path());
  QFileInfo f_info;
  quint16 bytes_to_read;
  quint8 comment_length;
  QString comment;
  quint16 pages;
  quint8 buffer[270]; // buffer for reading data; no data is read with more than 270 bytes
  quint32 ptr;
  int i; // running index
  
  // open file and try to get the comment
  f.open(QIODevice::ReadOnly);
  
  if ( f.isOpen() == false ){
    kDebug(7034) << "cannot open file";
    return false;
  }
  
  f_info.setFile(f); // create fileinfoobject
  bytes_to_read = qMin((int)f_info.size(), 270); // check, if the file size is smaller than 270 bytes
  // (if the comment is as large as possible, we don't have to
  // read more than 270 bytes)
  
  if ( f.read((char *)buffer, bytes_to_read) != bytes_to_read ){ // cast to (char *) is necessary
    kDebug(7034) << "read error (1)";
    return false;
  }
  
  if ( (buffer[0] != 247)  ||  (buffer[1] != 2) ){
    // magic numbers are not right
    kDebug(7034) << "wrong file format";;
    return false;
  }
  
  comment_length = buffer[14]; // set up length of comment
  comment.resize(comment_length); // used to avoid permanent reallocation when extracting the comment from buffer
  
  for ( i = 15; i <= 14+comment_length; ++i ) // extract comment from buffer
    comment[i-15] = (char)buffer[i];

  appendItem(GeneralGroup, "6_Comment", comment.simplified() );
  
  // comment is ok, now get total number of pages
  f.seek( f.size() - 13);
  if ( f.read((char *)buffer, 13) != 13 ){
    kDebug(7034) << "read error (2)";
    return false;
  }
  
  i = 12; // reset running index i
  while ( buffer[i] == 223 ){ --i; } // skip all trailing bytes
  
  if ( (buffer[i] != 2) || (i > 8) || (i < 5) ){
    kDebug(7034) << "wrong file formatx";
    return false;
  }
  
  // now we know the position of the pointer to the beginning of the postamble and we can read it
  ptr = buffer[i-4];
  ptr = (ptr << 8) | buffer[i-3];
  ptr = (ptr << 8) | buffer[i-2];
  ptr = (ptr << 8) | buffer[i-1];
  
  // bytes for total number of pages have a offset of 27 to the beginning of the postamble
  f.seek(ptr + 27);
  
  // now read total number of pages from file
  if ( f.read((char *)buffer, 2) != 2 ){
    kDebug(7034) << "read error (3)";
    return false;
  }
  pages = buffer[0];
  pages = (pages << 8) | buffer[1];
  
  appendItem(GeneralGroup, "7_Pages", QVariant(pages) );
  
  f.close();
  
  // now get and set up some basic information about the file (same information would be displayed, if there is no dvi-plugin)
  appendItem(GeneralGroup, "1_Type", QVariant( i18n("TeX Device Independent file") ) ); // set up type of file
  
  appendItem(GeneralGroup, "4_Modified", QVariant(f_info.lastModified().toString("yyyy-MM-dd hh:mm")) );
  // ISO 8601 date format (without seconds)
  
  return true;
}

#include "kfile_dvi.moc"
