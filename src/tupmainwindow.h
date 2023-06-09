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

#ifndef TUPMAINWINDOW_H
#define TUPMAINWINDOW_H

#include "tupglobal.h"
#include "tupproject.h"
#include "tuppathitem.h"
#include "tupcanvas.h"

#include "tupdrawtoolsmenu.h"
#include "tuptoolsflatmenu.h"
#include "tuptoolstopmenu.h"
#include "tupfileflatmenu.h"
#include "tupfiletopmenu.h"
#include "tuptabletfilemenu.h"
#include "tuppenpropertiesmenu.h"
#include "tupframesmenu.h"

#include "tuphelpdialog.h"
#include "tupthicknessdialog.h"
#include "tupopacitydialog.h"
#include "tupcolordialog.h"
#include "tupbrushdialog.h"
#include "tupnethandler.h"
#include "tuppostdialog.h"
#include "tupsettings.h"

#include <QMainWindow>
#include <QToolBar>
#include <QAction>
#include <QSettings>
#include <QDockWidget>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

#if defined(Q_OS_ANDROID) && defined(TUP_DEBUG)
#include "tuplogdialog.h"
#endif

class TupMainWindow : public QMainWindow
{
    Q_OBJECT

    public:
        enum Mode { Canvas = 0, Player };
        enum PostMode { Anonymous = 0, LoggedUser };
        enum FormMode { Post = 0, Mode, SignUp, LoginForm, Login, Send };

        TupMainWindow();
        ~TupMainWindow(); 

    protected:
        void resizeEvent(QResizeEvent *event);
        void keyPressEvent(QKeyEvent *event);

    signals:
        void cursorPositionChanged();

    private slots:
        void checkInternetConnection();
        // void backgroundsDialog();
        void setCanvasBg(const QString &bgId);
        void penPropertiesDialog();
        void framesDialog();
        void undo();
        void redo();
        void showDrawingToolsDialog();
        void showColorDialog();
        void showPenSizeDialog();
        void showOpacityDialog();
        void showBrushesDialog();

        void setCurrentTool(Tool tool);
        void handleMetadataDialog();

        void shareWork();
        void postAnonymousWork();
        void postWork();
        void processPostRequest();
        void sendData();
        void sendPackage();
        void slotError(QNetworkReply::NetworkError error);
        void showHelpDialog();

        void showToolsMenu();
        void showFileMenu();
        void showTabletFileMenu();

        void openWorksGallery();
        void showSettingsDialog();
        void updateSettings();
        void shareResources(const QString &url);

        void updatePenSize(int size);
        void updateColors(const QColor, const QColor, const QColor);
        void updateOpacities(double, double);
        void updateBrushes(Qt::BrushStyle, Qt::BrushStyle);

        void showNetError(const QString &msg);
        void saveFrame();

        void enableUI(bool flag);
        void enableButton(TupCanvas::Button type, bool flag);
        void goOneFrameBack();
        void goOneFrameForward();
        void setAnimationMode();

        void enableToolbar();
        void openProject(const QString &id);
        // void shareProject(const QString &id);
        void removeProject(const QString &id);
        void cloneCurrentFrame();

        void welcomeMsg();

        void processSignUpAnswer(QNetworkReply *reply);
        void processSignInAnswer(QNetworkReply *reply);
        void formatEmail();

        void saveAndSetNewProject();
        void saveTupFile();

        void insertNewFrameForward();
        void insertNewFrameBack();
        void removeCurrentFrame();
        void clearCanvas();

        void showExitDialog();
        void executeExitAction();

    private:
        void setGUIParams();
        void setScene();
        void loadAnimation();
        void loadFrames();
        void setBrushesGrid();
        void setToolBar();
        void setCanvas();
        void setMetadataWidget();
        int getPenSize();
        double getBorderOpacity();
        double getFillOpacity();

        QColor getBorderColor();
        QColor getFillColor();
        Qt::BrushStyle getBorderBrushStyle();
        Qt::BrushStyle getFillBrushStyle();
        float getSmoothLevel();
        bool getPreviousFrameVisibility();
        bool getNextFrameVisibility();

        int getFPS();
        bool animationIsEmpty();
        void createColorPalettes();
        void loadCurrentFrame();
        void savePreviousProject(bool openAction);

