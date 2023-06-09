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

#include "tupbrushespanel.h"

TupBrushesPanel::TupBrushesPanel(QColor currentColor, Qt::BrushStyle brushStyle, double opacityFactor,
                                 QWidget *parent) : QWidget(parent)
{
#ifdef TUP_DEBUG
    qDebug() << "TupBrushesPanel() - Tracing...";
#endif

    color = currentColor;
    color.setAlpha(static_cast<int>(opacityFactor * 255));
    bgColor = Qt::white;

    if (color.red() > 200 && color.green() > 200 && color.blue() > 200)
        bgColor = QColor(50, 50, 50);

    opacity = opacityFactor;
    stepX = -1;
    stepY = 0;
    selected = brushStyle - 1;
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

TupBrushesPanel::~TupBrushesPanel()
{
}

QSize TupBrushesPanel::minimumSizeHint() const
{
    return QSize(100, 106);
}

QSize TupBrushesPanel::sizeHint() const
{
    return QSize(100, 106);
}

void TupBrushesPanel::paintEvent(QPaintEvent *)
{
     points.clear();
     int w = width();
     int h = height();

     QPainter painter(this);
     painter.setRenderHint(QPainter::Antialiasing, true);
     painter.fillRect(0, 0, w, h, bgColor);

     QPen borderPen(QColor(0, 0, 0));
     borderPen.setWidth(1);
     painter.setPen(borderPen);
     painter.drawRect(0, 0, w, h);

     int rows = 2;
     int columns = 7;
     int delta = 5;
     if (w < h) {
         rows = 7;
         columns = 2;
     }

     int xSep = (columns + 1) * delta;
     int ySep = (rows + 1) * delta;

     stepX = (w - xSep)/columns;
     stepY = (h - ySep)/rows;

     int posX = delta;
     int posY = delta;
     int index = 1;

     painter.setOpacity(opacity);

     for (int i=0; i < rows; i++) {
          for (int j=0; j < columns; j++) {
               QPoint point(posX, posY);
               points << point;
               QRect border = QRect(point, QSize(stepX, stepY));
               QBrush brush(color, Qt::BrushStyle(index));

               // SQA: Take care of these lines to scale brushes in the future
               /*
               QTransform transform;
               transform.scale(3, 3);
               brush.setTransform(transform);
               */

               painter.fillRect(border, brush);
               posX += stepX + delta;
               index++;
          }
          posX = delta;
          posY += stepY + delta;
     }

     if (selected > -1) {
         if (selected < points.size()) {
             QPoint point = points.at(selected);
             QPen rectPen(QColor(0, 196, 255));
             if (color.red() < 200 && color.green() < 200 && color.blue() > 200)
                 rectPen = QPen(QColor(0, 200, 0));
             rectPen.setWidth(3);
             painter.setPen(rectPen);

             QRect border = QRect(point, QSize(stepX, stepY));
             painter.drawRect(border);
         }
     }
}

void TupBrushesPanel::mousePressEvent(QMouseEvent *event)
{
    int x = event->pos().x();
    int y = event->pos().y();

    for (int i=0; i < points.size(); i++) {
         int x0 = points.at(i).x();
         int y0 = points.at(i).y();
         int xmax = x0 + stepX;
         int ymax = y0 + stepY;

         if (x >= x0 && x <= xmax) {
             if (y >= y0 && y <= ymax) {
                 selected = i;
                 update();
                 emit styleClick(Qt::BrushStyle(i+1));
                 return;
             }
         }
    }
}

void TupBrushesPanel::setParameters(QColor currentColor, double opacityFactor)
{
#ifdef TUP_DEBUG
    qDebug() << "TupBrushesPanel::setParameters() - currentColor: " << currentColor;
    qDebug() << "TupBrushesPanel::setParameters() - opacityFactor: " << opacityFactor;
#endif

    color = currentColor;
    opacity = opacityFactor;

    this->update();
}
