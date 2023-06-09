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

#include "tuppostdialog.h"

#include <QGuiApplication>
#include <QScreen>

TupPostDialog::TupPostDialog(QSettings *config, QWidget *parent) : QDialog(parent)
{
    setWindowFlags(Qt::Popup);

    settings = config;
    isCellPhone = config->value("isCellPhone").toBool();
    QString bgColor = config->value("themeColor").toString();

    QString system = config->value("system").toString();
    imgPath = config->value("imgPath").toString();

    borderRadius = config->value("borderRadius").toString();
    iconWidth = config->value("iconWidth").toInt();
    textSize = settings->value("textSize").toInt();

#if defined(Q_OS_ANDROID) || defined(Q_OS_IOS)
    if (!isCellPhone)
        setStyleSheet("* { background-color: "
                      "qlineargradient(x1:0, y1:0, x2:0, y2:1, "
                      "stop:0 rgb(100, 100, 100), "
                      "stop:1 rgb(30, 30, 30)); }");
#endif

    QScreen *screen = QGuiApplication::screens().at(0);
    int width = screen->geometry().width();
    int height = screen->geometry().height();

    int min = height;
    if (width < height)
        min = width;
    if (isCellPhone) {
        postSpacing = static_cast<int>(min * 0.03);
    } else {
        #if defined(Q_OS_ANDROID)
            postSpacing = static_cast<int>(min * 0.04);
        #else
            postSpacing = min * 0.01;
        #endif
    }

    setDialogTitle();
    setProgressUI();
    setDialogButtonsPanel();

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(titleWidget);
    mainLayout->addWidget(progressWidget);
    mainLayout->addStretch();
    mainLayout->addLayout(bottomLayout);

    QWidget *widget = new QWidget;
    widget->setStyleSheet("* { background-color: rgb(" + bgColor + "); "
                          "border-radius: " + borderRadius + "px; }");
    widget->setLayout(mainLayout);
    if (!isCellPhone) {
        QSize size = config->value("postDialogSize").toSize();
        widget->setFixedSize(size);
    }

    QHBoxLayout *globalLayout = new QHBoxLayout;
    globalLayout->addWidget(widget);
    setLayout(globalLayout);

    if (isCellPhone)
        move(QPoint(0, 0));

    widget->activateWindow();
}

TupPostDialog::~TupPostDialog()
{
    progressTimer->stop();
}

void TupPostDialog::setDialogTitle()
{
    int titleSize = settings->value("titleSize").toInt();
    int titleHeight = settings->value("titleHeight").toInt();

    dialogFont = this->font();
    dialogFont.setBold(true);
    dialogFont.setPointSize(titleSize);

    dialogTitle = new QLabel(tr("Creating Media File..."));
    dialogTitle->setStyleSheet("QLabel { background-color: rgb(200,200,200); "
                               "color: black; "
                               "border-radius: " + borderRadius + "px; }");
    dialogTitle->setFixedHeight(titleHeight);
    dialogTitle->setFont(dialogFont);
    dialogTitle->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);

    titleWidget = new QWidget(this);
    QBoxLayout *titleLayout = new QHBoxLayout(titleWidget);
    titleLayout->addWidget(dialogTitle);
}

void TupPostDialog::setProgressUI()
{
    dialogFont.setBold(true);
    int progressFontSize = settings->value("progressFontSize").toInt();
    dialogFont.setPointSize(progressFontSize);

    int progressWidth = settings->value("progressWidth").toInt();
    for(int i=1; i<4; i++) {
        if (progressWidth < 0)
            pixmaps << QPixmap(imgPath + "loading0" + QString::number(i) + ".png");
        else
            pixmaps << QPixmap(imgPath + "loading0" + QString::number(i) + ".png").scaledToWidth(progressWidth);
    }

    progressLabel = new QLabel;
    progressLabel->setPixmap(pixmaps.at(0));
    progressLabel->setAlignment(Qt::AlignCenter);

    progressTimer = new QTimer(this);
    connect(progressTimer, SIGNAL(timeout()), this, SLOT(updateProgressLabel()));

    TupButton *cancelButton = new TupButton(imgPath + "close.png", iconWidth);
    connect(cancelButton, &TupButton::clicked, this, &TupPostDialog::cancelPost);

    TupLabel *cancelMsg = new TupLabel(0, tr("Cancel"));
    QFont font = this->font();
    font.setPointSize(textSize);
    cancelMsg->setFont(font);
    connect(cancelMsg, &TupLabel::clicked, this, &TupPostDialog::cancelPost);

    cancelMsg->setAlignment(Qt::AlignCenter);

    QWidget *blockWidget = new QWidget(this);
    QHBoxLayout *blockLayout = new QHBoxLayout(blockWidget);

    QWidget *cancelWidget = new QWidget;

#ifdef Q_OS_ANDROID
    QSize buttonSize = settings->value("postCancelButtonSize").toSize();
    cancelWidget->setFixedSize(buttonSize);
    cancelWidget->setStyleSheet("QWidget { background-color: rgb(200, 200, 200); "
                                "border-radius: " + borderRadius + "px; }");
#endif

    QHBoxLayout *cancelLayout = new QHBoxLayout(cancelWidget);
    cancelLayout->addStretch();
    cancelLayout->addWidget(cancelButton);
    cancelLayout->addWidget(cancelMsg);
    cancelLayout->addStretch();

    blockLayout->addWidget(cancelWidget);

    progressWidget = new QWidget(this);
    QVBoxLayout *progressLayout = new QVBoxLayout(progressWidget);
    progressLayout->addSpacing(postSpacing);
    progressLayout->addWidget(progressLabel);
    progressLayout->addSpacing(postSpacing);
    progressLayout->addWidget(blockWidget, Qt::AlignCenter);
    progressLayout->addStretch();
}

void TupPostDialog::setDialogButtonsPanel()
{
    int dialogButtonsHeight = settings->value("dialogButtonsHeight").toInt();

    okButton = new TupButton(imgPath + "ok.png", iconWidth);
    okButton->setFixedHeight(dialogButtonsHeight);
    connect(okButton, &TupButton::clicked, this, &TupPostDialog::executePost);

    closeButton = new TupButton(imgPath + "close.png", iconWidth);
    closeButton->setFixedHeight(dialogButtonsHeight);
    connect(closeButton, &TupButton::clicked, this, &QWidget::close);

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(closeButton);

    bottomLayout = new QHBoxLayout;
    bottomLayout->addLayout(buttonLayout, Qt::AlignRight);
    bottomLayout->setAlignment(buttonLayout, Qt::AlignRight);
}

void TupPostDialog::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event)

    if (isCellPhone) {
        QScreen *screen = QGuiApplication::screens().at(0);
        int width = screen->geometry().width();
        int height = screen->geometry().height();

        setFixedSize(width, height);
    }
}

void TupPostDialog::activateProgressUI()
{    
    counter = 0;
    operation = SUM;
    progressTimer->start(500); // time specified in ms

    progressWidget->show();
    okButton->hide();
    closeButton->hide();
}

void TupPostDialog::executePost()
{
    activateProgressUI();
    startPost();
}

void TupPostDialog::startPost()
{
     emit postActionStarted();
}

void TupPostDialog::cancelPost()
{
    progressTimer->stop();
    emit transmissionCancelled();
    close();
}

void TupPostDialog::updateProgressLabel()
{
    if (operation == SUM)
        counter++;
    else
        counter--;

    progressLabel->setPixmap(pixmaps.at(counter));
    if (counter == 2)
        operation = REST;
    if (counter == 0 && operation == REST)
        operation = SUM;
}
