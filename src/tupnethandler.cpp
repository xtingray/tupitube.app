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

#include "tupnethandler.h"
#include "tupdownloader.h"
#include "tupglobal.h"
#include "tupproject.h"

#include <QDir>
#include <QDesktopServices>

static int maxTries = 5;

TupNetHandler::TupNetHandler() : QObject()
{
#ifdef TUP_DEBUG
    qDebug() << "[TupNetHandler::TupNetHandler()]";
#endif

    tries = 0;
    wasCancelled = false;
}

TupNetHandler::~TupNetHandler()
{
}

void TupNetHandler::setProjectID(const QString id)
{
    projectID = id;
}

void TupNetHandler::sendPackage(const QDomDocument &doc)
{
    QDomElement root = doc.documentElement();
    projectType = root.attribute("type");
    message = doc.toString();

    /* SQA: Only for debugging purpose
#ifdef TUP_DEBUG
    qDebug() << "TupNetHandler::sendPackage() - Package:";
    qDebug() << message;
#endif
    */

    initSocket();
}

void TupNetHandler::initSocket()
{
#ifdef TUP_DEBUG
    qDebug() << "TupNetHandler::initSocket() - Tracing...";
#endif

    socket = new QTcpSocket;
    socket->setSocketOption(QAbstractSocket::LowDelayOption, 1);
    socket->connectToHost(MEDIA_URL, 8080, QIODevice::ReadWrite);

    connect(socket, SIGNAL(connected()), this, SLOT(sendRequest()));
    connect(socket, SIGNAL(readyRead()), this, SLOT(readFromServer()));
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(catchSocketError(QAbstractSocket::SocketError)));
}

void TupNetHandler::sendRequest()
{
#ifdef TUP_DEBUG
    qDebug() << "TupNetHandler::sendRequest() - Socket state: " << socket->state();
#endif

    if (socket->state() == QAbstractSocket::ConnectedState) {
        #ifdef TUP_DEBUG
            qDebug() << "TupNetHandler::sendRequest() - Connected!";
            qDebug() << "TupNetHandler::sendRequest() - Sending project metadata...";
        #endif
        QTextStream stream(socket);
        stream << message.toLocal8Bit().toBase64() << "%%" << Qt::endl;
    } else {
        #ifdef TUP_DEBUG
            qDebug() << "TupNetHandler::sendRequest() - Can´t connect to server! :(";
        #endif
        showNetError(TupDownloader::SocketError, tr("Wow! No Internet access? :("));
    }

#ifdef TUP_DEBUG
    qDebug() << "TupNetHandler::sendRequest() - Waiting for a response...";
#endif
}

void TupNetHandler::readFromServer()
{
#ifdef TUP_DEBUG
     qDebug() << "TupNetHandler::readFromServer() - Getting url from server...";
#endif

    QString url;
    while (socket->canReadLine()) {
        url += socket->readLine();
        if (url.endsWith("%%\n"))
            break;
    }

    if (!url.isEmpty()) {
        url.remove(url.lastIndexOf("%%"), 2);
        url = QString::fromLocal8Bit(QByteArray::fromBase64(url.toLocal8Bit()));
    } else {
        showNetError(TupDownloader::SocketError, "Answer from server is empty!");
        return;
    }

#if defined(Q_OS_ANDROID)
    double androidVersion = QSysInfo::productVersion().toDouble();
    if (androidVersion < 10)
        launchDownloader(url);
    else
        QDesktopServices::openUrl(url);
#else
#endif

    socket->disconnectFromHost();
}

void TupNetHandler::launchDownloader(const QString &url)
{
    // Downloading media file from server
    filename = TupProject::projectsDir() + "/" + projectID;

    QDir dir(filename);
    if (!dir.exists()) {
        if (!dir.mkpath(filename)) {
            #ifdef TUP_DEBUG
                qDebug() << "TupNetHandler::launchDownloader() - "
                            "Fatal Error: can't create directory -> " << filename;
            #endif
            showNetError(TupDownloader::IOError, "03 - Failed Path: " + filename);
            return;
        }
    }

    if (projectType == "image")
        filename += "/image.png";
    else
        filename += "/video.mp4";

    #ifdef TUP_DEBUG
        qDebug() << "TupNetHandler::launchDownloader() - Downloading media file from server...";
        qDebug() << "TupNetHandler::launchDownloader() - and saving at: " << filename;
    #endif

    TupDownloadManager *downloadManager = new TupDownloadManager(url, filename);
    connect(downloadManager, SIGNAL(failed(int, const QString &)),
            this, SLOT(showNetError(int, const QString &)));
    connect(downloadManager, SIGNAL(downloadFinished()), this, SLOT(shareFile()));
}

void TupNetHandler::catchSocketError(QAbstractSocket::SocketError error)
{
#ifdef TUP_DEBUG
    qDebug() << "TupNetHandler::catchSocketError() - QAbstractSocket::SocketError -> " << error;
#else
    Q_UNUSED(error)
#endif

    tries++;
    if (tries > maxTries) {
        socket->close();
        QThread::msleep(2000);
        initSocket();
    } else {
        #ifdef TUP_DEBUG
            qDebug() << "TupNetHandler::catchSocketError() - Fatal Error: Is TupiTube service down? :(";
        #endif
        showNetError(TupDownloader::SocketError, tr("Hey! No Internet access? :("));
    }
}

void TupNetHandler::showNetError(int errorType, const QString &desc)
{
    emit netError(desc);

    if (errorType == TupDownloader::SocketError) {
        disconnect(socket, SIGNAL(readyRead ()), this, SLOT(readFromServer()));
        disconnect(socket, SIGNAL(error(QAbstractSocket::SocketError)),
                   this, SLOT(catchSocketError(QAbstractSocket::SocketError)));
        socket->close();
    }
}

void TupNetHandler::cancelPost()
{
#ifdef TUP_DEBUG
    qDebug() << "TupNetHandler::cancelPost() - Post request has been cancelled by user! :O";
#endif
    wasCancelled = true;
    tries = 0;
}

void TupNetHandler::shareFile()
{
#ifdef TUP_DEBUG
    qDebug() << "TupNetHandler::shareFile() - Download has finished, now sharing media url! :D";
#endif
    if (!wasCancelled) {
        emit postReady(filename);
    } else {
        if (QFile::exists(filename)) {
            bool removed = QFile::remove(filename);
            if(!removed) {
                #ifdef TUP_DEBUG
                    qWarning() << "TupNetHandler::cancelPost() - Fatal Error: "
                                  "Failed to remove file -> " << filename;
                #endif
            }
        }
    }
}
