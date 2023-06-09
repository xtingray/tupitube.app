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

#ifndef TUPSLIDER_H
#define TUPSLIDER_H

#include "tupglobal.h"

#include <QGraphicsView>
#include <QMouseEvent>
#include <QPixmap>
#include <QPainterPath>

class TupSlider : public QGraphicsView
{
    Q_OBJECT

    public:
        enum Mode { Color = 0, Size, Opacity, Dial };

        TupSlider(Qt::Orientation orientation, const QString &imgPath, QSize sliderSize,
                  Mode mode, const QColor& start, const QColor& end, QWidget *parent = nullptr);
        ~TupSlider();

        void setParameters(Qt::BrushStyle style, double opacity);
        void setRange(int min, int max);
        void setColors(const QColor& start, const QColor& end);
        void setValue(int value);
        int currentValue();
        void setEnabled(bool flag);
        bool isEnabled();

    protected:
        void mousePressEvent(QMouseEvent *event);
        void mouseMoveEvent(QMouseEvent *event);
        void paintEvent(QPaintEvent *event);

        void paintScales();

    signals:
        void valueChanged(int v);

    private:
       void calculateNewPosition(int pos);

       int currentBase;       
       int min;
       int max;
       QColor startColor;
       QColor endColor;
       QPixmap pixmap;
       int value;
       bool enabled;

       Qt::Orientation orientation;
       TupSlider::Mode mode;
       Qt::BrushStyle style;
       double opacity;
};

#endif
