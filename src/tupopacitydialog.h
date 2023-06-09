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

#ifndef TUPOPACITYDIALOG_H
#define TUPOPACITYDIALOG_H

#include "tupglobal.h"
#include "tupopacitypanel.h"

#include <QDialog>
#include <QBoxLayout>
#include <QTabWidget>
#include <QSettings>

class TupOpacityDialog : public QDialog
{
    Q_OBJECT

    public:
        TupOpacityDialog(QSettings *config, QWidget *parent = nullptr);
        ~TupOpacityDialog();

        void setParameters(const QPen pen, const QColor fillColor, Qt::BrushStyle brushStyle,
                           double borderOpacity, double fillOpacity);

    protected:
        void mousePressEvent(QMouseEvent *event);

    signals:
        void opacitiesUpdated(double, double);

    private slots:
        void checkOpacityStatus(TupPenPreviewCanvas::CanvasType panelType);
        void closeDialog();

    private:
        void addOpacityTab(const QString &label);
        void setButtonsPanel();

        QSettings *settings;
        bool isCellPhone;
        QString imgPath;
        QSize size;
        QWidget *widget;

        QString borderRadius;
        int iconWidth;
        int dialogButtonsHeight;
        QSize sliderSize;

        QVBoxLayout *innerLayout;
        QTabWidget *tabWidget;

        TupOpacityPanel *borderPanel;
        TupOpacityPanel *fillPanel;
};

#endif
