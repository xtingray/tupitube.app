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

#include "tuppackagehandler.h"
#include "quazip/quazip.h"
#include "quazip/quazipfile.h"
#include "quazip/JlCompress.h"

#include <QFile>

TupPackageHandler::TupPackageHandler()
{
}

TupPackageHandler::~TupPackageHandler()
{
}

bool TupPackageHandler::makePackage(const QString &projectPath, const QString &packagePath)
{
    #ifdef TUP_DEBUG
        qWarning() << "TupPackageHandler::makePackage() - projectPath -> " + projectPath;
        qWarning() << "TupPackageHandler::makePackage() - packagePath -> " + packagePath;
    #endif

    if (!QFile::exists(projectPath)) {        
        #ifdef TUP_DEBUG
            qWarning() << "TupPackageHandler::makePackage() - Project path doesn't exist -> " + projectPath;
        #endif

        return false;
    }

    #ifdef TUP_DEBUG
        qDebug() << "TupPackageHandler::makePackage() - Calling JlCompress library...";
    #endif
    return JlCompress::compressDir(packagePath, projectPath, true);
}

/* SQA: This could be handy in the future
bool TupPackageHandler::importPackage(const QString &packagePath)
{
    #ifdef TUP_DEBUG
        qDebug() << "TupPackageHandler::importPackage() - packagePath -> " + packagePath;
    #endif

    QFileInfo file(packagePath);
    projectDir = file.baseName();
    QStringList list = JlCompress::extractDir(packagePath, CACHE_DIR);
    if (list.size() == 0) {
        #ifdef TUP_DEBUG
            qDebug() << "TupPackageHandler::importPackage() - Project file is empty! -> " + packagePath;
        #endif

        return false;
    }

    QString path = list.at(0);
    int index = path.indexOf("/", CACHE_DIR.length());
    gPath = path.left(index);

    return true;
}
*/

QString TupPackageHandler::importedProjectPath() const
{
    #ifdef TUP_DEBUG
        qDebug() << "TupPackageHandler::importedProjectPath() - project path -> " + gPath;
    #endif

    return gPath;
}

QString TupPackageHandler::projectDirectory() const
{
    return projectDir;
}
