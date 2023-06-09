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

#ifndef TUPEXITDIALOG_H
#define TUPEXITDIALOG_H

#include "tupglobal.h"

#include <QDialog>
#include <QBoxLayout>
#include <QSettings>
#include <QFont>

class TupExitDialog : public QDialog
{
    Q_OBJECT

    public:
        enum Actions { Back = 0, Share, SaveAndNew, Quit };
        TupExitDialog(QSettings *settings, QWidget *parent = nullptr);
        ~TupExitDialog();

    protected:
        void mousePressEvent(QMouseEvent *event);

    private slots:
        void updateSelection(int index);

    private:
        QHBoxLayout * addExitItem(Actions action, const QString &imgPath, const QString &title);

        QSettings *settings;
        QFont textFont;
        QWidget *centralWidget;
        QString borderRadius;
        int iconWidth;
        int separator;
        bool isCellPhone;
        QSize size;
};

#endif
