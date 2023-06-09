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

#ifndef TUPHELPDIALOG_H
#define TUPHELPDIALOG_H

#include "tupglobal.h"

#include <QDialog>
#include <QTabWidget>
#include <QBoxLayout>
#include <QSettings>

class TupHelpDialog : public QDialog
{
    Q_OBJECT

    public:
        TupHelpDialog(QSettings *config, QWidget *parent = nullptr);
        ~TupHelpDialog();

        void updateInternetFlag(bool flag);

    protected:
        void mousePressEvent(QMouseEvent *event);

    private slots:
        void handleEvents(int index);

    private:
        QWidget * helpPage();
#ifdef Q_OS_WIN
        QWidget * shortcutsPage();
#endif
        QWidget * smallHelpPage();
        QHBoxLayout * addHelpItem(const QString &iconPath, const QString &labelText);
        QWidget * aboutPage();

        bool isCellPhone;
        QString imgPath;
        QWidget *widget;
        bool lowRes;
        int titleSize;
		int titleHeight;
        int textSize;
        int helpTextSize;
        QString borderRadius;
        int iconWidth;
        QFont textFont;
        QSize size;

        QTabWidget *tabWidget;
};

#endif
