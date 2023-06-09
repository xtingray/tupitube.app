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

#include "tuprectitem.h"
#include "tupgraphicalgorithm.h"
#include "tupserializer.h"

TupRectItem::TupRectItem(QGraphicsItem *parent) : QGraphicsRectItem(parent)
{
    setAcceptDrops(true);
}

TupRectItem::TupRectItem(const QRectF& rect, QGraphicsItem * parent) : QGraphicsRectItem(rect, parent)
{
}

TupRectItem::~TupRectItem()
{
}

void TupRectItem::fromXml(const QString &xml)
{
    Q_UNUSED(xml)
}

QDomElement TupRectItem::toXml(QDomDocument &doc) const
{
    QDomElement root = doc.createElement("rect");
    
    root.setAttribute("x", QString::number(rect().x()));
    root.setAttribute("y", QString::number(rect().y()));
    root.setAttribute("width", QString::number(rect().width()));
    root.setAttribute("height", QString::number(rect().height()));
    
    root.appendChild(TupSerializer::properties(this, doc));
    
    QBrush brush = this->brush();
    root.appendChild(TupSerializer::brush(&brush, doc));
    
    QPen pen = this->pen();
    root.appendChild(TupSerializer::pen(&pen, doc));

    return root;
}

void TupRectItem::dragEnterEvent(QGraphicsSceneDragDropEvent *event)
{
    if (event->mimeData()->hasColor()) {
        event->setAccepted(true);
        dragOver = true;
        update();
    } else {
        event->setAccepted(false);
    }
}

void TupRectItem::dragLeaveEvent(QGraphicsSceneDragDropEvent *event)
{
    Q_UNUSED(event)

    dragOver = false;
    update();
}

void TupRectItem::dropEvent(QGraphicsSceneDragDropEvent *event)
{
    dragOver = false;
    if (event->mimeData()->hasColor()) {
        // setBrush(QBrush(qVariantValue<QColor>(event->mimeData()->colorData())));
        QVariant color = event->mimeData()->colorData();
        setBrush(QBrush(color.value<QColor>()));
    } else if (event->mimeData()->hasImage()) {
               // setBrush(QBrush(qVariantValue<QPixmap>(event->mimeData()->imageData())));
               QVariant pixmap = event->mimeData()->imageData();
               setBrush(QBrush(pixmap.value<QPixmap>()));
    }
    update();
}

bool TupRectItem::contains(const QPointF & point) const
{
/*
    double thickness = pen().widthF()+2;
    QRectF rectS(point-QPointF(thickness/2,thickness/2) , QSizeF(thickness,thickness));
    
    QPolygonF pol = shape().toFillPolygon();

    foreach (QPointF point, pol) {
        if (rectS.contains( point))
            return true;
    }
    
    QPolygonF::iterator it1 = pol.begin();
    QPolygonF::iterator it2 = pol.begin()+1;
    
    while (it2 != pol.end()) {
           QRectF rect((*it1).x(), (*it1).y(), (*it2).x()-(*it1).x(), (*it2).y()-(*it1).y());
        
           if (rect.intersects(rectS))
               return true;
           ++it1;
           ++it2;
    }
    
    return false;
*/
    
    return QGraphicsRectItem::contains(point);
}

int TupRectItem::type() const
{
    return Type;
}
