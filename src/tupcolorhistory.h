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

#ifndef TUPCOLORHISTORY_H
#define TUPCOLORHISTORY_H

#include "tupglobal.h"

#include <QWidget>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QSettings>
#include <QListWidget>
#include <QLabel>

class TupColorHistory : public QWidget
{
    Q_OBJECT

    public:
        TupColorHistory(ColorTab tab, QSettings *config, QWidget *parent = nullptr);
        ~TupColorHistory();

        void addHistoryRecord(int index, const QString &color);
        void appendHistoryRecord(const QString &color);
        void clearSelection();
        void resetColorList();
        void loadColorHistory(ColorTab tab);

    signals:
        void colorClicked(const QString &color);

    private slots:
        void setSelectedItem(int index, const QString &color);

    private:
        QWidget *colorsPage(ColorTab tab);
        QWidget *addColorItem(int index, const QString &color);

        int iconWidth;
        int textSize;

        bool noRecords;
        QBoxLayout *contentLayout;
        QScrollArea *listArea;
        QListWidget *listWidget;
        QSettings *settings;
        QWidget *previousItem;
        QString bgDefaultStyle;
};

#endif
