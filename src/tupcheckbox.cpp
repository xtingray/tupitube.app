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

#include "tupglobal.h"
#include "tupcheckbox.h"

#include <QBoxLayout>
#include <QPainter>

TupCheckBox::TupCheckBox(int index, const QString &iconPath, int width, const QString &themeColor,
                         const QString &borderRadius, QWidget *parent) : QWidget(parent)
{
    setStyleSheet("* { background-color: rgb(" + themeColor + "); "
                  "border-radius: " + borderRadius + "px; }");
    imgWidth = static_cast<int>(width * 0.7);
    id = index;

    QBoxLayout *layout = new QHBoxLayout(this);
    button = new QLabel;
    button->setPixmap(QPixmap(iconPath).scaledToWidth(imgWidth, Qt::SmoothTransformation));
    layout->addWidget(button);

    previousClick = QTime::currentTime();
}

TupCheckBox::TupCheckBox(int index, QPixmap pixmap, int width, const QString &themeColor,
                         const QString &borderRadius, QWidget *parent) : QWidget(parent)
{
    setStyleSheet("* { background-color: rgb(" + themeColor + "); "
                  "border-radius: " + borderRadius + "px; }");
    imgWidth = static_cast<int>(width * 0.7);
    id = index;
    QBoxLayout *layout = new QHBoxLayout(this);

    button = new QLabel;
    button->setPixmap(pixmap.scaledToWidth(imgWidth, Qt::SmoothTransformation));
    layout->addWidget(button);

    previousClick = QTime::currentTime();
}

TupCheckBox::TupCheckBox(const QString &index, QPixmap pixmap, const QString &themeColor,
                         const QString &borderRadius, QWidget *parent) : QWidget(parent)
{
    setStyleSheet("* { background-color: rgb(" + themeColor + "); "
                  "border-radius: " + borderRadius + "px; }");
    code = index;
    id = -1;

    QBoxLayout *layout = new QVBoxLayout(this);

    button = new QLabel;
    button->setAlignment(Qt::AlignCenter);

    unselectedImg = pixmap;
	button->setPixmap(pixmap);
    layout->addWidget(button);

    QPainter painter(&pixmap);
    painter.setBrush(QColor(0, 196, 255, 180));
    painter.drawRect(0, 0, pixmap.width() - 1, pixmap.height() - 1);
    selectedImg = pixmap;

    previousClick = QTime::currentTime();
}

void TupCheckBox::updateImage(const QString &iconPath)
{
    button->setPixmap(QPixmap(iconPath).scaledToWidth(imgWidth, Qt::SmoothTransformation));
}

void TupCheckBox::mousePressEvent(QMouseEvent *)
{
    QTime currentClick = QTime::currentTime();
    if (currentClick.msecsTo(previousClick) >= -300) {
        emit opened(code);
    } else {
        if (id != -1)
            emit clicked(id);
        else
            emit selected(code);
    }
    previousClick = currentClick;
}

void TupCheckBox::setChecked(bool flag)
{
    checkFlag = flag;
}

bool TupCheckBox::isChecked()
{
    return checkFlag;
}

#ifdef Q_OS_IOS
void TupCheckBox::updateBgColor(bool enable)
{
    if (enable)
        button->setStyleSheet("* { background-color: rgb(210,210,210); }");
    else
        button->setStyleSheet("* { background-color: rgb(180,180,180); }");
}
#endif

void TupCheckBox::updateSelection(bool selected)
{
    if (selected)
        button->setPixmap(selectedImg);
    else
        button->setPixmap(unselectedImg);
}

int TupCheckBox::index()
{
    return id;
}