        void setUndoRedoStatus(bool enable);
        void checkUndoRedoStatus(TupFrame *currentFrame);
        void setComplementaryFrames();
        void enableFramesUIMode(bool status);
        void goToFrame(int index);

        void addNewFrameForward(TupFrame *frame, bool copy);
        void updateFocus();
        void updateMetadata();
        void resetProjectUI();
        QString formatPromoComment();

        void copyCurrentFrame();
        void pasteFrameForward();
        // void showLoginForm();

        void checkTitle();
        void checkTags();

        void handleEmptyProject();
        void closeTabletFileMenu();
        void setToolbarVisible(bool flag);
        void closePenMenu();

#if defined(Q_OS_ANDROID)
        void closeFloatingMenu();
        void callSocialNetworks(const QString &url);
        void openLogFile();
#else
        void shareFromDesktop(const QString &url);
#endif

        void validateCredentials(const QString &username, const QString &passwd);

        // Global variables
        double androidVersion;
        double diagonal;
        bool isCellPhone;
        bool lowRes;
        bool hasInternet;
        QString system;
        QString imgPath;
        int iconSize;
        int iconWidth;
        int smallIconSize;
        QString barSpacing;
        QString borderRadius;

        TupCanvas::Mode mode;
        PostMode postMode;
        TupCanvas *canvas;
        bool canvasWasUpdated;

        QColor bgColor;

        QGraphicsScene *scene;
        QToolBar *toolbar;
        TupToolsFlatMenu *toolsFlatMenu;
        TupToolsTopMenu *toolsTopMenu;
        TupFileFlatMenu *fileFlatMenu;
        TupFileTopMenu *fileTopMenu;
        TupDrawToolsMenu *drawMenu;
        TupPenPropertiesMenu *penMenu;
        TupFramesMenu *framesMenu;
        TupTabletFileMenu *tabletFileMenu;

        TupHelpDialog *helpDialog;
        TupThicknessDialog *thicknessDialog;
        TupOpacityDialog *opacityDialog;
        TupBrushDialog *brushDialog;
        TupColorDialog *colorDialog;
        TupSettings *settingsDialog;

        QPen pen;
        QSettings *config;
        QSettings *project;
        QDockWidget *dockwidget;

        double borderOpacity;
        double fillOpacity;

        QSize screenSize;
        int androidW;
        int androidH;
        QPixmap *colorPalette;

        int projectsIndex;
        QString currentProjectID;
        QList<TupFrame *> frames;
        TupFrame *frameCopy;
        int currentFrameIndex;
        int framesTotal;
        int fps;
        int onion;
        QString frameKey;

        QDomDocument animDoc;
        QLineEdit *titleEdit;
        QPlainTextEdit *tagsEdit;
        QPlainTextEdit *editor;

        QLabel *postHeader;
        QLineEdit *emailEdit;
        QLineEdit *usernameEdit;
        QLineEdit *passwdEdit;

        QWidget *modePanel;
        QWidget *signUpPanel;
        QWidget *loginPanel;
        QWidget *postPanel;
        FormMode formStatus;

        QNetworkAccessManager *manager;
        QNetworkRequest request;
        TupButton *shareButton;

        QString defaultComment;

        QIcon penIcon;
        QIcon rectangleIcon;
        QIcon ellipseIcon;
        QIcon triangleIcon;

        QAction *miniMenu;
        QAction *drawToolsAction;
        QAction *frameActions;
        QAction *undoAction;
        QAction *redoAction;
        QAction *palette;
        QAction *penPropertiesAction;
        QAction *getPreviousFrame;
        QAction *getNextFrame;
        QAction *play;
        QAction *works;
        QAction *post;
        QAction *settings;
        QAction *help;
        QAction *fileActions;
        QAction *exit;

        TupFrame *previousFrame;
        TupFrame *nextFrame;

        QIcon stopIcon;
        QIcon playIcon;
        QIcon fileIcon;
        QAction *blankAction;

        int textSize;
        bool notifying;

        TupPostDialog *postDialog;
        QStringList colorHistoryList;

#if defined(Q_OS_ANDROID) && defined(TUP_DEBUG)
        TupLogDialog *logDialog;
#endif
};

#endif
