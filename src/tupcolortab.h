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

#ifndef TUPCOLORTAB_H
#define TUPCOLORTAB_H

#include "tupglobal.h"
#include "tupcheckbox.h"
#include "tupcurrentcolor.h"
#include "tupcolorpicker.h"
#include "tupcolorbuttonpanel.h"
#include "tupslider.h"
#include "tupcolorhistory.h"
#include "tupsettings.h"

#include <QBoxLayout>
#include <QTabWidget>
#include <QResizeEvent>
#include <QSettings>
#include <QLabel>
#include <QKeyEvent>
#include <QPaintEvent>

class TupColorTab : public QWidget
{
    Q_OBJECT

    public:
        TupColorTab(QSettings *settings, QPixmap *palette, ColorTab tab,
                    QColor colorVar, Qt::BrushStyle style, double opacityVal, QWidget *parent = nullptr);
        ~TupColorTab();

        void initSlider();
        void setColor(const QColor newColor);
        QColor currentColor() const;
        double baseOpacityValue();
        void setParameters(Qt::BrushStyle brushStyle, double opacity);
        void selectOptionsTab();
        void updateHistory(const QString &color);
        void resetColorHistory();
        void loadColorHistory();

    protected:
        void resizeEvent(QResizeEvent *event);

    signals:
        void colorUpdated(ColorTab tab, const QColor color);

    private slots:
        void updateFromPanel(QColor);
        void updateFromPicker(QColor);
        void updateFromSlider(int index);
        void setColor(const QString &color);

    private:
        void setLabelPanel();
        void setColorPanel();
        void setPickerArea(QPixmap *);
        void setBaseColorsPanel();
        void setSliderPanel();
        void clearHistorySelection();

        void updateColorLabel(const QColor &);
        QString colorToString(const QColor);

        QTabWidget *tabWidget;
        TupColorHistory *historyWidget;
        bool isCellPhone;
        bool lowRes;
        QString borderRadius;
        int titleSize;
        int iconWidth;
        int titleHeight;
        int headerHeight;

        QString imgPath;
        QSize sliderSize;
        int dialogButtonsHeight;
        QSize colorCellSize;

        int baseColorHeight;
        QSize baseColorCellSize;
        int baseSpacing;
        QString buttonParams;
        int colorSliderHeight;

        double opacity;
        double baseOpacity;
        QString opacityLabel;

        QVBoxLayout *innerLayout;
        QVBoxLayout *optionsLayout;
        QVBoxLayout *historyLayout;

        ColorTab colorTab;
        TupCurrentColor *colorCell;
        QLabel *colorLabel;
        TupColorPicker *pickerArea;
        TupColorButtonPanel *buttonPanel;
        TupSlider *colorSlider;

        QBrush colorBrush;
        QColor color;
        QColor topColor;

        QWidget *sliderWidget;
};

#endif // TUPCOLORTAB_H
