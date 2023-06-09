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

#ifndef TUPCOLORPREVIEWCANVAS_H
#define TUPCOLORPREVIEWCANVAS_H

#include "tupglobal.h"

#include <QWidget>
#include <QPaintEvent>
#include <QPen>
#include <QSize>

class TupColorPreviewCanvas : public QWidget
{
    Q_OBJECT

    public:
        TupColorPreviewCanvas(QPen pen, double opacity, QWidget *parent = nullptr);
        ~TupColorPreviewCanvas();

        QSize minimumSizeHint() const;
        QSize sizeHint() const;

    public slots:
        void render(const QColor color);
        
    protected:
        void paintEvent(QPaintEvent *e);
   
    private:
        double opacity;
        QColor color;
        Qt::BrushStyle style;
};

#endif
