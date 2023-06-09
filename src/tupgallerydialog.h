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

#ifndef TUPGALLERYDIALOG_H
#define TUPGALLERYDIALOG_H

#include <QDialog>
#include <QBoxLayout>
#include <QScrollArea>
#include <QSettings>
#include <QLabel>
#include <QHash>
#include <QResizeEvent>

#include "tupglobal.h"
#include "tupcheckbox.h"
#include "tupbutton.h"

class TupGalleryDialog : public QDialog
{
    Q_OBJECT

    public:
        TupGalleryDialog(QSettings *config, QWidget *parent = nullptr);
        ~TupGalleryDialog();

    protected:
        void resizeEvent(QResizeEvent *event);
        void mousePressEvent(QMouseEvent *event);

    signals:
        void openCall(const QString &id);
        void remove(const QString &id);
        void shareCall(const QString &id);

    private slots:
        void loadWorks();
        void updateSelection(const QString &project);
        void openProject();
        void openSelectedProject(const QString &code);

        // void shareProject();
        void confirmRemoveAction();
        void closeMessageDialog();
        void removeProject();

    private:
        void wideGalleryPage();
        void smallGalleryPage();
        void enableButtonsPanel(bool flag);
        QStringList projectsList(const QString &dir);

        // QSettings *config;
        bool isCellPhone;
        bool lowRes;
        QString themeColor;
        int titleSize;
        int titleHeight;
        int textSize;
        QString borderRadius;
        int iconWidth;
        QFont textFont;
        int rowSize;
        int dialogW;
        int dialogH;

        QLabel *titleLabel;
        QLabel *loadingLabel;
        QWidget *worksPanel;
        QScrollArea *workArea;
        QWidget *innerWidget;
        QWidget *buttonSeparator;

        TupButton *openButton;
        TupButton *closeButton;
        // TupButton *shareButton;
        TupButton *trashButton;

        QString id;
        QBoxLayout *contentLayout;
        int worksTotal;

        QWidget *msgWidget;
        QWidget *buttonsWidget;

        QHash<QString, QWidget *> widgetList;
        QHash<QString, TupCheckBox *> checkBoxList;
        QHash<QString, QPixmap> pixPreviewList;
};

#endif
