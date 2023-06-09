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

#include "tuphelpdialog.h"
#include "tupbutton.h"

#include <QScrollArea>
#include <QScroller>
#include <QLabel>
#include <QTextBrowser>
#include <QDesktopServices>
#include <QLocale>
#include <QFile>

TupHelpDialog::TupHelpDialog(QSettings *config, QWidget *parent) : QDialog(parent)
{
    setModal(true);
    setWindowFlags(Qt::Popup);

    isCellPhone = config->value("isCellPhone").toBool();

    QString system = config->value("system").toString();
    imgPath = config->value("imgPath").toString();

    lowRes = config->value("lowRes").toBool();
    QString bgColor = config->value("themeColor").toString();
    titleSize = config->value("titleSize").toInt();
    titleHeight = config->value("titleHeight").toInt();
    helpTextSize = config->value("helpTextSize").toInt();
    borderRadius = config->value("borderRadius").toString();
    iconWidth = config->value("iconWidth").toInt();
    int dialogButtonsHeight = config->value("dialogButtonsHeight").toInt();
    size = config->value("helpDialogSize").toSize();

#if defined(Q_OS_ANDROID)
    if (!isCellPhone && lowRes)
        setStyleSheet("* { background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
                      "stop:0 rgb(100, 100, 100), stop:1 rgb(30, 30, 30)); }");
#endif

    TupButton *closeButton = new TupButton(imgPath + "ok.png", iconWidth);
    closeButton->setFixedHeight(dialogButtonsHeight);
    connect(closeButton, &TupButton::clicked, this, &QWidget::close);

    tabWidget = new QTabWidget;
    tabWidget->setStyleSheet("QTabBar::tab:selected { "
                             "background-color: rgba(0, 137, 0, 100); "
                             "}"
                             "QTabBar::tab:!selected {"
                             "background-color: rgba(50, 50, 50, 50); "
                             "}");

    QScrollArea *helpArea = new QScrollArea;
    helpArea->setWidgetResizable(true);
    helpArea->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);

    if (!isCellPhone) {
        helpArea->setStyleSheet("QScrollArea {"
            "background-color: rgb(255,255,255);"
            "}"
            "QScrollBar:vertical {"
            "background-color: rgb(200,200,200);"
            "}"
            "QScrollBar::handle:vertical {"
            "background-color: rgb(130,130,130);"
            "}");
    } else {
        helpArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        helpArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    }

    if (isCellPhone || !lowRes) { // Android cellphones
        helpArea->setWidget(smallHelpPage());
        QScroller::grabGesture(helpArea, QScroller::LeftMouseButtonGesture);
    } else {
        helpArea->setWidget(helpPage());
    }

    tabWidget->addTab(helpArea, tr("Help"));

#ifdef Q_OS_WIN
    tabWidget->addTab(shortcutsPage(), tr("Shortcuts"));
#endif

    tabWidget->addTab(aboutPage(), tr("About TupiTube"));
    // connect(tabWidget, SIGNAL(currentChanged(int)), this, SLOT(handleEvents(int)));

    QBoxLayout *innerLayout = new QVBoxLayout;
    innerLayout->addWidget(tabWidget);
    innerLayout->addWidget(closeButton);
    innerLayout->setAlignment(closeButton, Qt::AlignRight);

QString style = "QWidget { background-color: rgb(" + bgColor + "); ";
#ifdef Q_OS_ANDROID
    style += "border-radius: " + borderRadius + "px; }";
#else
    style += "border-style: none; }";
#endif

    if (!isCellPhone && lowRes) {
        widget = new QWidget;
        widget->setStyleSheet(style);
        widget->setFixedSize(size);
        widget->setLayout(innerLayout);

        QHBoxLayout *globalLayout = new QHBoxLayout;
        globalLayout->addWidget(widget);
        setLayout(globalLayout);
    } else {
        setLayout(innerLayout);
        setStyleSheet(style);
    }
 }

TupHelpDialog::~TupHelpDialog()
{
}

