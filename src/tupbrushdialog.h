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

#ifndef TUPBRUSHDIALOG_H
#define TUPBRUSHDIALOG_H

#include "tupglobal.h"
#include "tupbrushespanel.h"

#include <QDialog>
#include <QBoxLayout>
#include <QTabWidget>
#include <QScrollArea>
#include <QSettings>

class TupBrushDialog : public QDialog
{
    Q_OBJECT

    public:
        enum ColorType { Border = 0, Fill };
        TupBrushDialog(QSettings *config, QWidget *parent = nullptr);
        ~TupBrushDialog();

        void setParameters(QColor borderColor, double borderOpacity,
                           QColor fillColor, double fillOpacity);

    protected:
        void mousePressEvent(QMouseEvent *event);

    signals:
        void brushesUpdated(Qt::BrushStyle, Qt::BrushStyle);

    private slots:
        void setBorderStyle(Qt::BrushStyle);
        void setFillStyle(Qt::BrushStyle);
        void closeDialog();

    private:
        QColor getColor(const QString &);
        void setBrushTab(ColorType colorType, QScrollArea *scrollArea);
        void addBrushTab(ColorType colorType);
        void removeBrushTab(ColorType colorType);
        void setMessagePanel(QWidget *msgWidget, const QString &msg);
        void addMsgPanel(ColorType colorType);
        void removeMsgPanel(ColorType colorType);

        void setButtonsPanel();

        QSettings *settings;
        QVBoxLayout *innerLayout;
        QTabWidget *tabWidget;
        TupBrushesPanel *borderPanel;
        TupBrushesPanel *fillPanel;
        bool isCellPhone;
        QSize size;
        QWidget *widget;
        QString imgPath;

        QColor borderColor;
        QColor fillColor;
        Qt::BrushStyle borderStyle;
        Qt::BrushStyle fillStyle;
        double borderOpacity;
        double fillOpacity;
        bool borderPanelAdded;
        bool fillPanelAdded;
        bool borderMsgAdded;
        bool fillMsgAdded;
        QScrollArea *borderScrollArea;
        QScrollArea *fillScrollArea;
        QWidget *borderMsgWidget;
        QWidget *fillMsgWidget;

        QString borderRadius;
        int iconWidth;
        int textSize;
        int dialogButtonsHeight;
};

#endif
