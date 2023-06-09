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

#include "tupguiparams.h"
#include "tupproject.h"

#include <QFile>
#include <QXmlStreamReader>
#include <cmath>

#ifdef Q_OS_WIN
#include <QApplication>
#include <QSysInfo>
#include <QFont>
#endif

TupGuiParams::TupGuiParams(int dpi, int devW, int devH) : QObject()
{	 
    width = devW;
    height = devH;
    if (devW < devH) {
        width = devH;
        height = devW;
    }

    config = new QSettings("MaeFloresta", "tupitube.app");
    QString system = "unknown";
    QString imgPath = ":images/icons/";

    #ifdef TUP_DEBUG
        qDebug() << "[TupGuiParams::TupGuiParams()] - DPI -> " << dpi;
        qDebug() << "[TupGuiParams::TupGuiParams()] - WIDTH -> " << width;
        qDebug() << "[TupGuiParams::TupGuiParams()] - HEIGHT -> " << height;
        #if defined(Q_OS_ANDROID)
            addToLog("[TupGuiParams::TupGuiParams()] - DPI -> " + QString::number(dpi));
            addToLog("[TupGuiParams::TupGuiParams()] - WIDTH -> " + QString::number(width));
            addToLog("[TupGuiParams::TupGuiParams()] - HEIGHT -> " + QString::number(height));
        #endif
    #endif

    diagonal = computeDiagonal(dpi);
    bool firstTime = config->value("firstTime", true).toBool();
    if (firstTime) {
        double proportion = width / height;        
        if (proportion > 2) {
            isCellPhone = true;
        } else {
            if (diagonal >= 7) {
                isCellPhone = false;
            } else {
                isCellPhone = false;
                if (diagonal <= 5.5)
                    isCellPhone = true;
                else if (height <= 700)
                    isCellPhone = true;
            }
        }
        config->setValue("isCellPhone", isCellPhone);
        isTablet = !isCellPhone;
        config->setValue("isTablet", isTablet);
    } else {
        isCellPhone = config->value("isCellPhone").toBool();
        isTablet = config->value("isTablet").toBool();
    }

    #ifdef TUP_DEBUG
        qDebug() << "[TupGuiParams::TupGuiParams()] - FIRST TIME ->" << firstTime;
        qDebug() << "[TupGuiParams::TupGuiParams()] - isCellPhone? ->" << isCellPhone;
        qDebug() << "[TupGuiParams::TupGuiParams()] - isTablet? ->" << isTablet;
        #if defined(Q_OS_ANDROID)
            addToLog("*** FIRST TIME ->" + QString::number(firstTime));
            addToLog("*** isCellPhone ->" + QString::number(isCellPhone));
            addToLog("*** isTablet ->" + QString::number(isTablet));
        #endif
    #endif

    themeColor = "232,232,232";
    menuTitleSize = 14;
    menuTextSize = 12;

#ifdef Q_OS_ANDROID
    system = "android";

    if (isCellPhone) { // Cellphone UI
        if (width < 1000)
            lowRes = true;
        else
            lowRes = false;

        #ifdef TUP_DEBUG
            qDebug() << "[TupGuiParams::TupGuiParams()] - lowRes ->" << lowRes;
            #if defined(Q_OS_ANDROID)
                addToLog("*** lowRes ->" + QString::number(lowRes));
            #endif
        #endif

        iconSize = width*0.06;
        smallIconSize = width*0.06;
        iconWidth = width*0.05;
        barSpacing = QString::number(width*0.03);

        borderRadius = QString::number(width*0.006);
        vSliderWidth = width*0.08;
        hSliderWidth = width*0.02;
        boldTitleHeight = width*0.08;

        if (isTablet) {
            titleSize = 26;
            titleHeight = width*0.05;
            menuTitleSize = 18;
            textSize = 22;
            menuTextSize = 16;
            helpTextSize = 22;
        } else {
            titleSize = 18;
            menuTitleSize = 14;
            titleHeight = width*0.05;
            textSize = 18;
            menuTextSize = 12;
            helpTextSize = 18;
        }

        galleryPortraitWidth = width*0.2;
        galleryLandscapeWidth = width*0.37;

        splashLogoWidth = width*0.5;
        splashSloganWidth = width*0.45;
        splashButtonWidth = height*0.15;
        splashFooterHeight = width*0.14;
        splashSpace = height*0.1;

        floatingMenuHeight = height*0.95;
        consoleFontSize = 18;
        consolePos = QPointF(width*0.01, width*0.01);

        dialogButtonsHeight = width*0.08;
        colorCellSize = QSizeF(width*0.06, width*0.06);
        colorTypeSpacing = width*0.01;
        colorTypeSeparator = width*0.01;
        colorTypeHeight = width*0.1;
        baseColorHeight = width*0.09;
        baseColorCellSize = QSizeF(width*0.07, width*0.05);
        baseColorSpacing = width*0.02;

        int one = static_cast<int>(width * 0.015);
        int two = static_cast<int>(one * 0.2);
        int three = static_cast<int>(two * 0.5);
        colorButtonParams = QString::number(one) + "," + QString::number(two) + "," + QString::number(three);

        colorSliderHeight = width*0.09;
        progressWidth = height*0.8;
        progressFontSize = 50;
        postCancelButtonSize = QSizeF(width*0.22, height*0.15);

        galleryPortraitWidth = height*0.6;
        galleryLandscapeWidth = height*0.7;

        removePortraitSize = QSizeF(width*0.41, height*0.92);
        removeLandscapeSize = QSizeF(width*0.45, height*0.75);
        removePortraitWidth = width*0.2;
        removeLandscapeWidth = height*0.7;
    } else { // Tablet UI
        lowRes = true;

        iconSize = width*0.033;
        smallIconSize = width*0.029;
        iconWidth = width*0.031;
        barSpacing = QString::number(width*0.008);

        borderRadius = QString::number(width*0.005);
        vSliderWidth = width*0.04;
        hSliderWidth = width*0.012;
        titleSize = 22;
        menuTitleSize = 16;
        titleHeight = width*0.026;
        boldTitleHeight = width*0.05;
        textSize = 20;
        menuTextSize = 12;
        helpTextSize = 18;

        galleryPortraitWidth = width*0.08;
        galleryLandscapeWidth = width*0.1;

        splashLogoWidth = height*0.34;
        splashSloganWidth = height*0.29;
        splashButtonWidth = height*0.07;
        splashFooterHeight = height*0.12;
        splashSpace = height*0.1;
        splashSize = QSizeF(height*0.6, height*0.42);

        consoleFontSize = 18;
        consolePos = QPointF(width*0.01, width*0.01);

        dialogButtonsHeight = width*0.048;
        colorCellSize = QSizeF(width*0.04, width*0.04);
        colorTypeSpacing = width*0.005;
        colorTypeSeparator = width*0.02;
        colorTypeHeight = width*0.05;
        baseColorHeight = width*0.04;
        baseColorCellSize = QSizeF(width*0.04, width*0.03);
        baseColorSpacing = width*0.02;

        int one = static_cast<int>(width*0.01);
        int two = static_cast<int>(one*0.2);
        int three = static_cast<int>(two*0.5);
        colorButtonParams = QString::number(one) + "," + QString::number(two) + "," + QString::number(three);

        colorSliderHeight = width*0.06;
        progressWidth = height*0.5;
        progressFontSize = 50;
        postCancelButtonSize = QSizeF(width*0.13, height*0.09);

        galleryPortraitWidth = height*0.18;
        galleryLandscapeWidth = height*0.24;

        removePortraitSize = QSizeF(height*0.35, height*0.65);
        removeLandscapeSize = QSizeF(height*0.55, height*0.45);
        removePortraitWidth = height*0.3;
        removeLandscapeWidth = height*0.5;

        actionDialogSize = QSizeF(height*0.55, height*0.48);
        settingsDialogSize = QSizeF(width*0.46, height*0.67);
        colorDialogSize = QSizeF(height*0.95, height*0.95);
        strokeDialogSize = QSizeF(width*0.35, height*0.5);
        opacityDialogSize = QSizeF(width*0.35, height*0.5);
        brushesDialogSize = QSizeF(height*0.9, height*0.5);
        postDialogSize = QSizeF(height*0.65, height*0.43);
        helpDialogSize = QSizeF(height*0.82, height*0.86);
        galleryRowSize = 3;
        galleryDialogSize = QSizeF(height, height);

        if (diagonal < 7) {
            titleSize = 18;
            textSize = 16;
            helpTextSize = 14;

            splashLogoWidth = height*0.45;
            splashSloganWidth = height*0.38;
            splashButtonWidth = height*0.08;
            splashFooterHeight = height*0.13;
            splashSpace = height*0.13;
            splashSize = QSizeF(height*0.8, height*0.56);

            postDialogSize = QSizeF(height*0.65, height*0.48);
            settingsDialogSize = QSizeF(width*0.46, height*0.62);
        }
    }
#elif defined(Q_OS_IOS)
    system = "ios";
    themeColor = "232,232,232";

    if (isCellPhone) { // Cellphone UI
        if (width < 1000)
            lowRes = true;
        else
            lowRes = false;

        iconSize = width*0.04;
        smallIconSize = width*0.05;
        iconWidth = width*0.04;
        barSpacing = QString::number(width*0.04);

        borderRadius = QString::number(width*0.006);
        vSliderWidth = width*0.08;
        hSliderWidth = width*0.02;
        titleSize = 16;
        textSize = 16;
        helpTextSize = 16;

        menuTitleSize = 14;
        titleHeight = width*0.05;
        boldTitleHeight = width*0.08;
        menuTextSize = 12;

        galleryPortraitWidth = width*0.2;
        galleryLandscapeWidth = width*0.37;

        splashLogoWidth = width*0.5;
        splashSloganWidth = width*0.45;
        splashButtonWidth = height*0.15;
        splashFooterHeight = width*0.13;
        splashSpace = height*0.1;

        floatingMenuHeight = height*0.95;
        consoleFontSize = 18;
        consolePos = QPoint(width*0.01, width*0.01);

        dialogButtonsHeight = width*0.08;
        colorCellSize = QSize(width*0.06, width*0.06);
        colorTypeSpacing = width*0.01;
        colorTypeSeparator = width*0.01;
        colorTypeHeight = width*0.1;
        baseColorHeight = width*0.07;
        baseColorCellSize = QSize(width*0.06, width*0.05);
        baseColorSpacing = width*0.03;

        int one = width*0.018;
        int two = one*0.1;
        int three = two*0.5;
        colorButtonParams = QString::number(one) + "," + QString::number(two) + "," + QString::number(three);

        colorSliderHeight = width*0.09;
        progressWidth = height*0.8;
        progressFontSize = 50;
        postCancelButtonSize = QSize(width*0.22, height*0.15);

        galleryPortraitWidth = height*0.6;
        galleryLandscapeWidth = height*0.7;

        removePortraitSize = QSize(width*0.41, height*0.92);
        removeLandscapeSize = QSize(width*0.45, height*0.75);
        removePortraitWidth = width*0.2;
        removeLandscapeWidth = height*0.7;
    } else { // Tablet UI
        lowRes = true;

        iconSize = width*0.033;
        smallIconSize = width*0.029;
        iconWidth = width*0.031;
        barSpacing = QString::number(width*0.008);

        borderRadius = QString::number(width*0.005);
        vSliderWidth = width*0.04;
        hSliderWidth = width*0.012;

        titleSize = 20;
        textSize = 18;
        helpTextSize = 16;

        menuTitleSize = 16;
        titleHeight = width*0.026;
        boldTitleHeight = width*0.05;
        menuTextSize = 12;

        galleryPortraitWidth = width*0.08;
        galleryLandscapeWidth = width*0.1;

        splashLogoWidth = height*0.34;
        splashSloganWidth = height*0.29;
        splashButtonWidth = height*0.07;
        splashFooterHeight = height*0.1;
        splashSpace = height*0.1;
        splashSize = QSize(height*0.6, height*0.42);

        consoleFontSize = 18;
        consolePos = QPoint(width*0.01, width*0.01);

        dialogButtonsHeight = height*0.08;
        colorCellSize = QSize(width*0.04, width*0.04);
        colorTypeSpacing = width*0.005;
        colorTypeSeparator = width*0.02;
        colorTypeHeight = width*0.05;
        baseColorHeight = width*0.04;
        baseColorCellSize = QSize(width*0.04, width*0.03);
        baseColorSpacing = width*0.02;

        int one = width*0.01;
        int two = one*0.2;
        int three = two*0.5;
        colorButtonParams = QString::number(one) + "," + QString::number(two) + "," + QString::number(three);

        colorSliderHeight = width*0.06;
        progressWidth = height*0.5;
        progressFontSize = 50;
        postCancelButtonSize = QSize(width*0.13, height*0.09);

        galleryPortraitWidth = height*0.18;
        galleryLandscapeWidth = height*0.24;

        removePortraitSize = QSize(height*0.35, height*0.65);
        removeLandscapeSize = QSize(height*0.55, height*0.45);
        removePortraitWidth = height*0.3;
        removeLandscapeWidth = height*0.5;

        actionDialogSize = QSize(height*0.55, height*0.48);
        settingsDialogSize = QSize(width*0.46, height*0.53);
        colorDialogSize = QSize(height*0.95, height*0.9);
        strokeDialogSize = QSize(width*0.35, height*0.5);
        opacityDialogSize = QSize(width*0.35, height*0.5);
        brushesDialogSize = QSize(height*0.9, height*0.5);
        postDialogSize = QSize(height*0.65, height*0.43);
        helpDialogSize = QSize(height*0.82, height*0.86);
        galleryRowSize = 3;
        galleryDialogSize = QSize(height*0.95, height*0.95);

        if (diagonal < 7) {
            titleSize = 18;
            textSize = 16;
            helpTextSize = 14;

            splashLogoWidth = height*0.45;
            splashSloganWidth = height*0.38;
            splashButtonWidth = height*0.09;
            splashFooterHeight = height*0.13;
            splashSpace = height*0.13;
            splashSize = QSize(height*0.8, height*0.56);

            postDialogSize = QSize(height*0.65, height*0.48);
            settingsDialogSize = QSize(width*0.46, height*0.56);
        }
    }

    /*
    if (width <= 568 && height <= 320 && dpi <= 163) {
        loadSettings(":resources/xml/iphone5_568_320_163dpi.xml");
    } else if (width <= 667 && height <= 375 && dpi <= 163) {
        loadSettings(":resources/xml/iphone6_667_375_163dpi.xml");
    } else if (width <= 1024 && height <= 768 && dpi <= 132) {  // iPad 2
        loadSettings(":resources/xml/ipad2_1024_768_132dpi.xml");
    }
    */
#elif defined(Q_OS_LINUX)
    system = "linux";
    lowRes = true;

    if (width <= 800 && height <= 600) {
        loadSettings(":resources/xml/display_800x600.xml");
    } else if (width <= 1024 && height <= 768) {
        loadSettings(":resources/xml/display_1024x768.xml");
    } else if (width <= 1152 && height <= 864) {
        loadSettings(":resources/xml/display_1152x864.xml");
    } else if (width <= 1280 && height <= 960) {
        loadSettings(":resources/xml/display_1280x960.xml");
    } else if (width <= 1280 && height <= 1024) {
        loadSettings(":resources/xml/display_1280x1024.xml");
    } else if (width <= 1366 && height <= 768) { // Tablet Resolution
        loadSettings(":resources/xml/display_1366x768.xml");
    } else if (width <= 1400 && height <= 1050) {
        loadSettings(":resources/xml/display_1400x1050.xml");
    } else if (width <= 1920 && height <= 1080) {
        loadSettings(":resources/xml/display_1920x1080.xml");
    }
#elif defined(Q_OS_MAC)
    system = "osx";
    // loadSettings(":resources/xml/osx.xml");
    if (width <= 800 && height <= 600) {
        loadSettings(":resources/xml/display_800x600.xml");
    } else if (width <= 1024 && height <= 768) {
        loadSettings(":resources/xml/display_1024x768.xml");
    } else if (width <= 1152 && height <= 864) {
        loadSettings(":resources/xml/display_1152x864.xml");
    } else if (width <= 1280 && height <= 960) {
        loadSettings(":resources/xml/display_1280x960.xml");
    } else if (width <= 1280 && height <= 1024) {
        loadSettings(":resources/xml/display_1280x1024.xml");
    } else if (width <= 1366 && height <= 768) { // Tablet Resolution
        loadSettings(":resources/xml/display_1366x768.xml");
    } else if (width <= 1400 && height <= 1050) {
        loadSettings(":resources/xml/display_1400x1050.xml");
    } else if (width <= 1920 && height <= 1080) {
        loadSettings(":resources/xml/display_1920x1080.xml");
    }
#elif defined(Q_OS_WIN)
    system = "win";
    #ifdef TUP_DEBUG
        qDebug() << "Windows Version: " << QSysInfo::windowsVersion();
        qDebug() << "Resolution: QSize(" << width << ", " << height << ")";
    #endif
	
    if (width <= 800 && height <= 600) {
        loadSettings(":resources/xml/display_800x600.xml");
    } else if (width <= 1024 && height <= 768) {
        loadSettings(":resources/xml/display_1024x768.xml");
    } else if (width <= 1152 && height <= 864) {
        loadSettings(":resources/xml/display_1152x864.xml");
    } else if (width <= 1280 && height <= 960) {
        loadSettings(":resources/xml/display_1280x960.xml");
    } else if (width <= 1280 && height <= 1024) {
        loadSettings(":resources/xml/display_1280x1024.xml");
    } else if (width <= 1366 && height <= 768) { // Tablet Resolution
        loadSettings(":resources/xml/display_1366x768.xml");		
    } else if (width <= 1400 && height <= 1050) {
        loadSettings(":resources/xml/display_1400x1050.xml");
    } else if (width <= 1920 && height <= 1080) {
        loadSettings(":resources/xml/display_1920x1080.xml");
    }
#endif

    config->setValue("system", system);
    config->setValue("imgPath", imgPath);
    config->setValue("width", static_cast<int>(width));
    config->setValue("height", static_cast<int>(height));
    config->setValue("themeColor", themeColor);
    config->setValue("lowRes", lowRes);

    config->setValue("iconSize", static_cast<int>(iconSize));
    config->setValue("smallIconSize", static_cast<int>(smallIconSize));
    config->setValue("iconWidth", static_cast<int>(iconWidth));
    config->setValue("barSpacing", barSpacing);
    config->setValue("borderRadius", borderRadius);
    config->setValue("vSliderWidth", static_cast<int>(vSliderWidth));
    config->setValue("hSliderWidth", static_cast<int>(hSliderWidth));
    config->setValue("consoleFontSize", static_cast<int>(consoleFontSize));
    config->setValue("consolePos", consolePos);
    config->setValue("titleSize", static_cast<int>(titleSize));
    config->setValue("menuTitleSize", static_cast<int>(menuTitleSize));
    config->setValue("menuTextSize", static_cast<int>(menuTextSize));
    config->setValue("titleHeight", static_cast<int>(titleHeight));
    config->setValue("boldTitleHeight", static_cast<int>(boldTitleHeight));
    config->setValue("textSize", static_cast<int>(textSize));
    config->setValue("helpTextSize", static_cast<int>(helpTextSize));
    config->setValue("galleryPortraitWidth", static_cast<int>(galleryPortraitWidth));
    config->setValue("galleryLandscapeWidth", static_cast<int>(galleryLandscapeWidth));
    config->setValue("progressWidth", static_cast<int>(progressWidth));

    #ifdef Q_OS_ANDROID
        config->setValue("diagonal", diagonal);
        config->setValue("splashLogoWidth", static_cast<int>(splashLogoWidth));
        config->setValue("splashSloganWidth", static_cast<int>(splashSloganWidth));
        config->setValue("splashButtonWidth", static_cast<int>(splashButtonWidth));
        config->setValue("splashSpace", static_cast<int>(splashSpace));
        config->setValue("splashFooterHeight", static_cast<int>(splashFooterHeight));

        if (!isCellPhone) {
            config->setValue("splashSize", splashSize);
        } else {
            config->setValue("hiddenIconWidth", static_cast<int>(hiddenIconWidth));
            config->setValue("floatingMenuHeight", static_cast<int>(floatingMenuHeight));
        }
    #endif

    config->setValue("dialogButtonsHeight", static_cast<int>(dialogButtonsHeight));
    config->setValue("colorCellSize", colorCellSize);
    config->setValue("colorTypeSpacing", static_cast<int>(colorTypeSpacing));
    config->setValue("colorTypeSeparator", static_cast<int>(colorTypeSeparator));
    config->setValue("colorTypeHeight", static_cast<int>(colorTypeHeight));
    config->setValue("baseColorHeight", static_cast<int>(baseColorHeight));
    config->setValue("baseColorCellSize", baseColorCellSize);
    config->setValue("baseColorSpacing", static_cast<int>(baseColorSpacing));
    config->setValue("colorButtonParams", colorButtonParams);
    config->setValue("colorSliderHeight", static_cast<int>(colorSliderHeight));

    config->setValue("progressFontSize", static_cast<int>(progressFontSize));
    config->setValue("postCancelButtonSize", postCancelButtonSize);

    config->setValue("removePortraitSize", removePortraitSize);
    config->setValue("removeLandscapeSize", removeLandscapeSize);
    config->setValue("removePortraitWidth", static_cast<int>(removePortraitWidth));
    config->setValue("removeLandscapeWidth", static_cast<int>(removeLandscapeWidth));

    if (!isCellPhone) {
        config->setValue("actionDialogSize", actionDialogSize);
        config->setValue("settingsDialogSize", settingsDialogSize);
        config->setValue("colorDialogSize", colorDialogSize);
        config->setValue("strokeDialogSize", strokeDialogSize);
        config->setValue("opacityDialogSize", opacityDialogSize);
        config->setValue("brushesDialogSize", brushesDialogSize);
        config->setValue("postDialogSize", postDialogSize);
        config->setValue("helpDialogSize", helpDialogSize);
        config->setValue("galleryRowSize", galleryRowSize);
        config->setValue("galleryDialogSize", galleryDialogSize);
        config->setValue("exitDialogSize", exitDialogSize);
    }
}

