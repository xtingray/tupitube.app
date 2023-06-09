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

#include "tupsettings.h"

#include <QDialog>
#include <QGuiApplication>
#include <QApplication>
#include <QScreen>
#include <QBoxLayout>
#include <QTabWidget>
#include <QScrollArea>
#include <QDesktopServices>
#include <QTimer>
#include <QKeyEvent>
#include <QRegularExpression>

TupSettings::TupSettings(QSettings *config, int speed, int onionSkin, QWidget *parent) : QDialog(parent)
{
#ifdef TUP_DEBUG
    qDebug() << "[TupSettings::TupSettings()]";
#endif

    setModal(true);
    setWindowFlags(Qt::Popup);

    QScreen *screen = QGuiApplication::screens().at(0);
    int width = screen->geometry().width();

    settings = config;
    fps = speed;
    onion = onionSkin;
    isCellPhone = config->value("isCellPhone").toBool();
    isTablet = settings->value("isTablet").toBool();

    QString system = config->value("system").toString();
    imgPath = config->value("imgPath").toString();

    height = config->value("height").toInt();
    int titleSize = config->value("titleSize").toInt();
    int titleHeight = config->value("titleHeight").toInt();
    textSize = config->value("textSize").toInt();
    themeColor = config->value("themeColor").toString();
    borderRadius = config->value("borderRadius").toString();
    iconWidth = config->value("iconWidth").toInt();
    int dialogButtonsHeight = config->value("dialogButtonsHeight").toInt();
    int vSliderWidth = config->value("vSliderWidth").toInt();
    int hSliderWidth = config->value("hSliderWidth").toInt();
    sliderSize = QSize(vSliderWidth, hSliderWidth);
    if (!isCellPhone)
        dialogSize = config->value("settingsDialogSize").toSize();
    else
        dialogSize = QSize(width, height);

    oldUsername = settings->value("username", "").toString();
    oldPasswd = encryptPassword(settings->value("passwd", "").toString());

#if defined(Q_OS_ANDROID)
    if (!isCellPhone) {
        setStyleSheet("* { background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
                      "stop:0 rgb(100, 100, 100), stop:1 rgb(30, 30, 30)); }");
        colSpacing = width * (0.02);
    } else {
        colSpacing = width * (0.05);
    }
#else
    colSpacing = width * (0.01);
#endif

    QTabWidget *tabWidget = new QTabWidget;
    tabWidget->setStyleSheet("QTabBar::tab:selected { "
                             "background-color: rgba(0,137,0,100); "
                             "}"
                             "QTabBar::tab:!selected {"
                             "background-color: rgba(50,50,50,50); "
                             "}");

    QString scrollStyle = "QScrollArea {"
                          "background-color: rgb(255,255,255);"
                          "}"
                          "QScrollBar:vertical {"
                          "background-color: rgb(200,200,200);"
                          "}"
                          "QScrollBar::handle:vertical {"
                          "background-color: rgb(130,130,130);"
                          "}";

    QScrollArea *generalArea = new QScrollArea;
    generalArea->setStyleSheet(scrollStyle);
    generalArea->setWidgetResizable(true);

    generalSettings();
    generalArea->setWidget(generalTab);
    generalArea->setAlignment(Qt::AlignHCenter);
    tabWidget->addTab(generalArea, tr("General"));

    QScrollArea *canvasArea = new QScrollArea;
    canvasArea->setStyleSheet(scrollStyle);
    canvasArea->setWidgetResizable(true);

    canvasSettings();
    canvasArea->setWidget(canvasTab);
    canvasArea->setAlignment(Qt::AlignHCenter);
    tabWidget->addTab(canvasArea, tr("Canvas"));

    QScrollArea *brushArea = new QScrollArea;
    brushArea->setStyleSheet(scrollStyle);
    brushArea->setWidgetResizable(true);

    brushSettings();
    brushArea->setWidget(brushTab);
    brushArea->setAlignment(Qt::AlignHCenter);
    tabWidget->addTab(brushArea, tr("Brush"));

    QScrollArea *tupArea = new QScrollArea;
    tupArea->setStyleSheet(scrollStyle);
    tupArea->setWidgetResizable(true);

    tupitubeSettings();
    tupArea->setWidget(tupitubeTab);
    tupArea->setAlignment(Qt::AlignHCenter);
    tabWidget->addTab(tupArea, tr("TupiTube"));

    TupButton *okButton = new TupButton(imgPath + "ok.png", iconWidth);
    okButton->setFixedHeight(dialogButtonsHeight);
    connect(okButton, &TupButton::clicked, this, &TupSettings::save);

    TupButton *closeButton = new TupButton(imgPath + "close.png", iconWidth);
    closeButton->setFixedHeight(dialogButtonsHeight);
    connect(closeButton, &TupButton::clicked, this, &QWidget::close);

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(closeButton);
    buttonLayout->addWidget(okButton);

    QHBoxLayout *bottomLayout = new QHBoxLayout;

    if (isTablet) {
        QFont font = this->font();
        font.setBold(true);
        log = new QLabel();
        log->setFont(font);
        log->setStyleSheet("QLabel { color: #007000; }");
        log->setAlignment(Qt::AlignHCenter);

        QWidget *space = new QWidget;
        space->setFixedWidth(iconWidth / 2);
        bottomLayout->addWidget(space);
        bottomLayout->addWidget(log);
        bottomLayout->setAlignment(log, Qt::AlignLeft|Qt::AlignVCenter);
    }

    bottomLayout->addLayout(buttonLayout);
    bottomLayout->setAlignment(buttonLayout, Qt::AlignRight);

    QVBoxLayout *mainLayout = new QVBoxLayout;

    if (!isCellPhone) {
        QFont font = this->font();
        font.setBold(true);
        font.setPointSize(titleSize);

        QLabel *label = new QLabel(tr("TupiTube Settings"));
        label->setStyleSheet("QLabel { background-color: rgb(200,200,200);"
                             "border-radius: " + borderRadius + "px; }");
        label->setFixedHeight(titleHeight);
        label->setFont(font);
        label->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        mainLayout->addWidget(label);
    }

    mainLayout->addWidget(tabWidget);
    mainLayout->addLayout(bottomLayout);

    mainWidget = new QWidget;
    QString style = "QWidget { background-color: rgb(" + themeColor + "); ";
    #ifdef Q_OS_ANDROID
        style += "border-radius: " + borderRadius + "px; }";
    #else
        style += "border-style: none; }";
    #endif
    mainWidget->setStyleSheet(style);

    mainWidget->setLayout(mainLayout);
    if (!isCellPhone)
        mainWidget->setFixedSize(dialogSize);

    QHBoxLayout *globalLayout = new QHBoxLayout;
    globalLayout->addWidget(mainWidget);
    setLayout(globalLayout);
    setStyleSheet(style);

    if (isCellPhone)
        move(QPoint(0, 0));
}

