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

#ifndef TUPTOOLSTOPMENU_H
#define TUPTOOLSTOPMENU_H

#include <QDialog>
#include <QSettings>
#include <QKeyEvent>

#include "tupglobal.h"
#include "tupcanvas.h"
#include "tupbutton.h"

class TupToolsTopMenu : public QDialog
{
    Q_OBJECT

    public:
        TupToolsTopMenu(QSettings *settings, QWidget *parent = nullptr);
        ~TupToolsTopMenu();

        void enableButton(TupCanvas::Button type, bool flag);

    signals:
        /*
        void pencilRequested();
        void rectRequested();
        void ellipseRequested();
        void triangleRequested();
        */

        void cloneRequested();
        void undo();
        void redo();
        void color();
        void stroke();
        void opacity();
        void brushes();
        void actions();
        void toolsMenuClosed();

        void callTool(Tool tool);

    protected:
        void keyReleaseEvent(QKeyEvent *);

    private slots:

        void callPenTool();
        void callRectangleTool();
        void callEllipseTool();
        void callTriangleTool();

    private:
        TupButton *undoButton;
        TupButton *redoButton;
};

#endif
