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

#include "tupitemfactory.h"
#include "tupsvg2qt.h"
#include "tuppathitem.h"
#include "tuprectitem.h"
#include "tupellipseitem.h"
#include "tupgraphicalgorithm.h"
#include "tupserializer.h"

#include <QFont>

TupItemFactory::TupItemFactory() : QObject()
{
    // item = nullptr;
    // isLoading = false;
}

TupItemFactory::~TupItemFactory()
{
}

/*
QGraphicsItem * TupItemFactory::create(const QString &root)
{
    QGraphicsItem* item = nullptr;
    type = TupItemFactory::Vectorial;

    if (root == "path") {
        item = new TupPathItem;
    } else if (root == "rect") {
        item = new TupRectItem;
    } else if (root == "ellipse") {
        item = new TupEllipseItem;
    }

    return item;
}

bool TupItemFactory::startTag(const QString& qname, const QXmlAttributes& atts)
{
    if (qname == "path") {
        QPainterPath path;
        TupSvg2Qt::svgpath2qtpath(atts.value("coords"), path);

        if (!item)
            item = createItem(qname);

        qgraphicsitem_cast<TupPathItem *>(item)->setPath(path);
        objects.push(item);
    } else if (qname == "rect") {
        QRectF rect(atts.value("x").toDouble(), atts.value("y").toDouble(), atts.value("width").toDouble(),
                    atts.value("height").toDouble());
        if (!item)
            item = createItem(qname);
        qgraphicsitem_cast<TupRectItem *>(item)->setRect(rect);
        objects.push(item);
    } else if (qname == "ellipse") {
        QRectF rect(QPointF((atts.value("cx").toDouble() - atts.value("rx").toDouble()),
                             atts.value("cy").toDouble() - atts.value("ry").toDouble()),
                    QSizeF(2 * atts.value("rx").toDouble(), 2 * atts.value("ry").toDouble()));
        if (!item)
            item = createItem(qname);

        qgraphicsitem_cast<TupEllipseItem *>(item)->setRect(rect);
        objects.push(item);
    }

    if (qname == "properties" && !objects.isEmpty()) {
        TupSerializer::loadProperties(objects.last(), atts);
    } else if (qname == "brush") {
               QBrush brush;
               TupSerializer::loadBrush(brush, atts);

               if (currentTag() == "pen") {
                   loading = "pen";
                   QPen pen = itemPen();
                   pen.setBrush(brush);
                   setItemPen(pen);
               } else {
                   loading = qname;
                   setItemBrush(brush);
               }
    } else if (qname == "pen") {
               QPen pen;
               loading = qname;
               TupSerializer::loadPen(pen, atts);
               setItemPen(pen);
    } else if (qname == "font") {
               QFont font;
               TupSerializer::loadFont(font, atts);
    } else if (qname == "stop") {
               if (gradient) {
                   QColor c(atts.value("colorName"));
                   c.setAlpha(atts.value("alpha").toInt());
                   gradient->setColorAt(atts.value("value").toDouble(), c);
               }
    } else if (qname == "gradient") {
               gradient = TupSerializer::createGradient( atts);
    }

    return true;
}

void TupItemFactory::text(const QString & ch)
{
    textReaded += ch;
}

bool TupItemFactory::endTag(const QString& qname)
{
    if (qname == "path") {
        objects.pop();
    } else if (qname == "rect") {
        objects.pop();
    } else if (qname == "ellipse") {
        objects.pop();
    }

    return true;
}

void TupItemFactory::setItemPen(const QPen &pen)
{
    if (objects.isEmpty())
        return;

    if (QGraphicsLineItem *line = qgraphicsitem_cast<QGraphicsLineItem *>(objects.last())) {
        line->setPen(pen);
    } else if (QAbstractGraphicsShapeItem *shape = qgraphicsitem_cast<QAbstractGraphicsShapeItem *>(objects.last())) {
        shape->setPen(pen);
    }
}

void TupItemFactory::setItemBrush(const QBrush &brush)
{
    if (objects.isEmpty())
        return;

    if (QAbstractGraphicsShapeItem *shape = qgraphicsitem_cast<QAbstractGraphicsShapeItem *>(objects.last()))
        shape->setBrush(brush);
}

void  TupItemFactory::setItemGradient(const QGradient& gradient, bool brush)
{
    if (objects.isEmpty())
        return;

    QBrush gBrush(gradient);

    if (QAbstractGraphicsShapeItem *shape = qgraphicsitem_cast<QAbstractGraphicsShapeItem *>(objects.last())) {
        if (brush) {
            gBrush.setMatrix(shape->brush().matrix());
            shape->setBrush(gBrush);
        } else {
            gBrush.setMatrix(shape->pen().brush().matrix());
            QPen pen = shape->pen();
            pen.setBrush(gBrush);
            shape->setPen(pen);
        }
    } else if (QGraphicsLineItem *line = qgraphicsitem_cast<QGraphicsLineItem *>(objects.last())) {
               gBrush.setMatrix(line->pen().brush().matrix());
               QPen pen = line->pen();
               pen.setBrush(gBrush);
               line->setPen(pen);
    }
}

QPen TupItemFactory::itemPen() const
{
    if (! objects.isEmpty()) {
        if (QGraphicsLineItem *line = qgraphicsitem_cast<QGraphicsLineItem *>(objects.last())) {
            return line->pen();
        } else if (QAbstractGraphicsShapeItem *shape = qgraphicsitem_cast<QAbstractGraphicsShapeItem *>(objects.last())) {
                   return shape->pen();
        }
    }

    return QPen(Qt::transparent, 1);
}

QBrush TupItemFactory::itemBrush() const
{
    if (! objects.isEmpty()) {
        if (QAbstractGraphicsShapeItem *shape = qgraphicsitem_cast<QAbstractGraphicsShapeItem *>(objects.last()))
            return shape->brush();
    }

    return Qt::transparent;
}

bool TupItemFactory::loadItem(QGraphicsItem *gItem, const QString &xml)
{
    item = gItem;
    // isLoading = true;
    bool ok = parse(xml);
    // isLoading = false;

    return ok;
}

QGraphicsItem *TupItemFactory::create(const QString &xml)
{
    if (loadItem(0, xml))
        return item;

    return nullptr;
}
*/

QGraphicsItem *TupItemFactory::create(const QString &xml)
{
    #ifdef TUP_DEBUG
        qDebug() << "[TupItemFactory::create()] - xml:";
        qDebug() << xml;
    #endif

    TupFactoryHandler handler(xml);
    return handler.getItem();
}

QString TupItemFactory::itemID(const QString &xml)
{
    QDomDocument doc;

    if (!doc.setContent(xml))
        return "item";

    QDomElement root = doc.documentElement();
    QString id = root.attribute("id");

    if (id.length() > 0)
        return id;

    return "item";
}

// Check if this method has sense
/*
TupFactoryHandler::Type TupItemFactory::getType()
{
    return handler.getType();
}
*/

