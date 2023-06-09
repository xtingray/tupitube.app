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

#include "tupproject.h"

#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QXmlStreamWriter>
#include <QDomDocument>
#include <QDirIterator>
#include <QTextStream>

// Constructor to load a project
TupProject::TupProject(const QString &id) : QObject()
{
    isOk = false;

    QString dir = TupProject::projectsDir();
    dir += "/" + id;

    if (!QDir(dir).exists()) {
        #ifdef TUP_DEBUG
            qDebug() << "TupProject() -> Fatal Error: invalid project id -> " << id;
        #endif
        return;
    }

#ifdef TUP_DEBUG
    qDebug() << "TupProject() -> Loading project -> " << id;
#endif

    QString configPath = dir + "/config.xml";
    QFile configFile(configPath);
    if (!configFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        #ifdef TUP_DEBUG
            qDebug() << "TupProject() - Fatal error: no access to config file -> " << configPath;
        #endif
        return;
    }

    settings = new QSettings("MaeFloresta", "CurrentProject");

    QXmlStreamReader *configReader = new QXmlStreamReader(&configFile);
    if (configReader->readNextStartElement() && configReader->name() == QString("config")) {
        while (configReader->readNextStartElement()) {
            QString key = configReader->name().toString();
            QString value = configReader->readElementText();
            if (key == "title") {
                settings->setValue("title", value);
            } else if (key == "tags") {
                settings->setValue("tags", value);
            } else if (key == "description") {
                settings->setValue("description", value);
            } else if (key == "bgColor") {
                settings->setValue("bgColor", value);
            } else if (key == "fps") {
                settings->setValue("fps", value.toInt());
            } else if (key == "framesTotal") {
                settings->setValue("framesTotal", value.toInt());
            } else if (key == "borderHistory") {
                settings->setValue("borderHistory", value);
            } else if (key == "fillHistory") {
                settings->setValue("fillHistory", value);
            } else if (key == "bgHistory") {
                settings->setValue("bgHistory", value);
            }
        }
    }

    configFile.close();

    QString framesPath = dir + "/frames.xml";
    QFile framesFile(framesPath);
    if (!framesFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        #ifdef TUP_DEBUG
            qDebug() << "TupProject() - Fatal error: no access to config file -> " << framesPath;
        #endif
        return;
    }

    QTextStream in(&framesFile);
    QString xml = in.readAll();

    QDomDocument doc;
    if (!doc.setContent(xml)) {
        #ifdef TUP_DEBUG
            qDebug() << "TupProject() - Fatal error: frames.xml is corrupt!";
        #endif
        return;
    }

    QDomElement root = doc.documentElement();
    QDomNode n = root.firstChild();
    int i = 0;
    while (!n.isNull()) {
           QDomElement e = n.toElement();
           if (!e.isNull()) {
               if (e.tagName() == "frame") {
                   QString frame;
                   {
                     QTextStream ts(&frame);
                     ts << n;
                   }

                   /*
                   #ifdef TUP_DEBUG
                       qDebug() << "*** FRAME " << i <<"***";
                       qDebug() << frame;
                   #endif
                   */

                   QString frameKey = "frame_" + QString::number(i);
                   settings->setValue(frameKey, frame);
                   i++;
               }
           }
           n = n.nextSibling();
    }

#ifdef TUP_DEBUG
    qDebug() << "TupProject() - Frames total loaded -> " << i;
#endif

    framesFile.close();
    isOk = true;
}