QWidget * TupHelpDialog::smallHelpPage()
{
    QWidget *helpTab = new QWidget;
    helpTab->setStyleSheet("* { background-color: rgb(255,255,255); }");

    QBoxLayout *contentLayout = new QVBoxLayout;
    contentLayout->addLayout(addHelpItem(imgPath + "tools.png", tr("Open tools dialog")));
    contentLayout->addLayout(addHelpItem(imgPath + "back.png", tr("Back one frame")));
    contentLayout->addLayout(addHelpItem(imgPath + "forward.png", tr("Add new frame forward")));
    contentLayout->addLayout(addHelpItem(imgPath + "play.png", tr("Play animation")));
    contentLayout->addLayout(addHelpItem(imgPath + "stop.png", tr("Stop animation")));
    contentLayout->addLayout(addHelpItem(imgPath + "palette.png", tr("Change pen color")));
    contentLayout->addLayout(addHelpItem(imgPath + "thickness.png", tr("Change thickness pen")));
    contentLayout->addLayout(addHelpItem(imgPath + "opacity.png", tr("Change pen opacity")));
    contentLayout->addLayout(addHelpItem(imgPath + "brush.png", tr("Change pen brush pattern")));
    contentLayout->addLayout(addHelpItem(imgPath + "undo.png", tr("Erase line")));
    contentLayout->addLayout(addHelpItem(imgPath + "redo.png", tr("Redo line")));
    contentLayout->addLayout(addHelpItem(imgPath + "clone.png", tr("Clone current frame")));
    contentLayout->addLayout(addHelpItem(imgPath + "frames.png", tr("Open frame tools")));
    contentLayout->addLayout(addHelpItem(imgPath + "gallery.png", tr("Project options menu")));
    contentLayout->addLayout(addHelpItem(imgPath + "settings.png", tr("Modify app settings")));
    contentLayout->addLayout(addHelpItem(imgPath + "new.png", tr("Create new project")));
    contentLayout->addLayout(addHelpItem(imgPath + "share.png", tr("Post your work")));
    contentLayout->addLayout(addHelpItem(imgPath + "works.png", tr("Projects gallery")));
    contentLayout->addLayout(addHelpItem(imgPath + "save.png", tr("Save Animation Source File")));

    contentLayout->addStretch();
    helpTab->setLayout(contentLayout);

    return helpTab;
}

QWidget * TupHelpDialog::helpPage()
{
    QWidget *helpTab = new QWidget;
    helpTab->setStyleSheet("* { background-color: rgb(255,255,255); }");

    QBoxLayout *leftLayout = new QVBoxLayout;
    leftLayout->addLayout(addHelpItem(imgPath + "tools.png", tr("Open tools dialog")));
    leftLayout->addLayout(addHelpItem(imgPath + "back.png", tr("Back one frame")));
    leftLayout->addLayout(addHelpItem(imgPath + "forward.png", tr("Add new frame forward")));
    leftLayout->addLayout(addHelpItem(imgPath + "play.png", tr("Play animation")));
    leftLayout->addLayout(addHelpItem(imgPath + "stop.png", tr("Stop animation")));
    leftLayout->addLayout(addHelpItem(imgPath + "palette.png", tr("Change pen color")));
    leftLayout->addLayout(addHelpItem(imgPath + "thickness.png", tr("Change thickness pen")));
    leftLayout->addLayout(addHelpItem(imgPath + "opacity.png", tr("Change pen opacity")));
    leftLayout->addLayout(addHelpItem(imgPath + "brush.png", tr("Change pen brush pattern")));

    QBoxLayout *rightLayout = new QVBoxLayout;
    rightLayout->addLayout(addHelpItem(imgPath + "undo.png", tr("Erase line")));
    rightLayout->addLayout(addHelpItem(imgPath + "redo.png", tr("Redo line")));
    rightLayout->addLayout(addHelpItem(imgPath + "clone.png", tr("Clone current frame")));
    rightLayout->addLayout(addHelpItem(imgPath + "frames.png", tr("Open frame tools")));
    rightLayout->addLayout(addHelpItem(imgPath + "settings.png", tr("Modify app settings")));
    rightLayout->addLayout(addHelpItem(imgPath + "new.png", tr("Create new project")));
    rightLayout->addLayout(addHelpItem(imgPath + "share.png", tr("Post your work")));
    rightLayout->addLayout(addHelpItem(imgPath + "works.png", tr("Projects gallery")));
    rightLayout->addLayout(addHelpItem(imgPath + "save.png", tr("Save Animation Source File")));

    QBoxLayout *contentLayout = new QHBoxLayout;
    int spaceH = static_cast<int>(iconWidth * 0.5);
    int spaceV = static_cast<int>(iconWidth * 0.5);
    contentLayout->setContentsMargins(spaceV, spaceH, spaceV, spaceH);
    contentLayout->setSpacing(static_cast<int>(iconWidth * 0.3));
    contentLayout->addLayout(leftLayout);
    contentLayout->addLayout(rightLayout);
    contentLayout->addStretch();

    helpTab->setLayout(contentLayout);

    return helpTab;
}

