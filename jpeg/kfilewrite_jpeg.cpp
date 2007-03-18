/* This file is part of the KDE project
 * Copyright (C) 2007 Jos van den Oever <jos@vandenoever.info>
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

#include "kfilewrite_jpeg.h"
#include <kgenericfactory.h>

typedef KGenericFactory<JpegWritePlugin> JpegFactory;

K_EXPORT_COMPONENT_FACTORY(kfilewrite_jpeg, JpegFactory("kfilewrite_jpeg"))

JpegWritePlugin::JpegWritePlugin(QObject* parent, const QStringList& args)
        : KFileWritePlugin(parent, args) {
}
bool
JpegWritePlugin::canWrite(QIODevice& file, const QString& key) {
    qDebug() << "checking if we can write in JpegWritePlugin";
    return true;
}
bool
JpegWritePlugin::write(QIODevice& file, const QVariantMap& data) {
    qDebug() << "writing in JpegWritePlugin";
    return false;
}

#include "kfilewrite_jpeg.moc"
