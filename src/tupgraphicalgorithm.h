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

#ifndef TUPGRAPHICALGORITHM_H
#define TUPGRAPHICALGORITHM_H

#include "tupglobal.h"

#include <QPolygon>
#include <QPainterPath>

class TupGraphicalAlgorithm
{
    private:
        TupGraphicalAlgorithm() {}
        ~TupGraphicalAlgorithm() {}

    public:
        static QPainterPath bezierFit(QPolygonF &points_, float error, int from = 0, int to = -1,
                                      bool closePath = false);
        static QPolygonF polygonFit(const QPolygonF &points);
        static bool intersectLine(const QPointF &start, const QPointF& end, const QRectF& rect);
        static char calculateCode(const QPointF &point, const QRectF &window);
        static double distanceToPoint(const QPointF &pos);
        static double angleForPos(const QPointF &pos, const QPointF &anchor = QPointF(0,0));
        
    private:
        enum CohenSutherlandCode
        {
            Bit1 = 1<<1,
            Bit2 = 1<<2,
            Bit3 = 1<<3,
            Bit4 = 1<<4
        };
        
        static void printCode(char code);
};

#endif
