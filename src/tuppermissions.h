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

#ifndef TUPPERMISSIONS_H
#define TUPPERMISSIONS_H

#include "tupglobal.h"

#include <QObject>
#ifdef Q_OS_ANDROID
    // #include <QAndroidJniObject>
    // #include <QAndroidJniEnvironment>
    // #include <QtAndroid>
    #include <QJniObject>
    #include <QJniEnvironment>
    #include <QtCore/private/qandroidextras_p.h>
#endif

class TupPermissions : public QObject
{
    Q_OBJECT
public:
    explicit TupPermissions(QObject *parent = nullptr);

    // Method to request permissions
    void requestExternalStoragePermission();

    // Method to get the permission granted state
    bool getPermissionResult();

public slots:

private:

    // Variable indicating if the permission to read / write has been granted
    int permissionResult;  //  true - "Granted", false - "Denied"

#if defined(Q_OS_ANDROID)
    // Object used to obtain permissions on Android Marshmallow
    // QAndroidJniObject ShowPermissionRationale;
    QJniObject ShowPermissionRationale;
#endif

};

#endif // TUPPERMISSIONS_H
