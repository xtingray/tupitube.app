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

#ifndef TUPCOLORBUTTONPANEL_H
#define TUPCOLORBUTTONPANEL_H

#include "tupglobal.h"
#include "tupcolorbutton.h"
#include <QWidget>

class TupColorButtonPanel : public QWidget
{
    Q_OBJECT

    public:
        TupColorButtonPanel(const QColor color, Qt::BrushStyle style, const QSize &baseColorCellSize, int spacing,
                            const QString &buttonParams, QWidget *parent = nullptr);
        ~TupColorButtonPanel();

        void resetPanel();
        void setBrushStyle(Qt::BrushStyle style);

    signals:
        void clickColor(const QColor color);

    private slots:
        void updateSelection(int index);

    private:
       void setPanel(const QColor color, const QSize &cellSize, const QString &buttonParams);
       void setState(int index, bool isSelected);

       int spacing;
       QList<TupColorButton *> baseColors;
       Qt::BrushStyle style;
       int currentColorIndex;
};

#endif