QSettings * TupGuiParams::settings()
{
    return config;
}

qreal TupGuiParams::computeDiagonal(int dpi)
{
    qreal w = width / dpi;
    qreal h = height / dpi;
    qreal d = sqrt((w*w) + (h*h));
    #ifdef TUP_DEBUG
        qDebug() << "[TupGuiParams::computeDiagonal()] - Screen diagonal size -> " << d;
    #endif

    return d;
}

void TupGuiParams::loadSettings(const QString &path)
{
    QFile file(path);

    #ifdef TUP_DEBUG
        qDebug() << "[TupGuiParams::loadSettings()] - Loading xml file -> " << path;
    #endif

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        #ifdef TUP_DEBUG
            qDebug() << "[TupGuiParams::loadSettings()] - Fatal error loading xml file -> " << path;
        #endif

        return;
    }

    QXmlStreamReader *reader = new QXmlStreamReader(&file);
    if (reader->readNextStartElement() && reader->name() == QString("settings")) {
        while (reader->readNextStartElement()) {
            QString key = reader->name().toString();
            QString value = reader->readElementText();

            if (key == "isCellPhone") {
                isCellPhone = false;
                if (value == "true")
                    isCellPhone = true;
            } else if (key == "diagonal") {
                diagonal = value.toDouble();
            } else if (key == "lowRes") {
                lowRes = false;
                if (value == "true")
                    lowRes = true;
            } else if (key == "iconSize") {
                iconSize = width * value.toDouble();
            } else if (key == "smallIconSize") {
                smallIconSize = width * value.toDouble();
            } else if (key == "iconWidth") {
                iconWidth = width * value.toDouble();
            } else if (key == "barSpacing") {                
                int space = static_cast<int>(width * value.toDouble());
                barSpacing = QString::number(space);
            } else if (key == "borderRadius") {
                int radius = static_cast<int>(width * value.toDouble());
                borderRadius = QString::number(radius);
            } else if (key == "vSliderWidth") {
                vSliderWidth = width * value.toDouble();
            } else if (key == "hSliderWidth") {
                hSliderWidth = height * value.toDouble();
            } else if (key == "titleHeight") {
                titleHeight = height * value.toDouble();
            } else if (key == "titleSize") {
                titleSize = value.toInt();
            } else if (key == "menuTitleSize") {
                menuTitleSize = value.toInt();
            } else if (key == "menuTextSize") {
                menuTextSize = value.toInt();
            } else if (key == "textSize") {
                textSize = value.toInt();
            } else if (key == "helpTextSize") {
                helpTextSize = value.toInt();
            } else if (key == "boldTitleHeight") {
                boldTitleHeight = height * value.toDouble();
            } else if (key == "dialogButtonsHeight") {
                dialogButtonsHeight = height * value.toDouble();
            } else if (key == "consoleFontSize") {
                consoleFontSize = value.toInt();
            } else if (key == "consolePos") {
                QStringList coords = value.split(",");
                double x = width * coords.at(0).toDouble();
                double y = width * coords.at(1).toDouble();
                consolePos = QPointF(x, y);
            } else if (key == "progressFontSize") {
                progressFontSize = value.toInt();
            } else if (key == "progressWidth") {
                progressWidth = value.toInt();
            } else if (key == "postCancelButtonSize") {
                QStringList coords = value.split(",");
                double x = width * coords.at(0).toDouble();
                double y = height * coords.at(1).toDouble();
                postCancelButtonSize = QSizeF(width * x, height * y);
            } else if (key == "colorCellSize") {
                QStringList coords = value.split(",");
                double x = width * coords.at(0).toDouble();
                double y = width * coords.at(1).toDouble();
                if (x > 0)
                    colorCellSize = QSizeF(x, y);
                else
                    colorCellSize = QSizeF(width, height);
            } else if (key == "colorTypeSpacing") {
                colorTypeSpacing = width * value.toDouble();
            } else if (key == "colorTypeSeparator") {
                colorTypeSeparator = width * value.toDouble();
            } else if (key == "colorTypeHeight") {
                colorTypeHeight = width * value.toDouble();
            } else if (key == "baseColorHeight") {
                baseColorHeight = width * value.toDouble();
            } else if (key == "baseColorCellSize") {
                QStringList coords = value.split(",");
                double x = width * coords.at(0).toDouble();
                double y = width * coords.at(1).toDouble();
                baseColorCellSize = QSizeF(x, y);
            } else if (key == "baseColorSpacing") {
                baseColorSpacing = width * value.toDouble();
            } else if (key == "colorButtonParams") {
                int one = static_cast<int>(width * value.toDouble());
                int two = static_cast<int>(one * 0.2);
                int three = static_cast<int>(two * 0.5);
                colorButtonParams = QString::number(one) + "," + QString::number(two) + "," + QString::number(three);
            } else if (key == "colorSliderHeight") {
                colorSliderHeight = height * value.toDouble();
            } else if (key == "actionDialogSize") {
                QStringList coords = value.split(",");
                double x = width * coords.at(0).toDouble();
                double y = height * coords.at(1).toDouble();
                if (x > 0)
                    actionDialogSize = QSizeF(x, y);
                else
                    actionDialogSize = QSizeF(width, height);
            } else if (key == "settingsDialogSize") {
                QStringList coords = value.split(",");
                double x = width * coords.at(0).toDouble();
                double y = height * coords.at(1).toDouble();
                if (x > 0)
                    settingsDialogSize = QSizeF(x, y);
                else
                    settingsDialogSize = QSizeF(width, height);
            } else if (key == "colorDialogSize") {
                QStringList coords = value.split(",");
                double x = height * coords.at(0).toDouble();
                double y = height * coords.at(1).toDouble();
                if (x > 0)
                    colorDialogSize = QSizeF(x, y);
                else
                    colorDialogSize = QSizeF(width, height);
            } else if (key == "strokeDialogSize") {
                QStringList coords = value.split(",");
                double x = width * coords.at(0).toDouble();
                double y = height * coords.at(1).toDouble();
                if (x > 0)
                    strokeDialogSize = QSizeF(x, y);
                else
                    strokeDialogSize = QSizeF(width, height);
            } else if (key == "opacityDialogSize") {
                QStringList coords = value.split(",");
                double x = width * coords.at(0).toDouble();
                double y = height * coords.at(1).toDouble();
                if (x > 0)
                    opacityDialogSize = QSizeF(x, y);
                else
                    opacityDialogSize = QSizeF(width, height);
            } else if (key == "brushesDialogSize") {
                QStringList coords = value.split(",");
                double x = height * coords.at(0).toDouble();
                double y = height * coords.at(1).toDouble();
                if (x > 0)
                    brushesDialogSize = QSizeF(x, y);
                else
                    brushesDialogSize = QSizeF(width, height);
            } else if (key == "postDialogSize") {
                QStringList coords = value.split(",");
                double x = height * coords.at(0).toDouble();
                double y = height * coords.at(1).toDouble();
                if (x > 0)
                    postDialogSize = QSizeF(x, y);
                else
                    postDialogSize = QSizeF(width, height);
            } else if (key == "galleryPortraitWidth") {
                galleryPortraitWidth = height * value.toDouble();
            } else if (key == "galleryLandscapeWidth") {
                galleryLandscapeWidth = height * value.toDouble();
            } else if (key == "galleryRowSize") {
                galleryRowSize = value.toInt();
            } else if (key == "galleryDialogSize") {
                QStringList coords = value.split(",");
                double x = width * coords.at(0).toDouble();
                double y = height * coords.at(1).toDouble();
                if (x > 0)
                    galleryDialogSize = QSizeF(x, y);
                else
                    galleryDialogSize = QSizeF(width, height);
            } else if (key == "removePortraitWidth") {
                removePortraitWidth = height * value.toDouble();
            } else if (key == "removeLandscapeWidth") {
                removeLandscapeWidth = height * value.toDouble();
            } else if (key == "removePortraitSize") {
                QStringList coords = value.split(",");
                double x = height * coords.at(0).toDouble();
                double y = height * coords.at(1).toDouble();
                if (x > 0)
                    removePortraitSize = QSizeF(x, y);
                else
                    removePortraitSize = QSizeF(width, height);
            } else if (key == "removeLandscapeSize") {
                QStringList coords = value.split(",");
                double x = height * coords.at(0).toDouble();
                double y = height * coords.at(1).toDouble();
                if (x > 0)
                    removeLandscapeSize = QSizeF(x, y);
                else
                    removeLandscapeSize = QSizeF(width, height);
            } else if (key == "helpDialogSize") {
                QStringList coords = value.split(",");
                double x = height * coords.at(0).toDouble();
                double y = height * coords.at(1).toDouble();
                if (x > 0)
                    helpDialogSize = QSizeF(x, y);
                else
                    helpDialogSize = QSizeF(width, height);
            } else if (key == "exitDialogSize") {
                QStringList coords = value.split(",");
                double x = height * coords.at(0).toDouble();
                double y = height * coords.at(1).toDouble();
                if (x > 0)
                    exitDialogSize = QSizeF(x, y);
                else
                    exitDialogSize = QSizeF(width, height);
            }
        }
    }

    file.close();
}

#if defined(Q_OS_ANDROID) && defined(TUP_DEBUG)
void TupGuiParams::addToLog(const QString &record)
{
    log += record + "\n";
}

QString TupGuiParams::getLog()
{
    return log;
}
#endif
