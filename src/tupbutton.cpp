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

#include "tupbutton.h"
#include <QBoxLayout>

TupButton::TupButton(const QString &iconPath, int w, const QString &text, int fontSize,
                     bool isSplash, Qt::Orientation orientation, QWidget *parent) : QWidget(parent)
{
    QString bg = "255,255,255";
    if (!isSplash)
        bg = "transparent";
    setStyleSheet("QWidget { background-color: " + bg + "; }");

    width = w;
    QBoxLayout *layout;

    if (orientation == Qt::Vertical)
        layout = new QVBoxLayout(this);
    else
        layout = new QHBoxLayout(this);

    int margin = static_cast<int>(w * 0.2);
    QMargins margins = QMargins(margin, margin, margin, margin);
    layout->setContentsMargins(margins);
    // layout->setMargin(static_cast<int>(w * 0.2));

    layout->setSpacing(0);

    if (!iconPath.isEmpty()) {
        button = new QLabel;
        button->setAlignment(Qt::AlignCenter);
        button->setPixmap(QPixmap(iconPath).scaledToWidth(width, Qt::SmoothTransformation));
        layout->addWidget(button, Qt::AlignCenter);
    }

    if (!text.isEmpty()) {
        QLabel *label = new QLabel(text);
        if (fontSize > 0) {
            QFont font = this->font();
            font.setPointSize(fontSize - 2);
            label->setFont(font);
        }
        label->setAlignment(Qt::AlignCenter);
        layout->addWidget(label, Qt::AlignCenter);
        layout->addStretch(0);
    }
}

void TupButton::mousePressEvent(QMouseEvent *)
{
    emit clicked();
}

void TupButton::setPixmap(const QString &iconPath)
{
    button->setPixmap(QPixmap(iconPath).scaledToWidth(width, Qt::SmoothTransformation));
}
