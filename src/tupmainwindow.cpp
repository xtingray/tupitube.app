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

#include "tupmainwindow.h"
#include "tupsplash.h"

#include "tupopacitydialog.h"
#include "tupbutton.h"
#include "tupexitdialog.h"

// #if defined(Q_OS_ANDROID)
// #include <QAndroidJniObject>
// #include <QJniObject>
// #include "shareutils.h"
// #include "tuppermissions.h"
// #else

#ifndef Q_OS_ANDROID
#include <QFileDialog>
#include <QStandardPaths>
#include "tuppackagehandler.h"
#endif

#include "tupguiparams.h"

#include "tupgallerydialog.h"
#include "tupspace.h"

#include <QApplication>
#include <QtGui>
#include <QGraphicsScene>
#include <QLabel>
#include <QToolButton>
#include <QHBoxLayout>
#include <QSysInfo>
#include <QPushButton>

TupMainWindow::TupMainWindow() : QMainWindow()
{
    if (!QSslSocket::supportsSsl()) {
        #ifdef TUP_DEBUG
            qDebug() << "[TupMainWindow::TupMainWindow()] - Fatal Error: SSL support is missing! :O";
        #endif
    } else {
        request.setSslConfiguration(QSslConfiguration::defaultConfiguration());
        #ifdef TUP_DEBUG
            qDebug() << "[TupMainWindow::TupMainWindow()] - SSL support is enabled! :D";
        #endif
    }

    hasInternet = false;
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(checkInternetConnection()));
    timer->start(30000); // Check Internet connection every 30 segs

    setGUIParams();

#if defined(Q_OS_ANDROID)
    androidVersion = QSysInfo::productVersion().toDouble();
    #ifdef TUP_DEBUG
        qDebug() << "---";
        qDebug() << "[TupMainWindow::TupMainWindow()] - Product Version -> " << QSysInfo::productVersion();
        qDebug() << "[TupMainWindow::TupMainWindow()] - MAJOR RELEASE -> " << androidVersion;
        #if defined(Q_OS_ANDROID)
            logDialog->appendRecord("---");
            logDialog->appendRecord("* Product Version: " + QSysInfo::productVersion());
            logDialog->appendRecord("* MAJOR RELEASE: " + QString::number(androidVersion));
        #endif
    #endif
    /*
    if (androidVersion >= 6) {
        TupPermissions *permission = new TupPermissions();
        permission->requestExternalStoragePermission();
        bool result = permission->getPermissionResult();
        #ifdef TUP_DEBUG
            qDebug() << "* Permission Granted? -> " << result;
            #if defined(Q_OS_ANDROID)
                logDialog->appendRecord("* Permission Granted? -> " + QString::number(result));
            #endif
        #endif

        if (!result)
            close();
    }
    */
#endif

    mode = TupCanvas::Frames;
    postMode = Anonymous;
    scene = nullptr;
    canvas = nullptr;
    canvasWasUpdated = false;

    currentFrameIndex = 0;
    frameKey = "frame_0";
    notifying = false;

    borderOpacity = getBorderOpacity();

    colorHistoryList << "borderHistory" << "fillHistory" << "bgHistory";

    pen = QPen(getBorderColor(), getPenSize(), Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    QBrush brush = pen.brush();
    brush.setStyle(getBorderBrushStyle());
    pen.setBrush(brush);

    fillOpacity = getFillOpacity();
    QColor fillColor = getFillColor();
    fillColor.setAlpha(static_cast<int>(fillOpacity * 255));
    brush = pen.brush();
    brush.setColor(fillColor);
    pen.setBrush(brush);

    defaultComment = tr("Create and share animations easily with TupiTube. Available on Google Play.");
    setMouseTracking(true);

    setToolBar();
    loadAnimation();
    setMetadataWidget();
    showFullScreen();

    helpDialog = new TupHelpDialog(config, this);

    thicknessDialog = new TupThicknessDialog(config, pen, borderOpacity, this);
    connect(thicknessDialog, &TupThicknessDialog::penUpdated, this, &TupMainWindow::updatePenSize);
    connect(thicknessDialog, &TupThicknessDialog::finished, this, &TupMainWindow::closePenMenu);

    opacityDialog = new TupOpacityDialog(config, this);
    connect(opacityDialog, &TupOpacityDialog::opacitiesUpdated, this, &TupMainWindow::updateOpacities);
    connect(opacityDialog, &TupOpacityDialog::finished, this, &TupMainWindow::closePenMenu);

    colorDialog = new TupColorDialog(config, colorPalette, this);
    connect(colorDialog, &TupColorDialog::colorsUpdated, this, &TupMainWindow::updateColors);

    brushDialog = new TupBrushDialog(config, this);
    connect(brushDialog, &TupBrushDialog::brushesUpdated, this, &TupMainWindow::updateBrushes);
    connect(brushDialog, &TupBrushDialog::finished, this, &TupMainWindow::closePenMenu);

    postDialog = new TupPostDialog(config, this);
    connect(postDialog, &TupPostDialog::postActionStarted, this, &TupMainWindow::sendData);

    settingsDialog = new TupSettings(config, fps, onion, this);
    connect(settingsDialog, &QDialog::accepted, this, &TupMainWindow::updateSettings);

#if defined(Q_OS_ANDROID)
    TupSplash *splash = new TupSplash(config, this);
    connect(splash, &TupSplash::helpRequired, this, &TupMainWindow::showHelpDialog);
    connect(splash, &TupSplash::accepted, this, &TupMainWindow::welcomeMsg);
    splash->showMaximized();

    if (!isCellPhone) {
        undoAction->setVisible(true);
        palette->setVisible(true);
    } else {
        if (androidH > androidW) {
            undoAction->setVisible(false);
            redoAction->setVisible(false);
            palette->setVisible(false);
        } else {
            undoAction->setVisible(true);
            redoAction->setVisible(true);
            palette->setVisible(true);
        }
    }
#endif

    /* SQA: Code for debugging issues
    config->setValue("email", "");
    config->setValue("username", "");
    config->setValue("passwd", "");
    */
}

TupMainWindow::~TupMainWindow()
{
}

void TupMainWindow::checkInternetConnection()
{
    QNetworkAccessManager netManager;
    QNetworkRequest netRequest(QUrl("http://www.google.com"));
    QNetworkReply *reply = netManager.get(netRequest);

    QEventLoop loop;
    connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();
    if (reply->bytesAvailable())
       hasInternet = true;
    else
       hasInternet = false;
}

void TupMainWindow::setGUIParams()
{
#if defined(Q_OS_ANDROID)
    QJniObject qtActivity = QJniObject::callStaticObjectMethod("org/qtproject/qt/android/QtNative",
                                                                             "activity", "()Landroid/app/Activity;");
    QJniObject resources = qtActivity.callObjectMethod("getResources","()Landroid/content/res/Resources;");
    QJniObject displayMetrics = resources.callObjectMethod("getDisplayMetrics","()Landroid/util/DisplayMetrics;");
    int dpi = displayMetrics.getField<int>("densityDpi");
    int width = displayMetrics.getField<int>("widthPixels");
    int height = displayMetrics.getField<int>("heightPixels");
    androidW = width;
    androidH = height;

    #ifdef TUP_DEBUG
        QScreen *screen = QGuiApplication::screens().at(0);
        QRect rect = screen->geometry();
        QSizeF size = screen->physicalSize();
        qreal dots = screen->physicalDotsPerInch();
        int dpiX =  this->physicalDpiX();
        int dpiY =  this->physicalDpiY();
        qDebug() << "---";
        qDebug() << "[TupMainWindow::setGUIParams()] - Screen data: Geometry -> " << rect << " | physicalSize -> " << size
                 << " | DPI -> " << dpi << " | physicalDotsPerInch() -> " << dots;
        qDebug() << "[TupMainWindow::setGUIParams()] - DPI X -> " << dpiX;
        qDebug() << "[TupMainWindow::setGUIParams()] - DPI Y -> " << dpiY;
        qDebug() << "[TupMainWindow::setGUIParams()] - widthPixels -> " << width;
        qDebug() << "[TupMainWindow::setGUIParams()] - heightPixels -> " << height;
        qDebug() << "---";
    #endif
#else
    QScreen *displayDevice = QGuiApplication::screens().at(0);
    int dpi = this->physicalDpiX();
    int width = displayDevice->geometry().width();
    int height = displayDevice->geometry().height();

    #ifdef TUP_DEBUG
        qDebug() << "*** Screen data: Geometry -> " << displayDevice->geometry() << " | physicalSize -> " << displayDevice->physicalSize()
                 << " | DPI: " << displayDevice->physicalDotsPerInch();
        qDebug() << "*** DPI X -> " << this->physicalDpiX();
        qDebug() << "*** DPI -> " << this->physicalDpiY();
        qDebug() << "*** WIDTH -> " << width;
        qDebug() << "*** HEIGHT -> " << height;
    #endif
#endif

    TupGuiParams *guiParams = new TupGuiParams(dpi, width, height);

    config = guiParams->settings();

#if defined(Q_OS_ANDROID) && defined(TUP_DEBUG)
    logDialog = new TupLogDialog(config, this);
    logDialog->appendRecord("*** Screen data: Geometry -> (" + QString::number(rect.width()) + ", "
                            + QString::number(rect.height()) + ")"
                            + " | physicalSize -> ("
                            + QString::number(size.width()) + ", " + QString::number(size.height())
                            + ") | DPI: " + QString::number(dots));
    logDialog->appendRecord("*** DPI X -> " + QString::number(dpiX));
    logDialog->appendRecord("*** DPI Y -> " + QString::number(dpiY));
    logDialog->appendRecord("*** WIDTH -> " + QString::number(width));
    logDialog->appendRecord("*** HEIGHT -> " + QString::number(height));

    logDialog->appendRecord(guiParams->getLog());
#endif

    setStyleSheet("* { background-color: rgb(" + config->value("themeColor").toString() + "); color: black; border-style: none; }");

    system = config->value("system").toString();
    imgPath = config->value("imgPath").toString();

    isCellPhone = config->value("isCellPhone").toBool();
    diagonal = config->value("diagonal").toDouble();
    lowRes = config->value("lowRes").toBool();

    iconSize = config->value("iconSize").toInt();
    iconWidth = config->value("iconWidth").toInt();
    smallIconSize = config->value("smallIconSize").toInt();

    barSpacing = config->value("barSpacing").toString();
    borderRadius = config->value("borderRadius").toString();
    textSize = config->value("textSize").toInt();

    QImage stopImage(imgPath + "stop.png");
    stopIcon = QIcon(QPixmap::fromImage(stopImage));
    QImage playImage(imgPath + "play.png");
    playIcon = QIcon(QPixmap::fromImage(playImage));

    QImage fileImage(imgPath + "gallery.png");
    fileIcon = QIcon(QPixmap::fromImage(fileImage));
}

void TupMainWindow::resizeEvent(QResizeEvent *event)
{
    #ifdef TUP_DEBUG
        qDebug() << "[TupMainWindow::resizeEvent()] - Resizing interface...";
    #endif

    setCanvas();
    canvasWasUpdated = true;

    QSize screen = event->size();
    int w = screen.width();
    int h = screen.height();

    if (dockwidget->isVisible())
        canvas->disableScreen(true);

    if (!isCellPhone) {
        undoAction->setVisible(true);
        palette->setVisible(true);
        QSize size(iconSize, iconSize);
        if (h > w) {
            size = QSize(smallIconSize, smallIconSize);
            help->setVisible(false);
        } else {
            help->setVisible(true);
        }

        toolbar->setIconSize(size);

        #ifdef Q_OS_ANDROID
            closeTabletFileMenu();
        #endif
    } else {
        bool flag = true;
        if (h > w)
            flag = false;

        undoAction->setVisible(flag);
        redoAction->setVisible(flag);
        palette->setVisible(flag);

        #ifdef Q_OS_ANDROID
            closeFloatingMenu();
        #endif
    }

    if (drawMenu) {
        if (drawMenu->isVisible())
            drawMenu->close();
    }

    if (penMenu) {
        if (penMenu->isVisible())
            penMenu->close();
    }

    if (framesMenu) {
        if (framesMenu->isVisible())
            framesMenu->close();
    }
}

#if defined(Q_OS_ANDROID)
void TupMainWindow::closeFloatingMenu()
{
    if (isCellPhone) {
        if (toolsFlatMenu) {
            if (toolsFlatMenu->isVisible()) {
                toolsFlatMenu->close();
                return;
            }
        }

        if (toolsTopMenu) {
            if (toolsTopMenu->isVisible()) {
                toolsTopMenu->close();
                return;
            }
        }

        if (fileFlatMenu) {
            if (fileFlatMenu->isVisible()) {
                fileFlatMenu->close();
                return;
            }
        }

        if (fileTopMenu) {
            if (fileTopMenu->isVisible()) {
                fileTopMenu->close();
                return;
            }
        }
    }
}
#endif

void TupMainWindow::closeTabletFileMenu()
{
    #ifdef TUP_DEBUG
        qDebug() << "[TupMainWindow::closeTabletFileMenu()] - Tracing...";
    #endif

    if (!isCellPhone && tabletFileMenu) {
        if (tabletFileMenu->isVisible())
            tabletFileMenu->close();
    }
}

void TupMainWindow::closePenMenu()
{
    #ifdef TUP_DEBUG
        qDebug() << "[TupMainWindow::closePenMenu()] - Tracing...";
    #endif

    if (penMenu) {
        if (penMenu->isVisible()) {
            penMenu->close();
            return;
        }
    }
}

void TupMainWindow::keyPressEvent(QKeyEvent *event)
{
    #ifdef TUP_DEBUG
        qDebug() << "[TupMainWindow::keyPressEvent()] - key ->" << event->key();
        qDebug() << "[TupMainWindow::keyPressEvent()] - text ->" << event->text();
    #endif

#if !defined(Q_OS_ANDROID)
    if (event->key() == Qt::RightArrow) {
        goOneFrameForward();
    } else if (event->key() == Qt::LeftArrow) {
        goOneFrameBack();
    } else if (event->key() == Qt::Key_D) {
        cloneCurrentFrame();
    } else if (event->key() == Qt::Key_Insert) {
        insertNewFrameForward();
    } else if (event->modifiers() & Qt::ShiftModifier) {
        if (event->key() == Qt::Key_I)
            insertNewFrameForward();
    } else if (event->key() == Qt::Key_M) {
        framesDialog();
    } else if (event->modifiers() & Qt::ControlModifier) {
        if (event->key() == Qt::Key_C)
            copyCurrentFrame();
        else if (event->key() == Qt::Key_V)
            pasteFrameForward();
        else if (event->key() == Qt::Key_Z)
            undo();
        else if (event->key() == Qt::Key_Y)
            redo();
    } else if (event->key() == Qt::Key_X) {
        removeCurrentFrame();
    } else if (event->key() == Qt::Key_Space || event->key() == Qt::Key_Return) {
        if (!dockwidget->isVisible())
            setAnimationMode();
    } else if (event->key() == Qt::Key_Escape || event->key() == Qt::Key_Q) {
        if (mode == TupCanvas::Player)
            setAnimationMode();
        showExitDialog();
    }
    /* SQA: Code for "inside" artists
    } else if (event->key() == Qt::Key_F) {
        showFullScreen();
    }
    */
#else
    if (event->key() == Qt::Key_Back)
        QApplication::quit();
#endif
}

