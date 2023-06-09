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

#ifndef TUPNETHANDLER_H
#define TUPNETHANDLER_H

#include "tupglobal.h"
#include "tupdownloadmanager.h"

#include <QObject>
#include <QTcpSocket>
#include <QDomDocument>
#include <QFile>

class TupNetHandler : public QObject
{
    Q_OBJECT

    public:
        enum AndroidVersion { Standard = 1 };
        TupNetHandler();
        ~TupNetHandler();

        void setProjectID(const QString id);
        void sendPackage(const QDomDocument &doc);

    signals:
        void postReady(const QString &url);
        void netError(const QString &msg);

    private slots:
        void sendRequest();
        void readFromServer();
        void catchSocketError(QAbstractSocket::SocketError error);
        void shareFile();
        void showNetError(int errorType, const QString &desc);

    public slots:
        void cancelPost();

    private:
        void initSocket();
        void launchDownloader(const QString &url);

        QTcpSocket *socket;
        QString message;
        int tries;
        AndroidVersion formatVersion;
        QString projectID;
        QString filename;
        QFile *mediaFile;
        QString projectType;
        bool wasCancelled;
};

#endif
