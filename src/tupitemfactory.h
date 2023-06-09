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

#ifndef TUPITEMFACTORY_H
#define TUPITEMFACTORY_H

#include "tupglobal.h"
// #include "tupxmlparser.h"
#include "tupfactoryhandler.h"

#include <QStack>
#include <QPen>
#include <QBrush>
#include <QGraphicsItem>

// class QGraphicsItem;

class TupItemFactory : public QObject 
{
    public:
        enum Type
        {
            Vectorial = 1,
            Library 
        };

    	TupItemFactory();
    	~TupItemFactory();

        QGraphicsItem *create(const QString &xml);
        QString itemID(const QString &xml);
        // TupFactoryHandler::Type getType();
        // QPointF itemPos();

        /*
    	bool startTag(const QString& qname, const QXmlAttributes& atts);
    	void text(const QString &ch);
    	bool endTag(const QString& qname);

    	bool loadItem(QGraphicsItem *item, const QString &xml);
        QString itemID(const QString &xml);
        QPointF itemPos();
        */

    /* 
    private:
    	void setItemPen(const QPen &pen);
    	void setItemBrush(const QBrush &brush);
    	void setItemGradient(const QGradient& gradient, bool brush);
    	QPen itemPen() const;
    	QBrush itemBrush() const;
    	QGraphicsItem* createItem(const QString &xml);

        QGraphicsItem *item;
        QGradient *gradient;
        QString loading;
        QStack<QGraphicsItem *> objects;
        bool isLoading;
        QString textReaded;
        TupItemFactory::Type type;
    */

    private:
        TupFactoryHandler handler;
};

#endif
