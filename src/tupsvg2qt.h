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

#ifndef TUPSVG2QT_H
#define TUPSVG2QT_H

#include "tupglobal.h"

#include <QString>
#include <QPainterPath>
#include <QPen>
// #include <QXmlAttributes>
#include <QBrush>

class TupSvg2Qt
{
    private:
        TupSvg2Qt();
        ~TupSvg2Qt();
        
    public:
        static bool svgpath2qtpath(const QString &data, QPainterPath &path);
        // static bool svgmatrix2qtmatrix(const QString &data, QMatrix &matrix);
        static bool svgmatrix2qtmatrix(const QString &data, QTransform &transform);
        static bool parsePointF(const QString &pointstr, QPointF &point);
        
        // static void parsePen(QPen &pen, const QXmlAttributes &attributes);
        // static bool parseBrush(QBrush &brush, const QXmlAttributes &attributes);
               
        static QList<qreal> parseNumbersList(QString::const_iterator &itr);
};

#endif