TupSettings::~TupSettings()
{
}

void TupSettings::generalSettings()
{
    generalTab = new QWidget;
    generalTab->setStyleSheet("QWidget { background-color: rgb(255,255,255); }");

    double verticalSpacing = dialogSize.height() * 0.02;
    generalTab->setFixedWidth(static_cast<int>(dialogSize.width() * 0.9));

    QFont font = this->font();
    font.setBold(false);
    font.setPointSize(textSize);
    font.setBold(true);

    fpsLabel = new QLabel(tr("Speed (FPS): ") + QString::number(fps));
    fpsLabel->setFont(font);
    fpsLabel->setAlignment(Qt::AlignCenter);

    fpsSlider = new TupSlider(Qt::Horizontal, imgPath, sliderSize, TupSlider::Dial,
                              QColor(0, 196, 255),
                              QColor(0, 196, 255));
    if (isCellPhone)
        fpsSlider->setFixedHeight(sliderSize.height() * 4);

    fpsSlider->setRange(1, 24);
    fpsSlider->setValue(fps);
    fpsSlider->setParameters(Qt::SolidPattern, 1.0);
    connect(fpsSlider, &TupSlider::valueChanged, this, &TupSettings::updateFPS);

    QWidget *sliderWidget = new QWidget;
    QVBoxLayout *sliderLayout = new QVBoxLayout(sliderWidget);
    sliderLayout->setAlignment(Qt::AlignHCenter);
    sliderLayout->addWidget(fpsLabel);
    sliderLayout->addWidget(fpsSlider);

    if (!isCellPhone)
        sliderWidget->setFixedWidth(static_cast<int>(dialogSize.width() * 0.9));

    double factor = 0.15;
    if (isCellPhone)
        factor = 0.28;
    sliderWidget->setFixedHeight(static_cast<int>(height * factor));

    QVBoxLayout *contentLayout = new QVBoxLayout(generalTab);
    contentLayout->setAlignment(Qt::AlignHCenter);
    contentLayout->addSpacing(static_cast<int>(verticalSpacing));
    contentLayout->addWidget(sliderWidget);

#ifdef Q_OS_ANDROID
    if (isTablet) {
        QFrame *separator = new QFrame(this);
        separator->setFrameShape(QFrame::HLine);
        separator->setFrameShadow(QFrame::Sunken);
        separator->setStyleSheet("QWidget { border: 1px solid #ccc; }");

        QWidget *uiWidget = new QWidget;
        QLabel *uiLabel = new QLabel(tr("Interface Mode"));
        uiLabel->setFont(font);
        uiLabel->setAlignment(Qt::AlignCenter);

        font.setBold(false);

        phoneCheckBox = new TupCheckBox(0, imgPath + "unchecked.png", iconWidth,
                                        themeColor, borderRadius);

        TupLabel *phoneLabel = new TupLabel(1, tr("Phone"));
        phoneLabel->setFont(font);

        tabletCheckBox = new TupCheckBox(1, imgPath + "unchecked.png", iconWidth,
                                         themeColor, borderRadius);

        TupLabel *tabletLabel = new TupLabel(1, tr("Tablet"));
        tabletLabel->setFont(font);

        QHBoxLayout *uiLayout0 = new QHBoxLayout;
        uiLayout0->setAlignment(Qt::AlignHCenter);
        uiLayout0->setSpacing(static_cast<int>(colSpacing));
        uiLayout0->addWidget(phoneCheckBox);
        uiLayout0->addWidget(phoneLabel);

        QHBoxLayout *uiLayout1 = new QHBoxLayout;
        uiLayout1->setAlignment(Qt::AlignHCenter);
        uiLayout1->setSpacing(static_cast<int>(colSpacing));
        uiLayout1->addWidget(tabletCheckBox);
        uiLayout1->addWidget(tabletLabel);

        QVBoxLayout *uiLayout = new QVBoxLayout(uiWidget);
        uiLayout->addWidget(uiLabel);
        uiLayout->addLayout(uiLayout0);
        uiLayout->addLayout(uiLayout1);
        contentLayout->addWidget(separator);
        contentLayout->addWidget(uiWidget);

        if (isCellPhone)
            updateUIOption(0);
        else
            updateUIOption(1);

        connect(phoneCheckBox, &TupCheckBox::clicked, this, &TupSettings::updateUIMode);
        connect(phoneLabel, &TupLabel::clicked, this, &TupSettings::updateUIMode);
        connect(tabletCheckBox, &TupCheckBox::clicked, this, &TupSettings::updateUIMode);
        connect(tabletLabel, &TupLabel::clicked, this, &TupSettings::updateUIMode);
    }
#endif

    contentLayout->addStretch();
}

