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

#ifndef TUPPROJECT_H
#define TUPPROJECT_H

#include "tupglobal.h"
#include "tupframe.h"

#include <QObject>
#include <QSettings>
#include <QList>
#include <QPixmap>

class TupProject : public QObject
{
    Q_OBJECT

    public:
        TupProject(const QString &id);
        static bool save(const QString &id, const QSettings *project, const QPixmap &pixmap);

        QSettings * parameters();
        bool loadIsOk();
        static bool firstTime();
        static QString rootDir();
        static QString projectsDir();
        static bool makeDirs();
        static int nextProjectID();
        static bool remove(const QString &id);
        static bool imageFileExists(const QString &id);
        static bool videoFileExists(const QString &id);

        static bool colorPaletteExist();
        static bool saveColorPalette(const QPixmap &palette);
        static bool createTupFile(QSettings *project, const QString &dimension, QList<TupFrame *> frames);

    private:
        bool isOk;
        QSettings *settings;
};

#endif // TUPPROJECT_H
