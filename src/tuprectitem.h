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

#ifndef TUPRECTITEM_H
#define TUPRECTITEM_H

#include "tupglobal.h"

#include <QGraphicsRectItem>
#include <QGraphicsSceneDragDropEvent>
#include <QDomDocument>
#include <QMimeData>
#include <QBrush>
#include <QGraphicsScene>

class TupRectItem : public QGraphicsRectItem
{
    public:
        enum { Type = UserType + 2 };
        TupRectItem(QGraphicsItem * parent = nullptr);
        TupRectItem(const QRectF& rect, QGraphicsItem * parent = nullptr);
        ~TupRectItem();

        int type() const;
        bool contains(const QPointF & point) const;
        virtual void fromXml(const QString &xml);
        virtual QDomElement toXml(QDomDocument &doc) const;

    protected:
        virtual void dragEnterEvent(QGraphicsSceneDragDropEvent *event);
        virtual void dragLeaveEvent(QGraphicsSceneDragDropEvent *event);
        virtual void dropEvent(QGraphicsSceneDragDropEvent *event);

    private:
        bool dragOver;
};

#endif