void TupSettings::canvasSettings()
{
    canvasTab = new QWidget;
    canvasTab->setStyleSheet("QWidget { background-color: rgb(255,255,255); }");

    double verticalSpacing = dialogSize.height() * 0.02;
    canvasTab->setFixedWidth(static_cast<int>(dialogSize.width() * 0.9));

    QFont font = this->font();
    font.setBold(false);
    font.setPointSize(textSize);

    previewText = tr("Show previous frame in canvas");
    nextText = tr("Show next frame in canvas");
    if (isCellPhone) {
        if (width < height) {
            previewText = tr("Show previous frame");
            nextText = tr("Show next frame");
        }
    }

    previousFrameCheckBox = new TupCheckBox(0, imgPath + "unchecked.png", iconWidth, themeColor,
                                            borderRadius);
    connect(previousFrameCheckBox, &TupCheckBox::clicked, this, &TupSettings::updateCanvasSelection);
    bool showPrevious = settings->value("showPreviousFrame", true).toBool();
    if (showPrevious)
        enableOption(true, previousFrameCheckBox);
    previousFrameCheckBox->setChecked(showPrevious);

    previousLabel = new TupLabel(0, previewText);
    previousLabel->setFont(font);
    connect(previousLabel, &TupLabel::clicked, this, &TupSettings::updateCanvasSelection);

    QHBoxLayout *layout0 = new QHBoxLayout;
    layout0->setSpacing(static_cast<int>(colSpacing));

#ifndef Q_OS_ANDROID
    int space = dialogSize.width() * 0.04;
    layout0->addSpacing(space);
#endif

    layout0->addWidget(previousFrameCheckBox);
    layout0->addWidget(previousLabel);
    layout0->addStretch();

    nextFrameCheckBox = new TupCheckBox(1, imgPath + "unchecked.png", iconWidth, themeColor, borderRadius);
    connect(nextFrameCheckBox, &TupCheckBox::clicked, this, &TupSettings::updateCanvasSelection);
    bool showNext = settings->value("showNextFrame", true).toBool();
    if (showNext)
        enableOption(true, nextFrameCheckBox);
    nextFrameCheckBox->setChecked(showNext);

    nextLabel = new TupLabel(1, nextText);
    nextLabel->setFont(font);
    connect(nextLabel, &TupLabel::clicked, this, &TupSettings::updateCanvasSelection);

    QHBoxLayout *layout1 = new QHBoxLayout;
    layout1->setSpacing(static_cast<int>(colSpacing));

#ifndef Q_OS_ANDROID
    layout1->addSpacing(space);
#endif
    layout1->addWidget(nextFrameCheckBox);
    layout1->addWidget(nextLabel);
    layout1->addStretch();

    QFrame *separator = new QFrame(this);
    separator->setFrameShape(QFrame::HLine);
    separator->setFrameShadow(QFrame::Sunken);
    separator->setStyleSheet("QWidget { border: 1px solid #ccc; }");

    // Onion skin UI

    font.setBold(true);
    qreal skin = onion / 100.0;

    onionLabel = new QLabel(tr("Frames Transparency: ") + QString::number(skin, 'f', 2));
    onionLabel->setFont(font);
    onionLabel->setAlignment(Qt::AlignCenter);

    onionSlider = new TupSlider(Qt::Horizontal, imgPath, sliderSize, TupSlider::Dial,
                                QColor(0, 196, 255), QColor(0, 196, 255));
    if (isCellPhone)
        onionSlider->setFixedHeight(sliderSize.height()*4);

    onionSlider->setRange(0, 100);
    onionSlider->setValue(onion);
    onionSlider->setParameters(Qt::SolidPattern, 1.0);
    connect(onionSlider, &TupSlider::valueChanged, this, &TupSettings::updateOnionSkin);

    QWidget *onionWidget = new QWidget;
    QVBoxLayout *opacityLayout = new QVBoxLayout(onionWidget);
    opacityLayout->setAlignment(Qt::AlignHCenter);
    opacityLayout->addWidget(onionLabel);
    opacityLayout->addWidget(onionSlider);
    opacityLayout->addStretch();

    double factor = 0.15;
    if (isCellPhone)
        factor = 0.28;
    onionWidget->setFixedHeight(static_cast<int>(height * factor));

    QVBoxLayout *optionsLayout = new QVBoxLayout;
    optionsLayout->addSpacing(static_cast<int>(verticalSpacing));
    optionsLayout->addLayout(layout0);
    optionsLayout->addLayout(layout1);
    optionsLayout->addSpacing(static_cast<int>(iconWidth * 0.4));
    optionsLayout->addWidget(separator);
    optionsLayout->addSpacing(static_cast<int>(verticalSpacing));
    optionsLayout->addWidget(onionWidget);
    optionsLayout->addStretch();

    QVBoxLayout *contentLayout = new QVBoxLayout(canvasTab);
    contentLayout->setAlignment(Qt::AlignHCenter);
    contentLayout->addLayout(optionsLayout);
}

