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

#include "tupslider.h"

TupSlider::TupSlider(Qt::Orientation position, const QString &imgPath, QSize sliderSize, TupSlider::Mode modeVar,
                     const QColor &start, const QColor &end, QWidget *parent) : QGraphicsView(parent)
{
    orientation = position;
    mode = modeVar;
    startColor = start;
    endColor = end;
    value = 0;
    enabled = true;

    if (orientation == Qt::Vertical) {
        QPixmap *pmx = new QPixmap(QString(imgPath + "tip_v.png"));
        pixmap = pmx->scaledToWidth(sliderSize.width() - 2);
    } else {
        QPixmap *pmx = new QPixmap(QString(imgPath + "tip_h.png"));
        pixmap = pmx->scaledToWidth(sliderSize.height() - 2);
    }

    setUpdatesEnabled(true);

    if (orientation == Qt::Vertical)
        currentBase = this->height();
    else
        currentBase = this->width();
}

TupSlider::~TupSlider()
{
}

void TupSlider::setParameters(Qt::BrushStyle styleVar, double transp)
{
    style = styleVar;
    opacity = transp;
    this->update();
}

void TupSlider::setRange(int minVar, int maxVar)
{
    min = minVar;
    max = maxVar;
}

void TupSlider::setValue(int param)
{	
    if (orientation == Qt::Vertical) {
        int height = this->height();
        if (param == max) {
            value = height - pixmap.size().height();
        } else if (param == min) {
            value = 0;
        } else {
            value = (height * param) / max;
        }
    } else {
        int width = this->width();
        /*
        qWarning() << "";
        qWarning() << "setValue()";
        qWarning() << "slider width: " << width;
        qWarning() << "slider value: " << value;
        */
        if (param == max) {
            value = width - (pixmap.size().width());
            /*
            qWarning() << "*** MAX ***";
            qWarning() << "max new position: " << value;
            qWarning() << "max pixmap width: " << pixmap.size().width();
            qWarning() << "***";
            */
        } else if (param == min) {
            value = 0;
        } else {
            value = (width * param) / max;
        }
    }

    this->update();
}

void TupSlider::setEnabled(bool flag)
{
    enabled = flag;
    this->update();
}

bool TupSlider::isEnabled()
{
    return enabled;
}

void TupSlider::setColors(const QColor &start, const QColor &end)
{
#ifdef TUP_DEBUG
    qDebug() << "TupSlider::setColors() - start color: " << start;
    qDebug() << "TupSlider::setColors() - end color: " << end;
#endif

    startColor = start;
    endColor = end;
    if (endColor.alpha() != 255)
        endColor.setAlpha(255);

    this->update();
}

void TupSlider::mousePressEvent(QMouseEvent *event)
{
    if (!enabled)
        return;

    int pos = -1;
    if (orientation == Qt::Vertical)
        pos = event->position().y();
    else
        pos = event->position().x();

    calculateNewPosition(pos);
}

void TupSlider::mouseMoveEvent(QMouseEvent *event)
{
    if (!enabled)
        return;

    int pos = -1;
    if (orientation == Qt::Vertical)
        pos = event->position().y();
    else
        pos = event->position().x();

    calculateNewPosition(pos);
}

void TupSlider::calculateNewPosition(int pos)
{
/*
#ifdef TUP_DEBUG
    qDebug() << "TupSlider::calculateNewPosition() - pos: " << pos;
#endif
*/

    int length = -1;

    if (orientation == Qt::Vertical) {
        length = this->height();
        value = pos;
        if (pos > (length - pixmap.size().height())) {
            value = length - pixmap.size().height();
            this->update();
            emit valueChanged(max);
            return;
        } else if (pos < pixmap.size().height()) {
            value = pixmap.size().height();
            this->update();
            emit valueChanged(min);
            return;
        }

    } else {
        length = this->width();
        value = pos;
        if (pos > (length - pixmap.size().width())) {
            value = length - pixmap.size().width();
            this->update();
            emit valueChanged(max);
            return;
        } else if (pos < pixmap.size().width()) {
            value = pixmap.size().width();
            this->update();
            emit valueChanged(min);
            return;
        }
    }

    int value = -1;
    if (orientation == Qt::Vertical)
        value = min + (max - min) * (1.0 - float(pos)/float(length));
    else 
        value = min + (max - min) * (float(pos)/float(length));

    if (value < 0)
        value = 0;

    if (value < min)
        value = min;

    this->update();
    emit valueChanged(value);
}

