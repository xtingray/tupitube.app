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

#include "tupdownloader.h"

#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QNetworkRequest>
#include <QStringList>
#include <QUrl>
#include <QTimer>

TupDownloader::TupDownloader(const QString &mediaFile, const QString &mediaPath)
{
#ifdef TUP_DEBUG
    qDebug() << "TupDownloader::TupDownloader() - Creating instance of TupDownloader class";
#endif

    url = mediaFile;
    path = mediaPath;
    connect(&manager, SIGNAL(finished(QNetworkReply *)), this, SLOT(saveFile(QNetworkReply *)));
}

void TupDownloader::execute()
{
#ifdef TUP_DEBUG
    qDebug() << "TupDownloader::execute() - Requesting file -> " << url;
#endif

    QUrl resource(url);
    QNetworkRequest request(resource);
    request.setSslConfiguration(QSslConfiguration::defaultConfiguration());
    request.setRawHeader("User-Agent", BROWSER_FINGERPRINT);
    QNetworkReply *reply = manager.get(request);

    /* SQA: Just for debugging purpose
    connect(reply, SIGNAL(downloadProgress(qint64, qint64)),
            this, SLOT(logProgress(qint64, qint64)));
    */

#ifndef QT_NO_SSL
    connect(reply, &QNetworkReply::sslErrors, this, &TupDownloader::sslErrors);
#endif
}

void TupDownloader::saveFile(QNetworkReply *reply)
{
#ifdef TUP_DEBUG
    qDebug() << "TupDownloader::saveFile() - Tracing...";
#endif

    if (reply->error()) {
        #ifdef TUP_DEBUG
            qDebug() << "TupDownloader::saveFile() - Reply error: " << reply->errorString();
        #endif
        emit failed(TupDownloader::DownloadError, tr("Yay! No Internet access? :("));
        return;
    } else {
        QFile file(path);
        QFileInfo info(file);
        QDir dir = info.dir();
        if (!dir.exists()) {
            #ifdef TUP_DEBUG
                qDebug() << "TupDownloader::saveFile() - Project's dir doesn't exist! -> " << dir.absolutePath();
            #endif
            emit failed(TupDownloader::IOError, "04 - " + tr("Project's dir is missing!"));
            return;
        } else {
            #ifdef TUP_DEBUG
                qDebug() << "TupDownloader::saveFile() - Project's dir -> " << dir.absolutePath();
            #endif
        }

        if (!file.open(QIODevice::WriteOnly)) {
            #ifdef TUP_DEBUG
                qDebug() << "TupDownloader::saveFile() - Lack of permissions to write file -> " << path;
            #endif
            emit failed(TupDownloader::IOError, "05 - " + tr("Can't create media file! :("));
            return;
        }

        file.write(reply->readAll());

        qint64 size = file.size();
        if (size == 0) {
            file.remove();
            file.close();
            reply->deleteLater();

            #ifdef TUP_DEBUG
                qDebug() << "TupDownloader::saveFile() - Fatal Error: Media file is empty!";
            #endif
            emit failed(TupDownloader::DownloadError, tr("Wow! Media file is empty! :("));
            return;
        }

        #ifdef TUP_DEBUG
            qDebug() << "TupDownloader::saveFile() - Media file has been saved at -> " << path;
            qDebug() << "TupDownloader::saveFile() - Size file: " << size;
        #endif
        file.close();
        reply->deleteLater();
    }

    emit downloadFinished();
}

void TupDownloader::sslErrors(const QList<QSslError> &)
{
#ifdef TUP_DEBUG
    qDebug() << "TupDownloader::sslErrors() - Tracing SSL errors...";
#endif

#ifndef QT_NO_SSL
    emit failed(TupDownloader::DownloadError, tr("Ouch! No Internet access? :("));
#endif
}

/* SQA: Just for debugging purpose
void TupDownloader::logProgress(qint64 progress, qint64 total)
{
    qDebug() << "Download Progress -> " << progress << " / Total: " << total;
}
*/