void TupSettings::brushSettings()
{
    brushTab = new QWidget;
    brushTab->setStyleSheet("QWidget { background-color: rgb(255,255,255); }");

    double verticalSpacing = dialogSize.height() * 0.02;
    brushTab->setFixedWidth(static_cast<int>(dialogSize.width() * 0.9));

    QFont font = this->font();
    font.setPointSize(textSize);
    font.setBold(true);

    smoothLevel = settings->value("smoothLevel", 4.0).toFloat();

    smoothLevelLabel = new QLabel(tr("Auto-Smoothness Level: ") + QString::number(static_cast<double>(smoothLevel), 'f', 2));
    smoothLevelLabel->setFont(font);
    smoothLevelLabel->setAlignment(Qt::AlignCenter);

    smoothLevelSlider = new TupSlider(Qt::Horizontal, imgPath, sliderSize, TupSlider::Dial,
                                      QColor(0, 196, 255), QColor(0, 196, 255));
    if (isCellPhone)
        smoothLevelSlider->setFixedHeight(sliderSize.height() * 4);

    smoothLevelSlider->setRange(0, 1200);
    smoothLevelSlider->setValue(static_cast<int>(smoothLevel * 100));
    smoothLevelSlider->setParameters(Qt::SolidPattern, 1);
    connect(smoothLevelSlider, &TupSlider::valueChanged, this, &TupSettings::updateSmoothLevel);

    QVBoxLayout *contentLayout = new QVBoxLayout(brushTab);

    QWidget *sliderWidget = new QWidget;
    QVBoxLayout *sliderLayout = new QVBoxLayout(sliderWidget);
    sliderLayout->addWidget(smoothLevelLabel);
    sliderLayout->addWidget(smoothLevelSlider);
    sliderLayout->addStretch();

    double factor = 0.15;
    if (isCellPhone)
        factor = 0.28;
    sliderWidget->setFixedHeight(static_cast<int>(height * factor));

    if (!isCellPhone)
        sliderWidget->setFixedWidth(static_cast<int>(dialogSize.width() * 0.9));

    contentLayout->addSpacing(static_cast<int>(verticalSpacing));
    contentLayout->addWidget(sliderWidget);
    contentLayout->addStretch();
}

