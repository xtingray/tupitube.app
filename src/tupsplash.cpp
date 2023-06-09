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

#include "tupsplash.h"

#include <QGuiApplication>
#include <QScreen>
#include <QLabel>
#include <QPixmap>

TupSplash::TupSplash(QSettings *settings, QWidget *parent) : QDialog(parent)
{
    setModal(true);
    setWindowFlags(Qt::Popup);

    isCellPhone = settings->value("isCellPhone").toBool();
    borderRadius = settings->value("borderRadius").toString();
    splashLogoWidth = settings->value("splashLogoWidth").toInt();
    splashSloganWidth = settings->value("splashSloganWidth").toInt();
    splashButtonWidth = settings->value("splashButtonWidth").toInt();
    splashFooterHeight = settings->value("splashFooterHeight").toInt();
    splashSpace = settings->value("splashSpace").toInt();
    size = settings->value("splashSize").toSize();
    imgPath = settings->value("imgPath").toString();

#ifdef Q_OS_ANDROID
    if (!isCellPhone)
        setStyleSheet("* { background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 rgb(100, 100, 100), stop:1 rgb(30, 30, 30)); }");
    else
        setStyleSheet("* { background-color: rgb(255,255,255); }");
#else
    setStyleSheet("QWidget { background-color: rgb(255,255,255); color: black; border-radius: " + borderRadius + "px; }");
#endif

    setSplashComponents();

    globalLayout = new QHBoxLayout;
    globalLayout->addWidget(mainWidget);
    setLayout(globalLayout);
}

TupSplash::~TupSplash()
{
}

void TupSplash::setSplashComponents()
{
    QPixmap logoPixmap(imgPath + "splash_logo.png");
    QLabel *logoLabel = new QLabel;
    logoLabel->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    logoLabel->setPixmap(logoPixmap.scaledToWidth(splashLogoWidth, Qt::SmoothTransformation));
 
    QPixmap sloganPixmap(imgPath + "splash_slogan.png");
    QLabel *sloganLabel = new QLabel;
    sloganLabel->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    sloganLabel->setPixmap(sloganPixmap.scaledToWidth(splashSloganWidth, Qt::SmoothTransformation));

    if (isCellPhone) {
        queue.enqueue(logoLabel);
        queue.enqueue(sloganLabel);
    }

    dynamicLayout = new QGridLayout;
    dynamicLayout->addWidget(logoLabel, 0, 0);
    dynamicLayout->addWidget(sloganLabel, 0, 1);

    helpButton = new TupButton(imgPath + "help_button.png", splashButtonWidth, "", 0, true);
    connect(helpButton, &TupButton::clicked, this, &TupSplash::callHelp);

    startButton = new TupButton(imgPath + "start.png", splashButtonWidth, "", 0, true);
    connect(startButton, &TupButton::clicked, this, &TupSplash::closeSplash);

    QWidget *bottomWidget = new QWidget;
    if (!isCellPhone)
        bottomWidget->setStyleSheet("QWidget { background-color: rgb(240,240,240); }");
    bottomWidget->setFixedHeight(splashFooterHeight);

    bottomLayout = new QHBoxLayout(bottomWidget);
    bottomLayout->addStretch();
    bottomLayout->addWidget(helpButton);
    bottomLayout->addSpacing(splashSpace);
    bottomLayout->addWidget(startButton);
    bottomLayout->addStretch();

    if (!isCellPhone) {
        bottomLayout->setAlignment(helpButton, Qt::AlignRight|Qt::AlignVCenter);
        bottomLayout->setAlignment(startButton, Qt::AlignRight|Qt::AlignVCenter);
    }

    mainWidget = new QWidget;
    mainWidget->setStyleSheet("QWidget { background-color: rgb(255,255,255); "
                              "border-radius: " + borderRadius + "px; }");

    QVBoxLayout *contentLayout = new QVBoxLayout;
    contentLayout->addLayout(dynamicLayout);
    contentLayout->addWidget(bottomWidget);

    if (!isCellPhone)
        mainWidget->setFixedSize(size);

    mainWidget->setLayout(contentLayout);
}

void TupSplash::resizeEvent(QResizeEvent *)
{
    if (isCellPhone) {
        QScreen *screen = QGuiApplication::screens().at(0);
        int width = screen->geometry().width();
        int height = screen->geometry().height();

        globalLayout->setSizeConstraint(QLayout::SetNoConstraint);
        globalLayout->setContentsMargins(0, 0, 0, 0);
        bottomLayout->setSizeConstraint(QLayout::SetNoConstraint);

        foreach (QWidget *widget, queue)
            dynamicLayout->removeWidget(widget);

        if (width > height) {
            dynamicLayout->setSpacing(0);
            dynamicLayout->addWidget(queue[0], 0, 0);
            dynamicLayout->addWidget(queue[1], 0, 1);
        } else {
            dynamicLayout->setSpacing(20);
            dynamicLayout->addWidget(queue[0], 0, 0);
            dynamicLayout->addWidget(queue[1], 1, 0);
        }

        bottomLayout->setSizeConstraint(QLayout::SetDefaultConstraint);
        globalLayout->setSizeConstraint(QLayout::SetDefaultConstraint);

        mainWidget->setFixedSize(width, height);
    }
}

void TupSplash::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Back)
        closeSplash();
}

void TupSplash::closeSplash()
{
    activateWindow();
    raise();
    accept();
}

void TupSplash::callHelp()
{
    reject();
    emit helpRequired();
}

void TupSplash::mousePressEvent(QMouseEvent *event)
{
    if (!isCellPhone) {
        QPoint point = event->pos();
        int x0 = mainWidget->pos().x();
        int y0 = mainWidget->pos().y();
        int x1 = x0 + size.width();
        int y1 = y0 + size.height();
        if ((point.x() < x0 || point.x() > x1) || (point.y() < y0 || point.y() > y1))
            close();
    }
}
