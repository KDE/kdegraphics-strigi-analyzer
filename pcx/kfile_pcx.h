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
    Q_UINT8 r;
    Q_UINT8 g;
    Q_UINT8 b;
  } p[ 16 ];
};

struct PCXHEADER
{
  Q_UINT8  Manufacturer;    // Constant Flag, 10 = ZSoft .pcx
  Q_UINT8  Version;         // Version information·
                            // 0 = Version 2.5 of PC Paintbrush·
                            // 2 = Version 2.8 w/palette information·
                            // 3 = Version 2.8 w/o palette information·
                            // 4 = PC Paintbrush for Windows(Plus for
                            //     Windows uses Ver 5)·
                            // 5 = Version 3.0 and > of PC Paintbrush
                            //     and PC Paintbrush +, includes
                            //     Publisher's Paintbrush . Includes
                            //     24-bit .PCX files·
  Q_UINT8  Encoding;        // 1 = .PCX run length encoding
  Q_UINT8  Bpp;             // Number of bits to represent a pixel
                            // (per Plane) - 1, 2, 4, or 8·
  Q_UINT16 XMin;
  Q_UINT16 YMin;
  Q_UINT16 XMax;
  Q_UINT16 YMax;
  Q_UINT16 HDpi;
  Q_UINT16 YDpi;
  struct PALETTE Palette;
  Q_UINT8  Reserved;        // Should be set to 0.
  Q_UINT8  NPlanes;         // Number of color planes
  Q_UINT16 BytesPerLine;    // Number of bytes to allocate for a scanline
                            // plane.  MUST be an EVEN number.  Do NOT
                            // calculate from Xmax-Xmin.·
  Q_UINT16 PaletteInfo;     // How to interpret palette- 1 = Color/BW,
                            // 2 = Grayscale ( ignored in PB IV/ IV + )·
  Q_UINT16 HScreenSize;     // Horizontal screen size in pixels. New field
                            // found only in PB IV/IV Plus
  Q_UINT16 VScreenSize;     // Vertical screen size in pixels. New field
                            // found only in PB IV/IV Plus
  Q_UINT8  Filler[ 54 ];    // Blank to fill out 128 byte header.  Set all
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

