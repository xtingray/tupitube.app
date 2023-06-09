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

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QSslError>
#include <QList>

class TupDownloader: public QObject
{
    Q_OBJECT

    public:
        enum ErrorType { SocketError = 0, DownloadError, IOError  };
        TupDownloader(const QString &mediaFile, const QString &mediaPath);

    signals:
       void failed(int errorType, const QString &desc);
       void downloadFinished();

    public slots:
        void execute();
        void saveFile(QNetworkReply *reply);
        void sslErrors(const QList<QSslError> &);
        // void logProgress(qint64 progress, qint64 total);

    private:
        QNetworkAccessManager manager;
        QString url;
        QString path;
};

// Q_DECLARE_METATYPE(TupDownloader::ErrorType)
