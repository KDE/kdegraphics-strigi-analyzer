/* This file is part of the KDE project
 * Copyright (C) Steffen Hansen <hansen@kde.org>
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

#include "kcamerarawplugin.h"

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <klocale.h>
#include <kgenericfactory.h>
#include <kdebug.h>
#include <ktemporaryfile.h>
#include <kimageio.h>
#include <qfile.h>
#include <qimage.h>
#include <qwmatrix.h>
#include <cstdio>


typedef KGenericFactory<KCameraRawPlugin> RawFactory;

K_EXPORT_COMPONENT_FACTORY(kfile_raw, RawFactory("kfile_raw"))

#ifndef KDE_EXPORT
# define KDE_EXPORT
#endif

/* Main entry point into raw parser */
extern "C" {
  int extract_thumbnail( FILE*, FILE*, int* );
  extern char make[];
  extern char model[];
}

bool KCameraRawPlugin::createPreview(const QString &path, QImage &img)
{
  /* Open file and extract thumbnail */
  FILE* input = fopen( QFile::encodeName(path), "rb" );
  if( !input ) return false;
  KTemporaryFile output;
  output.open();
  FILE* output_fs = fopen(output.fileName().toAscii(), "r+");
  int orientation = 0;
  if( extract_thumbnail( input, output_fs, &orientation ) ) {
    fclose(input);
    fclose(output_fs);
    return false;
  }
  fclose(input);
  fclose(output_fs);
  if( !img.load( output.fileName() ) ) return false;

  if(orientation) {
    QMatrix M;
    QMatrix flip= QMatrix(-1,0,0,1,0,0);
    switch(orientation+1) {  // notice intentional fallthroughs
    case 2: M = flip; break;
    case 4: M = flip;
    case 3: M.rotate(180); break;
    case 5: M = flip;
    case 6: M.rotate(90); break;
    case 7: M = flip;
    case 8: M.rotate(270); break;
    default: break; // should never happen
    }
    img = img.transformed(M);
  }
  return true;	
}

KCameraRawPlugin::KCameraRawPlugin(QObject *parent, const QStringList &args )
    : KFilePlugin(parent, args)
{
  kDebug(7034) << "KCameraRawPlugin c'tor";

  //
  // define all possible meta info items
  //
  KFileMimeTypeInfo *info = addMimeTypeInfo("image/x-dcraw");
  KFileMimeTypeInfo::GroupInfo *group = addGroupInfo( info, "Info",
						      i18n("Image Info") );
  KFileMimeTypeInfo::ItemInfo* item;
  
  item = addItemInfo( group, "Manufacturer", i18n("Camera Manufacturer"),
		      QVariant::String );
  item = addItemInfo( group, "Model", i18n("Camera Model"),
		      QVariant::String );
  item = addItemInfo( group, "Thumbnail", i18n("Thumbnail"),
                      QVariant::Image );
  setHint( item, KFileMimeTypeInfo::Thumbnail );
}

bool KCameraRawPlugin::readInfo( KFileMetaInfo& info, uint what )
{
    kDebug(7034) << "KCameraRawPlugin::readInfo()";
    
    const QString path( info.path() );
    if ( path.isEmpty() ) // remote file
        return false;
    
    KFileMetaInfoGroup group = appendGroup( info, "Info" );
    if ( what & KFileMetaInfo::Thumbnail ){
      QImage img;
      if( createPreview( path,img ) ) {
	appendItem( group, "Thumbnail", img );
	kDebug(7034) << "thumbnail " << path << " created";
      }
    } else {
      // HACK: We have to extract thumbnail to get any info...
      QImage img;
      createPreview( path,img );      
    }
    kDebug(7034) << "make=" << make;
    kDebug(7034) << "model=" << model;
    if( make[0] ) {
      appendItem( group, "Manufacturer", &make[0] );
    }
    if( model[0] ) {
      appendItem( group, "Model", &model[0] );
    }

    return true;
}

#include "kcamerarawplugin.moc"
