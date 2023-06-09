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

#include "tupellipseitem.h"
#include "tupsvg2qt.h"
#include "tupgraphicalgorithm.h"
#include "tupserializer.h"

#include <QGraphicsSceneDragDropEvent>
#include <QMimeData>
#include <QBrush>
#include <QPainter>
#include <QPainterPath>

TupEllipseItem::TupEllipseItem(QGraphicsItem *parent): QGraphicsEllipseItem(parent), dragOver(false)
{
    setAcceptDrops(true);
}

TupEllipseItem::TupEllipseItem(const QRectF &rect, QGraphicsItem *parent): QGraphicsEllipseItem(rect, parent), dragOver(false)
{
    setAcceptDrops(true);
}

TupEllipseItem::~TupEllipseItem()
{
}

void TupEllipseItem::fromXml(const QString &xml)
{
    Q_UNUSED(xml)
}

QDomElement TupEllipseItem::toXml(QDomDocument &doc) const
{
    QDomElement root = doc.createElement("ellipse");

    QString cx = QString::number(rect().center().x());
    QString cy = QString::number(rect().center().y());
    QString rx = QString::number(rect().width()/2);
    QString ry = QString::number(rect().height()/2);
    
    root.setAttribute("cx", cx);
    root.setAttribute("cy", cy);
    root.setAttribute("rx", rx);
    root.setAttribute("ry", ry);
    
    root.appendChild(TupSerializer::properties(this, doc));
    
    QBrush brush = this->brush();
    root.appendChild(TupSerializer::brush(&brush, doc));
    
    QPen pen = this->pen();
    root.appendChild(TupSerializer::pen(&pen, doc));
    
    return root;
}

bool TupEllipseItem::contains(const QPointF & point) const
{
    // SQA: Check if this code is really doing something
    /*
    int thickness = pen().width();
    QRectF rectS(point-QPointF(thickness/2,thickness/2) , QSizeF(thickness,thickness));
    
    QPolygonF pol = shape().toFillPolygon ();
    foreach (QPointF point, pol) {
             if (rectS.contains(point))
                 return true;
    }
    QPolygonF::iterator it1 = pol.begin() ;
    QPolygonF::iterator it2 = pol.begin()+1;
    
    while (it2 != pol.end()) {
           if (TupGraphicalAlgorithm::intersectLine( (*it1), (*it2), rectS  ))
               return true;
           ++it1;
           ++it2;
    }
    return false;
    */

    return QGraphicsEllipseItem::contains(point);
}

void TupEllipseItem::dragEnterEvent(QGraphicsSceneDragDropEvent *event)
{
    if (event->mimeData()->hasColor()) {
        event->setAccepted(true);
        dragOver = true;
        update();
    } else {
        event->setAccepted(false);
    }
}

void TupEllipseItem::dragLeaveEvent(QGraphicsSceneDragDropEvent *event)
{
    Q_UNUSED(event)
    dragOver = false;
    update();
}

void TupEllipseItem::dropEvent(QGraphicsSceneDragDropEvent *event)
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

int TupEllipseItem::type() const
{
    return Type;
}