void TupSettings::tupitubeSettings()
{
    tupitubeTab = new QWidget;
    tupitubeTab->setStyleSheet("QWidget { background-color: rgb(255,255,255); }");

    double verticalSpacing = dialogSize.height() * 0.02;
    if (!isCellPhone)
        tupitubeTab->setFixedWidth(static_cast<int>(dialogSize.width() * 0.9));

    QFont font = this->font();
    font.setBold(true);
    font.setPointSize(textSize);

    QString style = "* { background-color: rgb(200, 200, 200); }";

    QLabel *loginLabel = new QLabel(tr("TupiTube Credentials"));
    loginLabel->setFont(font);
    loginLabel->setAlignment(Qt::AlignHCenter);

    double width = height * 0.8;

    font.setBold(false);
    QLabel *usernameLabel = new QLabel(tr("Username: "));
    usernameLabel->setFont(font);
    usernameEdit = new QLineEdit();
    usernameEdit->setMaximumWidth(static_cast<int>(width));
    usernameEdit->setStyleSheet(style);
    usernameEdit->setFont(font);

    QLabel *passwdLabel = new QLabel(tr("Password: "));
    passwdLabel->setFont(font);
    passwdEdit = new QLineEdit();
    passwdEdit->setMaximumWidth(static_cast<int>(width));
    passwdEdit->setMaxLength(100);
    passwdEdit->setEchoMode(QLineEdit::Password);
    passwdEdit->setStyleSheet(style);
    passwdEdit->setFont(font);

    QHBoxLayout *usernameLayout = new QHBoxLayout;
    usernameLayout->addWidget(usernameLabel);
    usernameLayout->addWidget(usernameEdit);

    QHBoxLayout *passwdLayout = new QHBoxLayout;
    passwdLayout->addWidget(passwdLabel);
    passwdLayout->addWidget(passwdEdit);

    usernameEdit->setText(settings->value("username", "").toString());
    passwdEdit->setText(encryptPassword(settings->value("passwd", "").toString()));

    QString buttonStyle = "QWidget { background-color: rgb(170,200,170); ";
    buttonStyle += "padding: " + QString::number(font.pointSize()) + "px; ";
    buttonStyle += "border-radius: " + borderRadius + "px; }";

    recoverButton = new TupButton("", iconWidth, tr("Forgot Password?"),
                                   textSize, false, Qt::Horizontal, this);
    recoverButton->setStyleSheet(buttonStyle);
    connect(recoverButton, &TupButton::clicked, this, &TupSettings::sendPasswordRequest);

    QHBoxLayout *recoverLayout = new QHBoxLayout;
    recoverLayout->addStretch(0);
    recoverLayout->addWidget(new QWidget);
    recoverLayout->addWidget(recoverButton);
    recoverLayout->addWidget(new QWidget);
    recoverLayout->addStretch(0);

    font.setBold(true);
    QLabel *registerLabel = new QLabel(tr("Don't have a TupiTube account?"));
    registerLabel->setFont(font);
    registerLabel->setAlignment(Qt::AlignHCenter);

    font.setBold(false);
    QLabel *emailLabel = new QLabel(tr("Email: "));
    emailLabel->setFont(font);
    emailEdit = new QLineEdit();
    connect(emailEdit, SIGNAL(returnPressed()), this, SLOT(formatEmail()));
    emailEdit->setMaximumWidth(static_cast<int>(width));
    emailEdit->setStyleSheet(style);
    emailEdit->setFont(font);

    QHBoxLayout *emailLayout = new QHBoxLayout;
    emailLayout->addWidget(emailLabel);
    emailLayout->addWidget(emailEdit);

    registerButton = new TupButton("", iconWidth, tr("Sign Up"),
                                   textSize, false, Qt::Horizontal, this);
    registerButton->setStyleSheet(buttonStyle);
    connect(registerButton, &TupButton::clicked, this, &TupSettings::sendRegisterRequest);

    QHBoxLayout *registerLayout = new QHBoxLayout;
    registerLayout->addStretch(0);
    registerLayout->addWidget(new QWidget);
    registerLayout->addWidget(registerButton);
    registerLayout->addWidget(new QWidget);
    registerLayout->addStretch(0);

    QWidget *registerPanel = new QWidget();
    registerPanel->setLayout(registerLayout);

    int space = settings->value("borderRadius").toInt();

    QVBoxLayout *loginLayout = new QVBoxLayout;
    loginLayout->addWidget(loginLabel);
    loginLayout->addSpacing(space);
    loginLayout->addLayout(usernameLayout);
    loginLayout->addLayout(passwdLayout);
    loginLayout->addLayout(recoverLayout);
    loginLayout->addStretch(0);

    QVBoxLayout *signUpLayout = new QVBoxLayout;
    signUpLayout->addWidget(registerLabel);
    signUpLayout->addLayout(emailLayout);
    signUpLayout->addWidget(registerPanel);
    signUpLayout->addStretch(0);

    QWidget *credentialsPanel = new QWidget();
    credentialsPanel->setLayout(loginLayout);
    QWidget *signUpPanel = new QWidget();
    signUpPanel->setLayout(signUpLayout);

    QTabWidget *tabWidget = new QTabWidget;
    tabWidget->setStyleSheet("QTabBar::tab:selected { "
                             "background-color: rgba(0,137,0,100); "
                             "}"
                             "QTabBar::tab:!selected {"
                             "background-color: rgba(50,50,50,50); "
                             "}");

    QString scrollStyle = "QScrollArea {"
                          "background-color: rgb(255,255,255);"
                          "}"
                          "QScrollBar:vertical {"
                          "background-color: rgb(200,200,200);"
                          "}"
                          "QScrollBar::handle:vertical {"
                          "background-color: rgb(130,130,130);"
                          "}";

    tabWidget->addTab(credentialsPanel, tr("Credentials"));
    tabWidget->addTab(signUpPanel, tr("Register"));

    QVBoxLayout *contentLayout = new QVBoxLayout(tupitubeTab);
    contentLayout->setAlignment(Qt::AlignCenter);
    contentLayout->addSpacing(static_cast<int>(verticalSpacing));
    contentLayout->addWidget(tabWidget);
    contentLayout->addStretch(0);
}

