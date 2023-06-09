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

#include "tuplabel.h"

TupLabel::TupLabel(int index, const QString &text, QWidget *parent) : QLabel(text, parent)
{
    id = index;
    type = NUMBER;
}

TupLabel::TupLabel(int index, const QPixmap &pix, QWidget *parent) : QLabel(parent)
{
    id = index;
    type = NUMBER;
    setPixmap(pix);
}

TupLabel::TupLabel(const QString &color, int index, QWidget *parent) : QLabel(parent)
{
    id = index;
    type = COLOR;
    this->color = color;
    if (color.compare(tr("No Colors")) == 0) {
        setText(color);
    } else {
        QColor name(color);
        setText("  " + QString::number(name.red()) + ", " + QString::number(name.green()) + ", " + QString::number(name.blue()));
    }
}

TupLabel::TupLabel(const QString &color, int index, const QPixmap &pix, QWidget *parent) : QLabel(parent)
{
    id = index;
    type = COLOR;
    this->color = color;
    setPixmap(pix);
}

void TupLabel::mousePressEvent(QMouseEvent *)
{
    if (type == NUMBER)
        emit clicked(id);
    else
        emit colorClicked(id, color);
}
