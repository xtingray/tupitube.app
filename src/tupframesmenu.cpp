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

#include "tupframesmenu.h"
#include "tupbutton.h"

#include <QDialog>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>

TupFramesMenu::TupFramesMenu(QSettings *config, QWidget *parent) : QDialog(parent)
{
    setModal(false);
    setWindowFlags(Qt::Popup);

    QString bgColor = config->value("themeColor").toString();
    setStyleSheet("QDialog { background-color: rgb(" + bgColor + "); }");

    bool isCellPhone = config->value("isCellPhone").toBool();

    QString system = config->value("system").toString();
    QString imgPath = config->value("imgPath").toString();

    int menuTitleSize = config->value("menuTitleSize").toInt();
    int menuTextSize = config->value("menuTextSize").toInt();

    borderRadius = config->value("borderRadius").toString();
    iconWidth = config->value("iconWidth").toInt();
    size = config->value("actionDialogSize").toSize();

    textFont = this->font();
    textFont.setBold(true);
    textFont.setPointSize(menuTitleSize);

    QLabel *titleLabel = new QLabel(tr("Frame Tools"));
    titleLabel->setStyleSheet("QLabel { background-color: rgb(200,200,200); "
                              "border-radius: " + borderRadius + "px; }");
    titleLabel->setFont(textFont);
    titleLabel->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);

    TupButton *forwardButton = new TupButton(imgPath + "insert_forward.png", iconWidth, tr("Insert"), menuTextSize);
    connect(forwardButton, SIGNAL(clicked()), this, SIGNAL(frameAddedForward()));

    TupButton *backButton = new TupButton(imgPath + "insert_back.png", iconWidth, tr("Insert"), menuTextSize);
    connect(backButton, SIGNAL(clicked()), this, SIGNAL(frameAddedBack()));

    TupButton *clearButton = new TupButton(imgPath + "clear.png", iconWidth, tr("Clear"), menuTextSize);
    connect(clearButton, SIGNAL(clicked()), this, SIGNAL(frameCleared()));

    TupButton *removeButton = new TupButton(imgPath + "remove.png", iconWidth, tr("Remove"), menuTextSize);
    connect(removeButton, SIGNAL(clicked()), this, SIGNAL(frameRemoved()));

    QGridLayout *gridLayout = new QGridLayout;

    gridLayout->addWidget(forwardButton, 0, 0);
    gridLayout->addWidget(backButton, 0, 1);

    if (!isCellPhone) {
        TupButton *cloneButton = new TupButton(imgPath + "clone.png", iconWidth, tr("Clone"), menuTextSize);
        connect(cloneButton, SIGNAL(clicked()), this, SIGNAL(frameCloned()));

        gridLayout->addWidget(cloneButton, 0, 2);
        gridLayout->addWidget(clearButton, 0, 3);
        gridLayout->addWidget(removeButton, 0, 4);
    } else {
        gridLayout->addWidget(clearButton, 0, 2);
        gridLayout->addWidget(removeButton, 0, 3);
    }

    QVBoxLayout *layout = new QVBoxLayout(this);

/*
#if !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS)
    QPushButton *closeButton = new QPushButton("X");
    closeButton->setFixedWidth(10);
    connect(closeButton, SIGNAL(clicked()), this, SLOT(closeMenu()));

    QHBoxLayout *headerLayout = new QHBoxLayout(this);
    headerLayout->addWidget(titleLabel);
    headerLayout->addWidget(closeButton);
    layout->addLayout(headerLayout);
#else
    layout->addWidget(titleLabel);
#endif
*/

    layout->addWidget(titleLabel);
    layout->addLayout(gridLayout);
}

TupFramesMenu::~TupFramesMenu()
{
}

void TupFramesMenu::keyReleaseEvent(QKeyEvent *event)
{
#if defined(Q_OS_ANDROID)
    if (event->key() == Qt::Key_Back)
        closeMenu();
#else
    if (event->key() == Qt::Key_Left) {
        emit leftArrowClicked();
    } else if (event->key() == Qt::Key_Right) {
        emit rightArrowClicked();
    } else if (event->key() == Qt::Key_Back) {
        closeMenu();
    } else if (event->key() == Qt::Key_Z) {
        emit undoRequested();
    } else if (event->key() == Qt::Key_Y) {
        emit redoRequested();
    }

#endif

    QDialog::keyPressEvent(event);
}

void TupFramesMenu::closeMenu()
{
    emit fileMenuClosed();
    activateWindow();
    raise();
    accept();
}