void TupMainWindow::showExitDialog()
{
    TupExitDialog *dialog = new TupExitDialog(config, this);
    connect(dialog, &QDialog::accepted, this, &TupMainWindow::executeExitAction);
    dialog->showMaximized();
}

void TupMainWindow::executeExitAction()
{
    int option = config->value("exitAction", 1).toInt();
    switch (option) {
        case TupExitDialog::SaveAndNew : // Save and set new Project
            saveAndSetNewProject();
        break;
        case TupExitDialog::Quit : // Exit App
            QApplication::quit();
        break;
        default:
        break;
    }
}

void TupMainWindow::loadAnimation()
{
    bool firstTime = config->value("firstTime", true).toBool();

    project = new QSettings("MaeFloresta", "CurrentProject");
    QString color = project->value("bgColor", "#ffffff").toString();
    bgColor = QColor(color);

    #ifdef TUP_DEBUG
        #if defined(Q_OS_ANDROID)
            qDebug() << "[TupMainWindow::loadAnimation()] - OS ->" << QSysInfo::productType();
            qDebug() << "---";
            logDialog->appendRecord("* OS: " + QSysInfo::productType());
            logDialog->appendRecord("---");
        #endif
    #endif

    if (firstTime) { // Running app for the first time
        #ifdef TUP_DEBUG
            QString msg = "[TupMainWindow::loadAnimation()] - Opening app for the first time!";
            qDebug() << msg;
            #if defined(Q_OS_ANDROID)
                logDialog->appendRecord(msg);
            #endif
        #endif

        if (TupProject::firstTime()) {
            if (TupProject::makeDirs()) {
                #ifdef TUP_DEBUG
                    QString msg = "[TupMainWindow::loadAnimation()] - App dirs were created successfully :)";
                    qDebug() << msg;
                    #if defined(Q_OS_ANDROID)
                        logDialog->appendRecord(msg);
                    #endif
                #endif
            } else {
                #ifdef TUP_DEBUG
                    QString msg = "[TupMainWindow::loadAnimation()] - Fatal Error: Couldn't create app dirs! :(";
                    qDebug() << msg;
                    #if defined(Q_OS_ANDROID)
                        logDialog->appendRecord(msg);
                    #endif
                #endif
            }

            projectsIndex = 0;
            currentProjectID = "project_00";
        } else {
            projectsIndex = TupProject::nextProjectID();
            QString prefix = "project_";
            if (projectsIndex < 10)
                prefix = "project_0";
            currentProjectID = prefix + QString::number(projectsIndex);
        }

        createColorPalettes();
        config->setValue("firstTime", false);

        /*
        formatVersion = TupNetHandler::Standard;
        if (QSysInfo::productType().startsWith("android", Qt::CaseInsensitive)) {
            int version = QSysInfo::productVersion().mid(0, 1).toInt();
            #ifdef TUP_DEBUG
                qDebug() << "* MAJOR RELEASE: " << version;
            #endif
            if (version >= 6)
                formatVersion = TupNetHandler::Other;
        }

        config->setValue("formatVersion", formatVersion);
        */

        project->setValue("isModified", true);

        TupFrame *frame = new TupFrame();
        frames << frame;
        QString data = frame->toString();
        project->setValue(frameKey, data);
        framesTotal = 1;
        fps = 12;
        onion = 50;

        project->setValue("framesTotal", framesTotal);
        project->setValue("fps", fps);

        project->setValue("bgColor", bgColor.name());
        project->setValue("title", tr("My Animation"));
        project->setValue("tags", tr("#tupitube #animation #fun"));
        project->setValue("comment", defaultComment);

        config->setValue("borderColor", "#000000");
        config->setValue("fillColor", "");
        config->setValue("bgColor", bgColor.name());
        config->setValue("penSize", 8);
        config->setValue("borderOpacity", 1.0);
        config->setValue("borderBrush", 1);
        config->setValue("fillOpacity", 0);
        config->setValue("fillBrush", 1);
        config->setValue("fillColor", "");
        config->setValue("onionSkin", onion);

        config->setValue("projectsIndex", projectsIndex);
        config->setValue("currentProjectID", currentProjectID);
    } else {
        #ifdef TUP_DEBUG 
            QString msg = "[TupMainWindow::loadAnimation()] - Opening app for the second time!";
            qDebug() << msg;
            #if defined(Q_OS_ANDROID)
                logDialog->appendRecord(msg);
            #endif
        #endif

        if (!TupProject::colorPaletteExist())
            createColorPalettes();

        // int index = config->value("formatVersion", 0).toInt();
        // formatVersion = TupNetHandler::AndroidVersion(index);
        framesTotal = project->value("framesTotal", 0).toInt();
        fps = project->value("fps", 12).toInt();
        config->setValue("fps", fps);
        onion = config->value("onionSkin", 50).toInt();

        projectsIndex = config->value("projectsIndex", 0).toInt();
        currentProjectID = config->value("currentProjectID", "project_00").toString();

        #ifdef TUP_DEBUG
            QString msg1 = "[TupMainWindow::loadAnimation()] - projectIndex -> " + QString::number(projectsIndex);
            QString msg2 = "[TupMainWindow::loadAnimation()] - currentProjectID -> " + currentProjectID;
            qDebug() << msg1;
            qDebug() << msg2;
            #if defined(Q_OS_ANDROID)
                logDialog->appendRecord(msg1);
                logDialog->appendRecord(msg2);
            #endif
        #endif

        loadFrames();
    }

    colorPalette = new QPixmap(TupProject::rootDir() + "/palette.png");
}

void TupMainWindow::loadFrames()
{
    frames.clear();

#ifdef TUP_DEBUG
    QString msg = "[TupMainWindow::loadFrames()] - Loading " + QString::number(framesTotal) + " project frames";
    qDebug() << msg;
    #if defined(Q_OS_ANDROID)
        logDialog->appendRecord(msg);
    #endif
#endif

    for(int i=0; i<framesTotal; i++) {
        QString frameKey = "frame_" + QString::number(i);
        QString input = project->value(frameKey, QString("")).toString();
        TupFrame *frame = new TupFrame();
        if (!input.isEmpty())
            frame->fromXml(input);

        frames << frame;
    }
}

