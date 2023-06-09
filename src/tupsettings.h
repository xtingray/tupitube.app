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

#ifndef TUPSETTINGS_H
#define TUPSETTINGS_H

#include <QDialog>
#include <QSettings>
#include <QResizeEvent>
#include <QLineEdit>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

#include "tupglobal.h"
#include "tupcheckbox.h"
#include "tuplabel.h"
#include "tupslider.h"
#include "tupbutton.h"

class TupSettings : public QDialog
{
    Q_OBJECT

    public:
        // enum ColorType { Border = 0, Fill, BgColor };

        TupSettings(QSettings *settings, int speed, int onionSkin, QWidget *parent = nullptr);
        ~TupSettings();

    protected:
        void resizeEvent(QResizeEvent *event);
        void mousePressEvent(QMouseEvent *event);

    private slots:
        void updateUIOption(int index);
        void updateUIMode(int index);
        void updateCanvasSelection(int index);
        void updateFPS(int speed);
        void updateOnionSkin(int onionFactor);
        void updateSmoothLevel(int level);
        void sendRegisterRequest();
        void sendPasswordRequest();
        void slotError(QNetworkReply::NetworkError error);
        void cleanMessage();
        void cleanLog();
        void registerAnswer(QNetworkReply *reply);
        void save();
        void formatEmail();

    private:
        void generalSettings();
        void canvasSettings();
        void brushSettings();
        void tupitubeSettings();

        void enableOption(bool isChecked, TupCheckBox *control);

        QSettings *settings;
        QString themeColor;
        bool isCellPhone;
        bool isTablet;
        QString imgPath;
        int fps;
        int onion;
        float smoothLevel;
        int iconWidth;
        QString borderRadius;
        QWidget *mainWidget;

        TupCheckBox *phoneCheckBox;
        TupCheckBox *tabletCheckBox;

        TupLabel *nextLabel;
        TupLabel *previousLabel;
        TupCheckBox *previousFrameCheckBox;
        TupCheckBox *nextFrameCheckBox;

        QSize sliderSize;
        int textSize;
        int width;
        int height;

        QWidget *generalTab;
        QWidget *canvasTab;
        QWidget *brushTab;
        QWidget *tupitubeTab;

        double colSpacing;

        QString nextText;
        QString previewText;

        QLabel *fpsLabel;
        QLabel *smoothLevelLabel;
        QLabel *onionLabel;

        TupSlider *fpsSlider;
        TupSlider *smoothLevelSlider;
        TupSlider *onionSlider;
        QSize dialogSize;

        QLineEdit *usernameEdit;
        QLineEdit *passwdEdit;
        QString oldUsername;
        QString oldPasswd;
        QLineEdit *emailEdit;
        QLabel *log;

        QNetworkAccessManager *manager;
        QNetworkRequest request;

        TupButton *recoverButton;
        TupButton *registerButton;
};

#endif
