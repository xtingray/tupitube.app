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

#ifndef TUPPENPREVIEWCANVAS_H
#define TUPPENPREVIEWCANVAS_H

#include "tupglobal.h"

#include <QWidget>
#include <QPaintEvent>
#include <QSize>

class TupPenPreviewCanvas : public QWidget
{
    Q_OBJECT

    public:
        enum CanvasType { Border = 0, Fill };

        TupPenPreviewCanvas(CanvasType type, int penSize, QColor colorParam, Qt::BrushStyle styleParam,
                            double transparency, QWidget *parent = nullptr);
        ~TupPenPreviewCanvas();

        QSize minimumSizeHint() const;
        QSize sizeHint() const;

        void setParameters(const QColor &colorParam, double transparency, Qt::BrushStyle styleParam);
        void setParameters(const QColor &colorParam, Qt::BrushStyle styleParam);
        void setParameters(int size, double transparency, Qt::BrushStyle styleParam);
        void setParameters(const QPen &pen);

    public slots:
        void updatePenSize(int size);
        void updateOpacity(double transparency);

    protected:
        void paintEvent(QPaintEvent *);
   
    private:
        CanvasType canvasType;

        int penSize;
        double opacity;
        QColor color;
        Qt::BrushStyle style;
};

#endif