void TupMainWindow::setToolBar()
{
#if !defined(Q_OS_ANDROID)
    QString helpLabel = tr("Help");
    QString drawLabel = tr("Drawing Tools");
    QString framesLabel = tr("Frame Tools");
    QString undoLabel = tr("Eraser");
    QString redoLabel = tr("Redo");
    QString colorLabel = tr("Colors");
    QString propertiesLabel = tr("Pen Properties");
    QString transparencyLabel = tr("Transparency");
    QString brushLabel = tr("Brushes");
    QString prevLabel = tr("Previous Frame");
    QString nextLabel = tr("Next Frame");
    QString worksLabel = tr("Projects");
    QString shareLabel = tr("Share");
    QString playLabel = tr("Play");
    QString settingsLabel = tr("Settings");
    QString fileLabel = tr("File Manager");
    QString exitLabel = tr("Exit");
#else
    QString helpLabel = "";
    QString drawLabel = "";
    QString framesLabel = "";
    QString undoLabel = "";
    QString redoLabel = "";
    QString colorLabel = "";
    QString propertiesLabel = "";
    // QString transparencyLabel = "";
    // QString brushLabel = "";
    QString prevLabel = "";
    QString nextLabel = "";
    QString playLabel = "";
    QString settingsLabel = "";
    QString fileLabel = "";
#endif

    QImage undoImg(imgPath + "undo.png");
    undoAction = new QAction(QIcon(QPixmap::fromImage(undoImg)), undoLabel, this);
    connect(undoAction, &QAction::triggered, this, &TupMainWindow::undo);

    QImage redoImg(imgPath + "redo.png");
    redoAction = new QAction(QIcon(QPixmap::fromImage(redoImg)), redoLabel, this);
    connect(redoAction, &QAction::triggered, this, &TupMainWindow::redo);

    QImage penImg(imgPath + "pen.png");
    penIcon = QIcon(QPixmap::fromImage(penImg));

    QImage rectImg(imgPath + "rectangle.png");
    rectangleIcon = QIcon(QPixmap::fromImage(rectImg));

    QImage ellipseImg(imgPath + "ellipse.png");
    ellipseIcon = QIcon(QPixmap::fromImage(ellipseImg));

    QImage triangleImg(imgPath + "triangle.png");
    triangleIcon = QIcon(QPixmap::fromImage(triangleImg));

    QImage colorsImg(imgPath + "palette.png");
    palette = new QAction(QIcon(QPixmap::fromImage(colorsImg)), colorLabel, this);
    connect(palette, &QAction::triggered, this, &TupMainWindow::showColorDialog);

    QImage backImg(imgPath + "back.png");
    getPreviousFrame = new QAction(QIcon(QPixmap::fromImage(backImg)), prevLabel, this);
    connect(getPreviousFrame, &QAction::triggered, this, &TupMainWindow::goOneFrameBack);
    getPreviousFrame->setEnabled(false);

    QImage fwdImg(imgPath + "forward.png");
    getNextFrame = new QAction(QIcon(QPixmap::fromImage(fwdImg)), nextLabel, this);
    connect(getNextFrame, &QAction::triggered, this, &TupMainWindow::goOneFrameForward);

    QImage playImg(imgPath + "play.png");
    play = new QAction(QIcon(QPixmap::fromImage(playImg)), playLabel, this);
    connect(play, &QAction::triggered, this, &TupMainWindow::setAnimationMode);

    toolbar = new QToolBar;
    toolbar->setMovable(false);
    toolbar->setFloatable(false);
    toolbar->setIconSize(QSize(iconSize, iconSize));

    QImage blankImage(imgPath + "blank_icon.png");
    QIcon blankIcon = QIcon(QPixmap::fromImage(blankImage));
    blankAction = new QAction;
    blankAction->setIcon(blankIcon);
    blankAction->setVisible(false);
    toolbar->addAction(blankAction);

    QWidget *space1 = new QWidget(this);
    space1->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    QWidget *space2 = new QWidget(this);
    space2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    if (isCellPhone) {
        toolsFlatMenu = new TupToolsFlatMenu(config);
        // SQA: Pending background feature
        connect(toolsFlatMenu, &TupToolsFlatMenu::callTool, this, &TupMainWindow::setCurrentTool);
        connect(toolsFlatMenu, &TupToolsFlatMenu::stroke, this, &TupMainWindow::showPenSizeDialog);
        connect(toolsFlatMenu, &TupToolsFlatMenu::opacity, this, &TupMainWindow::showOpacityDialog);
        connect(toolsFlatMenu, &TupToolsFlatMenu::brushes, this, &TupMainWindow::showBrushesDialog);
        connect(toolsFlatMenu, &TupToolsFlatMenu::cloneRequested, this, &TupMainWindow::cloneCurrentFrame);
        connect(toolsFlatMenu, &TupToolsFlatMenu::actions, this, &TupMainWindow::framesDialog);
        connect(toolsFlatMenu, &TupToolsFlatMenu::toolsMenuClosed, this, &TupMainWindow::enableToolbar);
        connect(toolsFlatMenu, &TupToolsFlatMenu::rejected, this, &TupMainWindow::enableToolbar);

        toolsTopMenu = new TupToolsTopMenu(config);
        connect(toolsTopMenu, &TupToolsTopMenu::callTool, this, &TupMainWindow::setCurrentTool);

        connect(toolsTopMenu, &TupToolsTopMenu::color, this, &TupMainWindow::showColorDialog);
        connect(toolsTopMenu, &TupToolsTopMenu::stroke, this, &TupMainWindow::showPenSizeDialog);
        connect(toolsTopMenu, &TupToolsTopMenu::opacity, this, &TupMainWindow::showOpacityDialog);
        connect(toolsTopMenu, &TupToolsTopMenu::brushes, this, &TupMainWindow::showBrushesDialog);
        connect(toolsTopMenu, &TupToolsTopMenu::cloneRequested, this, &TupMainWindow::cloneCurrentFrame);
        connect(toolsTopMenu, &TupToolsTopMenu::undo, this, &TupMainWindow::undo);
        connect(toolsTopMenu, &TupToolsTopMenu::redo, this, &TupMainWindow::redo);
        connect(toolsTopMenu, &TupToolsTopMenu::actions, this, &TupMainWindow::framesDialog);
        connect(toolsTopMenu, &TupToolsTopMenu::toolsMenuClosed, this, &TupMainWindow::enableToolbar);
        connect(toolsTopMenu, &TupToolsTopMenu::rejected, this, &TupMainWindow::enableToolbar);

        fileFlatMenu = new TupFileFlatMenu(config);
        connect(fileFlatMenu, &TupFileFlatMenu::postRequested, this, &TupMainWindow::shareWork);
        connect(fileFlatMenu, &TupFileFlatMenu::settings, this, &TupMainWindow::showSettingsDialog);
        connect(fileFlatMenu, &TupFileFlatMenu::newProject, this, &TupMainWindow::saveAndSetNewProject);
        connect(fileFlatMenu, &TupFileFlatMenu::works, this, &TupMainWindow::openWorksGallery);
        connect(fileFlatMenu, &TupFileFlatMenu::tupFile, this, &TupMainWindow::saveTupFile);
        connect(fileFlatMenu, &TupFileFlatMenu::help, this, &TupMainWindow::showHelpDialog);
        connect(fileFlatMenu, &TupFileFlatMenu::fileMenuClosed, this, &TupMainWindow::enableToolbar);
        connect(fileFlatMenu, &TupFileFlatMenu::rejected, this, &TupMainWindow::enableToolbar);

#if defined(Q_OS_ANDROID) && defined(TUP_DEBUG)
        connect(fileFlatMenu, &TupFileFlatMenu::openLog, this, &TupMainWindow::openLogFile);
#endif

        fileTopMenu = new TupFileTopMenu(config);
        // connect(fileTopMenu, &TupFileTopMenu::postRequested, this, &TupMainWindow::metadataDialog);
        connect(fileTopMenu, &TupFileTopMenu::postRequested, this, &TupMainWindow::shareWork);
        connect(fileTopMenu, &TupFileTopMenu::settings, this, &TupMainWindow::showSettingsDialog);
        connect(fileTopMenu, &TupFileTopMenu::newProject, this, &TupMainWindow::saveAndSetNewProject);
        connect(fileTopMenu, &TupFileTopMenu::works, this, &TupMainWindow::openWorksGallery);
        connect(fileTopMenu, &TupFileTopMenu::tupFile, this, &TupMainWindow::saveTupFile);
        connect(fileTopMenu, &TupFileTopMenu::help, this, &TupMainWindow::showHelpDialog);
        connect(fileTopMenu, &TupFileTopMenu::fileMenuClosed, this, &TupMainWindow::enableToolbar);
        connect(fileTopMenu, &TupFileTopMenu::rejected, this, &TupMainWindow::enableToolbar);

#if defined(Q_OS_ANDROID) && defined(TUP_DEBUG)
        connect(fileTopMenu, &TupFileTopMenu::openLog, this, &TupMainWindow::openLogFile);
#endif

        toolbar->setStyleSheet("* { spacing: " + barSpacing + "px; border-style: none; }");
        toolbar->addWidget(space1);

        QImage menuImg(imgPath + "tools.png");
        miniMenu = new QAction(QIcon(QPixmap::fromImage(menuImg)), "", this);
        connect(miniMenu, &QAction::triggered, this, &TupMainWindow::showToolsMenu);

        fileActions = new QAction(fileIcon, fileLabel, this);
        connect(fileActions, &QAction::triggered, this, &TupMainWindow::showFileMenu);

        toolbar->addAction(miniMenu);
        toolbar->addAction(undoAction);
        toolbar->addAction(redoAction);
        toolbar->addAction(palette);
        toolbar->addAction(getPreviousFrame);
        toolbar->addAction(getNextFrame);
        toolbar->addAction(play);
        toolbar->addAction(fileActions);
        toolbar->addWidget(space2);
    } else { // Toolbar for Tablets
        drawToolsAction = new QAction(penIcon, drawLabel, this);
        connect(drawToolsAction, &QAction::triggered, this, &TupMainWindow::showDrawingToolsDialog);

        QImage helpImg(imgPath + "help.png");
        help = new QAction(QIcon(QPixmap::fromImage(helpImg)), helpLabel, this);
        connect(help, &QAction::triggered, this, &TupMainWindow::showHelpDialog);

        QImage thickImg(imgPath + "thickness.png");
        penPropertiesAction = new QAction(QIcon(QPixmap::fromImage(thickImg)), propertiesLabel, this);
        connect(penPropertiesAction, &QAction::triggered, this, &TupMainWindow::penPropertiesDialog);

        /*
        QImage brushImg(imgPath + "brush.png");
        brush = new QAction(QIcon(QPixmap::fromImage(brushImg)), brushLabel, this);
        connect(brush, &QAction::triggered, this, &TupMainWindow::showBrushesDialog);

        QImage opacityImg(imgPath + "opacity.png");
        opacity = new QAction(QIcon(QPixmap::fromImage(opacityImg)), transparencyLabel, this);
        connect(opacity, &QAction::triggered, this, &TupMainWindow::showOpacityDialog);
        */

        QImage toolsImg(imgPath + "frames.png");
        frameActions = new QAction(QIcon(QPixmap::fromImage(toolsImg)), framesLabel, this);
        connect(frameActions, &QAction::triggered, this, &TupMainWindow::framesDialog);

        QImage settingsImage(imgPath + "settings.png");
        settings = new QAction(QIcon(QPixmap::fromImage(settingsImage)), settingsLabel, this);
        connect(settings, &QAction::triggered, this, &TupMainWindow::showSettingsDialog);

        fileActions = new QAction(fileIcon, fileLabel, this);
        connect(fileActions, &QAction::triggered, this, &TupMainWindow::showTabletFileMenu);

        tabletFileMenu = new TupTabletFileMenu(config);
        connect(tabletFileMenu, &TupTabletFileMenu::postRequested, this, &TupMainWindow::shareWork);
        connect(tabletFileMenu, &TupTabletFileMenu::newProject, this, &TupMainWindow::saveAndSetNewProject);
        connect(tabletFileMenu, &TupTabletFileMenu::works, this, &TupMainWindow::openWorksGallery);
        connect(tabletFileMenu, &TupTabletFileMenu::tupFile, this, &TupMainWindow::saveTupFile);

        #if defined(Q_OS_ANDROID) && defined(TUP_DEBUG)
            connect(tabletFileMenu, &TupTabletFileMenu::openLog, this, &TupMainWindow::openLogFile);
        #endif

        connect(tabletFileMenu, &TupTabletFileMenu::fileMenuClosed, this, &TupMainWindow::enableToolbar);
        connect(tabletFileMenu, &TupTabletFileMenu::rejected, this, &TupMainWindow::enableToolbar);

        toolbar->setStyleSheet("* { spacing: " + barSpacing + "px; padding: 5px; border-style: none; }");

        toolbar->addWidget(space1);
        toolbar->addAction(help);
        toolbar->addAction(undoAction);
        toolbar->addAction(redoAction);
        toolbar->addAction(drawToolsAction);
        toolbar->addAction(palette);
        toolbar->addAction(penPropertiesAction);
        toolbar->addAction(frameActions);
        toolbar->addAction(getPreviousFrame);
        toolbar->addAction(getNextFrame);
        toolbar->addAction(play);
        toolbar->addAction(settings);
        toolbar->addAction(fileActions);

        #if !defined(Q_OS_ANDROID)
            if (config->value("width").toInt() > 800)
                toolbar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

            QImage exitImg(imgPath + "exit.png");
            exit = new QAction(QIcon(QPixmap::fromImage(exitImg)), exitLabel, this);
            connect(exit, &QAction::triggered, this, &TupMainWindow::showExitDialog);
            toolbar->addAction(exit);
        #endif

        toolbar->addWidget(space2);        
    }

    drawMenu = new TupDrawToolsMenu(config, this);
    connect(drawMenu, &TupDrawToolsMenu::callTool, this, &TupMainWindow::setCurrentTool);
    connect(drawMenu, &TupDrawToolsMenu::drawMenuClosed, this, &TupMainWindow::enableToolbar);
    connect(drawMenu, &TupDrawToolsMenu::rejected, this, &TupMainWindow::enableToolbar);

    penMenu = new TupPenPropertiesMenu(config, this);
    connect(penMenu, &TupPenPropertiesMenu::thicknessClicked, this, &TupMainWindow::showPenSizeDialog);
    connect(penMenu, &TupPenPropertiesMenu::transparencyClicked, this, &TupMainWindow::showOpacityDialog);
    connect(penMenu, &TupPenPropertiesMenu::brushesClicked, this, &TupMainWindow::showBrushesDialog);
    connect(penMenu, &TupPenPropertiesMenu::propertiesMenuClosed, this, &TupMainWindow::enableToolbar);
    connect(penMenu, &TupPenPropertiesMenu::rejected, this, &TupMainWindow::enableToolbar);

    framesMenu = new TupFramesMenu(config, this);
    connect(framesMenu, &TupFramesMenu::frameAddedForward, this, &TupMainWindow::insertNewFrameForward);
    connect(framesMenu, &TupFramesMenu::frameAddedBack, this, &TupMainWindow::insertNewFrameBack);

    if (!isCellPhone) {
        connect(framesMenu, &TupFramesMenu::frameCloned, this, &TupMainWindow::cloneCurrentFrame);
        connect(framesMenu, &TupFramesMenu::leftArrowClicked, this, &TupMainWindow::goOneFrameBack);
        connect(framesMenu, &TupFramesMenu::rightArrowClicked, this, &TupMainWindow::goOneFrameForward);
        connect(framesMenu, &TupFramesMenu::undoRequested, this, &TupMainWindow::undo);
        connect(framesMenu, &TupFramesMenu::redoRequested, this, &TupMainWindow::redo);
    }

    connect(framesMenu, &TupFramesMenu::frameRemoved, this, &TupMainWindow::removeCurrentFrame);
    connect(framesMenu, &TupFramesMenu::frameCleared, this, &TupMainWindow::clearCanvas);
    connect(framesMenu, &TupFramesMenu::fileMenuClosed, this, &TupMainWindow::enableToolbar);
    connect(framesMenu, &TupFramesMenu::rejected, this, &TupMainWindow::enableToolbar);

    addToolBar(Qt::BottomToolBarArea, toolbar);
}

void TupMainWindow::welcomeMsg()
{
    if (!frames.isEmpty()) {
        TupFrame *frame = frames.at(0);
        if (frame) {
            if (frame->isEmpty())
                canvas->notify(TupCanvas::Info, tr("Animation Frame No. 1"), 1000);
        }
    }
}

// Post Widget
void TupMainWindow::setMetadataWidget()
{
    QString title = project->value("title").toString();
    QString tags = project->value("tags").toString();
    QString comment = project->value("comment").toString();

    dockwidget = new QDockWidget;
    dockwidget->setTitleBarWidget(new QWidget);

    QFont font = this->font();
    font.setBold(false);
    font.setPointSize(textSize);

    QString style = "* { background-color: rgb(255, 255, 255); }";

    postHeader = new QLabel(tr("Share"));
    font.setBold(true);
    postHeader->setFont(font);
    postHeader->setAlignment(Qt::AlignHCenter);
    postHeader->setStyleSheet("QWidget { background-color: rgb(180,180,180); "
                              "border-radius: " + borderRadius + "px; }");

    // Mode Panel (Anonymous/User)

    QString btnStyle = "QWidget { background-color: rgb(170,200,170); ";
    btnStyle += "padding: " + QString::number(font.pointSize()) + "px; ";
    btnStyle += "border-radius: " + borderRadius + "px; }";

    TupButton *anonymousButton = new TupButton("", iconWidth, tr("Anonymous"),
                                   textSize, false, Qt::Horizontal, this);
    anonymousButton->setStyleSheet(btnStyle);
    connect(anonymousButton, &TupButton::clicked, this, &TupMainWindow::postAnonymousWork);

    TupButton *signButton = new TupButton("", iconWidth, tr("Login"),
                                   textSize, false, Qt::Horizontal, this);
    signButton->setStyleSheet(btnStyle);
    connect(signButton, &TupButton::clicked, this, &TupMainWindow::processPostRequest);

    QHBoxLayout *modeLayout = new QHBoxLayout;
    modeLayout->addWidget(anonymousButton);
    modeLayout->addWidget(signButton);

    modePanel = new QWidget;
    modePanel->setStyleSheet("QWidget { background-color: rgb(220,220,220); "
                             "border-radius: " + borderRadius + "px; }");
    modePanel->setLayout(modeLayout);
    modePanel->setVisible(false);

    // Sign Up Panel

    font.setBold(false);
    QLabel *emailLabel = new QLabel(tr("Email: "));
    emailLabel->setFont(font);
    emailEdit = new QLineEdit();
    connect(emailEdit, SIGNAL(returnPressed()), this, SLOT(formatEmail()));
    emailEdit->setFixedWidth(static_cast<int>(0.8 * config->value("height").toInt()));
    emailEdit->setStyleSheet(style);
    emailEdit->setFont(font);

#if defined(Q_OS_ANDROID)
    QVBoxLayout *emailLayout = new QVBoxLayout;
#else
    QHBoxLayout *emailLayout = new QHBoxLayout;
#endif

    emailLayout->addWidget(emailLabel);
    emailLayout->addWidget(emailEdit);

    signUpPanel = new QWidget;
    signUpPanel->setStyleSheet("QWidget { background-color: rgb(220,220,220); "
                                "border-radius: " + borderRadius + "px; }");
    signUpPanel->setLayout(emailLayout);
    signUpPanel->setVisible(false);

    // Login Panel

    QLabel *usernameLabel = new QLabel(tr("Username: "));
    usernameLabel->setFont(font);
    usernameEdit = new QLineEdit();
    usernameEdit->setStyleSheet(style);
    usernameEdit->setFont(font);

    QLabel *passwdLabel = new QLabel(tr("Password: "));
    passwdLabel->setFont(font);
    passwdEdit = new QLineEdit();
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

    QVBoxLayout *loginLayout = new QVBoxLayout;
    int space = font.pointSize();
    loginLayout->setContentsMargins(space, space, space, space);
    loginLayout->addLayout(usernameLayout);
    loginLayout->addSpacing(space);
    loginLayout->addLayout(passwdLayout);

    loginPanel = new QWidget;
    loginPanel->setStyleSheet("QWidget { background-color: rgb(220,220,220); "
                              "border-radius: " + borderRadius + "px; }");
    loginPanel->setLayout(loginLayout);
    loginPanel->setVisible(false);

    // Post Panel

    formStatus = Post;

    QLabel *titleLabel = new QLabel(tr("Title: "));
    titleLabel->setFont(font);
    titleEdit = new QLineEdit(title);
    titleEdit->setStyleSheet(style);

    titleEdit->setFont(font);
    QHBoxLayout *titleLayout = new QHBoxLayout;
    titleLayout->addWidget(titleLabel);
    titleLayout->addWidget(titleEdit);

    QLabel *tagsLabel = new QLabel(tr("Tags: "));
    tagsLabel->setFont(font);

    tagsEdit = new QPlainTextEdit(tags);
    tagsEdit->setStyleSheet(style);
    tagsEdit->setFont(font);
    QHBoxLayout *tagsLabelLayout = new QHBoxLayout;
    tagsLabelLayout->addWidget(tagsLabel);

    QHBoxLayout *tagsFieldLayout = new QHBoxLayout;
    tagsFieldLayout->addWidget(tagsEdit);

    QLabel *commentLabel = new QLabel(tr("Comment: "));
    commentLabel->setFont(font);
    QHBoxLayout *descLayout = new QHBoxLayout;
    descLayout->addWidget(commentLabel);

    editor = new QPlainTextEdit(comment);
    editor->setStyleSheet(style);
    editor->setFont(font);
    QHBoxLayout *editorLayout = new QHBoxLayout;
    editorLayout->addWidget(editor); 

    shareButton = new TupButton(imgPath + "share.png", iconWidth);
    connect(shareButton, &TupButton::clicked, this, &TupMainWindow::processPostRequest);

    TupButton *closeButton = new TupButton(imgPath + "close.png", iconWidth);
    connect(closeButton, &TupButton::clicked, this, &TupMainWindow::handleMetadataDialog);

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(closeButton);
    buttonLayout->addWidget(shareButton);

    QVBoxLayout *bottomLayout = new QVBoxLayout;
    bottomLayout->addLayout(buttonLayout);
    bottomLayout->setAlignment(buttonLayout, Qt::AlignRight);

    QVBoxLayout *postLayout = new QVBoxLayout;
    postLayout->setContentsMargins(space, space, space, space);
    postLayout->addLayout(titleLayout);
    postLayout->addLayout(tagsLabelLayout);
    postLayout->addLayout(tagsFieldLayout);
    postLayout->addLayout(descLayout);
    postLayout->addLayout(editorLayout);

    postPanel = new QWidget;
    postPanel->setStyleSheet("QWidget { background-color: rgb(220,220,220); "
                             "border-radius: " + borderRadius + "px; }");
    postPanel->setLayout(postLayout);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    space = static_cast<int> (iconWidth * 0.5);
    mainLayout->setContentsMargins(space, space, space, space);
    mainLayout->addWidget(postHeader);
    mainLayout->addWidget(modePanel);
    mainLayout->addWidget(signUpPanel);
    mainLayout->addWidget(loginPanel);
    mainLayout->addWidget(postPanel);
    mainLayout->addLayout(bottomLayout);

    mainLayout->setAlignment(bottomLayout, Qt::AlignRight);
    mainLayout->addStretch();

    QHBoxLayout *layout = new QHBoxLayout;

    TupSpace *space1 = new TupSpace;
    connect(space1, &TupSpace::clicked, this, &TupMainWindow::handleMetadataDialog);
    space1->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    TupSpace *space2 = new TupSpace;
    connect(space2, &TupSpace::clicked, this, &TupMainWindow::handleMetadataDialog);
    space2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QWidget *center = new QWidget;
    center->setStyleSheet("QWidget { background-color: rgb(220,220,220); "
                          "border-radius: " + borderRadius + "px; }");
    center->setLayout(mainLayout);

    layout->addWidget(space1);
    layout->addWidget(center);
    layout->addWidget(space2);

    QWidget *global = new QWidget;
    global->setLayout(layout);

#if defined(Q_OS_ANDROID)
    dockwidget->setStyleSheet("* { background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
                                   "stop:0 rgb(100, 100, 100), stop:1 rgb(30, 30, 30)); }");
#else
    dockwidget->setStyleSheet("* { background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
                              "stop:0 rgb(255, 255, 255), stop:1 rgb(100, 100, 100)); }");
#endif

    dockwidget->setFeatures(QDockWidget::NoDockWidgetFeatures);
    dockwidget->setWidget(global);

    addDockWidget(Qt::TopDockWidgetArea, dockwidget);
    dockwidget->hide();
}

