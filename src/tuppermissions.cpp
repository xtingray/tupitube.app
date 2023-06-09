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

#include "tuppermissions.h"
#include <QMessageBox>
#include <QApplication>

TupPermissions::TupPermissions(QObject *parent) : QObject(parent)
{
}

// Method to request permissions
void TupPermissions::requestExternalStoragePermission()
{
    #if defined(Q_OS_ANDROID)
        auto r = QtAndroidPrivate::checkPermission("android.permission.WRITE_EXTERNAL_STORAGE").result();
        if (r == QtAndroidPrivate::Denied)
        {
            r = QtAndroidPrivate::requestPermission("android.permission.WRITE_EXTERNAL_STORAGE").result();
            if (r == QtAndroidPrivate::Denied)
                this->permissionResult = false;
        }
        this->permissionResult = true;

        /*
        QtAndroid::PermissionResult request = QtAndroid::checkPermission("android.permission.WRITE_EXTERNAL_STORAGE");
        if (request == QtAndroid::PermissionResult::Denied) {
            QtAndroid::requestPermissionsSync(QStringList() <<  "android.permission.WRITE_EXTERNAL_STORAGE");
            request = QtAndroid::checkPermission("android.permission.WRITE_EXTERNAL_STORAGE");

            if (request == QtAndroid::PermissionResult::Denied) {
                this->permissionResult = false;
                if (QtAndroid::shouldShowRequestPermissionRationale("android.permission.WRITE_EXTERNAL_STORAGE")) {
                    ShowPermissionRationale = QAndroidJniObject("com/maefloresta/QShowPermissionRationale",
                                                                "(Landroid/app/Activity;)V",
                                                                 QtAndroid::androidActivity().object<jobject>());
                    // Checking for errors in the JNI
                    QAndroidJniEnvironment env;
                    if (env->ExceptionCheck()) {
                        // Handle exception here.
                        env->ExceptionClear();
                    }
                }
            } else {
                this->permissionResult = true;
            }
        } else {
            this->permissionResult = true;
        }
        */
    #else
        this->permissionResult = false;
    #endif
}

// Method to get the permission granted state
bool TupPermissions::getPermissionResult()
{
    return permissionResult;
}