QHBoxLayout * TupHelpDialog::addHelpItem(const QString &iconPath, const QString &labelText)
{
    QHBoxLayout *itemLayout = new QHBoxLayout;
    itemLayout->setSpacing(static_cast<int>(iconWidth * 0.2));

    QImage itemImg(iconPath);
    QLabel *itemIcon = new QLabel;
    itemIcon->setPixmap(QPixmap::fromImage(itemImg).scaledToWidth(iconWidth, Qt::SmoothTransformation));

    QFont textFont = this->font();
    textFont.setBold(false);
    textFont.setPointSize(helpTextSize);
	
    QLabel *itemLabel = new QLabel(labelText);
    itemLabel->setFont(textFont);

    itemLayout->addWidget(itemIcon);
    itemLayout->addWidget(itemLabel);
    itemLayout->addStretch();

    return itemLayout;
}

QWidget * TupHelpDialog::aboutPage()
{
    QWidget *aboutTab = new QWidget;
    aboutTab->setStyleSheet("* { background-color: rgb(232,232,232); }");

    QBoxLayout *innerLayout = new QVBoxLayout(aboutTab);

    QFont font = this->font();
    font.setBold(true);
    font.setPointSize(titleSize);

    QLabel *aboutLabel = new QLabel(tr("About TupiTube"));
    aboutLabel->setStyleSheet("QLabel { background-color: rgb(200,200,200); "
	                           "border-radius: " + borderRadius + "px; }");
    aboutLabel->setFont(font);
    aboutLabel->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
	aboutLabel->setFixedHeight(titleHeight);

    QWidget *titleWidget = new QWidget(this);	
    QHBoxLayout *titleLayout = new QHBoxLayout(titleWidget);
    titleLayout->addWidget(aboutLabel);

    QString text = ":resources/about.html";
    QStringList langSupport;
    // langSupport << "es" << "pt" << "jp" << "cn" << "fr";
    langSupport << "es" << "pt";
    QString locale = QString(QLocale::system().name()).left(2);
    if (locale.compare("en") != 0 && langSupport.contains(locale))
        text = ":resources/about_" + locale + ".html";

    /*
    #ifdef TUP_CPE
        text = ":resources/about_es_ccc.html";
    #endif
    */

    QFile file(text);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
#ifdef TUP_DEBUG
        qDebug() << "Fatal Error: about file not found -> " << text;
#endif
        return nullptr;
    }

    QString line = ""; 
    while (!file.atEnd())
           line += file.readLine();

    QFont textFont = this->font();
    textFont.setBold(false);
    textFont.setPointSize(helpTextSize);

    QStringList paths;
    paths << ":resources/";
    QTextBrowser *description = new QTextBrowser;
    description->setSearchPaths(paths);
    description->setFont(textFont);
    description->setOpenExternalLinks(true);

#ifdef Q_OS_ANDROID
    description->setStyleSheet("* { background-color: rgb(255,255,255); "
                               "font: normal 24px; }");
#else
    description->setStyleSheet("* { background-color: rgb(255,255,255); "
                               "font: normal 18px; }");
#endif

    description->setHtml(line);
    description->setReadOnly(true);
    description->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    description->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    innerLayout->addWidget(titleWidget);

    QImage image(":images/common/about.png");
    QLabel *icon = new QLabel("");
    icon->setAlignment(Qt::AlignHCenter);
    int imgW = iconWidth;
    if (!isCellPhone)
        imgW = static_cast<int>(size.height() * 0.25);

    icon->setPixmap(QPixmap::fromImage(image).scaledToWidth(imgW,
                    Qt::SmoothTransformation));
    innerLayout->addWidget(icon);    

    description->setStyleSheet("QTextEdit {"
                               "background-color: rgb(255,255,255);"
                               "}"
                               "QScrollBar:vertical {"
                               "background-color: rgb(200,200,200);"
                               "}"
                               "QScrollBar::handle:vertical {"
                               "background-color: rgb(130,130,130);"
                               "}");

    innerLayout->addWidget(description);
    QScroller::grabGesture(description, QScroller::LeftMouseButtonGesture);

    return aboutTab;
}