void TupMainWindow::postAnonymousWork()
{
    postMode = Anonymous;
    postWork();
}

void TupMainWindow::formatEmail()
{
    QString input = emailEdit->text();
    input = input.trimmed();
    emailEdit->setText(input.toLower());
}

void TupMainWindow::updateMetadata()
{
    QString titleStr = titleEdit->text();
    if (titleStr.length() == 0) {
        titleStr = tr("My Animation");
        if (framesTotal == 1)
            titleStr = tr("My Sketch");
    } else {
        if (titleStr.length() > 50)
            titleStr = titleStr.left(50);
    }

    QString tagStr = tagsEdit->toPlainText();
    if (tagStr.length() == 0) {
        tagStr = tr("#tupitube #animation #fun");
        if (framesTotal == 1)
            tagStr = tr("#tupitube #drawing #fun");
    } else {
        if (tagStr.length() > 50)
            tagStr = tagStr.left(50);
    }

    QString commentStr = editor->toPlainText();
    if (commentStr.length() == 0) {
        commentStr = defaultComment;
    } else {
        if (commentStr.length() > 500)
            commentStr = commentStr.left(500);
    }

    project->setValue("title", titleStr);
    project->setValue("tags", tagStr);
    project->setValue("comment", commentStr);
}

void TupMainWindow::setScene()
{
    screenSize = QSize(width(), height() - toolbar->height());

    QRectF rect = QRectF(QPointF(0, 0), screenSize);
    scene = new QGraphicsScene;
    scene->setBackgroundBrush(bgColor);
    scene->setSceneRect(rect);
}

void TupMainWindow::setCanvas()
{
    setScene();

    canvas = new TupCanvas(scene, config, bgColor, getFillColor(), pen, fps);
    canvas->setRenderHints(QPainter::Antialiasing);
    connect(canvas, &TupCanvas::saveFrame, this, &TupMainWindow::saveFrame);
    connect(canvas, &TupCanvas::enableUI, this, &TupMainWindow::enableUI);
    connect(canvas, &TupCanvas::updateButtonState, this, &TupMainWindow::enableButton);
    connect(canvas, &TupCanvas::leftArrowClicked, this, &TupMainWindow::goOneFrameBack);
    connect(canvas, &TupCanvas::rightArrowClicked, this, &TupMainWindow::goOneFrameForward);

    setCentralWidget(canvas);

    if (mode == TupCanvas::Frames) {
        loadCurrentFrame();
    } else {
        canvas->setMode(mode);
        canvas->renderProject(frames);
        canvas->playAnimation();
    }
}

void TupMainWindow::setCurrentTool(Tool tool)
{
    if (!isCellPhone) {
        switch(tool) {
            case PenTool:
                drawToolsAction->setIcon(penIcon);
            break;
            case RectangleTool:
                drawToolsAction->setIcon(rectangleIcon);
            break;
            case EllipseTool:
                drawToolsAction->setIcon(ellipseIcon);
            break;
            case TriangleTool:
                drawToolsAction->setIcon(triangleIcon);
            break;
        }
    }

    canvas->setCurrentTool(tool);
}

void TupMainWindow::loadCurrentFrame()
{
    if (framesTotal > 0) {  
        setComplementaryFrames();

        TupFrame *currentFrame = frames.at(currentFrameIndex);
        canvas->loadFrame(previousFrame, currentFrame, nextFrame,
                          QString::number(currentFrameIndex + 1) + "/" + QString::number(framesTotal));

        checkUndoRedoStatus(currentFrame);
    }
}

/* SQA: Backgrounds dialog feature
void TupMainWindow::backgroundsDialog()
{
#if defined(Q_OS_ANDROID)
    closeFloatingMenu();
#endif

    TupBgDialog *dialog = new TupBgDialog(config, this);
    connect(dialog, &TupBgDialog::bgSelected, this, &TupMainWindow::setCanvasBg);
    dialog->showMaximized();
}
*/

void TupMainWindow::setCanvasBg(const QString &bgId)
{
    Q_UNUSED(bgId)
}

void TupMainWindow::setToolbarVisible(bool enabled)
{
    #ifdef TUP_DEBUG
        qDebug() << "[TupMainWindow::setToolbarVisible()] - enable ->" << enabled;
    #endif

    QString bg = QString::number(bgColor.red()) + ","
               + QString::number(bgColor.green()) + ","
               + QString::number(bgColor.blue());
    if (enabled)
        bg = "232,232,232";

    setStyleSheet("* { background-color: rgb(" + bg + "); border-style: none; }");
    blankAction->setVisible(!enabled);

    QScreen *screen = QGuiApplication::screens().at(0);
    int width = screen->geometry().width();
    int height = screen->geometry().height();

    if (isCellPhone) {
        if (width > height) {
            undoAction->setVisible(enabled);
            redoAction->setVisible(enabled);
            palette->setVisible(enabled);
        }

        miniMenu->setVisible(enabled);
        getPreviousFrame->setVisible(enabled);
        getNextFrame->setVisible(enabled);
        play->setVisible(enabled);
        fileActions->setVisible(enabled);
    } else {
        #if defined(Q_OS_ANDROID)
            if (width > height) {
                help->setVisible(enabled);
            } else {
                if (diagonal >= 7)
                    help->setVisible(enabled);
                else if (!enabled)
                    help->setVisible(enabled);
            }
        #else
            help->setVisible(enabled);
            exit->setVisible(enabled);
        #endif

        undoAction->setVisible(enabled);
        redoAction->setVisible(enabled);
        drawToolsAction->setVisible(enabled);
        palette->setVisible(enabled);
        penPropertiesAction->setVisible(enabled);
        // opacity->setVisible(enabled);
        // brush->setVisible(enabled);
        frameActions->setVisible(enabled);
        getPreviousFrame->setVisible(enabled);
        getNextFrame->setVisible(enabled);
        play->setVisible(enabled);
        settings->setVisible(enabled);
        fileActions->setVisible(enabled);
    }
}

void TupMainWindow::showDrawingToolsDialog()
{
    #ifdef TUP_DEBUG
        qDebug() << "[TupMainWindow::showDrawingToolsDialog()] - Tracing...";
    #endif

#if defined(Q_OS_ANDROID)
    closeFloatingMenu();
#endif

    setToolbarVisible(false);

    QScreen *screen = QGuiApplication::screens().at(0);
    int width = screen->geometry().width();
    int height = screen->geometry().height();

    drawMenu->show();
    drawMenu->move(static_cast<int> ((width - drawMenu->width()) / 2),
                   static_cast<int> ((height - drawMenu->height())));
}

void TupMainWindow::penPropertiesDialog()
{
    #ifdef TUP_DEBUG
        qDebug() << "[TupMainWindow::penPropertiesDialog()] - Tracing...";
    #endif

#if defined(Q_OS_ANDROID)
    closeFloatingMenu();
#endif

    setToolbarVisible(false);

    QScreen *screen = QGuiApplication::screens().at(0);
    int width = screen->geometry().width();
    int height = screen->geometry().height();

    penMenu->show();
    penMenu->move(static_cast<int> ((width - penMenu->width()) / 2),
                     static_cast<int> ((height - penMenu->height())));
}

void TupMainWindow::framesDialog()
{
    #ifdef TUP_DEBUG
        qDebug() << "[TupMainWindow::framesDialog()] - Tracing...";
    #endif

    #if defined(Q_OS_ANDROID)
        closeFloatingMenu();
    #endif

    setToolbarVisible(false);

    QScreen *screen = QGuiApplication::screens().at(0);
    int width = screen->geometry().width();
    int height = screen->geometry().height();

    framesMenu->show();
    framesMenu->move(static_cast<int> ((width - framesMenu->width()) / 2),
                     static_cast<int> ((height - framesMenu->height())));
}

void TupMainWindow::insertNewFrameForward()
{
    addNewFrameForward(new TupFrame(), false);
}

void TupMainWindow::openWorksGallery()
{
#if defined(Q_OS_ANDROID)
    closeFloatingMenu();
    if (!isCellPhone)
        closeTabletFileMenu();
#endif

    // SQA: This dialog should be instanced just once
    TupGalleryDialog *dialog = new TupGalleryDialog(config, this);
    connect(dialog, &TupGalleryDialog::openCall, this, &TupMainWindow::openProject);
    // SQA: Pending for development
    // connect(dialog, &TupGalleryDialog::shareCall, this, &TupMainWindow::shareProject);
    connect(dialog, &TupGalleryDialog::remove, this, &TupMainWindow::removeProject);
    dialog->showMaximized();
}

void TupMainWindow::openProject(const QString &id)
{
    #ifdef TUP_DEBUG
        qDebug() << "[TupMainWindow::openProject()] - Opening project ->" << id;
    #endif

#if defined(Q_OS_ANDROID)
    closeFloatingMenu();
#else
    closeTabletFileMenu();
    enableToolbar();
#endif

    if (currentProjectID.compare(id) == 0) {
        #ifdef TUP_DEBUG
            qDebug() << "[TupMainWindow::openProject()] - Warning: Project is already opened ->" << id;
        #endif

        return;
    }

    savePreviousProject(true);

    TupProject *projectFile = new TupProject(id);
    if (projectFile->loadIsOk()) {
        config->setValue("currentProjectID", id);
        project = projectFile->parameters();
        QString color = project->value("bgColor", "#ffffff").toString();
        config->setValue("bgColor", color);

        foreach (QString tag, colorHistoryList) {
            qDebug() << "[TupMainWindow::openProject()] - Loading project history colors ->" << project->value(tag, tr("No Colors"));
            config->setValue(tag, project->value(tag, tr("No Colors")));
        }

        colorDialog->loadHistory();

        bgColor = QColor(color);
        fps = project->value("fps", 12).toInt();
        config->setValue("fps", fps);
        canvas->updateFPS(fps);

        currentProjectID = id;
        currentFrameIndex = 0;

        framesTotal = project->value("framesTotal", 1).toInt();
        loadFrames();

        setUndoRedoStatus(false);
        canvas->fullReset(bgColor);
        updateColors(Qt::black, Qt::transparent, bgColor);

        loadCurrentFrame();
    } else {
        #ifdef TUP_DEBUG
            qDebug() << "[TupMainWindow::openProject()] - Fatal error: can't load project ->" << id;
        #endif
        canvas->notify(TupCanvas::Error, tr("Sorry, project can't be opened :("), 2000);
    }
}

/*
void TupMainWindow::shareProject(const QString &id)
{
#ifdef TUP_DEBUG
    qDebug() << "TupMainWindow::shareProject() - Sharing project -> " << id;
#endif
}
*/

void TupMainWindow::savePreviousProject(bool openAction)
{
    #ifdef TUP_DEBUG
        qDebug() << "[TupMainWindow::savePreviousProject()] - action flag ->" << openAction;
    #endif

    if (framesTotal > 1 || !canvas->isEmpty()) {
        int index = 0;
        bool found = false;
        for (int i=0; i < frames.count(); i++) {
             if (!frames.at(i)->isEmpty()) {
                 found = true;
                 break;
             }
             index++;
        }

        if (found) {
            foreach (QString tag, colorHistoryList) {
                project->setValue(tag, config->value(tag));
                config->setValue(tag, "");
            }

            project->setValue("bgColor", bgColor);
            project->setValue("title", titleEdit->text());
            project->setValue("tags", tagsEdit->toPlainText());
            project->setValue("comment", editor->toPlainText());

            goToFrame(index);
            int portraitWidth = config->value("galleryPortraitWidth").toInt();
            int landscapeWidth = config->value("galleryLandscapeWidth").toInt();
			
			QPixmap pixmap = canvas->renderFrame(frames.at(index));
			if (!pixmap.isNull()) {
			    int imgWidth = portraitWidth;
                if (pixmap.width() > pixmap.height())
                    imgWidth = landscapeWidth;
			
                if (TupProject::save(currentProjectID, project, pixmap.scaledToWidth(imgWidth, Qt::SmoothTransformation))) {
                    #ifdef Q_OS_ANDROID
                        if (!openAction)
                            canvas->notify(TupCanvas::Info, tr("Previous project saved & stored! :)"), 1000);
                    #endif
                } else {
                    #ifdef Q_OS_ANDROID
                        canvas->notify(TupCanvas::Error, tr("Can't save project! :("), 3000);
                        #ifdef TUP_DEBUG
                            logDialog->appendRecord("Can't save project! :(");
                        #endif
                    #endif
                }
			} else {
                #ifdef TUP_DEBUG
                    qDebug() << "[TupMainWindow::savePreviousProject()] - Fatal error: can't load rendered frame from project ->"
                             << currentProjectID;
                #endif				
			}
        } else {
            #ifdef TUP_DEBUG
                qDebug() << "[TupMainWindow::savePreviousProject()] - Warning: previous project is empty. Not saved.";
            #endif
        }
    }
}

