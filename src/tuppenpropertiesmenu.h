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

#ifndef TUPPENPROPERTIESMENU_H
#define TUPPENPROPERTIESMENU_H

#include "tupglobal.h"
#include "tupcheckbox.h"
#include "tuplabel.h"

#include <QDialog>
#include <QBoxLayout>
#include <QKeyEvent>
#include <QSettings>
#include <QFont>

class TupPenPropertiesMenu : public QDialog
{
    Q_OBJECT

    public:
        TupPenPropertiesMenu(QSettings *config, QWidget *parent = nullptr);
        ~TupPenPropertiesMenu();

    protected:
        void keyReleaseEvent(QKeyEvent *event);

    signals:
        void thicknessClicked();
        void transparencyClicked();
        void brushesClicked();

        /*
        void frameAddedForward();
        void frameAddedBack();
        void frameCloned();
        void frameRemoved();
        void frameCleared();
        */

        // void leftArrowClicked();
        // void rightArrowClicked();
        void propertiesMenuClosed();
        void undoRequested();
        void redoRequested();

    private slots:
        void closeMenu();

    private:
        QList<TupCheckBox *> actionsList;
        QList<TupLabel *> labelsList;

        QSize size;
        QWidget *widget;

        QFont textFont;
        QString borderRadius;
        int iconWidth;
        int separator;
};

#endif
