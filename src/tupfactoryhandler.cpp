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

#include "tupfactoryhandler.h"
#include "tupsvg2qt.h"
#include "tuppathitem.h"
#include "tuprectitem.h"
#include "tupellipseitem.h"
#include "tupgraphicalgorithm.h"
#include "tupserializer.h"


TupFactoryHandler::TupFactoryHandler() : QXmlStreamReader()
{
    item = nullptr;
    // addToGroup = false;
    isLoading = false;
    parentTag = "";
}

TupFactoryHandler::TupFactoryHandler(const QString &xml) : QXmlStreamReader(xml)
{
    item = nullptr;
    // addToGroup = false;
    isLoading = false;
    parentTag = "";

    parse();
}

TupFactoryHandler::~TupFactoryHandler()
{
}

QGraphicsItem* TupFactoryHandler::createItem(const QString &root)
{
    /*
    #ifdef TUP_DEBUG
        qDebug() << "[TupFactoryHandler::createItem()] - root -> " << root;
    #endif
    */

    QGraphicsItem* localItem = nullptr;
    type = TupFactoryHandler::Vectorial;

    if (root == "path") {
        localItem = new TupPathItem;
    } else if (root == "rect") {
        localItem = new TupRectItem;
    } else if (root == "ellipse") {
        localItem = new TupEllipseItem;
    }

    return localItem;
}

void TupFactoryHandler::setItemPen(const QPen &pen)
{
    if (objects.isEmpty())
        return;

    if (QGraphicsLineItem *line = qgraphicsitem_cast<QGraphicsLineItem *>(objects.last())) {
        line->setPen(pen);
    } else if (QAbstractGraphicsShapeItem *shape = qgraphicsitem_cast<QAbstractGraphicsShapeItem *>(objects.last())) {
               shape->setPen(pen);
    }
}

void TupFactoryHandler::setItemBrush(const QBrush &brush)
{
    if (objects.isEmpty())
        return;

    if (QGraphicsTextItem *textItem = qgraphicsitem_cast<QGraphicsTextItem *>(objects.last())) {
        textItem->setDefaultTextColor(brush.color());
    } else if (QAbstractGraphicsShapeItem *shape = qgraphicsitem_cast<QAbstractGraphicsShapeItem *>(objects.last()))
        shape->setBrush(brush);
}

void  TupFactoryHandler::setItemGradient(const QGradient& gradient, bool brush)
{
    if (objects.isEmpty())
        return;

    QBrush gBrush(gradient);

    if (QAbstractGraphicsShapeItem *shape = qgraphicsitem_cast<QAbstractGraphicsShapeItem *>(objects.last())) {
        if (brush) {
            gBrush.setTransform(shape->brush().transform());
            shape->setBrush(gBrush);
        } else {
            gBrush.setTransform(shape->pen().brush().transform());
            QPen pen = shape->pen();
            pen.setBrush(gBrush);
            shape->setPen(pen);
        }
    } else if (QGraphicsLineItem *line = qgraphicsitem_cast<QGraphicsLineItem *>(objects.last())) {
        gBrush.setTransform(line->pen().brush().transform());
        QPen pen = line->pen();
        pen.setBrush(gBrush);
        line->setPen(pen);
    }
}

QPen TupFactoryHandler::itemPen() const
{
    if (!objects.isEmpty()) {
        if (QGraphicsLineItem *line = qgraphicsitem_cast<QGraphicsLineItem *>(objects.last())) {
            return line->pen();
        } else if (QAbstractGraphicsShapeItem *shape = qgraphicsitem_cast<QAbstractGraphicsShapeItem *>(objects.last())) {
            return shape->pen();
        }
    }

    return QPen(Qt::transparent, 1);
}

QBrush TupFactoryHandler::itemBrush() const
{
    if (! objects.isEmpty()) {
        if (QAbstractGraphicsShapeItem *shape = qgraphicsitem_cast<QAbstractGraphicsShapeItem *>(objects.last()))
            return shape->brush();
    }

    return Qt::transparent;
}

QGraphicsItem *TupFactoryHandler::getItem()
{
    if (item)
        return item;

    return nullptr;
}

QString TupFactoryHandler::itemID(const QString &xml)
{
    QDomDocument document;

    if (!document.setContent(xml))
        return "item";

    QDomElement root = document.documentElement();
    QString id = root.attribute("id");

    if (id.length() > 0)
        return id;

    return "item";
}

TupFactoryHandler::Type TupFactoryHandler::getType()
{
    return type;
}

bool TupFactoryHandler::parse()
{
    while (!atEnd()) {
        readNext();
        QString tag = name().toString();
        if (isStartElement()) {
            if (tag == "path") {
                QPainterPath path;
                TupSvg2Qt::svgpath2qtpath(attributes().value("coords").toString(), path);

                if (!item)
                    item = createItem(tag);

                qgraphicsitem_cast<TupPathItem *>(item)->setPath(path);
                objects.push(item);
            } else if (tag == "rect") {
                QRectF rect(attributes().value("x").toDouble(), attributes().value("y").toDouble(),
                            attributes().value("width").toDouble(), attributes().value("height").toDouble());

                if (!item)
                    item = createItem(tag);
                qgraphicsitem_cast<TupRectItem *>(item)->setRect(rect);
                objects.push(item);
            } else if (tag == "ellipse") {
                QRectF rect(QPointF((attributes().value("cx").toDouble() - attributes().value("rx").toDouble()),
                            attributes().value("cy").toDouble() - attributes().value("ry").toDouble()),
                            QSizeF(2 * attributes().value("rx").toDouble(), 2 * attributes().value("ry").toDouble()));

                if (!item)
                    item = createItem(tag);

                qgraphicsitem_cast<TupEllipseItem *>(item)->setRect(rect);
                objects.push(item);
            } else if (tag == "properties" && !objects.isEmpty()) {
                TupSerializer::loadProperties(objects.last(), attributes());
            } else if (tag == "brush") {
                QBrush brush;
                TupSerializer::loadBrush(brush, attributes());

                if (parentTag == "pen") {
                    loading = "pen";
                    QPen pen = itemPen();
                    pen.setBrush(brush);
                    setItemPen(pen);
                } else {
                    loading = tag;
                    setItemBrush(brush);
                }

                parentTag = tag;
            } else if (tag == "pen") {
                QPen pen;
                parentTag = tag;
                loading = tag;
                TupSerializer::loadPen(pen, attributes());
                setItemPen(pen);
            } else if (tag == "font") {
                QFont font;
                TupSerializer::loadFont(font, attributes());
            } else if (tag == "stop") {
                if (gradient) {
                    QColor c(attributes().value("colorName"));
                    c.setAlpha(attributes().value("alpha").toInt());
                    gradient->setColorAt(attributes().value("value").toDouble(), c);
                }
            } else if (tag == "gradient") {
                gradient = TupSerializer::createGradient( attributes());
            }
        } else if (isEndElement()) { // Ending Tag
            if (tag == "path") {
                objects.pop();
            } else if (tag == "rect") {
                objects.pop();
            } else if (tag == "ellipse") {
                objects.pop();
            } else if (tag == "gradient") {
                if (loading == "brush")
                    setItemGradient(*gradient, true);
                else
                    setItemGradient(*gradient, false);
            }
        }
    }

    if (hasError()) {
        #ifdef TUP_DEBUG
            qDebug() << "[TupFactoryHandler::parser()] - Fatal Error: Can't process xml!";
        #endif
        return false;
    }

    return true;
}
