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

#ifndef TUPBRUSHESPANEL_H
#define TUPBRUSHESPANEL_H

#include "tupglobal.h"

#include <QWidget>
#include <QPainter>
#include <QPaintEvent>
#include <QPen>
#include <QSize>

class TupBrushesPanel : public QWidget
{
    Q_OBJECT

    public:
        TupBrushesPanel(QColor color, Qt::BrushStyle brushStyle, double opacity, QWidget *parent = nullptr);
        ~TupBrushesPanel();

        void setParameters(QColor color, double opacity);
        QSize minimumSizeHint() const;
        QSize sizeHint() const;

    signals:
        void styleClick(Qt::BrushStyle style);

    protected:
        void paintEvent(QPaintEvent *event);
        void mousePressEvent(QMouseEvent *event);
   
    private:
        bool isCellPhone;

        QColor color;
        QColor bgColor;

        double opacity;
        int stepX;
        int stepY;
        QList<QPoint> points;
        int selected;
};

#endif
