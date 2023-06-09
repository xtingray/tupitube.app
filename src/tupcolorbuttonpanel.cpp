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

#include "tupcolorbuttonpanel.h"

#include <QBoxLayout>

TupColorButtonPanel::TupColorButtonPanel(const QColor color, Qt::BrushStyle brushStyle, const QSize &baseColorCellSize, int distance,
                                         const QString &buttonParams, QWidget *parent) : QWidget(parent)
{
    style = brushStyle;
    currentColorIndex = -1;
    spacing = distance;

    setPanel(color, baseColorCellSize, buttonParams);
}

void TupColorButtonPanel::setPanel(const QColor color, const QSize &cellSize, const QString &buttonParams)
{
    QColor redColor(255, 0, 0);
    QBrush redBrush(redColor,style);
    TupColorButton *red = new TupColorButton(0, redBrush, cellSize, buttonParams);
    connect(red, &TupColorButton::clicked, this, &TupColorButtonPanel::updateSelection);
    baseColors << red;

    if (color == redColor) {
        red->setState(true);
       currentColorIndex = 0;
    }

    QColor greenColor(0, 255, 0);
    QBrush greenBrush(greenColor,style);
    TupColorButton *green = new TupColorButton(1, greenBrush, cellSize, buttonParams);
    connect(green, &TupColorButton::clicked, this, &TupColorButtonPanel::updateSelection);
    baseColors << green;

    if (color == greenColor) {
        green->setState(true);
       currentColorIndex = 1;
    }

    QColor blueColor(0, 0, 255);
    QBrush blueBrush(blueColor,style);
    TupColorButton *blue = new TupColorButton(2, blueBrush, cellSize, buttonParams);
    connect(blue, &TupColorButton::clicked, this, &TupColorButtonPanel::updateSelection);
    baseColors << blue;

    if (color == blueColor) {
        blue->setState(true);
        currentColorIndex = 2;
    }

    QColor whiteColor(255, 255, 255);
    QBrush whiteBrush(whiteColor, style);
    TupColorButton *white = new TupColorButton(3, whiteBrush, cellSize, buttonParams);
    connect(white, &TupColorButton::clicked, this, &TupColorButtonPanel::updateSelection);
    baseColors << white;

    if (color == whiteColor) {
        white->setState(true);
        currentColorIndex = 3;
    }

    QColor blackColor(0, 0, 0);
    QBrush blackBrush(blackColor,style);
    TupColorButton *black = new TupColorButton(4, blackBrush, cellSize, buttonParams);
    connect(black, &TupColorButton::clicked, this, &TupColorButtonPanel::updateSelection);
    baseColors << black;

    if (color == blackColor) {
        black->setState(true);
        currentColorIndex = 4;
    }

    QBoxLayout *bottomLayout = new QHBoxLayout(this);
    bottomLayout->setAlignment(Qt::AlignHCenter);
    bottomLayout->setContentsMargins(3, 5, 3, 3);

    bottomLayout->setSpacing(spacing);

    bottomLayout->addWidget(red);
    bottomLayout->addWidget(green);
    bottomLayout->addWidget(blue);
    bottomLayout->addWidget(white);

    bottomLayout->addWidget(black);
}

TupColorButtonPanel::~TupColorButtonPanel()
{
}

void TupColorButtonPanel::updateSelection(int index)
{
    if (index !=currentColorIndex) {
        if (currentColorIndex >= 0) {
            TupColorButton *button = (TupColorButton *) baseColors.at(currentColorIndex);
            button->setState(false);
        }

        TupColorButton *selection = (TupColorButton *) baseColors.at(index);
        QColor color = selection->color();
        currentColorIndex = index;

        emit clickColor(color);
    }
}

void TupColorButtonPanel::setState(int index, bool isSelected)
{
    if (index !=currentColorIndex &&currentColorIndex >= 0) {
        TupColorButton *button = (TupColorButton *) baseColors.at(currentColorIndex);
        button->setState(isSelected);
    }
}

void TupColorButtonPanel::resetPanel()
{
    if (currentColorIndex >= 0) {
        TupColorButton *button = (TupColorButton *) baseColors.at(currentColorIndex);
        button->setState(false);
        currentColorIndex = -1;
    }
}

void TupColorButtonPanel::setBrushStyle(Qt::BrushStyle style)
{
    foreach(TupColorButton *button,baseColors)
        button->setBrushStyle(style);
}
