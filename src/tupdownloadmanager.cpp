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

#include "tupdownloadmanager.h"
#include "tupdownloader.h"

TupDownloadManager::TupDownloadManager(const QString &mediaFile, const QString &mediaPath)
{
#ifdef TUP_DEBUG
    qDebug() << "TupDownloaderManager::TupDownloaderManager() - Creating instance of TupDownloaderManager class";
#endif

    thread = new QThread(this);

    TupDownloader *downloader = new TupDownloader(mediaFile, mediaPath);
    downloader->moveToThread(thread);

    connect(thread, &QThread::finished, downloader, &TupDownloader::deleteLater);
    connect(downloader, &TupDownloader::failed, this, &TupDownloadManager::failed);
    connect(downloader, SIGNAL(downloadFinished()), this, SIGNAL(downloadFinished()));

    thread->start();
    downloader->execute();
}

TupDownloadManager::~TupDownloadManager()
{
    thread->quit();
    thread->wait();
}
