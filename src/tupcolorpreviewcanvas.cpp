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

#include "tupcolorpreviewcanvas.h"

#include <QPainter>

TupColorPreviewCanvas::TupColorPreviewCanvas(QPen pen, double transp, QWidget *parent) : QWidget(parent)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    color = pen.color();
    opacity = transp;
    style = pen.brush().style();
}

TupColorPreviewCanvas::~TupColorPreviewCanvas()
{
}

void TupColorPreviewCanvas::render(const QColor c)
{
    color = c;
    update();
}

QSize TupColorPreviewCanvas::minimumSizeHint() const
{
    return QSize(100, 106);
}

QSize TupColorPreviewCanvas::sizeHint() const
{
    return QSize(100, 106);
}

void TupColorPreviewCanvas::paintEvent(QPaintEvent *)
{
     int w = width();
     int h = height();
     int middleX = w/2;
     int middleY = h/2;

     QPainter painter(this);
     painter.setRenderHint(QPainter::Antialiasing, true);
     painter.fillRect(0, 0, w, h, QColor(255, 255, 255));

     QPen border(QColor(0, 0, 0));
     border.setWidth(1);
     painter.setPen(border);
     painter.drawRect(0, 0, w, h);

     QBrush brush;
     brush = QBrush(color, style);

     painter.translate(middleX, middleY);

     QPen pen(Qt::NoPen);
     painter.setPen(pen);
     painter.setBrush(brush);
     painter.setOpacity(opacity);

     QRectF rect = QRectF(-(h/4), -(h/4), middleY, middleY);
     painter.fillRect(rect, brush);
}