#ifdef Q_OS_WIN
QWidget * TupHelpDialog::shortcutsPage()
{
    QWidget *shortcutsTab = new QWidget;
    shortcutsTab->setStyleSheet("* { background-color: rgb(232,232,232); }");

    QBoxLayout *innerLayout = new QVBoxLayout(shortcutsTab);

    QFont font = this->font();
    font.setBold(true);
    font.setPointSize(titleSize);

    /*
    QLabel *shortcutsLabel = new QLabel(tr("Shortcuts"));
    shortcutsLabel->setStyleSheet("QLabel { background-color: rgb(200,200,200); "
                                  "border-radius: " + borderRadius + "px; }");
    shortcutsLabel->setFont(font);
    shortcutsLabel->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    shortcutsLabel->setFixedHeight(titleHeight);

    QWidget *titleWidget = new QWidget(this);
    QHBoxLayout *titleLayout = new QHBoxLayout(titleWidget);
    titleLayout->addWidget(shortcutsLabel);
    */

    QString text = ":resources/shortcuts.html";
    QStringList langSupport;
    // langSupport << "es" << "pt" << "jp" << "cn" << "fr";
    langSupport << "es" << "pt";
    QString locale = QString(QLocale::system().name()).left(2);

    if (locale.compare("en") != 0 && langSupport.contains(locale))
        text = ":resources/shortcuts_" + locale + ".html";

    QFile file(text);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
#ifdef TUP_DEBUG
        qDebug() << "Fatal Error: about file not found -> " << text;
#endif
        return nullptr;
    }

    QString line = "";
    while (!file.atEnd())
           line += file.readLine();

    QFont textFont = this->font();
    textFont.setBold(false);
    textFont.setPointSize(helpTextSize);

    QStringList paths;
    paths << ":resources/";
    QTextBrowser *commands = new QTextBrowser;
    commands->setSearchPaths(paths);
    commands->setFont(textFont);
    commands->setOpenExternalLinks(true);
    commands->setStyleSheet("* { background-color: rgb(255,255,255); "
                               "font: normal 18px; }");

    commands->setHtml(line);
    commands->setReadOnly(true);
    commands->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    commands->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // innerLayout->addWidget(titleWidget);

    commands->setStyleSheet("QTextEdit {"
                               "background-color: rgb(255,255,255);"
                               "}"
                               "QScrollBar:vertical {"
                               "background-color: rgb(200,200,200);"
                               "}"
                               "QScrollBar::handle:vertical {"
                               "background-color: rgb(130,130,130);"
                               "}");

    innerLayout->addWidget(commands);
    QScroller::grabGesture(commands, QScroller::LeftMouseButtonGesture);

    return shortcutsTab;
}
#endif

void TupHelpDialog::mousePressEvent(QMouseEvent *event)
{
    if (!isCellPhone) {
        QPoint point = event->pos();
        int x0 = widget->pos().x();
        int y0 = widget->pos().y();
        int x1 = x0 + size.width();
        int y1 = y0 + size.height();
        if ((point.x() < x0 || point.x() > x1) || (point.y() < y0 || point.y() > y1))
            close();
    }
}

void TupHelpDialog::handleEvents(int index)
{
    if (index == 2) {
        close();
        tabWidget->setCurrentIndex(0);
        QDesktopServices::openUrl(QString("https://www.youtube.com/tupitube/videos"));
    }
}

void TupHelpDialog::updateInternetFlag(bool hasInternet)
{
#ifdef Q_OS_WIN
    if (hasInternet) {
        if (tabWidget->count() == 3)
            tabWidget->addTab(new QWidget, tr("Tutorials"));
    } else {
        if (tabWidget->count() == 4)
            tabWidget->removeTab(3);
    }
#else
    if (hasInternet) {
        if (tabWidget->count() == 2)
            tabWidget->addTab(new QWidget, tr("Tutorials"));
    } else {
        if (tabWidget->count() == 3)
            tabWidget->removeTab(2);
    }
#endif
}
