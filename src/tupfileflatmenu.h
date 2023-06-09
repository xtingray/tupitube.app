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

#ifndef TUPFILEFLATMENU_H
#define TUPFILEFLATMENU_H

#include <QDialog>
#include <QSettings>
#include <QKeyEvent>

#include "tupglobal.h"
#include "tupbutton.h"

class TupFileFlatMenu : public QDialog
{
    Q_OBJECT

    public:
        TupFileFlatMenu(QSettings *config, QWidget *parent = nullptr);
        ~TupFileFlatMenu();

    signals:
        void postRequested();
        void settings();
        void newProject();
        void works();
        void tupFile();
        void help();
        void fileMenuClosed();        
        #if defined(Q_OS_ANDROID) && defined(TUP_DEBUG)
            void openLog();
        #endif

    protected:
        void keyReleaseEvent(QKeyEvent *event);
};

#endif
