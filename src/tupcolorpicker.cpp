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

#include "tupcolorpicker.h"

#include <QPoint>
#include <QColor>
#include <QImage>
#include <QSizePolicy>
#include <QPixmap>
#include <QSize>
#include <QRect>
#include <QPainter>
#include <QMouseEvent>

TupColorPicker::TupColorPicker(QPixmap *paletteImg, QWidget* parent) : QFrame(parent)
{
    setFrameStyle(QFrame::Box | QFrame::Sunken);
    setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
    setAttribute(Qt::WA_NoSystemBackground);

    palette = paletteImg;

    hue = 0;
    saturation = 0;
}

TupColorPicker::~TupColorPicker()
{
}

QPoint TupColorPicker::colorPoint()
{ 
    return QPoint((360-hue) * (imgWidth-1) / 360, (255-saturation) * (imgHeight-1) / 255);
}

int TupColorPicker::huePoint(const QPoint &pt)
{ 
    return 360 - pt.x() * 360 / (imgWidth-1);
}

int TupColorPicker::saturationPoint(const QPoint &pt)
{ 
    return 255 - pt.y() * 255 / (imgHeight-1);
}

void TupColorPicker::setColor(const QPoint &pt)
{ 
    setColor(huePoint(pt), saturationPoint(pt)); 
}

void TupColorPicker::setColor(int hueParam, int satParam)
{
    int newHue = qMin(qMax(0, hueParam), 359);
    int nsat = qMin(qMax(0, satParam), 255);

    if (newHue == hue && nsat == saturation)
        return;

    hue = newHue;
    saturation = nsat;
}

void TupColorPicker::setHUE(int hueParam)
{
    int newHue = qMin(qMax(0, hueParam), 359);
    if (newHue == hue)
        return;

    hue = newHue;
}

void TupColorPicker::setSaturation(int satParam)
{
    int newSat = qMin(qMax(0, satParam), 255);
    if (newSat == saturation)
        return;

    saturation = newSat;
}

void TupColorPicker::mousePressEvent(QMouseEvent *event)
{
    QPoint point = event->pos() - contentsRect().topLeft();
    setColor(point);

    int luminance = 255;
    if (hue == 0 && saturation == 0)
        luminance = 0;

    QColor color;
    color.setHsv(hue, saturation, luminance, 255);

    emit newColor(color);
}

void TupColorPicker::mouseMoveEvent(QMouseEvent *event)
{
    QPoint point = event->pos() - contentsRect().topLeft();
    setColor(point);

    int luminance = 255;
    if (hue == 0 && saturation == 0)
        luminance = 0;

    QColor color;
    color.setHsv(hue, saturation, luminance, 255);

    emit newColor(color);
}

void TupColorPicker::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    drawFrame(&painter);
    QRect rect = contentsRect();

    QPixmap image = palette->scaled(QSize(rect.width(), rect.height()),
                                    Qt::IgnoreAspectRatio, Qt::FastTransformation);
    imgWidth = image.size().width();
    imgHeight = image.size().height();

    painter.drawPixmap(rect.topLeft(), image);
}

int TupColorPicker::hueVal()
{
    return hue;
}

int TupColorPicker::saturationVal()
{
    return saturation;
}
