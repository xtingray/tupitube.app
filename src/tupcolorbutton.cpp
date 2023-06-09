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

#include "tupcolorbutton.h"

#include <QPainter>

TupColorButton::TupColorButton(int i, const QBrush &b, const QSize &s,
                               const QString &params)
{
    index = i;
    editable = true;
    selected = false;
    brush = b;
    size = s;

    QStringList values = params.split(",");
    border1 = values.at(0).toInt();
    border2 = values.at(1).toInt();
    border3 = values.at(2).toInt();

    setFixedSize(size);
}

TupColorButton::~TupColorButton()
{
}

QSize TupColorButton::sizeHint() const
{
    return size;
}

void TupColorButton::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    painter.fillRect(rect(), brush);
    QRect border = rect();

    if (selected && editable) {
        painter.setPen(QPen(QColor(200, 200, 200), border1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        painter.drawRect(border);
        painter.setPen(QPen(QColor(190, 190, 190), border2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        painter.drawRect(border);
        painter.setPen(QPen(QColor(150, 150, 150), border3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        painter.drawRect(border);
    } else {
        painter.setPen(QPen(QColor(190, 190, 190), border3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        painter.drawRect(border);
    }
}

void TupColorButton::mousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event)

    emit clicked(index);
    setState(true);
}

void TupColorButton::mouseDoubleClickEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
    emit doubledClicked(index);
}

QColor TupColorButton::color()
{
    return brush.color();
}

void TupColorButton::setState(bool isSelected)
{
    selected = isSelected;
    update();
}

bool TupColorButton::isSelected()
{
    return selected;
}

void TupColorButton::setBrush(const QBrush &b)
{
    brush = b;
    update();
}

void TupColorButton::setBrushStyle(Qt::BrushStyle style)
{
    brush.setStyle(style);
    update();
}

void TupColorButton::setEditable(bool flag)
{
    editable = flag;
}