void TupMainWindow::removeProject(const QString &id)
{ 
    Q_UNUSED(id)

    if (currentProjectID.compare(id) == 0)
        resetProjectUI();
}

void TupMainWindow::showSettingsDialog()
{
    settingsDialog->showMaximized();
}

void TupMainWindow::updateSettings()
{
#if defined(Q_OS_ANDROID)
    closeFloatingMenu();
#endif

    canvas->updateSmoothness(getSmoothLevel());

    onion = config->value("onionSkin", 50).toInt();
    canvas->updateOnionValue(onion);
    canvas->updateFramesVisibility(getPreviousFrameVisibility(),
                                   getNextFrameVisibility());

    int newFPS = getFPS();
    if (fps != newFPS) {
        project->setValue("isModified", true);
        fps = newFPS;
        project->setValue("fps", fps);
        canvas->updateFPS(fps);
    }

    bool isTablet = config->value("isTablet").toBool();
    if (isTablet) {
         bool restart = config->value("restart").toBool();
         if (restart) {
             config->sync();
             QApplication::quit();
         }
    }
}

void TupMainWindow::handleMetadataDialog()
{
    if (dockwidget->isVisible()) { // Hide Post panel
        #ifdef TUP_DEBUG
            qDebug() << "[TupMainWindow::metadataDialog()] - formStatus ->" << formStatus;
        #endif

        postHeader->setText(tr("Share"));
        loginPanel->setVisible(false);
        signUpPanel->setVisible(false);
        modePanel->setVisible(false);
        postPanel->setVisible(true);

        formStatus = Post;

        canvas->disableScreen(false);
        dockwidget->hide();
        toolbar->setVisible(true);
        formStatus = Post;
    } else { // Show Post panel
        #if defined(Q_OS_ANDROID)
            closeFloatingMenu();
            if (!isCellPhone)
                closeTabletFileMenu();
        #endif

        checkTitle();
        checkTags();

        canvas->disableScreen(true);
        toolbar->setVisible(false);

        formStatus = Post;
        postHeader->setText(tr("Share"));
        loginPanel->setVisible(false);
        signUpPanel->setVisible(false);
        modePanel->setVisible(false);
        shareButton->setVisible(true);
        postPanel->setVisible(true);

        dockwidget->show();
    }
}

void TupMainWindow::checkTitle()
{
    QString titleStr = titleEdit->text();
    if (titleStr.compare(tr("My Animation")) == 0) {
        if (framesTotal == 1) {
            titleStr = tr("My Sketch");
            titleEdit->setText(titleStr);
            project->setValue("title", titleStr);
        }
    } else if (titleStr.compare(tr("My Sketch")) == 0) {
        if (framesTotal > 1) {
            titleStr = tr("My Animation");
            titleEdit->setText(titleStr);
            project->setValue("title", titleStr);
        }
    }
}

void TupMainWindow::checkTags()
{
    QString tagsStr = tagsEdit->toPlainText();
    if (tagsStr.compare(tr("#tupitube #animation #fun")) == 0) {
        if (framesTotal == 1) {
            tagsStr = tr("#tupitube #drawing #fun");
            tagsEdit->setPlainText(tagsStr);
            project->setValue("tags", tagsStr);
        }
    }

    if (tagsStr.compare(tr("#tupitube #drawing #fun")) == 0) {
        if (framesTotal > 1) {
            tagsStr = tr("#tupitube #animation #fun");
            tagsEdit->setPlainText(tagsStr);
            project->setValue("tags", tagsStr);
        }
    }
}

void TupMainWindow::handleEmptyProject()
{
    if (dockwidget->isVisible()) {
        updateMetadata();
        dockwidget->hide();
    }

    if (!toolbar->isVisible())
        toolbar->setVisible(true);

    canvas->notify(TupCanvas::Warning, tr("Please, draw something first! ;)"), 2000);
    #ifdef TUP_DEBUG
        qDebug() << "[TupMainWindow::handleEmptyProject()] - Warning: Canvas is empty. Please, draw something! ;)";
    #endif
}

void TupMainWindow::postWork()
{
    #ifdef TUP_DEBUG
        qDebug() << "[TupMainWindow::postWork()] - Executing post...";
    #endif

    if (dockwidget->isVisible()) {
        updateMetadata();
        dockwidget->hide();
    }

    if (!toolbar->isVisible())
        toolbar->setVisible(true);

    canvas->disableScreen(false);

    #ifdef TUP_DEBUG
        qDebug() << "[TupMainWindow::postWork()] - Uploading and processing project...";
    #endif
    postDialog->activateProgressUI();
    postDialog->showMaximized();
    postDialog->startPost();
}

void TupMainWindow::shareWork()
{
#ifdef TUP_DEBUG
    QString msg = "[TupMainWindow::shareWork()] - Sharing work...";
    qDebug() << msg;
    #ifdef Q_OS_ANDROID
        logDialog->appendRecord(msg);
    #endif
#endif

   if (project->value("isModified", true).toBool()) {
       #ifdef TUP_DEBUG
           qDebug() << "[TupMainWindow::shareWork()] - Work must be rendered first!";
       #endif
       handleMetadataDialog();
   } else {
       #ifdef TUP_DEBUG
           qDebug() << "[TupMainWindow::shareWork()] - Sharing file from local filesystem...";
       #endif
       QString filename = TupProject::projectsDir() + "/" + currentProjectID;
       if (framesTotal == 1)
           filename += "/image.png";
       else
           filename += "/video.mp4";

       #if defined(Q_OS_ANDROID)
           // callSocialNetworks(filename);
       #else
           shareFromDesktop(filename);
       #endif
   }
}

void TupMainWindow::processPostRequest()
{
#ifdef TUP_DEBUG
   QString msg = "[TupMainWindow::processPostRequest()] - Posting work...";
   qDebug() << msg;
   qDebug() << "formStatus -> " << formStatus;
   #ifdef Q_OS_ANDROID
       logDialog->appendRecord(msg);
   #endif
#endif

   if (framesTotal == 1) {
        if (canvas->isEmpty()) {
            handleEmptyProject();
            return;
        } 
    } else {
       if (animationIsEmpty()) {
           handleEmptyProject();
           return;
       }
    }

    // QString tags = tagsEdit->toPlainText().toLower();
    if (formStatus == Post) { // User clicked the Post form
        formStatus = Mode;
        loginPanel->setVisible(false);
        postPanel->setVisible(false);
        signUpPanel->setVisible(false);
        shareButton->setVisible(false);
        modePanel->setVisible(true);
        return;
    } else if (formStatus == SignUp) { // Make GET call to register user
        #ifdef TUP_DEBUG
            qDebug() << "[TupMainWindow::processPostRequest()] - Registering user at TupiTube";
        #endif

        if (dockwidget->isVisible())
            dockwidget->hide();

        QString email = emailEdit->text().toLower();
        email = email.trimmed();
        if (!email.isEmpty()) {
            // Invalid email
            QRegularExpression pattern("\\b[A-Z0-9._%+-]+@[A-Z0-9.-]+\\.[A-Z]{2,4}\\b");
            pattern.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
            QRegularExpressionMatch match = pattern.match(email);

            /*
            QRegExp mailREX("\\b[A-Z0-9._%+-]+@[A-Z0-9.-]+\\.[A-Z]{2,4}\\b");
            mailREX.setCaseSensitivity(Qt::CaseInsensitive);
            mailREX.setPatternSyntax(QRegExp::RegExp);
            */

            // if (!mailREX.exactMatch(email)) {
            if (!match.hasMatch()) {
                canvas->notify(TupCanvas::Warning, tr("Invalid email. Please, fix it! ;)"), 2000);
                #ifdef TUP_DEBUG
                    qDebug() << "[TupMainWindow::processPostRequest()] - Error: Invalid email! ->" << email;
                #endif
                if (!dockwidget->isVisible())
                    dockwidget->show();
                return;
            }

            QString url = createCryptoURL(TUPITUBE_URL, email);
            manager = new QNetworkAccessManager(this);
            connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(processSignUpAnswer(QNetworkReply*)));

            request.setUrl(QUrl(url));
            request.setRawHeader("User-Agent", BROWSER_FINGERPRINT);

            QNetworkReply *reply = manager->get(request);
            connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(slotError(QNetworkReply::NetworkError)));
        } else {
            canvas->notify(TupCanvas::Warning, tr("Please, insert your email address!"), 2000);
            #ifdef TUP_DEBUG
                qDebug() << "[TupMainWindow::processPostRequest()] - Error: email address is missing!";
            #endif
            if (!dockwidget->isVisible())
                dockwidget->show();
        }

        return;
    } else if (formStatus == Mode) {
        QString username = config->value("username").toString();
        QString crypto = encryptPassword(config->value("passwd").toString());
        if (username.isEmpty() || crypto.isEmpty()) {
            QString email = config->value("email").toString();
            if (email.isEmpty()) { // Showing email form
                formStatus = SignUp;
                postHeader->setText(tr("Sign Up"));
                shareButton->setPixmap(imgPath + "ok.png");

                loginPanel->setVisible(false);
                postPanel->setVisible(false);
                modePanel->setVisible(false);
                shareButton->setVisible(true);
                signUpPanel->setVisible(true);
                return;
            } else { // Showing Login form
                QString username = config->value("username").toString();
                if (username.isEmpty() || crypto.isEmpty()) { // Showing Login form
                    formStatus = Login;
                    postHeader->setText(tr("Sign In"));
                    shareButton->setVisible(true);

                    modePanel->setVisible(false);
                    signUpPanel->setVisible(false);
                    postPanel->setVisible(false);
                    loginPanel->setVisible(true);
                    canvas->notify(TupCanvas::Warning, tr("You must log in first! ;)"), 2000);

                    #ifdef TUP_DEBUG
                        qDebug() << "[TupMainWindow::processPostRequest()] - Warning: Login form is incomplete!";
                    #endif
                    return;
                } else { // Validating user and password
                    validateCredentials(username, crypto);
                    return;
                }
            }
        } else { // Validating user and password
            validateCredentials(username, crypto);
            return;
        }
    } else if (formStatus == LoginForm) {  // Show Login form
        formStatus = Login;
        postHeader->setText(tr("Sign In"));
        shareButton->setPixmap(imgPath + "ok.png");
        shareButton->setVisible(true);

        modePanel->setVisible(false);
        signUpPanel->setVisible(false);
        loginPanel->setVisible(true);
        return;
    } else if (formStatus == Login) {  // Make GET call to validate user/passwd
        #ifdef TUP_DEBUG
            qDebug() << "[TupMainWindow::processPostRequest()] - Signing in user at TupiTube";
        #endif

        QString username = usernameEdit->text();
        QString passwd = passwdEdit->text();
        if (!username.isEmpty() && !passwd.isEmpty()) {
            QString crypto = encryptPassword(passwd);

            config->setValue("username", username);
            config->setValue("passwd", crypto);

            QString url = createCryptoURL(TUPITUBE_URL, username, webCrypto);
            manager = new QNetworkAccessManager(this);
            connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(processSignInAnswer(QNetworkReply*)));

            request.setUrl(QUrl(url));
            request.setRawHeader("User-Agent", BROWSER_FINGERPRINT);

            QNetworkReply *reply = manager->get(request);
            connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(slotError(QNetworkReply::NetworkError)));

            if (dockwidget->isVisible())
                dockwidget->hide();
            if (!toolbar->isVisible())
                toolbar->setVisible(true);
        } else {
            canvas->notify(TupCanvas::Warning, tr("Please, complete the form! ;)"), 2000);
            #ifdef TUP_DEBUG
                qDebug() << "[TupMainWindow::processPostRequest()] - Error: the login form is incomplete!";
            #endif
        }
        return;
    } else if (formStatus == Send) { // Time to send user's work to the server
        postHeader->setText(tr("Share"));
        shareButton->setPixmap(imgPath + "share.png");
        postPanel->setVisible(true);
        postWork();
    }
}

void TupMainWindow::validateCredentials(const QString &username, const QString &passwd)
{
    QString webCrypto = encryptForWeb(passwd);
    QString url = createCryptoURL(TUPITUBE_URL, username, webCrypto);
    manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(processSignInAnswer(QNetworkReply*)));

    request.setUrl(QUrl(url));
    request.setRawHeader("User-Agent", BROWSER_FINGERPRINT);

    QNetworkReply *reply = manager->get(request);
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(slotError(QNetworkReply::NetworkError)));

    if (dockwidget->isVisible())
        dockwidget->hide();
    if (!toolbar->isVisible())
        toolbar->setVisible(true);

}

void TupMainWindow::processSignUpAnswer(QNetworkReply *reply)
{
    #ifdef TUP_DEBUG
       qDebug() << "[TupMainWindow::processSignUpAnswer()] - Tracing...";
    #endif

    QByteArray array = reply->readAll();
    QString answer(array);

    if (!answer.isEmpty()) {
        if (answer.compare("FALSE") == 0) { // email previously registered
            #ifdef TUP_DEBUG
                qDebug() << "[TupMainWindow::processSignUpAnswer()] - Warning: e-mail already registered! :(";
            #endif
            config->setValue("email", emailEdit->text());
            formStatus = LoginForm;
            processPostRequest();
        } else {
            #ifdef TUP_DEBUG
                qDebug() << "[TupMainWindow::processSignUpAnswer()] - URL ->" << answer;
            #endif
            if (answer.startsWith("http")) { // email registered successfully
                config->setValue("email", emailEdit->text());
                formStatus = LoginForm;
                processPostRequest();

                QDesktopServices::openUrl(answer);
            } else {
                #ifdef TUP_DEBUG
                    qDebug() << "[TupMainWindow::processSignUpAnswer()] - Error: Invalid register URL! :(";
                #endif
            }
        }
    } else {
        #ifdef TUP_DEBUG
            qDebug() << "[TupMainWindow::processSignUpAnswer()] - Error: No data from server! :(";
        #endif
        canvas->notify(TupCanvas::Warning, tr("Network error. Please, contact us!") + " :S", 2000);
    }

    manager->deleteLater();
    if (!dockwidget->isVisible())
        dockwidget->show();
}

