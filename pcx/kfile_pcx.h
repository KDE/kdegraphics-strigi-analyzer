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

#ifndef __KFILE_PCX_H_
#define __KFILE_PCX_H_

#include <kfilemetainfo.h>

struct PALETTE
{
  struct
  {
    quint8 r;
    quint8 g;
    quint8 b;
  } p[ 16 ];
};

struct PCXHEADER
{
  quint8  Manufacturer;    // Constant Flag, 10 = ZSoft .pcx
  quint8  Version;         // Version information·
                            // 0 = Version 2.5 of PC Paintbrush·
                            // 2 = Version 2.8 w/palette information·
                            // 3 = Version 2.8 w/o palette information·
                            // 4 = PC Paintbrush for Windows(Plus for
                            //     Windows uses Ver 5)·
                            // 5 = Version 3.0 and > of PC Paintbrush
                            //     and PC Paintbrush +, includes
                            //     Publisher's Paintbrush . Includes
                            //     24-bit .PCX files·
  quint8  Encoding;        // 1 = .PCX run length encoding
  quint8  Bpp;             // Number of bits to represent a pixel
                            // (per Plane) - 1, 2, 4, or 8·
  quint16 XMin;
  quint16 YMin;
  quint16 XMax;
  quint16 YMax;
  quint16 HDpi;
  quint16 YDpi;
  struct PALETTE Palette;
  quint8  Reserved;        // Should be set to 0.
  quint8  NPlanes;         // Number of color planes
  quint16 BytesPerLine;    // Number of bytes to allocate for a scanline
                            // plane.  MUST be an EVEN number.  Do NOT
                            // calculate from Xmax-Xmin.·
  quint16 PaletteInfo;     // How to interpret palette- 1 = Color/BW,
                            // 2 = Grayscale ( ignored in PB IV/ IV + )·
  quint16 HScreenSize;     // Horizontal screen size in pixels. New field
                            // found only in PB IV/IV Plus
  quint16 VScreenSize;     // Vertical screen size in pixels. New field
                            // found only in PB IV/IV Plus
  quint8  Filler[ 54 ];    // Blank to fill out 128 byte header.  Set all
                            // bytes to 0
};

class KPcxPlugin: public KFilePlugin
{
  Q_OBJECT

public:
  KPcxPlugin(QObject *parent, const char *name, const QStringList& args);
  virtual bool readInfo(KFileMetaInfo& info, uint what);

private:
};

#endif

/* vim: et sw=2 ts=2
*/

