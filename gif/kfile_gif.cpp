/*
 * This file is part of the KDE project, added by Bryce Nesbitt
 *
 * This is a plugin for Konqeror/KFile which processes 'extra' information
 * contained in a .gif image file (In particular the comment, and resolution).
 *
 **************************************
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

#include <stdlib.h>
#include "kfile_gif.h"

#include <kurl.h>
#include <kprocess.h>
#include <klocale.h>
#include <kgenericfactory.h>
#include <kdebug.h>

#include <qcstring.h>
#include <qfile.h>
#include <qdatetime.h>
#include <qdict.h>
#include <qvalidator.h>
#include <qimage.h>

typedef KGenericFactory<KGifPlugin> GifFactory;

K_EXPORT_COMPONENT_FACTORY(kfile_gif, GifFactory("kfile_gif"));

KGifPlugin::KGifPlugin(QObject *parent, const char *name,
                       const QStringList &args)
    : KFilePlugin(parent, name, args)
{
    kdDebug(7034) << "gif KFileMetaInfo plugin\n";
    
    KFileMimeTypeInfo* info = addMimeTypeInfo( "image/gif" );

    KFileMimeTypeInfo::GroupInfo* group = 0L;

    group = addGroupInfo(info, "General", "General");

    KFileMimeTypeInfo::ItemInfo* item;

    item = addItemInfo(group, "Comment", i18n("Comment"), QVariant::String);
    setAttributes(item, KFileMimeTypeInfo::Modifiable);
    setHint(item,  KFileMimeTypeInfo::Description);

    addItemInfo(group, "Resolution", i18n("Resolution"), QVariant::Int);
}

QValidator* KGifPlugin::createValidator( const QString& mimetype,
                                         const QString& group,
                                         const QString& key,
                                         QObject* parent, const char* name) const
{
    return new QRegExpValidator(QRegExp(".*"), parent, name);
}

bool KGifPlugin::writeInfo( const KFileMetaInfo& info ) const
{
    QString comment = info["General"]["Comment"].value().toString();
    QString path    = info.path();

    kdDebug(7034) << "gif KFileMetaInfo writeInfo: " << info.path() << " \"" << comment << "\"\n";

    /*
        Do a strictly safe insertion of the comment:

        Scan original to verify it's a proper gif
        Open a unique temporary file in this directory
        Write temporary, replacing all COM blocks with this one.
        Scan temporary, to verify it's a proper gif
        Rename original to another unique name
        Rename temporary to original
        Unlink original
    */
    /*
        The gif standard specifies 7 bit ascii for the COM block.
        Rather than inserting national characters here,
        I'm assuming it's better to write unicode utf-8,
        which is fully backwards compatible with readers expecting ascii.
    */
    //if( safe_copy_and_modify( path.latin1(), comment.utf8() ) ) {
    //        return false;
    //    }
    return true;
}

bool KGifPlugin::readInfo( KFileMetaInfo& info, uint what )
{
    QString tag;

    kdDebug(7034) << "gif KFileMetaInfo readInfo\n";

    KFileMetaInfoGroup group = appendGroup(info, "General");
    // I insert a comment always, so that the user can edit it
    // items can be made addable, so you don't need to insert them if there
    // is none
    tag = "placeholder comment";
    kdDebug(7034) << "gif plugin inserting Comment: " << tag << "\n";
    appendItem(group, "Comment",	QString(tag));

    tag = "unknown x unknown";
    if (tag.length()) {
    	appendItem(group, "Resolution", QSize(123,456));
    }

  //DiscardData();
  return true;
}

#include "kfile_gif.moc"