void TupMainWindow::processSignInAnswer(QNetworkReply *reply)
{
    #ifdef TUP_DEBUG
       qDebug() << "[TupMainWindow::processSignInAnswer()] - Tracing...";
    #endif

    QByteArray array = reply->readAll();
    QString answer(array);

    if (!answer.isEmpty()) {
        if (answer.compare("TRUE") == 0) { // Valid credentials
            #ifdef TUP_DEBUG
                qDebug() << "[TupMainWindow::processSignInAnswer()] - Login successful! :)";
            #endif
            loginPanel->setVisible(false);
            formStatus = Send;
            postMode = LoggedUser;

            processPostRequest();
        } else { // Invalid credentials
            config->setValue("username", "");
            config->setValue("passwd", "");
            passwdEdit->setText("");

            if (formStatus == Mode) {
                formStatus = LoginForm;
                processPostRequest();
            } else if (formStatus == Login) {
                formStatus = Login;
            }

            canvas->notify(TupCanvas::Warning, tr("Access denied! Invalid user/password?"), 2000);
            if (!dockwidget->isVisible())
                dockwidget->show();
        }
    } else {
        #ifdef TUP_DEBUG
            qDebug() << "[TupMainWindow::processSignInAnswer()] - Error: No data from server! :(";
        #endif
    }

    manager->deleteLater();
}

void TupMainWindow::slotError(QNetworkReply::NetworkError error)
{
    #ifdef TUP_DEBUG
        qDebug() << "[TupMainWindow::slotError()] - Processing network errors...";
    #endif

    QString errorMsg = tr("Network error. Please, contact us!") + " ";
    switch (error) {
        case QNetworkReply::HostNotFoundError:
             {
                 #ifdef TUP_DEBUG
                     qDebug() << "[TupMainWindow::slotError()] - Network Error: Host not found";
                 #endif
                 canvas->notify(TupCanvas::Warning, errorMsg + "(Code 1)", 2000);
             }
        break;
        case QNetworkReply::TimeoutError:
             {
                 #ifdef TUP_DEBUG
                     qDebug() << "[TupMainWindow::slotError()] - Network Error: Time out!";
                 #endif
                 canvas->notify(TupCanvas::Warning, errorMsg + "(Code 2)", 2000);
             }
        break;
        case QNetworkReply::ConnectionRefusedError:
             {
                 #ifdef TUP_DEBUG
                     qDebug() << "[TupMainWindow::slotError()] - Network Error: Connection Refused!";
                 #endif
                 canvas->notify(TupCanvas::Warning, errorMsg + "(Code 3)", 2000);
             }
        break;
        case QNetworkReply::ContentNotFoundError:
             {
                 #ifdef TUP_DEBUG
                     qDebug() << "[TupMainWindow::slotError()] - Network Error: Content not found!";
                 #endif
                 canvas->notify(TupCanvas::Warning, errorMsg + "(Code 4)", 2000);
             }
        break;
        case QNetworkReply::UnknownNetworkError:
        default:
             {
                 #ifdef TUP_DEBUG
                     qDebug() << "[TupMainWindow::slotError()] - Network Error: Unknown Network error!";
                 #endif
                 canvas->notify(TupCanvas::Warning, errorMsg + "(Code 5)", 2000);
             }
        break;
    }
}

bool TupMainWindow::animationIsEmpty()
{
    bool empty = true;
    foreach(TupFrame *frame, frames) {
        if (!frame->isEmpty()) {
            empty = false;
            break;
        }
    }

    if (empty) {
        canvas->notify(TupCanvas::Warning, tr("Please, draw something in the frames!"), 2000);
        #ifdef TUP_DEBUG
            qDebug() << "[TupMainWindow::animationIsEmpty()] - Warning: Animation is empty. Please, draw something in the frames! ;)";
        #endif
    }

    return empty;
}

void TupMainWindow::sendData()
{   
    #ifdef TUP_DEBUG
        qDebug() << "[TupMainWindow::sendData()] - Sending data to server...";
    #endif

    notifying = true;
    int w = screenSize.width();
    int h = screenSize.height();

    animDoc = QDomDocument();
    QDomElement root = animDoc.createElement("mobile");
    root.setAttribute("version", TupNetHandler::Standard);

    int framesCounter = framesTotal;

    if (framesCounter == 1) { // Image
        root.setAttribute("type", "image");
    } else { // Animation
        root.setAttribute("type", "animation");
        root.setAttribute("frames", framesCounter);
        root.setAttribute("fps", fps);
    }

    root.setAttribute("os", system);
    animDoc.appendChild(root);

    QString username = config->value("username", "anonymous").toString();
    if (postMode == Anonymous)
        username = "anonymous";

    QDomElement userDom = animDoc.createElement("username");
    QDomText value = animDoc.createTextNode(username);
    userDom.appendChild(value);
    root.appendChild(userDom);

    QString title = titleEdit->text();
    if (title.length() > 0) {
        if (title.contains("<") || title.contains(">") || title.contains("http")) {
            #ifdef TUP_DEBUG
                qDebug() << "[TupMainWindow::sendData()] - Title has invalid data ->" << title;
            #endif

            if (framesTotal == 1)
                title = tr("My Sketch");
            else
                title = tr("My Animation");
        }

        if (title.length() > 40) {
            title = title.left(40);
            titleEdit->setText(title);
        }

        #ifdef TUP_DEBUG
            qDebug() << "[TupMainWindow::sendData()] - Title ->" << title;
        #endif

        QDomElement titleDom = animDoc.createElement("title");
        value = animDoc.createTextNode(title.simplified());
        titleDom.appendChild(value);
        root.appendChild(titleDom);
    }

    QString tags = tagsEdit->toPlainText();
    if (tags.length() > 0) {
        if (tags.contains("<") || tags.contains(">") || tags.contains("http")) {
            if (framesCounter == 1)
                tags = tr("#tupitube #drawing #fun");
            else
                tags = tr("#tupitube #animation #fun");
        }

        if (tags.length() > 50) {
            tags = tags.left(50);
            tagsEdit->setPlainText(tags);
        }

        #ifdef TUP_DEBUG
            qDebug() << "[TupMainWindow::sendData()] - Tags ->" << tags;
        #endif

        QDomElement tagsDom = animDoc.createElement("tags");
        value = animDoc.createTextNode(tags.simplified());
        tagsDom.appendChild(value);
        root.appendChild(tagsDom);
    }

    QString comment = editor->toPlainText();
    if (comment.length() > 0) {
        if (comment.contains("<") || comment.contains(">") || comment.contains("http"))
            comment = "";

        if (comment.length() > 240) {
            comment = comment.left(240);
            editor->setPlainText(comment);
        }

        if (comment.compare(defaultComment) == 0)
            comment = formatPromoComment();

        #ifdef TUP_DEBUG
            qDebug() << "[TupMainWindow::sendData()] - Comment ->" << comment;
        #endif
    } else {
        comment = formatPromoComment();
    }

    QDomElement commentDom = animDoc.createElement("comment");
    value = animDoc.createTextNode(comment.simplified());
    commentDom.appendChild(value);
    root.appendChild(commentDom);

    QDomElement dimension = animDoc.createElement("dimension");
    QDomText size = animDoc.createTextNode(QString::number(w) + "," + QString::number(h));
    dimension.appendChild(size);
    root.appendChild(dimension);

    QDomElement bgDom = animDoc.createElement("bgcolor");
    QDomText color = animDoc.createTextNode(bgColor.name());
    bgDom.appendChild(color);
    root.appendChild(bgDom);

    if (framesCounter == 1) {
        TupFrame *frame = canvas->frame();
        QDomElement package = frame->toXml(animDoc);
        root.appendChild(package);
    } else {
        #ifdef TUP_DEBUG
            qDebug() << "[TupMainWindow::sendData()] - Adding frames array...";
        #endif
        foreach (TupFrame *frame, frames) {
            QDomElement package = frame->toXml(animDoc);
            root.appendChild(package);
        }
    }

    /* SQA: Enable for debugging
    #ifdef TUP_DEBUG
        qDebug() << "";
        qDebug() << animDoc.toString();
        qDebug() << "";
    #endif
    */

    QTimer::singleShot(0, this, SLOT(sendPackage()));
}

void TupMainWindow::sendPackage()
{
    TupNetHandler *net = new TupNetHandler;
    connect(net, &TupNetHandler::postReady, this, &TupMainWindow::shareResources);
    connect(net, &TupNetHandler::netError, this, &TupMainWindow::showNetError);
    connect(postDialog, &TupPostDialog::transmissionCancelled, net, &TupNetHandler::cancelPost);

    net->setProjectID(currentProjectID);
    net->sendPackage(animDoc);
}

QString TupMainWindow::formatPromoComment()
{
    QString desc = "promo.en";

    QStringList langSupport;
    // langSupport << "es" << "pt" << "jp" << "cn" << "fr";
    langSupport << "es" << "pt";
    QString locale = QString(QLocale::system().name()).left(2);

    if (locale.compare("en") != 0 && langSupport.contains(locale))
        desc = "promo." + locale;

    return desc;
}

void TupMainWindow::shareResources(const QString &url)
{
    #ifdef TUP_DEBUG
        QString msg = "[TupMainWindow::shareUrl()] - url ->" + url;
        qDebug() << msg;
        #ifdef Q_OS_ANDROID
            logDialog->appendRecord(msg);
        #endif
    #endif

    if (postDialog->isVisible()) {
        postDialog->close();
        canvas->repaintFrame();
    }

    if (url.compare("FAIL") == 0) {
        canvas->notify(TupCanvas::Error, "01 - " + tr("Can't create media file!"), 3000);
        enableUI(true);
        return;
    }

#if defined(Q_OS_ANDROID)
    // notifying = false;
    // canvas->removeNotification();
    // callSocialNetworks(url);
#else
    project->setValue("isModified", false);
    shareFromDesktop(url);
#endif
}

#if defined(Q_OS_ANDROID)
void TupMainWindow::callSocialNetworks(const QString &url)
{
    /*
    ShareUtils *resource = new ShareUtils();
    #ifdef TUP_DEBUG
        resource->setLog(TupProject::rootDir());
    #endif

    #ifdef TUP_DEBUG
        QString msg = "TupMainWindow::callSocialNetworks() - Sending media to social networks -> " + url;
        qDebug() << msg;
        #ifdef Q_OS_ANDROID
            logDialog->appendRecord(msg);
        #endif
    #endif

    QFile file(url);
    if (file.exists(url)) {
        #ifdef TUP_DEBUG
            QString msg = "TupMainWindow::callSocialNetworks() - Media file already exists. Sharing now!";
            qDebug() << msg;
            #ifdef Q_OS_ANDROID
                logDialog->appendRecord(msg);
            #endif
        #endif
        project->setValue("isModified", false);
        resource->shareMedia(url);
    } else {
        #ifdef TUP_DEBUG
            QString msg = "TupMainWindow::callSocialNetworks() - Fatal Error: File doesn't exist! -> " + url;
            qDebug() << msg;
            #ifdef Q_OS_ANDROID
               logDialog->appendRecord(msg);
            #endif
        #endif
        canvas->notify(TupCanvas::Error, "02 - " + tr("Can't create media file!"), 3000);
    }
    */
}
#else
void TupMainWindow::shareFromDesktop(const QString &url)
{    
    #ifdef TUP_DEBUG
        qDebug() << "TupMainWindow::shareFromDesktop() - url -> " << url;
    #endif

    QString filter = tr("Video File (*.mp4)");
    QString msg = tr("Save Animation Video");
    if (url.endsWith(".png")) {
        filter = tr("Image File (*.png)");
        msg = tr("Save Illustration");
    } else if (url.endsWith(".tup")) {
        filter = tr("TupiTube Source File (*.tup)");
        msg = tr("Save Source File");
    }
    QString path = QStandardPaths::standardLocations(QStandardPaths::DesktopLocation)[0];

    QString filename = QFileDialog::getSaveFileName(this, msg, path, filter);
    if (!filename.isEmpty()) {
        if (QFile::exists(filename)) {
            bool removed = QFile::remove(filename);
            if(!removed) {
                #ifdef TUP_DEBUG
                    qWarning() << "TupMainWindow::shareFromDesktop() - Fatal Error: "
                                  "Failed to remove file -> " << filename;
                #endif
                canvas->notify(TupCanvas::Error, tr("Can't overwrite media file! :("), 5000);
                return;
            }
        }

        if (url.endsWith(".png")) {
            if (!filename.endsWith(".png"))
                filename += ".png";
        } else if (url.endsWith(".mp4")) {
            if (!filename.endsWith(".mp4"))
                filename += ".mp4";
        } else if (url.endsWith(".tup")) {
            if (!filename.endsWith(".tup"))
                filename += ".tup";
        }

        bool copied = QFile::copy(url, filename);
        if (!copied) {
            #ifdef TUP_DEBUG
                qWarning() << "TupMainWindow::shareFromDesktop() - Fatal Error: "
                              "Failed to copy " << url << " to " << filename;
            #endif
            canvas->notify(TupCanvas::Error, tr("Can't save media file! :("), 5000);
        } else {
            #ifdef TUP_DEBUG
                qDebug() << "TupMainWindow::shareFromDesktop() - Success copy -> " << filename;
            #endif
            canvas->notify(TupCanvas::Info, tr("Media file saved successfully! :)"), 2000);
        }
    }
}
#endif

void TupMainWindow::showColorDialog()
{
    colorDialog->showMaximized();
    colorDialog->initSliders();
    colorDialog->setParameters(getBorderBrushStyle(), getBorderOpacity(),
                               getFillBrushStyle(), getFillOpacity());
}

void TupMainWindow::showPenSizeDialog()
{
    if (borderOpacity > 0) {
        #ifdef TUP_DEBUG
            qDebug() << "[TupMainWindow::penStrokeSizeDialog()] - border opacity ->" << borderOpacity;
            qDebug() << "[TupMainWindow::penStrokeSizeDialog()] - pen ->" << pen;
        #endif

        thicknessDialog->showMaximized();
        thicknessDialog->setParameters(pen.width(), pen, borderOpacity);
    } else {
        #if defined(Q_OS_ANDROID)
            closeFloatingMenu();
        #endif
        canvas->notify(TupCanvas::Info, tr("Sorry, border color is transparent."), 2000);
    }
}

void TupMainWindow::showOpacityDialog()
{
    opacityDialog->showMaximized();
    opacityDialog->setParameters(pen, getFillColor(), getFillBrushStyle(),
                                 getBorderOpacity(), getFillOpacity());
}

void TupMainWindow::showBrushesDialog()
{
    brushDialog->showMaximized();
    brushDialog->setParameters(getBorderColor(), getBorderOpacity(),
                               getFillColor(), getFillOpacity());
}

void TupMainWindow::updatePenSize(int size)
{
#if defined(Q_OS_ANDROID)
    closeFloatingMenu();
#endif

    #ifdef TUP_DEBUG
        qDebug() << "[TupMainWindow::updatePenSize()] - pen size ->" << size;
    #endif

    pen.setWidth(size);
    canvas->updatePenThickness(size);
    config->setValue("penSize", size);
}

