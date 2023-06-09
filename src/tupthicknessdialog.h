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

#ifndef TUPTHICKNESSDIALOG_H
#define TUPTHICKNESSDIALOG_H

#include <QDialog>
#include <QSettings>
#include <QKeyEvent>
#include <QResizeEvent>
#include <QBoxLayout>
#include <QLabel>
#include <QPen>

#include "tupglobal.h"
#include "tuppenpreviewcanvas.h"
#include "tupslider.h"

class TupThicknessDialog : public QDialog
{
    Q_OBJECT

    public:
        TupThicknessDialog(QSettings *settings, QPen penVar, double opacity, QWidget *parent = nullptr);
        ~TupThicknessDialog();

        void setParameters(int size, QPen penVar, double transp);

    signals:
        void penUpdated(int width);

    protected:
        void resizeEvent(QResizeEvent *);
        void mousePressEvent(QMouseEvent *);

    private slots:
        void modifySize(int value);
        void closeDialog();

    private:
        void setBrushCanvas();
        void setLabelPanel();
        void setSlider(int sliderW, int sliderH);
        void setButtonsPanel();

        QVBoxLayout *innerLayout;
        TupPenPreviewCanvas *thickPreview;
        QPen pen;
        TupSlider *slider;
        double opacity;
        QLabel *sizeLabel;
        int currentSize;

        bool isCellPhone;
        QString imgPath;
        QSize size;
        QWidget *widget;

        bool lowRes;
        int titleSize;
        int titleHeight;
        QString borderRadius;
        int iconWidth;
        int dialogButtonsHeight;
        QSize sliderSize;
};

#endif
