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

#include <QApplication>
#include <QCoreApplication>
#include <QResource>
#include <QTranslator>
#include <QLocale>

#include "tupmainwindow.h"

int main(int argc, char ** argv)
{
    QApplication app(argc, argv);
    QResource::registerResource("resources.qrc");

    QString locale = QString(QLocale::system().name());
    if (!locale.isEmpty()) {
        #ifdef TUP_DEBUG
            qDebug() << "[main.cpp] - LANG: " << locale;
        #endif
        locale = locale.left(2);

        QStringList langSupport;
        // langSupport << "es" << "pt" << "jp" << "cn" << "fr";
        langSupport << "es" << "pt";
        if (locale.compare("en") != 0 && langSupport.contains(locale)) {
           QTranslator *translator = new QTranslator;
           if (translator->load(":resources/" + locale + ".qm"))
               app.installTranslator(translator);
        }
    }

    /*
    QTranslator *translator = new QTranslator;
    translator->load(":resources/zh.qm");
    app.installTranslator(translator);
    */

    TupMainWindow window;
    return app.exec();
}
