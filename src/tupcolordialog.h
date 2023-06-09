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

#ifndef TUPCOLORDIALOG_H
#define TUPCOLORDIALOG_H

#include "tupglobal.h"
#include "tupcolortab.h"
#include "tupcheckbox.h"

#include <QDialog>
#include <QBoxLayout>
#include <QTabWidget>
#include <QResizeEvent>
#include <QSettings>

class TupColorDialog : public QDialog
{
    Q_OBJECT

    public:
        TupColorDialog(QSettings *config, QPixmap *paletteImg, QWidget *parent = nullptr);
        ~TupColorDialog();

        void initSliders();
        void setParameters(Qt::BrushStyle borderStyle, double borderOpacity,
                           Qt::BrushStyle fillStyle, double fillOpacity);
        void resetBorderColor();
        void resetColorHistory();
        void loadHistory();

    protected:
        void resizeEvent(QResizeEvent *event);
        void mousePressEvent(QMouseEvent *event);

    signals:
        void colorsUpdated(const QColor &, const QColor &, const QColor &);

    private slots:
        void updateUI(int);
        void updateOpacitySelection(int);
        void updateOpacityStatus(ColorTab, const QColor);
        void closeDialog();
        void cancel();

    private:
        QColor getColor(const QString &, double opacity);
        void addColorTab(const QString &, ColorTab tab);
        void updateOpacityCheckBox(bool flag);
        void setBorderTab();
        void setButtonsPanel();
        void storeColor(ColorTab tab, const QString &color);
        void orderInterface();

        QSettings *settings;
        QString themeColor;
        QPixmap *palette;
        QVBoxLayout *innerLayout;
        QWidget *widget;
        QString imgPath;
        QSize size;

        bool isCellPhone;
        QString borderRadius;
        int iconWidth;
        int dialogButtonsHeight;

        QTabWidget *tabWidget;
        TupColorTab *borderTab;
        TupColorTab *fillTab;
        TupColorTab *bgTab;

        TupCheckBox *opacityCheckBox;
        QWidget *opacityWidget;
        TupLabel *opacityLabel;

        QColor previousBorderColor;
        QColor previousFillColor;
        bool borderOpacityFlag;
        bool fillOpacityFlag;
};

#endif
