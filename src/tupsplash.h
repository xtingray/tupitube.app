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

#ifndef TUPSPLASH_H
#define TUPSPLASH_H

#include "tupglobal.h"
#include "tupbutton.h"

#include <QDialog>
#include <QSettings>
#include <QBoxLayout>
#include <QGridLayout>
#include <QQueue>
#include <QResizeEvent>
#include <QKeyEvent>
#include <QMouseEvent>

class TupSplash : public QDialog
{
    Q_OBJECT

    public:
        TupSplash(QSettings *settings, QWidget *parent = nullptr);
        ~TupSplash();

    protected:
        void resizeEvent(QResizeEvent *);
        void keyPressEvent(QKeyEvent *);
        void mousePressEvent(QMouseEvent *);

    signals:
        void helpRequired();

    private slots:
        void callHelp();
        void closeSplash();

    private:
        void setSplashComponents();

        QGridLayout *dynamicLayout;
        QBoxLayout *bottomLayout;
        QQueue<QWidget *> queue;
        QHBoxLayout *globalLayout;
        QWidget *mainWidget;
        TupButton *helpButton;
        TupButton *startButton;
        bool isCellPhone;
        QString borderRadius;

        int splashLogoWidth;
        int splashSloganWidth;
        int splashButtonWidth;
        int splashFooterHeight;
        int splashSpace;
        QSize size;
        QString imgPath;
};

#endif
