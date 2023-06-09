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

#ifndef TUPCHECKBOX_H
#define TUPCHECKBOX_H

#include <QWidget>
#include <QLabel>
#include <QMouseEvent>
#include <QTime>

class TupCheckBox : public QWidget
{
    Q_OBJECT

    public:
        TupCheckBox(int index, const QString &iconPath, int width, const QString &themeColor,
                    const QString &borderRadius, QWidget *parent = nullptr);
        TupCheckBox(int index, QPixmap pixmap, int width, const QString &themeColor,
                    const QString &borderRadius, QWidget *parent = nullptr);
        TupCheckBox(const QString &index, QPixmap pixmap, const QString &themeColor,
                    const QString &borderRadius, QWidget *parent = nullptr);

        void updateImage(const QString &iconPath);
        void setChecked(bool flag);
        bool isChecked();
        void updateSelection(bool selected);
        int index();

        #ifdef Q_OS_IOS
        void updateBgColor(bool enable);
        #endif

    protected:
        void mousePressEvent(QMouseEvent *);

    signals:
        void clicked(int id);
        void selected(const QString &code);
        void opened(const QString &code);

    private:
        int imgWidth;
        int id;
        QPixmap selectedImg;
        QPixmap unselectedImg;

        QString code;
        QLabel *button;
        bool checkFlag;
        QTime previousClick;
};

#endif // TUPCHECKBOX_H