// Method to save a project
bool TupProject::save(const QString &id, const QSettings *project, const QPixmap &pixmap)
{
    QString dir = TupProject::projectsDir();
    dir += "/" + id;
    if (!QDir(dir).exists()) {
        if (!QDir().mkpath(dir)) {
            #ifdef TUP_DEBUG
                qDebug() << "TupProject::save() - Fatal error: Insufficient permissions to create directory -> " << dir;
            #endif
            return false;
        }
    }

#ifdef TUP_DEBUG
    qDebug() << "TupProject::save() - Saving project in base path: " << dir;
#endif

    QFile preview(dir + "/preview.png");
    if (!preview.open(QIODevice::WriteOnly)) {
    #ifdef TUP_DEBUG
        qDebug() << "TupProject::save() - Fatal error: Insufficient permissions to create image -> " << preview.fileName();
    #endif
        return false;
    }
    pixmap.save(&preview, "PNG");

    QFile config(dir + "/config.xml");
    if (!config.open(QIODevice::WriteOnly)) {
        #ifdef TUP_DEBUG
            qDebug() << "TupProject::save() - Fatal error: Insufficient permissions to create config file -> " << config.fileName();
        #endif
        return false;
    }

    QXmlStreamWriter configXml(&config);
    configXml.setAutoFormatting(true);
    configXml.writeStartDocument();
    configXml.writeStartElement("config");
    configXml.writeTextElement("title", project->value("title").toString());
    configXml.writeTextElement("tags", project->value("tags").toString());
    configXml.writeTextElement("description", project->value("description").toString());
    configXml.writeTextElement("bgColor", project->value("bgColor").toString());
    configXml.writeTextElement("fps", project->value("fps").toString());
    configXml.writeTextElement("framesTotal", project->value("framesTotal").toString());
    configXml.writeTextElement("borderHistory", project->value("borderHistory").toString());
    configXml.writeTextElement("fillHistory", project->value("fillHistory").toString());
    configXml.writeTextElement("bgHistory", project->value("bgHistory").toString());
    configXml.writeEndElement();
    config.close();

    QFile animation(dir + "/frames.xml");
    animation.open(QIODevice::WriteOnly);
    QTextStream out(&animation);
    out << "<?xml version=\"1.0\"?>";
    out << "<frames>\n";

    int framesTotal = project->value("framesTotal").toInt();
#ifdef TUP_DEBUG
    qDebug() << "TupProject::save() - Storing " << framesTotal << " frames";
#endif

    for (int i=0; i < framesTotal; i++) {
        QString frameKey = "frame_" + QString::number(i);
        out << project->value(frameKey).toString() + "\n";
    }

    out << "</frames>";
    animation.close();

    return true;
}

QSettings * TupProject::parameters()
{
    return settings;
}

bool TupProject::loadIsOk()
{
    return isOk;
}

bool TupProject::remove(const QString &id)
{
    QString dir = TupProject::projectsDir();
    dir += "/" + id;
    if (!QDir(dir).exists()) {
        #ifdef TUP_DEBUG
            qDebug() << "TupProject::remove() - Fatal error: project doesn't exist -> " << id;
        #endif
        return false;
    }

#ifdef TUP_DEBUG
    qDebug() << "TupProject::remove() - Removing project dir -> " << dir;
#endif

    QStringList files;
    files << "preview.png" << "config.xml" << "frames.xml" << "image.png" << "video.mp4";
    foreach(QString target, files) {
            QFile file(dir + "/" + target);
            if (file.exists()) {
                if (!file.remove()) {
                    #ifdef TUP_DEBUG
                        qDebug() << "TupProject::remove() - Fatal error: can't remove project file -> " << target << " (" << id << ")";
                    #endif
                    return false;
                }
            }
    }

    if (!QDir().rmdir(dir)) {
        #ifdef TUP_DEBUG
            qDebug() << "TupProject::remove() - Fatal error: can't remove project directory -> " << dir;
        #endif
        return false;
    }

    return true;
}

bool TupProject::imageFileExists(const QString &id)
{
    QString image = TupProject::projectsDir() + "/" + id + "/image.png";
    if (QFile::exists(image))
        return true;

    return false;
}

bool TupProject::videoFileExists(const QString &id)
{
    QString video = TupProject::projectsDir() + "/" + id + "/video.mp4";
    if (QFile::exists(video))
        return true;

    return false;
}

bool TupProject::saveColorPalette(const QPixmap &palette)
{
    QString dir = TupProject::rootDir();
    QFile file(dir + "/palette.png");
    if (!file.open(QIODevice::WriteOnly)) {
        #ifdef TUP_DEBUG
           qDebug() << "TupProject::saveColorPalette() - Fatal error: Insufficient permissions to create color palette -> " << file.fileName();
           qDebug() << "TupProject::saveColorPalette() - Palette Path: " << dir;
        #endif
        return false;
    }
    palette.save(&file, "PNG");

    return true;
}

bool TupProject::colorPaletteExist()
{
    QString dir = TupProject::rootDir();
    QFile file(dir + "/palette.png");
    if (!file.exists())
        return false;

    return true;
}

