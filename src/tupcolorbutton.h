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

#ifndef TUPCOLORBUTTON_H
#define TUPCOLORBUTTON_H

#include "tupglobal.h"

#include <QBrush>
#include <QSize>
#include <QPaintEvent>
#include <QWidget>

class TupColorButton : public QWidget
{
    Q_OBJECT

    public:
        TupColorButton(int index, const QBrush &brush, const QSize &size, const QString &buttonParams);
        ~TupColorButton();

        QSize sizeHint() const;
        QColor color();
        void setState(bool isSelected);
        bool isSelected();
        void setBrush(const QBrush &brush);
        void setBrushStyle(Qt::BrushStyle style);
        void setEditable(bool flag);

    protected:
        void paintEvent(QPaintEvent *painter);
        void mousePressEvent(QMouseEvent *event);
        void mouseDoubleClickEvent(QMouseEvent *event);

    signals:
        void clicked(int index);
        void doubledClicked(int index);

    private:
        int border1;
        int border2;
        int border3;

        QBrush brush;
        int index;
        bool editable;
        bool selected;
        QSize size;
};

#endif
