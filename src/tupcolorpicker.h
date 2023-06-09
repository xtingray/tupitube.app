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

#ifndef TUPCOLORPICKER_H
#define TUPCOLORPICKER_H

#include "tupglobal.h"

#include <QFrame>

class TupColorPicker : public QFrame
{
    Q_OBJECT

    public:
        TupColorPicker(QPixmap *paletteImg, QWidget* parent = nullptr);
        ~TupColorPicker();

        int hueVal();
        int saturationVal();

    public slots:
        void setColor(int hueParam, int satParam);
        void setHUE(int hueVal);
        void setSaturation(int saturationVal);

    signals:
        void newColor(const QColor color);

    protected:
        void paintEvent(QPaintEvent *);
        void mousePressEvent(QMouseEvent *);
        void mouseMoveEvent(QMouseEvent *);

    private:
        int hue;
        int saturation;
        QPixmap *palette;
        int imgWidth;
        int imgHeight;

        QPoint colorPoint();
        int huePoint(const QPoint &pt);
        int saturationPoint(const QPoint &pt);
        void setColor(const QPoint &pt);
};

#endif