bool TupProject::firstTime()
{
    QString dir = rootDir() + "/works";
    return !QDir(dir).exists();
}

bool TupProject::makeDirs()
{
#ifdef TUP_DEBUG
    qDebug() << "TupProject::makeDirs() - Tracing...";
#endif

    QString dir = rootDir();

    // TupiTube folder
    if (!QDir(dir).exists()) {
        if (!QDir().mkpath(dir)) {
            #ifdef TUP_DEBUG
                qDebug() << "TupProject::makeDirs() - Fatal error: Insufficient permissions to create directory -> " << dir;
            #endif
            return false;
        }
    }

    // works folder
#if defined(Q_OS_WIN) || defined(Q_OS_LINUX)
    QString works = dir + "/works";
    if (!QDir(works).exists()) {
        if (!QDir().mkpath(works)) {
            #ifdef TUP_DEBUG
                qDebug() << "TupProject::makeDirs() - Fatal error: Insufficient permissions to create directory -> " << works;
            #endif
            return false;
        }
    }
#endif

    /* SQA: Pending code to validate
    QString backgrounds = dir + "/backgrounds";
    if (!QDir(backgrounds).exists()) {
        if (!QDir().mkdir(backgrounds)) {
            #ifdef TUP_DEBUG
            qDebug() << "TupProject::makeDirs() - Fatal error: Insufficient permissions to create directory -> " << backgrounds;
            #endif
            return false;
        }
    }

    QString anim = backgrounds + "/animations";
    if (!QDir(anim).exists()) {
        if (!QDir().mkdir(anim)) {
            #ifdef TUP_DEBUG
            qDebug() << "TupProject::makeDirs() - Fatal error: Insufficient permissions to create directory -> " << anim;
            #endif
            return false;
        }
    }

    QString drawings = backgrounds + "/drawings";
    if (!QDir(drawings).exists()) {
        if (!QDir().mkdir(drawings)) {
            #ifdef TUP_DEBUG
            qDebug() << "TupProject::makeDirs() - Fatal error: Insufficient permissions to create directory -> " << drawings;
            #endif
            return false;
        }
    }
    */

    return true;
}

QString TupProject::rootDir()
{
    QString dir = "";

#if defined(Q_OS_ANDROID)
    dir = QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation)[0] + "/TupiTube";
#elif defined(Q_OS_WIN) || defined(Q_OS_LINUX)
    dir = QStandardPaths::standardLocations(QStandardPaths::HomeLocation)[0] + "/.tupitube";
#endif

    return dir;
}

QString TupProject::projectsDir()
{
    QString dir = rootDir() + "/works";

#ifdef TUP_DEBUG
    qDebug() << "TupProject::projectsDir() - dir -> " << dir;
#endif

    return dir;
}

