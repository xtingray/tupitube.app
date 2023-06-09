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

#ifndef TUPPOSTDIALOG_H
#define TUPPOSTDIALOG_H

#include <QDialog>
#include <QBoxLayout>
#include <QLabel>
#include <QTextBrowser>
#include <QFont>
#include <QResizeEvent>
#include <QSettings>
#include <QTimer>

#include "tupglobal.h"
#include "tupcheckbox.h"
#include "tuplabel.h"
#include "tupbutton.h"

class TupPostDialog : public QDialog
{
    Q_OBJECT

    public:
        enum OP {SUM, REST};
        TupPostDialog(QSettings *settings, QWidget *parent = nullptr);
        ~TupPostDialog();

        void startPost();
        void activateProgressUI();

    protected:
        void resizeEvent(QResizeEvent *);

    signals:
        void confirmationFlagChanged(bool);
        void postActionStarted();
        void transmissionCancelled();

    private slots:
        void executePost();
        void updateProgressLabel();

    public slots:
        void cancelPost();

    private:
        void setDialogTitle();
        void setProgressUI();
        void setDialogButtonsPanel();
        void enableOption(bool);

        QSettings *settings;
        int w;
        int h;

        QWidget *titleWidget;
        QTextBrowser *mainLine;

        QHBoxLayout *checkLayout;
        QHBoxLayout *bottomLayout;

        bool isCellPhone;
        QString imgPath;
        int textSize;
        int iconWidth;
        QString borderRadius;
        int postSpacing;

        QFont dialogFont;
        QLabel *dialogTitle;
        QWidget *progressWidget;

        QLabel *progressLabel;
        QList<QPixmap> pixmaps;
        QTimer *progressTimer;
        int counter;
        OP operation;

        TupButton *okButton;
        TupButton *closeButton;
};

#endif
