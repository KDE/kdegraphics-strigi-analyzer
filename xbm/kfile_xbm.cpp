/* This file is part of the KDE project
 * Copyright (C) 2002 Shane Wright <me@shanewright.co.uk>
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

#include <config.h>
#include "kfile_xbm.h"

#include <kprocess.h>
#include <klocale.h>
#include <kgenericfactory.h>
#include <kstringvalidator.h>
#include <kdebug.h>

#include <qdict.h>
#include <qvalidator.h>
#include <qcstring.h>
#include <qfile.h>
#include <qdatetime.h>

#if !defined(__osf__)
#include <inttypes.h>
#else
typedef unsigned short uint32_t;
#endif

typedef KGenericFactory<KXbmPlugin> XbmFactory;

K_EXPORT_COMPONENT_FACTORY(kfile_xbm, XbmFactory( "kfile_xbm" ))

KXbmPlugin::KXbmPlugin(QObject *parent, const char *name,
                       const QStringList &args)

    : KFilePlugin(parent, name, args)
{
    KFileMimeTypeInfo* info = addMimeTypeInfo( "image/x-xbm" );

    KFileMimeTypeInfo::GroupInfo* group = 0L;

    group = addGroupInfo(info, "Technical", i18n("Technical Details"));

    KFileMimeTypeInfo::ItemInfo* item;

    item = addItemInfo(group, "Dimensions", i18n("Dimensions"), QVariant::Size);
    setHint( item, KFileMimeTypeInfo::Size );
    setUnit(item, KFileMimeTypeInfo::Pixels);
}


unsigned long KXbmPlugin::xbm_processLine(char * linebuf)
{
    const char * fsig = "#define ";

    // check it starts with #define
    if (memcmp(linebuf, fsig, 8))
        return 0;

    // scan for the 2nd space and set up a pointer
    uint32_t slen = strlen(linebuf);
    bool done = false;
    uint32_t spos = 0;
    unsigned char spacecount = 0;
    do {

        if (linebuf[spos] == 0x00)
            return 0;

        if (linebuf[spos] == ' ')
            ++spacecount;

        if (spacecount == 2)
            done = true;
        else
            ++spos;

    } while (!done);

    return atoi(linebuf + spos);
}


bool KXbmPlugin::readInfo( KFileMetaInfo& info, uint what)
{

    QFile file(info.path());

    if (!file.open(IO_ReadOnly))
    {
        kdDebug(7034) << "Couldn't open " << QFile::encodeName(info.path()) << endl;
        return false;
    }

    // we need a buffer for lines
    char linebuf[1000];

    // read the first line
    file.readLine(linebuf, sizeof( linebuf ));
    uint32_t width = xbm_processLine(linebuf);

    // read the 2nd line
    file.readLine(linebuf, sizeof( linebuf ));
    uint32_t height = xbm_processLine(linebuf);

    if ((width > 0) && (height > 0)) {
        // we have valid looking data
        KFileMetaInfoGroup group = appendGroup(info, "Technical");
        appendItem(group, "Dimensions", QSize(width, height));
        return true;
    }

    return false;
}

#include "kfile_xbm.moc"
