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

#include "tupfiletopmenu.h"
#include "tupbutton.h"

#include <QGridLayout>

TupFileTopMenu::TupFileTopMenu(QSettings *config, QWidget *parent) : QDialog(parent)
{
    setModal(false);
    setWindowFlags(Qt::Popup);
    QString bgColor = config->value("themeColor").toString();
    setStyleSheet("QDialog { background-color: rgb(" + bgColor + "); }");

    QString system = config->value("system").toString();
    QString imgPath = config->value("imgPath").toString();

    int iconWidth = config->value("iconWidth").toInt();
    int menuTitleSize = config->value("menuTitleSize").toInt();
    int menuTextSize = config->value("menuTextSize").toInt();
    QString borderRadius = config->value("borderRadius").toString();

    QFont textFont = this->font();
    textFont.setBold(true);
    textFont.setPointSize(menuTitleSize);

    QLabel *titleLabel = new QLabel(tr("Project Options"));
    titleLabel->setStyleSheet("QLabel { background-color: rgb(200,200,200); "
                              "border-radius: " + borderRadius + "px; }");
    titleLabel->setFont(textFont);
    titleLabel->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);

    TupButton *settingsButton = new TupButton(imgPath + "settings.png", iconWidth, tr("Settings"), menuTextSize);
    connect(settingsButton, SIGNAL(clicked()), this, SIGNAL(settings()));

    TupButton *newButton = new TupButton(imgPath + "new.png", iconWidth, tr("New"), menuTextSize);
    connect(newButton, SIGNAL(clicked()), this, SIGNAL(newProject()));

    TupButton *postButton = new TupButton(imgPath + "share.png", iconWidth, tr("Share"), menuTextSize);
    connect(postButton, SIGNAL(clicked()), this, SIGNAL(postRequested()));

    TupButton *worksButton = new TupButton(imgPath + "works.png", iconWidth, tr("Projects"), menuTextSize);
    connect(worksButton, SIGNAL(clicked()), this, SIGNAL(works()));

    TupButton *tupButton = new TupButton(imgPath + "save.png", iconWidth, tr("TUP File"), menuTextSize);
    connect(tupButton, SIGNAL(clicked()), this, SIGNAL(tupFile()));

    #ifndef TUP_DEBUG
        TupButton *helpButton = new TupButton(imgPath + "help.png", iconWidth, tr("Help"), menuTextSize);
        connect(helpButton, SIGNAL(clicked()), this, SIGNAL(help()));
    #endif

#if defined(Q_OS_ANDROID) && defined(TUP_DEBUG)
    TupButton *logButton = new TupButton(imgPath + "log.png", iconWidth, tr("Log"), menuTextSize);
    connect(logButton, SIGNAL(clicked()), this, SIGNAL(openLog()));
#endif

    QGridLayout *gridLayout = new QGridLayout;

    // row 0
    #ifndef TUP_DEBUG
        gridLayout->addWidget(helpButton, 0, 0);
    #endif
    gridLayout->addWidget(tupButton, 0, 1);
    gridLayout->addWidget(worksButton, 0, 2);
    gridLayout->addWidget(postButton, 0, 3);
    gridLayout->addWidget(newButton, 0, 4);
    gridLayout->addWidget(settingsButton, 0, 5);

#if defined(Q_OS_ANDROID) && defined(TUP_DEBUG)
    gridLayout->addWidget(logButton, 0, 6);
#endif

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(titleLabel);
    layout->addLayout(gridLayout);
}

TupFileTopMenu::~TupFileTopMenu()
{
}

void TupFileTopMenu::keyReleaseEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Back) {
        emit fileMenuClosed();

        activateWindow();
        raise();
        accept();
    }
}
