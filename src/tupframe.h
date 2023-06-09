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

#ifndef TUPFRAME_H
#define TUPFRAME_H

#include "tupglobal.h"
#include "tuppathitem.h"
#include "tuprectitem.h"
#include "tupellipseitem.h"

#include <QList>

class TupFrame : public QObject
{
    Q_OBJECT

    public:
       TupFrame();
       ~TupFrame();

       void addItem(TupPathItem *item);
       void addItem(TupRectItem *item);
       void addItem(TupEllipseItem *item);

       QList<QGraphicsItem *> items();
       QGraphicsItem * undoItem();
       QGraphicsItem * redoItem();
       void clear();
       bool isEmpty();
       int count();
       bool undoIsAvailable();
       bool timeToUndo();
       bool timeToRedo();
       bool redoIsAvailable();
       qreal opacity(int index);
       void updateOpacities();

       void fromXml(const QString &xml);
       QDomElement toXml(QDomDocument &doc) const;
       QString toString() const;

    private:
       void addItem(TupPathItem *item, const QString &path);
       void addItem(TupRectItem *item, const QString &path);
       void addItem(TupEllipseItem *item, const QString &path);

       QList<QGraphicsItem *> graphics;
       QList<QGraphicsItem *> redoStack;
       QList<QString> storageStack;
       QList<QString> redoStrings;
       QList<qreal> opacities;
       QList<qreal> redoOpacities;
};

#endif