void TupMainWindow::updateColors(const QColor border, const QColor fill, const QColor bg)
{
#if defined(Q_OS_ANDROID)
    closeFloatingMenu();
#endif

    QColor borderColor = getBorderColor();
    QColor fillColor = getFillColor();

#ifdef TUP_DEBUG
    qDebug() << "[TupMainWindow::updateColors()] - New border color ->" << border;
    qDebug() << "[TupMainWindow::updateColors()] - New fill color ->" << fill;
    qDebug() << "[TupMainWindow::updateColors()] - New fill bg color ->" << bg;
    qDebug() << "";
    qDebug() << "[TupMainWindow::updateColors()] - Current border color ->" << borderColor;
    qDebug() << "[TupMainWindow::updateColors()] - Current fill color ->" << fillColor;
    qDebug() << "[TupMainWindow::updateColors()] - Curreng bg color ->" << bgColor;
    qDebug() << "";
#endif

    if ((border != borderColor) || (border.alpha() != borderColor.alpha())) {
        #ifdef TUP_DEBUG
            qDebug() << "[TupMainWindow::updateColors()] - Updating border color ->" << border;
        #endif

        if (border.alpha() != borderColor.alpha()) {
            borderOpacity = border.alpha() / 255.0;
            canvas->updateBorderOpacity(border.alpha());
            config->setValue("borderOpacity", borderOpacity);
        }

        QBrush brush(border, pen.brush().style());
        pen.setColor(border);
        pen.setBrush(brush);
        canvas->updateBorderColor(border);
        config->setValue("borderColor", border.name());
    }

    if ((fill != fillColor) || (fill.alpha() != fillColor.alpha())) {
        #ifdef TUP_DEBUG
            qDebug() << "[TupMainWindow::updateColors()] - Updating new fill color ->" << fill;
        #endif

        if (fill.alpha() != fillColor.alpha()) {
            fillOpacity = fill.alpha()/255.0;
            canvas->updateFillOpacity(fill.alpha());
            config->setValue("fillOpacity", fillOpacity);
        }

        canvas->updateFillColor(fill);
        config->setValue("fillColor", fill.name());
    }

    if (bg != bgColor) {
        /*
        #ifdef TUP_DEBUG
            qDebug() << "[TupMainWindow::updateColors()] - Updating new bg color ->" << bg;
        #endif
        */
        bgColor = bg;
        canvas->updateBgColor(bgColor);
        config->setValue("bgColor", bg.name());
        project->setValue("bgColor", bg.name());
        project->setValue("isModified", true);
    }
}

void TupMainWindow::updateOpacities(double border, double fill)
{
    #ifdef TUP_DEBUG
        qDebug() << "[TupMainWindow::updateOpacities()] - border opacity ->" << border;
        qDebug() << "[TupMainWindow::updateOpacities()] - fill opacity ->" << fill;
    #endif

    #if defined(Q_OS_ANDROID)
        closeFloatingMenu();
    #endif

    borderOpacity = border;
    config->setValue("borderOpacity", border);

    QColor color = pen.color();
    color.setAlpha(static_cast<int>(border * 255.0));

    QBrush brush = pen.brush();
    brush.setColor(color);
    pen.setBrush(brush);

    fillOpacity = fill;
    config->setValue("fillOpacity", fill);

    canvas->updateOpacities(static_cast<int>(border * 255.0), static_cast<int>(fill * 255.0));
}

void TupMainWindow::updateBrushes(Qt::BrushStyle borderStyle, Qt::BrushStyle fillStyle)
{
    #if defined(Q_OS_ANDROID)
        closeFloatingMenu();
    #endif

    QBrush brush = pen.brush();
    brush.setStyle(borderStyle);
    pen.setBrush(brush);
    canvas->updateBrushes(borderStyle, fillStyle);

    int style = borderStyle;
    config->setValue("borderBrush", QVariant(style));
    style = fillStyle;
    config->setValue("fillBrush", QVariant(style));
}

void TupMainWindow::undo()
{
    canvas->undo();
    saveFrame();

    #if defined(Q_OS_ANDROID)
        canvas->notify(TupCanvas::Console, tr(" * Removing latest object... "), 700);
    #endif
}

void TupMainWindow::redo()
{
    canvas->redo();
    saveFrame();

    #if defined(Q_OS_ANDROID)
        canvas->notify(TupCanvas::Console, tr(" * Restoring removed object... "), 700);
    #endif
}

void TupMainWindow::setUndoRedoStatus(bool enable) 
{
    if (enable) {
        if (canvas->undoIsAvailable()) {
            if (isCellPhone)
                toolsTopMenu->enableButton(TupCanvas::Undo, enable);
            undoAction->setEnabled(enable);
        }
        if (canvas->redoIsAvailable()) {
            if (isCellPhone)
                toolsTopMenu->enableButton(TupCanvas::Redo, enable);
            redoAction->setEnabled(enable);
        }
    } else {
        if (isCellPhone) {
            toolsTopMenu->enableButton(TupCanvas::Undo, enable);
            toolsTopMenu->enableButton(TupCanvas::Redo, enable);
        }

        undoAction->setEnabled(enable);
        redoAction->setEnabled(enable);
    }
}

void TupMainWindow::checkUndoRedoStatus(TupFrame *currentFrame)
{
    if (!currentFrame->undoIsAvailable()) {
        if (isCellPhone)
            toolsTopMenu->enableButton(TupCanvas::Undo, false);
        undoAction->setEnabled(false);
    } else {
        if (isCellPhone)
            toolsTopMenu->enableButton(TupCanvas::Undo, true);
        undoAction->setEnabled(true);
    }

    if (!currentFrame->redoIsAvailable()) {
        if (isCellPhone)
            toolsTopMenu->enableButton(TupCanvas::Redo, false);
        redoAction->setEnabled(false);
    } else {
        if (isCellPhone)
            toolsTopMenu->enableButton(TupCanvas::Redo, true);
        redoAction->setEnabled(true);
    }
}

void TupMainWindow::clearCanvas()
{
/*
#if defined(Q_OS_ANDROID)
    closeFloatingMenu();
    enableToolbar();
#endif
*/
    canvas->reset();
    saveFrame();
    canvas->notify(TupCanvas::Info, tr("Canvas cleaned!"), 1000);
}

void TupMainWindow::addNewFrameForward(TupFrame *frame, bool copy)
{
    if ((currentFrameIndex < framesTotal - 1) || copy) {
        frames.insert(currentFrameIndex + 1, frame);

        int index = framesTotal - 1;
        for (int i=index; i > currentFrameIndex; i--) {
             QString key = "frame_" + QString::number(i);
             QString frameContent = project->value(key).toString();
             project->setValue("frame_" + QString::number(i+1), frameContent);
        }

        project->setValue("frame_" + QString::number(currentFrameIndex + 1), frame->toString());
        framesTotal++;
        project->setValue("framesTotal", framesTotal);
    }
    goOneFrameForward();

    if (copy)
        canvas->notify(TupCanvas::Info, tr("Frame copied forward!"), 1000);
    else
        canvas->notify(TupCanvas::Info, tr("Frame added forward!"), 1000);
}

void TupMainWindow::insertNewFrameBack()
{
    TupFrame *currentFrame = new TupFrame();
    frames.insert(currentFrameIndex, currentFrame);

    int index = framesTotal - 1;
    for (int i=index; i >= currentFrameIndex; i--) {
        QString key = "frame_" + QString::number(i);
        QString frameContent = project->value(key).toString();
        project->setValue("frame_" + QString::number(i+1), frameContent);
    }

    project->setValue("frame_" + QString::number(currentFrameIndex), currentFrame->toString());
    framesTotal++;
    project->setValue("framesTotal", framesTotal);
    currentFrameIndex++;
    project->setValue("isModified", true);

    goOneFrameBack();

    canvas->notify(TupCanvas::Info, tr("Frame added back!"), 1000);
}

void TupMainWindow::cloneCurrentFrame()
{
    TupFrame *copy = frames.at(currentFrameIndex);
    QString content = copy->toString();
    TupFrame *frame = new TupFrame();
    frame->fromXml(content);
    frames.insert(currentFrameIndex + 1, frame);

    int index = framesTotal - 1;
    for (int i=index; i > currentFrameIndex; i--) {
         QString key = "frame_" + QString::number(i);
         QString frameContent = project->value(key).toString();
         project->setValue("frame_" + QString::number(i+1), frameContent);
    }

    project->setValue("frame_" + QString::number(currentFrameIndex + 1), content);
    framesTotal++;
    project->setValue("framesTotal", framesTotal);
    project->setValue("isModified", true);

    goOneFrameForward();

    if (isCellPhone)
        canvas->notify(TupCanvas::Console, tr(" * Current frame cloned"), 700);
    else
        canvas->notify(TupCanvas::Info, tr("Current frame cloned!"), 1000);
}

void TupMainWindow::removeCurrentFrame()
{
/*
#if defined(Q_OS_ANDROID)
    closeFloatingMenu();
    enableToolbar();
#endif
*/

    if (framesTotal == 1) { // Removing last frame
        clearCanvas();
        getPreviousFrame->setEnabled(false);
    } else {
        if (currentFrameIndex < framesTotal - 1) {
            int index = currentFrameIndex;
            for (int i=index+1; i < framesTotal; i++) {
                 QString frameKey = "frame_" + QString::number(i);
                 QString frame = project->value(frameKey).toString();
                 project->setValue("frame_" + QString::number(i-1), frame);
                 frames.replace(i-1, frames.at(i));
            }
        }
        frames.removeLast();
        project->remove("frame_" + QString::number(framesTotal - 1));

        framesTotal--;
        project->setValue("framesTotal", framesTotal);

        if (framesTotal == 1)
            getPreviousFrame->setEnabled(false);

        if (currentFrameIndex == 0) {
            setComplementaryFrames();
            canvas->loadFrame(previousFrame, frames.at(0), nextFrame,
                              QString::number(currentFrameIndex + 1) + "/" + QString::number(framesTotal));
        } else {
            goOneFrameBack();
        }
    }

    project->setValue("isModified", true);
    canvas->notify(TupCanvas::Info, tr("Frame removed!"), 1000);
}

void TupMainWindow::copyCurrentFrame()
{    
    TupFrame *copy = frames.at(currentFrameIndex);
    if (copy) {
        QString content = copy->toString();
        frameCopy = new TupFrame();
        frameCopy->fromXml(content);
    } else {
        #ifdef TUP_DEBUG
            qDebug() << "[TupMainWindow::copyCurrentFrame()] - Can't copy frame. It's NULL!";
        #endif
    }
}

void TupMainWindow::pasteFrameForward()
{
    if (frameCopy)
        addNewFrameForward(frameCopy, true);
}

void TupMainWindow::saveAndSetNewProject()
{
    #ifdef TUP_DEBUG
        qDebug() << "[TupMainWindow::saveAndSetNewProject()] - Tracing...";
    #endif

    if (!isCellPhone)
        closeTabletFileMenu();

    #if !defined(Q_OS_ANDROID)
        if (mode == TupCanvas::Player) {
            mode = TupCanvas::Frames;
            canvas->setMode(mode);
        }
    #endif

    /*
    if (frames.size() == 1) {
        TupFrame *frame = frames.at(0);
        if (frame->isEmpty()) {
            #ifdef TUP_DEBUG
                qDebug() << "TupMainWindow::saveAndSetNewProject() - Current project is empty. Nothing to save!";
            #endif
            #if defined(Q_OS_ANDROID)
                closeFloatingMenu();
            #endif
            return;
        }
    }
    */

    savePreviousProject(false);

    projectsIndex++;
    config->setValue("projectsIndex", projectsIndex);
    resetProjectUI();
    setFocus();
}

void TupMainWindow::resetProjectUI()
{   
    #ifdef TUP_DEBUG
        qDebug() << "[TupMainWindow::resetProjectUI()] - Tracing...";
    #endif

    int number = projectsIndex + 1;
    QString prefix = "project_";
    if (number < 10)
        prefix += "0";

    currentProjectID = prefix + QString::number(number);
    config->setValue("currentProjectID", currentProjectID);

    project = new QSettings("MaeFloresta", "CurrentProject");
    project->clear();
    setUndoRedoStatus(false);
    getPreviousFrame->setEnabled(false);

    colorDialog->resetColorHistory();
    colorDialog->resetBorderColor();

    bgColor = Qt::white;
    project->setValue("bgColor", bgColor.name());
    project->setValue("fps", fps);

    config->setValue("bgColor", bgColor.name());
    updateBrushes(Qt::BrushStyle(1), Qt::BrushStyle(1));

    QString title = tr("My Sketch");
    QString tags = tr("#tupitube #drawing #fun");

    titleEdit->setText(title);
    tagsEdit->setPlainText(tags);
    editor->setPlainText(defaultComment);

    project->setValue("title", title);
    project->setValue("tags", tags);
    project->setValue("comment", defaultComment);

    canvas->fullReset();
    updateColors(Qt::black, Qt::transparent, Qt::white);

    frames.clear();
    TupFrame *frame = new TupFrame();
    frames << frame;

    currentFrameIndex = 0;
    frameKey = "frame_0";
    framesTotal = 1;
    project->setValue("framesTotal", 1);

    #ifdef Q_OS_ANDROID
        canvas->notify(TupCanvas::Info, tr("Animation Frame No. 1"), 1000);
    #endif
}

void TupMainWindow::showHelpDialog()
{
    helpDialog->updateInternetFlag(hasInternet);
    helpDialog->showMaximized();
}

void TupMainWindow::showNetError(const QString &msg)
{
    #ifdef TUP_DEBUG
        qDebug() << "[TupMainWindow::showNetError()] - Tracing messsage ->" << msg;
    #endif

    if (postDialog->isVisible())
        postDialog->close();

    enableUI(true);
    canvas->notify(TupCanvas::Error, msg, 3000);
}

int TupMainWindow::getPenSize()
{
    return config->value("penSize", 8).toInt();
}

double TupMainWindow::getBorderOpacity()
{
    return config->value("borderOpacity", 1.0).toDouble();
}

double TupMainWindow::getFillOpacity()
{
    return config->value("fillOpacity", 0).toDouble();
}

QColor TupMainWindow::getBorderColor()
{
    QString colorName = config->value("borderColor", "#000000").toString();

    double opacity = config->value("borderOpacity", 1.0).toDouble();
    QColor color(colorName);
    color.setAlpha(static_cast<int>(opacity * 255.0));

    return color;
}

