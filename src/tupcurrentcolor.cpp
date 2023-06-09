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

#include "tupcurrentcolor.h"

#include <QPainter>

TupCurrentColor::TupCurrentColor(QBrush qBrush, double transp, QSize dimension,
                                 QWidget *parent) : QGraphicsView(parent)
{
    brush = qBrush;
    opacity = transp;
    setFixedWidth(dimension.width());
    size = dimension;
    setUpdatesEnabled(true);
}

TupCurrentColor::~TupCurrentColor()
{
}

void TupCurrentColor::paintEvent(QPaintEvent *)
{
    QPainter painter(viewport());
    QColor color;
    if (opacity > 0) {
        painter.setOpacity(opacity);

        QRect bg = QRect(0, 0, size.width(), size.height());
        painter.fillRect(bg, QBrush(Qt::white));

        QRect cell = QRect(0, 0, size.width(), size.height());
        painter.fillRect(cell, brush);

        color = QColor(200, 200, 200);
    } else {
        painter.setOpacity(1);
        color = QColor(80, 80, 80);
    }

    painter.setPen(QPen(color, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    QRect border = QRect(0, 0, size.width() - 1, size.height() - 1);
    painter.drawRect(border);
}

void TupCurrentColor::setParameters(const QBrush &b, double transp)
{
#ifdef TUP_DEBUG
    qDebug() << "TupCurrentColor::setParameters() - brush -> " << b;
    qDebug() << "TupCurrentColor::setParameters() - opacity -> " << transp;
#endif

    brush = b;
    opacity = transp;

    update();
}
