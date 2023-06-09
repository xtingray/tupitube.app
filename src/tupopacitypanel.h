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

#ifndef TUPOPACITYPANEL_H
#define TUPOPACITYPANEL_H

#include "tupglobal.h"
#include "tuppenpreviewcanvas.h"
#include "tupslider.h"

#include <QWidget>
#include <QBoxLayout>
#include <QLabel>
#include <QSettings>
#include <QResizeEvent>
#include <QColor>

class TupOpacityPanel : public QWidget
{
    Q_OBJECT

    public:
        TupOpacityPanel(QSettings *settings, TupPenPreviewCanvas::CanvasType type, QWidget *parent = nullptr);
        ~TupOpacityPanel();

        void setOpacityValue(double value);
        void resetOpacityValue();
        double opacityValue();
        void setParameters(QPen pen);
        void setParameters(QColor color, Qt::BrushStyle brushStyle);

    protected:
        void resizeEvent(QResizeEvent *event);

    signals:
        void opacityZero(TupPenPreviewCanvas::CanvasType panelType);

    private slots:
        void modifyOpacity(int value);

    private:
        void setOpacityCanvas();
        void setLabelPanel();
        void setSlider(int sliderW, int sliderH);

        bool isCellPhone;
        bool lowRes;
        int titleSize;
        int titleHeight;
        QString borderRadius;
        QString imgPath;
        QSize sliderSize;

        QSettings *settings;
        TupPenPreviewCanvas::CanvasType canvasType;
        QVBoxLayout *innerLayout;

        TupPenPreviewCanvas *opacityPreview;
        TupSlider *slider;
        QLabel *opacityLabel;
        QColor color;

        int penSize;
        double opacity;
        Qt::BrushStyle style;
};

#endif