void TupSettings::formatEmail()
{
    QString input = emailEdit->text();
    emailEdit->setText(input.toLower());
}

void TupSettings::resizeEvent(QResizeEvent *event)
{
    if (isCellPhone) {
        QSize size = event->size();
        int w = size.width();
        int h = size.height();

        int tmp = width;
        width = height;
        height = tmp;

        previewText = tr("Show previous frame");
        nextText = tr("Show next frame");

        if (w > h) {
            previewText = tr("Show previous frame in canvas");
            nextText = tr("Show next frame in canvas");
        }
        previousLabel->setText(previewText);
        nextLabel->setText(nextText);

        setFixedSize(w, h);
    }
}

void TupSettings::updateFPS(int speed)
{
    fps = speed;
    fpsLabel->setText(tr("Speed (FPS): ") + QString::number(fps));
}

void TupSettings::updateOnionSkin(int onionFactor)
{
    onion = onionFactor;
    double value = onion / 100.0;
    onionLabel->setText(tr("Frames Transparency: ") + QString::number(value, 'f', 2));
}

void TupSettings::updateSmoothLevel(int level)
{
    smoothLevel = static_cast<float>(level / 100.0);
    smoothLevelLabel->setText(tr("Auto-Smoothness Level: ") + QString::number(static_cast<double>(smoothLevel), 'f', 2));
}

