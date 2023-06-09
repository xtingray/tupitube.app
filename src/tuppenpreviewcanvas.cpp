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

#include "tuppenpreviewcanvas.h"

#include <QPainter>

TupPenPreviewCanvas::TupPenPreviewCanvas(CanvasType type, int size, QColor colorParam, Qt::BrushStyle styleParam,
                                         double transparency, QWidget *parent) : QWidget(parent)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    canvasType = type;
    penSize = size;
    color = colorParam;
    opacity = transparency;
    style = styleParam;
}

TupPenPreviewCanvas::~TupPenPreviewCanvas()
{
}

void TupPenPreviewCanvas::setParameters(const QColor &colorParam, double transparency, Qt::BrushStyle styleParam)
{
    color = colorParam;
    opacity = transparency;
    style = styleParam;
}

void TupPenPreviewCanvas::setParameters(const QColor &colorParam, Qt::BrushStyle styleParam)
{
    color = colorParam;
    style = styleParam;
}

void TupPenPreviewCanvas::setParameters(int size, double transparency, Qt::BrushStyle styleParam)
{
    penSize = size;
    opacity = transparency;
    style = styleParam;
}

void TupPenPreviewCanvas::setParameters(const QPen &pen)
{
    penSize = pen.width();
    color = pen.color();
    style = pen.brush().style();
}

void TupPenPreviewCanvas::updatePenSize(int size)
{
    penSize = size;
    update();
}

void TupPenPreviewCanvas::updateOpacity(double transp)
{
#ifdef TUP_DEBUG
    qDebug() << "TupPenPreviewCanvas::updateOpacity() - color: " << color;
    qDebug() << "TupPenPreviewCanvas::updateOpacity() - opacity: " << transp;
#endif

    opacity = transp;
    color.setAlpha(static_cast<int>(opacity * 255));
    update();
}

QSize TupPenPreviewCanvas::minimumSizeHint() const
{
    return QSize(100, 106);
}

QSize TupPenPreviewCanvas::sizeHint() const
{
    return QSize(100, 106);
}

void TupPenPreviewCanvas::paintEvent(QPaintEvent *)
{
     QPainter painter(this);
     painter.setRenderHint(QPainter::Antialiasing, true);

     QColor bg = Qt::white;
     if ((color.red() > 200) && (color.green() > 200) && (color.blue() > 200))
         bg = QColor(50, 50 ,50);
     painter.fillRect(0, 0, width(), height(), bg);

     QPen border(QColor(0, 0, 0));
     border.setWidth(1);
     painter.setPen(border);
     painter.drawRect(0, 0, width(), height());

     painter.translate(width() / 2, height() / 2);

     QBrush brush;
     brush = QBrush(color, style);

     if (canvasType == Border) {
         QPen pen(Qt::NoPen);
         painter.setPen(pen);
         painter.setBrush(brush);
         painter.setOpacity(opacity);
         if (penSize > 1)
             painter.drawEllipse(-(penSize/2), -(penSize/2), penSize, penSize);
         else
             painter.drawEllipse(-1, -1, 3, 3);
     } else if (canvasType == Fill) {
         QPen pen(Qt::NoPen);
         painter.setPen(pen);
         painter.setBrush(brush);
         painter.setOpacity(opacity);
         QRect rect;
         rect = QRect(-(penSize/2), -(penSize/2), penSize, penSize);
         painter.drawRect(rect);
     }
}
