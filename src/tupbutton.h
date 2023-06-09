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

#ifndef TUPBUTTON_H
#define TUPBUTTON_H

#include "tupglobal.h"

#include <QWidget>
#include <QMouseEvent>
#include <QLabel>

class TupButton : public QWidget
{
    Q_OBJECT

    public:
        TupButton(const QString &iconPath, int width, const QString &text = QString(), int fontSize = 0,
                  bool isSplash = false, Qt::Orientation orientation = Qt::Vertical, QWidget *parent = nullptr);
        void setPixmap(const QString &iconPath);

    protected:
        void mousePressEvent(QMouseEvent *);

    signals:
        void clicked();

    private:
        QLabel *button;
        int width;
};

#endif // TUPBUTTON_H
