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

#ifndef TUPPATHITEM_H
#define TUPPATHITEM_H

#include "tupglobal.h"

#include <QGraphicsPathItem>
#include <QDomElement>

class TupPathItem : public QGraphicsPathItem
{
    public:
        enum { Type = UserType + 1 };
        TupPathItem(QGraphicsItem *parent = nullptr);
        ~TupPathItem();
        
        int type() const;
        QDomElement toXml(QDomDocument &doc) const;
        virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr);
        bool contains(const QPointF &point) const;
        
    protected:
        virtual void dragEnterEvent(QGraphicsSceneDragDropEvent *);
        virtual void dragLeaveEvent(QGraphicsSceneDragDropEvent *);
        virtual void dropEvent(QGraphicsSceneDragDropEvent *);
        
    private:
        bool m_dragOver;
};

#endif
