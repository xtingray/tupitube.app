/***************************************************************************
 *   Project TUPITUBE APP                                                  *
 *   Project Contact: info@tupitube.com                                    *
 *   Project Website: http://www.tupitube.com                              *
 *                                                                         *
 *   Developer:                                                            *
 *    Gustav Gonzalez / xtingray                                           *
 *                                                                         *
 *   License:                                                              *
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
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 ***************************************************************************/

#ifndef TGLOBAL_H
#define TGLOBAL_H

#include <QStringList>

#define TUPITUBE_URL "https://www.tupitube.com"
#define MEDIA_URL "media.tupitube.com"
#define BROWSER_FINGERPRINT "Tupi_Browser 2.0"

enum ColorTab { Border = 0, Fill, BgColor };
enum Tool { PenTool = 0, RectangleTool, EllipseTool, TriangleTool };

#ifdef TUP_DEBUG
#include <QDebug>
#endif

#endif
