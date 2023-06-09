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

#ifndef TUPGUIPARAMS_H
#define TUPGUIPARAMS_H

#include "tupglobal.h"

#include <QSettings>
#include <QPoint>
#include <QSize>

class TupGuiParams : public QObject
{
    Q_OBJECT

    public:
        TupGuiParams(int dpi, int devW, int devH);
        QSettings * settings();

        #if defined(Q_OS_ANDROID) && defined(TUP_DEBUG)
            QString getLog();
        #endif

    private:
        qreal computeDiagonal(int dpi);
        void loadSettings(const QString &path);

        #if defined(Q_OS_ANDROID)
            bool deviceIsACellphone();
            #ifdef TUP_DEBUG
                void addToLog(const QString &record);
                QString log;
            #endif
        #endif

        QSettings *config;

        double width;
        double height;
        qreal diagonal;
        bool isCellPhone;
        bool isTablet;
        bool lowRes;
        double iconSize;
        double smallIconSize;
        double iconWidth;
        double hiddenIconWidth;
        double floatingMenuHeight;
        QString themeColor;
        QString barSpacing;
        QString borderRadius;
        double vSliderWidth;
        double hSliderWidth;
        double titleSize;
        double menuTitleSize;
        double titleHeight;
        double textSize;
        double menuTextSize;
        double helpTextSize;
        double consoleFontSize;
        QPointF consolePos;

        double splashLogoWidth;
        double splashSloganWidth;
        double splashButtonWidth;
        double splashFooterHeight;
        double splashSpace;
        double boldTitleHeight;
        double dialogButtonsHeight;
        QSizeF colorCellSize;
        double colorTypeSpacing;
        double colorTypeSeparator;
        double colorTypeHeight;
        double baseColorHeight;
        QSizeF baseColorCellSize;
        double baseColorSpacing;
        QString colorButtonParams;
        double colorSliderHeight;

        QSizeF splashSize;
        QSizeF actionDialogSize;
        QSizeF settingsDialogSize;
        QSizeF strokeDialogSize;
        QSizeF opacityDialogSize;
        QSizeF brushesDialogSize;

        QSizeF postDialogSize;
        double progressWidth;
        double progressFontSize;
        QSizeF postCancelButtonSize;

        double galleryPortraitWidth;
        double galleryLandscapeWidth;
        double galleryRowSize;
        QSizeF galleryDialogSize;
        double removePortraitWidth;
        double removeLandscapeWidth;
        QSizeF removePortraitSize;
        QSizeF removeLandscapeSize;
        QSizeF helpDialogSize;
        QSizeF colorDialogSize;
        QSizeF exitDialogSize;
};

#endif // TUPGUIPARAMS_H
