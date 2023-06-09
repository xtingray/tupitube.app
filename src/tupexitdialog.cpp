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

#include "tupexitdialog.h"
#include "tupcheckbox.h"
#include "tuplabel.h"
#include "tupbutton.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QIcon>

TupExitDialog::TupExitDialog(QSettings *config, QWidget *parent) : QDialog(parent)
{
    setModal(true);
    setWindowFlags(Qt::Popup);

    settings = config;
    isCellPhone = settings->value("isCellPhone").toBool();
    int titleSize = settings->value("titleSize").toInt();
    int titleHeight = settings->value("titleHeight").toInt();
    int textSize = settings->value("textSize").toInt();
    iconWidth = settings->value("iconWidth").toInt();
    borderRadius = settings->value("borderRadius").toString();
    if (!isCellPhone)
        size = settings->value("exitDialogSize").toSize();
    separator = settings->value("height").toInt();
    separator = static_cast<int>(separator * 0.05);
    QString imgPath = config->value("imgPath").toString();

#ifdef Q_OS_ANDROID
    if (!isCellPhone) {
        setStyleSheet("* { background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
                       "stop:0 rgb(100, 100, 100), "
                       "stop:1 rgb(30, 30, 30)); }");
    }
#endif

    QLabel *titleLabel = new QLabel(tr("Want to Exit?"));
    titleLabel->setFixedHeight(titleHeight);

    textFont = this->font();
    textFont.setBold(true);
    textFont.setPointSize(titleSize);

    titleLabel->setStyleSheet("QLabel { background-color: rgb(200,200,200);"
                              "border-radius: " + borderRadius + "px; }");
    titleLabel->setFont(textFont);
    titleLabel->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);

    textFont.setBold(false);
    textFont.setPointSize(textSize);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(addExitItem(TupExitDialog::Back, imgPath + "back.png", tr("Back to project")));

    mainLayout->addLayout(addExitItem(TupExitDialog::SaveAndNew, imgPath + "new.png", tr("Start new project")));
#ifdef Q_OS_ANDROID
    mainLayout->addLayout(addExitItem(TupExitDialog::Share, imgPath + "save.png", tr("Save TUP source file")));
#endif
    mainLayout->addLayout(addExitItem(TupExitDialog::Quit, imgPath + "exit.png", tr("Quit TupiTube")));

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(titleLabel);
    layout->addLayout(mainLayout);
    layout->addStretch();

#ifdef Q_OS_IOS
    QString bgColor = "180,180,180";
#else
    QString bgColor = "232, 232, 232";
#endif

    centralWidget = new QWidget;
    centralWidget->setLayout(layout);

    if (!isCellPhone) {
        // layout->setContentsMargins(separator, separator*0.5, separator, separator*0.5);
        // #ifdef Q_OS_ANDROID
        centralWidget->setFixedSize(size);
        // #endif
    }

    QString style = "QWidget { background-color: rgb(" + bgColor + ");";
    #ifdef Q_OS_ANDROID
        if (!isCellPhone)
            style += "border-radius: " + borderRadius + "px;";
    #else
        style += "border-style: none;";
    #endif
    style += " }";

    centralWidget->setStyleSheet(style);

    QHBoxLayout *globalLayout = new QHBoxLayout;
    globalLayout->addWidget(centralWidget);

    setLayout(globalLayout);

    if (isCellPhone)
        move(QPoint(0, 0));
}

TupExitDialog::~TupExitDialog()
{
}

QHBoxLayout * TupExitDialog::addExitItem(Actions action, const QString &imgPath, const QString &title)
{
    QHBoxLayout *itemLayout = new QHBoxLayout;
    itemLayout->setSpacing(separator);    

    TupLabel *itemLabel = new TupLabel(action, title);
    if (action == TupExitDialog::Back)
        connect(itemLabel, &TupLabel::clicked, this, &QDialog::reject);
    else 
        connect(itemLabel, &TupLabel::clicked, this, &TupExitDialog::updateSelection);
    itemLabel->setFont(textFont);

    QPixmap pixmap = QPixmap::fromImage(QImage(imgPath)).scaledToWidth(iconWidth, Qt::SmoothTransformation);
    TupLabel *imgLabel = new TupLabel(action, pixmap);

    itemLayout->addWidget(imgLabel);
    itemLayout->addWidget(itemLabel);
    itemLayout->addStretch();

    return itemLayout;
}

void TupExitDialog::updateSelection(int index)
{
    settings->setValue("exitAction", index);
    accept();
}

void TupExitDialog::mousePressEvent(QMouseEvent *event)
{
    if (!isCellPhone) {
        QPoint point = event->pos();
        int x0 = centralWidget->pos().x();
        int y0 = centralWidget->pos().y();
        int x1 = x0 + size.width();
        int y1 = y0 + size.height();
        if ((point.x() < x0 || point.x() > x1) || (point.y() < y0 || point.y() > y1))
            close();
    }
}