bool TupProject::createTupFile(QSettings *project, const QString &dimension, QList<TupFrame *> frames)
{
    QString dir = TupProject::rootDir() + "/animation";
    // temporary .tup folder
    if (!QDir(dir).exists()) {
        if (!QDir().mkdir(dir)) {
            #ifdef TUP_DEBUG
                qDebug() << "TupProject() - Fatal error: Insufficient permissions to create directory -> " << dir;
            #endif
            return false;
        }
    }

#ifdef TUP_DEBUG
    qDebug() << "TupProject::createTupFile() - Creating TUP elements at -> " << dir;
#endif

    QFile tppFile(dir + "/project.tpp");
    if (!tppFile.open(QIODevice::WriteOnly)) {
        #ifdef TUP_DEBUG
            qDebug() << "TupProject::createTupFile() - Fatal error: Insufficient permissions to create TPP file -> " << tppFile.fileName();
        #endif
        return false;
    }

    QXmlStreamWriter configXml(&tppFile);
    configXml.setAutoFormatting(true);
    configXml.writeStartDocument();
    configXml.writeStartElement("Tupi");
    configXml.writeAttribute("version", "1");
    configXml.writeStartElement("project");
    configXml.writeAttribute("name", tr("My Project"));
    configXml.writeStartElement("meta");
    // Internet Attributes
    configXml.writeTextElement("title", project->value("title").toString());
    configXml.writeTextElement("tags", project->value("tags").toString());
    configXml.writeTextElement("description", project->value("description").toString());
    // Tup Attributes
    configXml.writeTextElement("author", tr("Your Name"));

    // qDebug() << "TupProject::createTupFile() - Bg Color: " << project->value("bgColor").toString();

    configXml.writeTextElement("bgcolor", project->value("bgColor").toString());
    configXml.writeTextElement("dimension", dimension);
    configXml.writeTextElement("fps", project->value("fps").toString());
    configXml.writeEndElement(); // meta
    configXml.writeEndElement(); // project
    configXml.writeEndElement(); // Tupi
    configXml.writeEndDocument();
    tppFile.close();

    QFile tplFile(dir + "/library.tpl");
    if (!tplFile.open(QIODevice::WriteOnly)) {
        #ifdef TUP_DEBUG
            qDebug() << "TupProject::createTupFile() - Fatal error: Insufficient permissions to create TPL file -> " << tplFile.fileName();
        #endif
        return false;
    }

    QXmlStreamWriter libraryXml(&tplFile);
    libraryXml.setAutoFormatting(true);
    libraryXml.writeStartDocument();
    libraryXml.writeStartElement("library");
    libraryXml.writeStartElement("folder");
    libraryXml.writeAttribute("id", "library");
    libraryXml.writeEndElement(); // folder
    libraryXml.writeEndElement(); // library
    libraryXml.writeEndDocument();
    tplFile.close();

    QFile tpsFile(dir + "/scene0.tps");
    if (!tpsFile.open(QIODevice::WriteOnly)) {
        #ifdef TUP_DEBUG
            qDebug() << "TupProject::createTupFile() - Fatal error: Insufficient permissions to create TPS file -> " << tpsFile.fileName();
        #endif
        return false;
    }

    QDomDocument sceneDoc = QDomDocument();
    QDomElement root = sceneDoc.createElement("scene");
    root.setAttribute("name", "Scene 1");

    QDomElement storyDom = sceneDoc.createElement("storyboard");

    QDomElement titleDom = sceneDoc.createElement("title");
    QDomElement authorDom = sceneDoc.createElement("author");
    QDomText author = sceneDoc.createTextNode(tr("Your Name"));
    authorDom.appendChild(author);
    QDomElement topicsDom = sceneDoc.createElement("topics");
    QDomElement summaryDom = sceneDoc.createElement("summary");

    storyDom.appendChild(titleDom);
    storyDom.appendChild(authorDom);
    storyDom.appendChild(topicsDom);
    storyDom.appendChild(summaryDom);

    root.appendChild(storyDom);

    QDomElement bgDom = sceneDoc.createElement("background");
    QDomElement dynamicDom = sceneDoc.createElement("frame");
    dynamicDom.setAttribute("name", "landscape_dynamic");
    dynamicDom.setAttribute("direction", "0");
    dynamicDom.setAttribute("opacity", "1");
    dynamicDom.setAttribute("shift", "5");
    QDomElement staticDom = sceneDoc.createElement("frame");
    staticDom.setAttribute("name", "landscape_static");
    staticDom.setAttribute("opacity", "1");
    bgDom.appendChild(dynamicDom);
    bgDom.appendChild(staticDom);

    root.appendChild(bgDom);

    QDomElement layerDom = sceneDoc.createElement("layer");
    layerDom.setAttribute("name", tr("Layer 1"));
    layerDom.setAttribute("visible", "1");
    layerDom.setAttribute("opacity", "1");

    foreach (TupFrame *frame, frames) {
        QDomElement frameDom = frame->toXml(sceneDoc);
        layerDom.appendChild(frameDom);
    }

    root.appendChild(layerDom);
    sceneDoc.appendChild(root);

    QTextStream stream(&tpsFile);
    stream << sceneDoc.toString();
    tpsFile.close();

    return true;
}

int TupProject::nextProjectID()
{
    QString worksPath = projectsDir();
    QDirIterator iterator(worksPath, QDirIterator::NoIteratorFlags);
    QString  projectID = "project_00";
    while (iterator.hasNext()) {
        iterator.next();
        QString record = iterator.fileName();
        if (record.startsWith("project_"))
            projectID = record;
    }
    QString lastIndex = projectID.mid(projectID.indexOf("_") + 1, projectID.length());

    return lastIndex.toInt() + 1;
}
