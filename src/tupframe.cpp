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

#include "tupframe.h"
#include "tupitemfactory.h"

#include <QTextStream>

TupFrame::TupFrame() : QObject()
{
}

TupFrame::~TupFrame()
{
}

void TupFrame::fromXml(const QString &xml)
{
    #ifdef TUP_DEBUG
        qDebug() << "TupFrame::fromXml() - Loading frame...";
        // qDebug() << xml;
    #endif

    QDomDocument doc;
    if (doc.setContent(xml)) {
        QDomElement root = doc.documentElement();
        QDomNode n = root.firstChild();
        while (!n.isNull()) {
            QDomElement e = n.toElement();
            QString itemXml;
            {
                QTextStream ts(&itemXml);
                ts << n;
            }
            TupItemFactory factory;
            QGraphicsItem *graphic = factory.create(itemXml);
            // factory.loadItem(graphic, itemXml);

            if (e.tagName() == "path") {
                TupPathItem *item = qgraphicsitem_cast<TupPathItem *> (graphic);
                addItem(item, itemXml);
                opacities << item->opacity();
            } else if (e.tagName() == "rect") {
                TupRectItem *item = qgraphicsitem_cast<TupRectItem *> (graphic);
                addItem(item, itemXml);
                opacities << item->opacity();
            } else if (e.tagName() == "ellipse") {
                TupEllipseItem *item = qgraphicsitem_cast<TupEllipseItem *> (graphic);
                addItem(item, itemXml);
                opacities << item->opacity();
            } else {
                #ifdef TUP_DEBUG
                    qDebug() << "TupCanvas::loadFrame() - Unknown object: " << e.tagName();
                #endif
            }
            n = n.nextSibling();
        }
    }
}

QDomElement TupFrame::toXml(QDomDocument &doc) const
{
    QDomElement root = doc.createElement("frame");
    root.setAttribute("name", "Frame");
    doc.appendChild(root);

    int count = graphics.count();
    for (int i=0; i<count; i++) {
         QDomElement object = doc.createElement("object");
         if (TupPathItem *item = qgraphicsitem_cast<TupPathItem *>(graphics.at(i))) {
             item->setOpacity(opacities.at(i));
             object.appendChild(item->toXml(doc));
             root.appendChild(object);
         } else if (TupRectItem *item = qgraphicsitem_cast<TupRectItem *>(graphics.at(i))) {
             item->setOpacity(opacities.at(i));
             object.appendChild(item->toXml(doc));
             root.appendChild(object);
         } else if (TupEllipseItem *item = qgraphicsitem_cast<TupEllipseItem *>(graphics.at(i))) {
             item->setOpacity(opacities.at(i));
             object.appendChild(item->toXml(doc));
             root.appendChild(object);
         }
    }

    return root;
}

QString TupFrame::toString() const
{
    #ifdef TUP_DEBUG
        qDebug() << "TupFrame::toString() - Saving frame...";
    #endif

    QString frame = "<frame name=\"Frame\">";
    int total = storageStack.count();
    for (int i=0; i<total; i++)
         frame += storageStack.at(i);
    frame += "</frame>";

    return frame;
}

void TupFrame::addItem(TupPathItem *item, const QString &path)
{
    if (item) {
        graphics << item;
        opacities << item->opacity();
        storageStack << path;
    }
}

void TupFrame::addItem(TupRectItem *item, const QString &path)
{
    if (item) {
        graphics << item;
        opacities << item->opacity();
        storageStack << path;
    }
}

void TupFrame::addItem(TupEllipseItem *item, const QString &path)
{
    if (item) {
        graphics << item;
        opacities << item->opacity();
        storageStack << path;
    }
}

void TupFrame::addItem(TupPathItem *item)
{
    if (item) {
        graphics << item;
        opacities << item->opacity();

        QDomDocument doc;
        QDomElement xml = item->toXml(doc);
        QString path;
        {
            QTextStream text(&path);
            text << xml;
        }
        storageStack.append(path);
    }
}

void TupFrame::addItem(TupRectItem *item)
{
    if (item) {
        graphics << item;
        opacities << item->opacity();

        QDomDocument doc;
        QDomElement xml = item->toXml(doc);
        QString path;
        {
            QTextStream text(&path);
            text << xml;
        }
        storageStack.append(path);
    }
}

void TupFrame::addItem(TupEllipseItem *item)
{
    if (item) {
        graphics << item;
        opacities << item->opacity();

        QDomDocument doc;
        QDomElement xml = item->toXml(doc);
        QString path;
        {
            QTextStream text(&path);
            text << xml;
        }
        storageStack.append(path);
    }
}

QGraphicsItem * TupFrame::undoItem()
{
    if (graphics.count() > 0) {
        QGraphicsItem *item = graphics.takeLast();
        qreal opacity = opacities.takeLast();
        redoStack << item;
        redoOpacities << opacity;
        redoStrings << storageStack.takeLast();
        return item;
    }

    return nullptr;
}

QGraphicsItem * TupFrame::redoItem()
{
    if (redoStack.count() > 0) {
        QGraphicsItem *item = redoStack.takeLast();
        qreal opacity = redoOpacities.takeLast();
        item->setOpacity(opacity);
        graphics << item;
        opacities << opacity;
        storageStack << redoStrings.takeLast();
        return item;
    }

    return nullptr;
}

void TupFrame::clear()
{
    if (graphics.count() > 0) {
        redoStack = graphics;
        redoStrings = storageStack;
        redoOpacities = opacities;
        graphics.clear();
        opacities.clear();
        storageStack.clear();
    }
}

QList<QGraphicsItem *> TupFrame::items()
{
    return graphics;
} 

bool TupFrame::isEmpty()
{
    return graphics.isEmpty();
}

int TupFrame::count()
{
    return graphics.count();
}

bool TupFrame::undoIsAvailable()
{
    return graphics.count() > 0;
}

bool TupFrame::redoIsAvailable()
{
    return redoStack.count() > 0;
}

bool TupFrame::timeToUndo()
{
    return graphics.count() == 1;
}

bool TupFrame::timeToRedo()
{
    return redoStack.count() == 1;
}

qreal TupFrame::opacity(int index)
{
    return opacities.at(index);
}

void TupFrame::updateOpacities()
{
    int count = graphics.count();
    for (int i=0; i<count; i++) {
         QGraphicsItem *item = graphics.at(i);
         item->setOpacity(opacities.at(i));
    }
}
