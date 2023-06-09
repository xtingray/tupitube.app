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

#ifndef TUPFACTORYHANDLER_H
#define TUPFACTORYHANDLER_H

#include <QXmlStreamReader>
#include <QStack>
#include <QPen>
#include <QBrush>
#include <QGraphicsItem>

class TupFactoryHandler : public QXmlStreamReader
{
    public:
        enum Type
        {
            Vectorial = 1,
            Library
        };

        TupFactoryHandler();
        TupFactoryHandler(const QString &xml);
    	~TupFactoryHandler();
    	
        QGraphicsItem* getItem();
        QString itemID(const QString &xml);
        Type getType();

    private:
    	void setItemPen(const QPen &pen);
    	void setItemBrush(const QBrush &brush);
    	void setItemGradient(const QGradient& gradient, bool brush);
        bool parse();

    	QPen itemPen() const;
    	QBrush itemBrush() const;
    	
    	QGraphicsItem* createItem(const QString &xml);
    	
    private:
        QGraphicsItem *item;
        QGradient *gradient;
        QString loading; // brush or pen

        // QStack<TupItemGroup *> groups;
        QStack<QGraphicsItem *> objects;
        // bool addToGroup;
        bool isLoading;
        Type type;
        QString parentTag;
};

#endif