void TupSlider::paintScales()
{
    QPainter painter(viewport());

    if (!enabled) {
        QColor color(232, 232, 232);
        painter.setPen(color);
        painter.setBrush(color);
        painter.drawRect(0, 0, this->width(), this->height());
        return;
    }

    int width = this->width();
    /*
    qWarning() << "";
    qWarning() << "paintScales()";
    qWarning() << "painter width: " << width;
    */

    int height = this->height();
    int imageW = pixmap.size().width() - 16;
    int imageH = pixmap.size().height() - 16;
    int length = this->width();
    if (orientation == Qt::Vertical)
        length = this->height();
    int segments = 32;
    int delta = length/(segments - 1);

    if (mode == Color) {
        for (int section=0; section<=segments; section++) {
             QColor color;
             int r;
             int g;
             int b;
             r = section*(endColor.red() - startColor.red()) / segments + startColor.red();
             g = section*(endColor.green() - startColor.green()) / segments + startColor.green();
             b = section*(endColor.blue() - startColor.blue()) / segments + startColor.blue();

             if ((r > -1 && r < 256) && (g > -1 && g < 256) && (b > -1 && b < 256)) {
                 color.setRed(r);
                 color.setGreen(g);
                 color.setBlue(b);

                 painter.setPen(color);
                 painter.setBrush(color);

                 if (orientation == Qt::Vertical)
                     painter.drawRect((width - imageW)/2, section*delta, imageW, delta);
                 else 
                     painter.drawRect(section * delta, (height - imageH)/2, delta, imageH);
            }
        }
    } else if (mode == Size) {
               height = pixmap.height();
               QPen pen(endColor);
               pen.setWidth(1);
               painter.setPen(pen);
               painter.setOpacity(opacity);
               painter.setBrush(QBrush(endColor, style));
               painter.setRenderHint(QPainter::Antialiasing);

               QPainterPath path;
               if (orientation == Qt::Vertical) {
                   path = QPainterPath(QPointF(0, 0));
                   path.lineTo(QPointF(0, height));
                   path.lineTo(QPointF(width, height));
                   path.lineTo(QPointF(0, 0));
               } else {
                   path = QPainterPath(QPointF(0, height - 2));
                   path.lineTo(QPointF(width, 0 + 2));
                   path.lineTo(QPointF(width, height - 2));
                   path.lineTo(QPointF(0, height - 2));
               }
               painter.drawPath(path);
               painter.setOpacity(1.0);
    } else if (mode == Opacity) {
               double opacityDelta = 1.0/32; 
               double opacity = 0;
               for (int section=0; section<=segments; section++) {
                    painter.setPen(QColor(232, 232, 232));
                    painter.setBrush(endColor);
                    painter.setOpacity(opacity);
                    opacity += opacityDelta;

                    if (orientation == Qt::Vertical)
                        painter.drawRect((width - imageW) / 2, section * delta, imageW, delta);
                    else
                        painter.drawRect(section * delta, (height - imageH) / 2, delta, imageH);
               }
               painter.setOpacity(1.0);
    } else if (mode == Dial) {
        painter.setPen(endColor);
        painter.setBrush(endColor);
        painter.drawRect(0, (height - imageH)/2, this->width(), imageH);
    }
	
#ifndef Q_OS_ANDROID
    if (orientation == Qt::Vertical)
        currentBase = this->height();
    else
        currentBase = this->width();
#endif

    if (orientation == Qt::Vertical) {
        int h = this->height();
        int imgW = pixmap.size().width();

        if (value >= h || currentBase != h) {
            value = (value * h)/currentBase;
            currentBase = h;
        }

        painter.drawPixmap((width/2)-(imgW/2), value, pixmap);
    } else {
        int w = this->width();
        /*
        qWarning() << "***";
        qWarning() << "value: " << value;
        qWarning() << "painting mark -> width: " << w;
        qWarning() << "painting mark -> currentBase previous: " << currentBase;
        */

        int imgH = pixmap.size().height();

        if (value >= w || currentBase != w) {
            value = (value * w)/currentBase;
            currentBase = w;
        }

        /*
        qWarning() << "painting mark -> mark pos: " << value;
        qWarning() << "currentBase: " << currentBase;
        qWarning() << "***";
        qWarning() << "";
        */
        painter.drawPixmap(value, (height/2)-(imgH/2), pixmap);
    }
}

void TupSlider::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    paintScales();
}

int TupSlider::currentValue()
{
    return value;
}