void TupSettings::save()
{
#ifdef TUP_DEBUG
    qDebug() << "[TupSettings::save()] - Saving settings...";
#endif

    settings->setValue("fps", fps);
    settings->setValue("onionSkin", onion);
    settings->setValue("smoothLevel", smoothLevel);
    settings->setValue("showPreviousFrame", previousFrameCheckBox->isChecked());
    settings->setValue("showNextFrame", nextFrameCheckBox->isChecked());

    QString username = usernameEdit->text();
    QString passwd = passwdEdit->text();
    if (!username.isEmpty()) {
        if (username.compare(oldUsername) != 0)
            settings->setValue("username", username);
    }
    if (!passwd.isEmpty()) {
        if (passwd.compare(oldPasswd) != 0)
            settings->setValue("passwd", encryptPassword(passwd));
    }

#ifdef Q_OS_ANDROID
    bool isTablet = settings->value("isTablet").toBool();
    if (isTablet) {
        bool oldFlag = settings->value("isCellPhone").toBool();
        bool newFlag = phoneCheckBox->isChecked();
        if (oldFlag != newFlag) {
            settings->setValue("isCellPhone", newFlag);
            settings->setValue("restart", true);
        } else {
            settings->setValue("restart", false);
        }
    }
#endif

    accept();
}

void TupSettings::updateUIOption(int index)
{
    switch(index) {
        case 0:
        {
            bool flag = false;
            if (!phoneCheckBox->isChecked())
                flag = true;

            phoneCheckBox->setChecked(flag);
            tabletCheckBox->setChecked(!flag);
            enableOption(flag, phoneCheckBox);
            enableOption(!flag, tabletCheckBox);
        }
        break;
        case 1:
        {
            bool flag = false;
            if (!tabletCheckBox->isChecked())
                flag = true;

            tabletCheckBox->setChecked(flag);
            phoneCheckBox->setChecked(!flag);
            enableOption(flag, tabletCheckBox);
            enableOption(!flag, phoneCheckBox);
        }
        break;
    }   
}

void TupSettings::updateUIMode(int index)
{
    updateUIOption(index);

    log->setText(tr("You must restart TupiTube!"));
    QTimer::singleShot(2000, this, SLOT(cleanLog()));
}

void TupSettings::updateCanvasSelection(int index)
{
    switch(index) {
        case 0:
        {
            bool flag = false;
            if (!previousFrameCheckBox->isChecked())
                flag = true;

            previousFrameCheckBox->setChecked(flag);
            enableOption(flag, previousFrameCheckBox);
        }
        break;
        case 1:
        {
            bool flag = false;
            if (!nextFrameCheckBox->isChecked())
                flag = true;

            nextFrameCheckBox->setChecked(flag);
            enableOption(flag, nextFrameCheckBox);
        }
        break;
    }
}

void TupSettings::enableOption(bool isChecked, TupCheckBox *control)
{
    if (isChecked) {
        control->setStyleSheet("* { background-color: rgb(200,200,200); "
                               "border-radius: " + borderRadius + "px; }");
        control->updateImage(imgPath + "checked.png");
    } else {
        control->setStyleSheet("* { background-color: rgb(232,232,232); "
                               "border-radius: " + borderRadius + "px; }");
        control->updateImage(imgPath + "unchecked.png");
    }
}

void TupSettings::mousePressEvent(QMouseEvent *event)
{
    if (!isCellPhone) {
        QPoint point = event->pos();
        int x0 = mainWidget->pos().x();
        int y0 = mainWidget->pos().y();
        int x1 = x0 + dialogSize.width();
        int y1 = y0 + dialogSize.height();
        if ((point.x() < x0 || point.x() > x1) || (point.y() < y0 || point.y() > y1))
            close();
    }
}

void TupSettings::sendPasswordRequest()
{
#ifdef TUP_DEBUG
    qDebug() << "TupSettings::sendRequest() - Tracing...";
#endif

    QDesktopServices::openUrl(QUrl(TUPITUBE_URL + QString("/api/recover.php")));
}