QColor TupMainWindow::getFillColor()
{
    QString colorName = config->value("fillColor", QString("")).toString();    
    if (colorName.isEmpty())
        colorName = "#000000";

    double opacity = config->value("fillOpacity", 1.0).toDouble();
    QColor color(colorName);
    color.setAlpha(static_cast<int>(opacity * 255.0));

    return color;
}

Qt::BrushStyle TupMainWindow::getBorderBrushStyle()
{
    int index = config->value("borderBrush", 1).toInt();
    return Qt::BrushStyle(index);
}

Qt::BrushStyle TupMainWindow::getFillBrushStyle()
{
    int index = config->value("fillBrush", 1).toInt();
    return Qt::BrushStyle(index);
}

float TupMainWindow::getSmoothLevel()
{
    return config->value("smoothLevel", 4.0).toFloat();
}

bool TupMainWindow::getPreviousFrameVisibility()
{
    return config->value("showPreviousFrame", true).toBool();
}

bool TupMainWindow::getNextFrameVisibility()
{
    return config->value("showNextFrame", true).toBool();
}

int TupMainWindow::getFPS()
{
    return config->value("fps", 12).toInt();
}

void TupMainWindow::saveFrame()
{
    TupFrame *frame = canvas->frame(); 
    frames[currentFrameIndex] = frame;
    QString data = frame->toString();
    project->setValue(frameKey, data);
    project->setValue("isModified", true);

    #ifdef TUP_DEBUG
        qDebug() << "[TupMainWindow::saveFrame()] - Saving frame ->" << frameKey;
        qDebug() << "[TupMainWindow::saveFrame()] - Frame Objects Total ->" << frame->items().count();
        qDebug() << "[TupMainWindow::saveFrame()] - Frame length ->" << data.length();
    #endif
}

void TupMainWindow::createColorPalettes()
{
    QScreen *screen = QGuiApplication::screens().at(0);
    int width = screen->geometry().width();
    int height = screen->geometry().height();

    QImage img(width, height, QImage::Format_RGB32);
    for (int y = 0; y < height; y++) {
         for (int x = 0; x < width; x++) {
              QPoint p(x, y);
              QColor c;
              int param1 = 360 - p.x()*360/(width - 1);
              int param2 = 255 - p.y()*255/(height - 1);
              c.setHsv(param1, param2, 200);
              img.setPixel(x, y, c.rgb());
         }
    }

    QPixmap palette = QPixmap(QPixmap::fromImage(img));
    if (!TupProject::saveColorPalette(palette)) {
        #ifdef TUP_DEBUG
            QString msg = "[TupMainWindow::createColorPalettes()] - Fatal Error: Can't create color palette!";
            qDebug() << msg;
            #ifdef Q_OS_ANDROID
                logDialog->appendRecord(msg);
            #endif
        #endif
    }
}

void TupMainWindow::enableUI(bool flag)
{
    #ifdef TUP_DEBUG
        qDebug() << "[TupMainWindow::enableUI()] - flag ->" << flag;
    #endif

    canvas->setEnabled(flag);
    toolbar->setEnabled(flag);
}

void TupMainWindow::updateFocus()
{
    activateWindow();
    raise();
}

void TupMainWindow::enableToolbar()
{
    #ifdef TUP_DEBUG
        qDebug() << "[TupMainWindow::enableToolbar()] - Tracing...";
    #endif

    setToolbarVisible(true);
}

void TupMainWindow::showToolsMenu()
{
    #ifdef TUP_DEBUG
        qDebug() << "[TupMainWindow::showToolsMenu()] - Tracing...";
    #endif

    if (dockwidget->isVisible()) {
        dockwidget->hide();
    } else {
        if (mode == TupCanvas::Player) {
            mode = TupCanvas::Frames;
            canvas->setMode(mode);
        }
    }

    QScreen *screen = QGuiApplication::screens().at(0);
    int width = screen->geometry().width();
    int height = screen->geometry().height();

    setToolbarVisible(false);

    if (width > height) {
        toolsFlatMenu->show();
        toolsFlatMenu->move(static_cast<int> ((width - toolsFlatMenu->width())/2),
                            static_cast<int> ((height - toolsFlatMenu->height())));
    } else {
        toolsTopMenu->show();
        toolsTopMenu->move(static_cast<int> ((width - toolsTopMenu->width())/2),
                           static_cast<int> ((height - toolsTopMenu->height())));
    }
}

void TupMainWindow::showFileMenu()
{
    #ifdef TUP_DEBUG
        qDebug() << "[TupMainWindow::showFileMenu()] - Tracing...";
    #endif

    if (dockwidget->isVisible()) {
        dockwidget->hide();
    } else {
        if (mode == TupCanvas::Player) {
            mode = TupCanvas::Frames;
            canvas->setMode(mode);
        }
    }

    QScreen *screen = QGuiApplication::screens().at(0);
    int width = screen->geometry().width();
    int height = screen->geometry().height();

    setToolbarVisible(false);

    if (width > height) {
        fileFlatMenu->show();
        fileFlatMenu->move(static_cast<int> ((width - fileFlatMenu->width()) / 2),
                           static_cast<int> ((height - fileFlatMenu->height())));
    } else {
        fileTopMenu->show();
        fileTopMenu->move(static_cast<int>((width - fileTopMenu->width()) / 2),
                          static_cast<int>((height - fileTopMenu->height())));
    }
}

void TupMainWindow::showTabletFileMenu()
{
    #ifdef TUP_DEBUG
        qDebug() << "[TupMainWindow::showTabletFileMenu()] - Tracing...";
    #endif

    if (dockwidget->isVisible()) {
        dockwidget->hide();
    } else {
        if (mode == TupCanvas::Player) {
            mode = TupCanvas::Frames;
            canvas->setMode(mode);
        }
    }

    QScreen *screen = QGuiApplication::screens().at(0);
    int width = screen->geometry().width();
    int height = screen->geometry().height();

    setToolbarVisible(false);

    tabletFileMenu->show();
    tabletFileMenu->move(static_cast<int> ((width - tabletFileMenu->width()) / 2),
                         static_cast<int> ((height - tabletFileMenu->height())));
}

void TupMainWindow::enableButton(TupCanvas::Button type, bool flag)
{
    if (isCellPhone) {
        toolsTopMenu->enableButton(type, flag);
        if (type == TupCanvas::Redo)
            redoAction->setEnabled(flag);
        if (type == TupCanvas::Undo)
            undoAction->setEnabled(flag);
    } else {
        if (type == TupCanvas::Redo)
            redoAction->setEnabled(flag);
        if (type == TupCanvas::Undo)
            undoAction->setEnabled(flag);
    }
}

void TupMainWindow::goOneFrameBack()
{       
    if (currentFrameIndex == 0) {
        return; 
    } else {
        currentFrameIndex--;
        if (currentFrameIndex == 0)
            getPreviousFrame->setEnabled(false);
    }

    frameKey = "frame_" + QString::number(currentFrameIndex);
    setComplementaryFrames();
    TupFrame *currentFrame = frames.at(currentFrameIndex);    
    canvas->loadFrame(previousFrame, currentFrame, nextFrame,
                      QString::number(currentFrameIndex + 1) + "/" + QString::number(framesTotal));

    checkUndoRedoStatus(currentFrame);
}

void TupMainWindow::goToFrame(int index)
{
    if (currentFrameIndex != index) {
        currentFrameIndex = index;
        frameKey = "frame_" + QString::number(currentFrameIndex);
        TupFrame *currentFrame = frames.at(currentFrameIndex);
        canvas->loadFrame(previousFrame, currentFrame, nextFrame, QString::number(currentFrameIndex + 1)
                          + "/" + QString::number(framesTotal));
    }
}

void TupMainWindow::goOneFrameForward()
{ 
    if (!getPreviousFrame->isEnabled())
        getPreviousFrame->setEnabled(true);

    currentFrameIndex++;

    #ifdef TUP_DEBUG
        qDebug() << "[TupMainWindow::goOneFrameForward()] - Moving to frame ->" << currentFrameIndex;
    #endif

    frameKey = "frame_" + QString::number(currentFrameIndex);
    TupFrame *currentFrame;
    if (currentFrameIndex < framesTotal) { // Frame already exists
        setComplementaryFrames();
        currentFrame = frames.at(currentFrameIndex);
        canvas->loadFrame(previousFrame, currentFrame, nextFrame, QString::number(currentFrameIndex + 1)
                          + "/" + QString::number(framesTotal));
    } else { // A new frame must be created
        framesTotal++;
        project->setValue("framesTotal", framesTotal);
        currentFrame = new TupFrame();
        frames << currentFrame;
        canvas->loadFrame(frames.at(currentFrameIndex - 1), currentFrame, new TupFrame(),
                          QString::number(currentFrameIndex + 1) + "/" + QString::number(framesTotal));
        saveFrame();
        project->setValue("isModified", true);
    }

    if (framesTotal > 1) {
        if (titleEdit->text().compare(tr("My Sketch")) == 0) {
            titleEdit->setText(tr("My Animation"));
            project->setValue("title", tr("My Animation"));
        }

        if (tagsEdit->toPlainText().compare(tr("#tupitube #drawing #fun")) == 0) {
            tagsEdit->setPlainText(tr("#tupitube #animation #fun"));
            project->setValue("tags", tr("#tupitube #animation #fun"));
        }
    }

    checkUndoRedoStatus(currentFrame);
}

void TupMainWindow::setAnimationMode()
{
    #ifdef TUP_DEBUG
        qDebug() << "[TupMainWindow::setAnimationMode()] - Tracing...";
    #endif

    if (mode == TupCanvas::Frames) { // Change to player mode
        if (animationIsEmpty())
            return;

        if (frames.count() > 1) {
            mode = TupCanvas::Player;
            play->setIcon(stopIcon);
            #if !defined(Q_OS_ANDROID)
                play->setText(tr("Stop"));
            #endif
            enableFramesUIMode(false);
        } else {
            QString msg = "ADD_FRAMES_TABLET";
            if (isCellPhone)
                msg = "ADD_FRAMES_PHONE";
            canvas->notify(TupCanvas::Warning, msg, 2000);
            return;
        }
    } else  { // Change to frames mode
        #if defined(Q_OS_ANDROID)
            if (canvasWasUpdated) {
                setComplementaryFrames();
                TupFrame *currentFrame = frames.at(currentFrameIndex);
                canvas->loadFrame(previousFrame, currentFrame, nextFrame,
                                  QString::number(currentFrameIndex + 1) + "/" + QString::number(framesTotal));
                canvasWasUpdated = false;
            }
        #endif

        mode = TupCanvas::Frames;
        play->setIcon(playIcon);
        #if !defined(Q_OS_ANDROID)
            play->setText(tr("Play"));
        #endif
        enableFramesUIMode(true);
    }

    canvas->setMode(mode);

    if (mode == TupCanvas::Player) {
        #ifdef TUP_DEBUG
            qDebug() << "[TupMainWindow::setAnimationMode()] - Playing animation!";
        #endif
        canvas->renderProject(frames);
        canvas->playAnimation();
    }
}

void TupMainWindow::setComplementaryFrames()
{
    previousFrame = new TupFrame();
    if (currentFrameIndex > 0)
        previousFrame = frames.at(currentFrameIndex - 1);

    nextFrame = new TupFrame();
    if (framesTotal > 1) {
       if (currentFrameIndex < (framesTotal-1))
           nextFrame = frames.at(currentFrameIndex + 1);
    }
}

void TupMainWindow::enableFramesUIMode(bool status)
{
    if (isCellPhone) {
        miniMenu->setEnabled(status);
        undoAction->setEnabled(status);
    } else {
        settings->setEnabled(status);
        drawToolsAction->setEnabled(status);
        frameActions->setEnabled(status);
        penPropertiesAction->setEnabled(status);
        // opacity->setEnabled(status);
        // brush->setEnabled(status);
        help->setEnabled(status);
        #if !defined(Q_OS_ANDROID)
            exit->setEnabled(status);
        #endif
    }

    palette->setEnabled(status);
    fileActions->setEnabled(status);
    setUndoRedoStatus(status);
    getNextFrame->setEnabled(status);

    if (!status)
        getPreviousFrame->setEnabled(status);
    else {
        if (currentFrameIndex > 0)
            getPreviousFrame->setEnabled(status);
    }
}

void TupMainWindow::saveTupFile()
{
    #ifdef TUP_DEBUG
        qDebug() << "[TupMainWindow::saveTupFile()] - Tracing...";
    #endif

    #if defined(Q_OS_ANDROID)
        closeFloatingMenu();
        if (!isCellPhone)
            closeTabletFileMenu();
    #endif

    if (framesTotal == 1 && canvas->isEmpty()) {
        canvas->notify(TupCanvas::Warning, tr("Project is empty. Draw something first!"), 2000);
        #ifdef TUP_DEBUG
            qDebug() << "[TupMainWindow::saveTupFile()] - Project is empty!";
        #endif

        return;
    }

    int w = screenSize.width();
    int h = screenSize.height();
    QString dimension = QString::number(w) + "," + QString::number(h);

#if defined(Q_OS_ANDROID) 
    if (TupProject::createTupFile(project, dimension, frames)) {
        QDesktopServices::openUrl(QString("https://www.tupitube.com"));

        /*
        ShareUtils *resource = new ShareUtils();
        #ifdef TUP_DEBUG
            resource->setLog(TupProject::rootDir());
            qDebug() << "TupMainWindow::saveTupFile() - Sharing source file...";
        #endif

        if (androidVersion >= 10)
            QDesktopServices::openUrl(QString("https://www.tupitube.com"));
        else
            resource->shareMedia(TupProject::rootDir() + "/animation.tup");
        */
    } else {
        #ifdef TUP_DEBUG
            QString msg = "[TupMainWindow::saveTupFile()] - Can't create file ->" + TupProject::rootDir() + "/animation.tup";
            qDebug() << msg;
            #ifdef Q_OS_ANDROID
                logDialog->appendRecord(msg);
            #endif
        #endif
        canvas->notify(TupCanvas::Error, tr("Fatal Error: Can't create TUP file!"), 3000);
    }
#else
    // Implement Desktop method here
    if (TupProject::createTupFile(project, dimension, frames)) {
        QString root = TupProject::rootDir();
        TupPackageHandler pack;
        bool ok = pack.makePackage(root + "/animation", root + "/my_project.tup");
        if (ok) {
            shareFromDesktop(root + "/my_project.tup");
        } else {
            #ifdef TUP_DEBUG
                qDebug() << "TupMainWindow::saveTupFile() - Fatal error: Can't create TUP file";
            #endif
        }
    }
#endif
}

#if defined(Q_OS_ANDROID) && defined(TUP_DEBUG)
void TupMainWindow::openLogFile()
{
    logDialog->loadJavaLog(TupProject::rootDir() + + "/java.log");
    logDialog->showMaximized();
}
#endif