void TupSettings::sendRegisterRequest()
{
#ifdef TUP_DEBUG
    qDebug() << "TupSettings::sendRequest() - Tracing...";
#endif

    QString email = emailEdit->text().toLower();
    if (!email.isEmpty()) {
        QRegularExpression pattern("\\b[A-Z0-9._%+-]+@[A-Z0-9.-]+\\.[A-Z]{2,4}\\b");
        pattern.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
        QRegularExpressionMatch match = pattern.match(email);

        /*
        QRegExp mailREX("\\b[A-Z0-9._%+-]+@[A-Z0-9.-]+\\.[A-Z]{2,4}\\b");
        mailREX.setCaseSensitivity(Qt::CaseInsensitive);
        mailREX.setPatternSyntax(QRegExp::RegExp);
        */

        // if (mailREX.exactMatch(email)) {
        if (match.hasMatch()) {
            #ifndef Q_OS_ANDROID
                QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
            #endif
            registerButton->setEnabled(false);
            emailEdit->setText(email);
            QString url = TUPITUBE_URL + QString("/api/?a=register&e=" + email);
            manager = new QNetworkAccessManager(this);
            connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(registerAnswer(QNetworkReply*)));
            #ifdef TUP_DEBUG
                qDebug() << "GET request -> " << url;
            #endif
            request.setSslConfiguration(QSslConfiguration::defaultConfiguration());
            request.setUrl(QUrl(url));
            request.setRawHeader("User-Agent", BROWSER_FINGERPRINT);

            QNetworkReply *reply = manager->get(request);
            connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(slotError(QNetworkReply::NetworkError)));
        } else {
            #ifdef TUP_DEBUG
                qDebug() << "TupSettings::sendRequest() - Error: Invalid email syntax! -> " << email;
            #endif
            emailEdit->setText(" " + tr("Email is invalid. Please, fix it!"));
            QTimer::singleShot(2000, this, SLOT(cleanMessage()));
        }
    } else {
        #ifdef TUP_DEBUG
            qDebug() << "TupSettings::sendRequest() - Invalid email: field is empty!";
        #endif
        emailEdit->setText(" " + tr("Email field is empty. Type one!"));
        QTimer::singleShot(2000, this, SLOT(cleanMessage()));
    }
}

void TupSettings::cleanMessage() {
    emailEdit->setText("");
}

void TupSettings::cleanLog() {
    log->setText("");
}

void TupSettings::registerAnswer(QNetworkReply *reply)
{
#ifdef TUP_DEBUG
   qDebug() << "TupSettings::registerAnswer() - Tracing...";
#endif

    QByteArray array = reply->readAll();
    QString answer(array);
    if (!answer.isEmpty()) {
        if (answer.compare("FALSE") == 0) {
            #ifdef TUP_DEBUG
                qDebug() << "TupSettings::registerAnswer() - Error: e-mail already registered! :(";
            #endif
            emailEdit->setText(" " + tr("Error: Email already registered!"));
            QTimer::singleShot(2000, this, SLOT(cleanMessage()));
        } else {
            #ifdef TUP_DEBUG
                qDebug() << "TupSettings::registerAnswer() - URL: " << answer;
            #endif
            if (answer.startsWith("http")) {
                settings->setValue("email", emailEdit->text());
                QDesktopServices::openUrl(answer);
            } else {
                #ifdef TUP_DEBUG
                    qDebug() << "TupSettings::registerAnswer() - Error: Invalid register URL! :(";
                #endif
                emailEdit->setText(" " + tr("Please contact us at info@maefloresta.com"));
                QTimer::singleShot(3000, this, SLOT(cleanMessage()));
            }
        }
    } else {
        #ifdef TUP_DEBUG
            qDebug() << "TupSettings::registerAnswer() - Error: No data from server! :(";
        #endif
        emailEdit->setText(" " + tr("Please contact us at info@maefloresta.com"));
        QTimer::singleShot(3000, this, SLOT(cleanMessage()));
    }

    manager->deleteLater();
    registerButton->setEnabled(true);
    #ifndef Q_OS_ANDROID
        QApplication::restoreOverrideCursor();
    #endif
}

void TupSettings::slotError(QNetworkReply::NetworkError error)
{
    switch (error) {
        case QNetworkReply::HostNotFoundError:
             {
             #ifdef TUP_DEBUG
                 qDebug() << "TupSettings::slotError() - Network Error: Host not found";
             #endif
             }
        break;
        case QNetworkReply::TimeoutError:
             {
             #ifdef TUP_DEBUG
                 qDebug() << "TupSettings::slotError() - Network Error: Time out!";
             #endif
             }
        break;
        case QNetworkReply::ConnectionRefusedError:
             {
             #ifdef TUP_DEBUG
                 qDebug() << "TupSettings::slotError() - Network Error: Connection Refused!";
             #endif
             }
        break;
        case QNetworkReply::ContentNotFoundError:
             {
             #ifdef TUP_DEBUG
                 qDebug() << "TupSettings::slotError() - Network Error: Content not found!";
             #endif
             }
        break;
        case QNetworkReply::UnknownNetworkError:
        default:
             {
             #ifdef TUP_DEBUG
                 qDebug() << "TupSettings::slotError() - Network Error: Unknown Network error!";
             #endif
             }
        break;
    }
}
