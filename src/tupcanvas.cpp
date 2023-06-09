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

#include "tupcanvas.h"
#include "tuppathitem.h"
#include "tupitemfactory.h"
#include "tupgraphicsscene.h"

#include <cmath>
#include <QTouchEvent>
#include <QWheelEvent>
#include <QGraphicsPixmapItem>
#include <QPixmap>
#include <QGraphicsEllipseItem>
#include <QGraphicsTextItem>
#include <QTimer>

TupCanvas::TupCanvas(QGraphicsScene *newScene, QSettings *settings, const QColor &bg, QColor fillColor,
                     const QPen newPen, int newFps, QWidget *parent) : QGraphicsView(newScene, parent)
{
    viewport()->setAttribute(Qt::WA_AcceptTouchEvents);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setMouseTracking(true);
    setInteractive(false);

    currentTool = PenTool;
    itemsCounter = BOTTOM_LEVEL;

    isCellPhone = settings->value("isCellPhone").toBool();
    onion = settings->value("onionSkin", 50).toInt();
    smoothLevel = settings->value("smoothLevel", 4.0).toFloat();

    previousIsVisible = settings->value("showPreviousFrame", true).toBool();
    nextIsVisible = settings->value("showNextFrame", true).toBool();

    consoleFontSize = settings->value("consoleFontSize").toInt();
    consolePos = settings->value("consolePos").toPoint();

    borderOpacity = static_cast<int> (settings->value("borderOpacity").toDouble() * 255.0);
    fillOpacity = static_cast<int> (settings->value("fillOpacity").toDouble() * 255.0);

    bgColor = bg;

    fillColor.setAlpha(fillOpacity);
    int style = settings->value("fillBrush").toInt();
    fillStyle = Qt::BrushStyle(style);
    fillBrush = QBrush(fillColor, fillStyle);

    mode = Frames; 
    currentIndex = 0;
    totalScaleFactor = 1;
    scene = newScene;
    pen = newPen;
    borderStyle = pen.brush().style();

    pathAdded = false;
    rectAdded = false;
    ellipseAdded = false;

    fabric = QPixmap(QSize(static_cast<int> (scene->width()), static_cast<int> (scene->height())));

#ifdef TUP_DEBUG
    qDebug() << "TupCanvas() - Canvas width: " << scene->width();
	qDebug() << "TupCanvas() - Canvas height: " << scene->height();
#endif
	
    fabric.fill(bgColor);
    screen = new QGraphicsPixmapItem(fabric);
    screen->setZValue(0);
    screenAtScene = false;

    pressed = false;
    size = 8;
    fps = newFps;
    screenIsDisabled = false;

    previousFrame = new TupFrame();
    currentFrame = new TupFrame();
    nextFrame = new TupFrame();

    counterLabel = new QGraphicsTextItem("");
    counterLabel->setZValue(BOARD_ZVALUE);
    counterLabel->setDefaultTextColor(setFgColor(bg));

    QFont f = font();
    f.setPointSize(30);
    f.setWeight(QFont::Normal);

    counterLabel->setFont(f);
    counterLabel->setPos(consolePos);
    scene->addItem(counterLabel);

    newTouchEvent = true;

    timer = new QTimer(this);
    timer->setTimerType(Qt::PreciseTimer);
    connect(timer, &QTimer::timeout, this, &TupCanvas::play);

#if !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS)
    QImage image(":images/cursors/target.png");
    cursor = QCursor(QPixmap::fromImage(image), 4, 4);
    setCursor(cursor);
#endif

    QPixmap arrow(":images/forward.png");
    iconSize = settings->value("iconSize").toInt();

    greenArrow = new QGraphicsPixmapItem(arrow);
    greenArrow->setScale(static_cast<double> (iconSize) / static_cast<double> (arrow.width()));
    arrowAdded = false;
}

TupCanvas::~TupCanvas()
{
}

void TupCanvas::setMode(Mode newMode)
{
#ifdef TUP_DEBUG
    qDebug() << "TupCanvas::setMode() - Tracing...";
#endif

    mode = newMode;
    resetView();
    if (mode == Player) {
        scene->removeItem(counterLabel);

        clearFrame(previousFrame, Previous);
        clearFrame(nextFrame, Next);
        clearFrame(currentFrame, Current);
    } else { // Edition Mode
        screenIsDisabled = false;

        if (timer->isActive()) {
            timer->stop();
            currentIndex = 0;
        }
        if (screenAtScene) {
            scene->removeItem(screen);
            screenAtScene = false;
        }
        scene->addItem(counterLabel);
        paintFrame(previousFrame, currentFrame, nextFrame);
    }

    viewport()->update();
}

void TupCanvas::setCurrentTool(Tool tool)
{
    currentTool = tool;
}

int TupCanvas::alphaValue(double newOpacity)
{
    int value = static_cast<int> (255.0 * newOpacity);
    return value;
}

void TupCanvas::updateSmoothness(float level)
{
    smoothLevel = level;
}

void TupCanvas::updateOnionValue(int onionSkin)
{
    onion = onionSkin;
}

void TupCanvas::updateFramesVisibility(bool previousFlag, bool nextFlag)
{
    previousIsVisible = previousFlag;
    nextIsVisible = nextFlag;

    repaintFrame();
}

void TupCanvas::updateFPS(int newFps)
{
    fps = newFps;
}

void TupCanvas::smoothPath(QPainterPath &path, float smoothParam, int from, int to)
{
    QPolygonF pol;
    QList<QPolygonF> polygons = path.toSubpathPolygons();
    QList<QPolygonF>::iterator it = polygons.begin();
    QPolygonF::iterator pointIt;

    while (it != polygons.end()) {
           pointIt = (*it).begin();

           while (pointIt <= (*it).end()-2) {
                  pol << (*pointIt);
                  pointIt += 2;
           }
           ++it;
    }

    if (smoothParam > 0) {
        path = TupGraphicalAlgorithm::bezierFit(pol, smoothParam, from, to);
    } else {
        path = QPainterPath();
        path.addPolygon(pol);
    }
}

TupFrame * TupCanvas::frame()
{
    return currentFrame;
}

void TupCanvas::drawBackground(QPainter *painter, const QRectF &rect)
{
    QGraphicsView::drawBackground(painter, rect);
    painter->save();

    if (mode == Frames) {
        painter->setRenderHint(QPainter::Antialiasing, true);
        painter->setPen(QPen(QColor(0, 0, 0, 80), 1));
        QRectF area = scene->sceneRect();
        painter->fillRect(area, bgColor);
    }

    bool hasAntialiasing = painter->renderHints() & QPainter::Antialiasing;
    painter->setRenderHint(QPainter::Antialiasing, hasAntialiasing);
    painter->restore();
}

void TupCanvas::updatePenThickness(int size)
{
    pen.setWidth(size);
}

void TupCanvas::updateBorderColor(QColor color)
{
    color.setAlpha(borderOpacity);
    QBrush brush(color, borderStyle);
    pen.setBrush(brush);
}

void TupCanvas::updateFillColor(QColor color)
{
    color.setAlpha(fillOpacity);
    fillBrush = QBrush(color, fillStyle);
}

void TupCanvas::updateOpacities(int border, int fill)
{
    updateBorderOpacity(border);
    updateFillOpacity(fill);
}

void TupCanvas::updateBorderOpacity(int border)
{
    borderOpacity = border;

    QBrush brush = pen.brush();
    QColor color = pen.color();
    color.setAlpha(border);
    pen.setColor(color);

    if (border > 0) {
        brush.setColor(color);
        brush.setStyle(borderStyle);
        pen.setBrush(brush);
    }
}

void TupCanvas::updateFillOpacity(int fill)
{
    fillOpacity = fill;

    QColor color = fillBrush.color();
    color.setAlpha(fill);
    fillBrush.setColor(color);
}

void TupCanvas::updateBrushes(Qt::BrushStyle border, Qt::BrushStyle fill)
{
    borderStyle = border;
    QBrush brush(pen.color(), border);
    pen.setBrush(brush);

    fillStyle = fill;
    fillBrush.setStyle(fillStyle);
}

void TupCanvas::updateBgColor(const QColor color)
{
    bgColor = color;
    fabric.fill(color);
    screen->setPixmap(fabric);
    counterLabel->setDefaultTextColor(setFgColor(color));

    viewport()->update();
}

QColor TupCanvas::setFgColor(QColor color)
{
    QColor fgColor(120, 120, 120);

    if (color == Qt::white) {
        fgColor = QColor(180, 180, 180);
    } else {
        if (color == Qt::black)
            fgColor = QColor(255, 255, 255);
        if (color.red() > 220 && color.green() > 220 && color.blue() > 220)
            fgColor = QColor(180, 180, 180);
        else if (color.red() > 220 && color.green() > 220 && color.blue() < 220)
            fgColor = QColor(150, 150, 150);
        else if (color.green() > 200 && (color.red() < 200 || color.blue() < 200))
            fgColor = QColor(120, 120, 120);
        else if (color.red() > 200 && (color.green() < 200 || color.blue() < 200))
            fgColor = QColor(255, 255, 255);
        else if (color.blue() > 200 && (color.red() < 200 || color.green() < 200))
            fgColor = QColor(255, 255, 255);
        else if (color.red() < 150 && color.green() < 150 && color.blue() < 150)
            fgColor = QColor(230, 230, 230);
    }

    return fgColor;
}

void TupCanvas::undo()
{
#ifdef TUP_DEBUG
    qDebug() << "";
    qDebug() << "TupCanvas::undo() - Tracing...";
#endif

    if (currentFrame->undoIsAvailable()) {
        if (QGraphicsItem *item = currentFrame->undoItem()) {
            if (item) {
                scene->removeItem(item);
                if (!currentFrame->undoIsAvailable())
                    emit updateButtonState(TupCanvas::Undo, false);
                if (currentFrame->timeToRedo())
                    emit updateButtonState(TupCanvas::Redo, true);
            } else {
                #ifdef TUP_DEBUG
                    qDebug() << "TupCanvas::undo() - QGraphicsItem is NULL!";
                #endif
            }
        } else {
            #ifdef TUP_DEBUG
                qDebug() << "TupCanvas::undo() - QGraphicsItem cast failed!";
            #endif
        }
    }
}

void TupCanvas::redo()
{
#ifdef TUP_DEBUG
    qDebug() << "";
    qDebug() << "TupCanvas::redo() - Tracing...";
#endif

    if (currentFrame->redoIsAvailable()) {
        if (QGraphicsItem *item = currentFrame->redoItem()) {
            if (item) {
                scene->addItem(item);
                if (!currentFrame->redoIsAvailable())
                    emit updateButtonState(TupCanvas::Redo, false);
                if (currentFrame->timeToUndo())
                    emit updateButtonState(TupCanvas::Undo, true);
            } else {
                #ifdef TUP_DEBUG
                    qDebug() << "TupCanvas::redo() - QGraphicsItem is NULL!";
                #endif
            }
        } else {
            #ifdef TUP_DEBUG
                qDebug() << "TupCanvas::redo() - QGraphicsItem cast failed!";
            #endif
        }
    }
}

void TupCanvas::reset()
{
#ifdef TUP_DEBUG
    qDebug() << "TupCanvas::reset() - Tracing...";
#endif

    resetView();
    clearFrame(currentFrame, Current);
    currentFrame->clear();

    emit updateButtonState(TupCanvas::Undo, false);
    if (currentFrame->redoIsAvailable())
        emit updateButtonState(TupCanvas::Redo, true);
    else
        emit updateButtonState(TupCanvas::Redo, false);
}

void TupCanvas::fullReset(const QColor &color)
{
#ifdef TUP_DEBUG
    qDebug() << "TupCanvas::fullReset() - Tracing...";
#endif

    resetView();
    label = "1/1";
    counterLabel->setPlainText(label);

    clearFrame(previousFrame, Previous);
    clearFrame(nextFrame, Next);
    clearFrame(currentFrame, Current);

    previousFrame = new TupFrame();
    nextFrame = new TupFrame();
    currentFrame = new TupFrame();

    updateBgColor(color);

    viewport()->update();
}

void TupCanvas::clearFrame(TupFrame *frame, FrameType type)
{
#ifdef TUP_DEBUG
    qDebug() << "TupCanvas::clearFrame() - frame items: " << frame->count();
#endif
    if (!frame->isEmpty()) {
        int bottomLimit = 0;
        int topLimit = 0;
        switch (type) {
            case Previous:
            {
                bottomLimit = 0;
                topLimit = TOP_LEVEL;
            }
            break;
            case Next:
            {
                bottomLimit = TOP_LEVEL;
                topLimit = BOTTOM_LEVEL;
            }
            break;
            case Current:
            {
                bottomLimit = BOTTOM_LEVEL;
                topLimit = BOARD_ZVALUE;
            }
            break;
        }

        QList<QGraphicsItem *> items = scene->items();
        foreach (QGraphicsItem *item, items) {
            if (item->zValue() >= bottomLimit && item->zValue() < topLimit)
                scene->removeItem(item);
        }
    }
}

void TupCanvas::resetView()
{
    if (totalScaleFactor != 1.0) {
        totalScaleFactor = 1.0;
        setTransform(QTransform().scale(1.0, 1.0));
    }
}

bool TupCanvas::isEmpty()
{
    return currentFrame->isEmpty();
}

void TupCanvas::clearMessagesFromCanvas()
{
#ifdef TUP_DEBUG
    qDebug() << "TupCanvas::clearMessagesFromCanvas() - Tracing...";
#endif

    if (!messages.isEmpty()) {
        int count = messages.count();
        for (int i=0; i<count; i++) {
             QGraphicsTextItem *text = messages.takeAt(i);
             scene->removeItem(text);
        }
        for (int i=0; i<count; i++) {
             QGraphicsRectItem *rect = panels.takeAt(i);
             scene->removeItem(rect);
        }
    }
}

QColor TupCanvas::messageColor(TupCanvas::Type type)
{
    QColor color;
    switch (type) {
      case TupCanvas::Error:
      {
          color = QColor(230, 0, 0);
      }
      break;
      case TupCanvas::Warning:
      {
          color = QColor(255, 120, 0);
      }
      break;
      case TupCanvas::Info:
      {
          #ifdef Q_OS_IOS
              color = QColor(50, 50, 50);
          #else
              color = QColor(0, 100, 0);
          #endif
      }
      break;
      case TupCanvas::Console:
      {
          #ifdef Q_OS_IOS
              color = QColor(50, 50, 50);
          #else
              color = QColor(0, 100, 0);
          #endif
      }
      break;
    }

    return color;
}

void TupCanvas::addMessageIntoScene(const QString &msg, TupCanvas::Type type)
{
#ifdef TUP_DEBUG
    qDebug() << "TupCanvas::addMessageIntoScene() - msg -> " << msg;
#endif

    QString text = msg;
    if (msg.compare("ADD_FRAMES_TABLET") == 0)
        text = tr("Tip: Press green right arrow to add new frames!");
    if (msg.compare("ADD_FRAMES_PHONE") == 0)
        text = tr("Tip: Press green arrow to add frames!");

    QGraphicsTextItem *message = new QGraphicsTextItem(text);
    message->setZValue(TEXT_ZVALUE);
    QColor color = messageColor(type);
    message->setDefaultTextColor(color);

    QRectF rect = scene->sceneRect();

    int fontSize = 24;
    QFont f = font();
    f.setPointSize(fontSize);
    f.setWeight(QFont::Normal);
    message->setFont(f);

    QRectF textRect = message->boundingRect();
    int width = static_cast<int>(textRect.width());
    int height = static_cast<int>(textRect.height());
    int screenW = static_cast<int>(scene->sceneRect().width());

    int posX;
    int posY;

    if (type != TupCanvas::Console) {
        while (width > screenW) {
            fontSize--;
            f.setPointSize(fontSize);
            message->setFont(f);
            textRect = message->boundingRect();
            width = static_cast<int>(textRect.width());
            height = static_cast<int>(textRect.height());
        }
        posX = static_cast<int>((rect.width() - width) / 2);
        posY = static_cast<int>((rect.height() - height) / 2);
    } else {        
        f.setPointSize(consoleFontSize);
        message->setFont(f);
        textRect = message->boundingRect();
        posX = static_cast<int>((rect.width() - textRect.width()) / 2);
        if (isCellPhone) {
            posY = static_cast<int>(textRect.height() / 2);
        } else {
            posY = static_cast<int>(rect.height() - (textRect.height() * 1.5));
        }
    }

    QGraphicsRectItem *msgRect = new QGraphicsRectItem(textRect);
    msgRect->setZValue(BOARD_ZVALUE);
    msgRect->setBrush(QBrush(QColor(232, 232, 232)));
    msgRect->setPen(QPen(QColor(180, 180, 180)));
    msgRect->setPos(QPointF(posX, posY));
    scene->addItem(msgRect);

    message->setPos(QPointF(posX, posY));
    scene->addItem(message);

    if (msg.startsWith("ADD_FRAMES")) {
        arrowAdded = true;
        int x = (screenW - iconSize) / 2;
        greenArrow->setPos(QPointF(x, posY + textRect.height() + 5));
        scene->addItem(greenArrow);
    }

    messages.append(message);
    panels.append(msgRect);
}

void TupCanvas::notify(TupCanvas::Type type, const QString &msg, int time)
{
#ifdef TUP_DEBUG
    qDebug() << "TupCanvas::notify() - msg -> " << msg;
#endif

    emit enableUI(false);
    screenIsDisabled = true;

    resetView();
    clearMessagesFromCanvas();
    addMessageIntoScene(msg, type);

    if (time > 0)
        QTimer::singleShot(time, this, SLOT(removeNotification()));
}

void TupCanvas::disableScreen(bool flag)
{
    screenIsDisabled = flag;
}

void TupCanvas::removeNotification()
{
#ifdef TUP_DEBUG
    qDebug() << "TupCanvas::removeNotification() - Tracing...";
#endif

    if (messages.count() == 1) {
        QGraphicsTextItem *msg = messages.takeAt(0);
        scene->removeItem(msg);

        QGraphicsRectItem *rect = panels.takeAt(0);
        scene->removeItem(rect);

        if (arrowAdded) {
            scene->removeItem(greenArrow);
            arrowAdded = false;
        }

        screenIsDisabled = false;
        emit enableUI(true);
    }
}

void TupCanvas::loadFrame(TupFrame *previous, TupFrame *current, TupFrame *next, const QString &newLabel)
{
    #ifdef TUP_DEBUG
        qDebug() << "TupCanvas::loadFrame() - Loading frame objects...";
    #endif

    label = newLabel;

    restoreCounterLabel(10);
    QTimer::singleShot(300, this, SLOT(setCounterLabel()));

    resetView();

    clearFrame(previousFrame, Previous);
    clearFrame(nextFrame, Next);
    clearFrame(currentFrame, Current);

    previousFrame = previous;
    currentFrame = current;
    nextFrame = next;

    itemsCounter = BOTTOM_LEVEL;

    paintFrame(previousFrame, currentFrame, nextFrame);
}

void TupCanvas::restoreCounterLabel(int delta)
{
    QFont font = counterLabel->font();
    font.setPointSize(font.pointSize() + delta);
    counterLabel->setFont(font);
    counterLabel->setPlainText(label);
}

void TupCanvas::setCounterLabel()
{
    restoreCounterLabel(-10);
}

void TupCanvas::paintFrame(TupFrame *previousFrame, TupFrame *currentFrame, TupFrame *nextFrame)
{
#ifdef TUP_DEBUG
    qDebug() << "TupCanvas::paintFrame() - Tracing...";
#endif

    QList<QGraphicsItem *> items;
    qreal onionSkin;
    qreal onionParam = onion/100.0;

    int zValue = 0;
    items = currentFrame->items();
    size = items.count();
    zValue = BOTTOM_LEVEL;
    for (int i=0; i<size; i++) {
         QGraphicsItem *item = items.at(i);
         onionSkin = currentFrame->opacity(i);
         item->setOpacity(onionSkin);
         item->setZValue(zValue);
         scene->addItem(item);
         zValue++;
    }

    if (previousIsVisible) {
        items = previousFrame->items();
        int size = items.count();
        zValue = MIDDLE_LEVEL;
        for (int i=0; i<size; i++) {
            QGraphicsItem *item = items.at(i);
            onionSkin = previousFrame->opacity(i);
            item->setOpacity(onionSkin*onionParam);
            item->setZValue(zValue);
            scene->addItem(item);
            zValue++;
        }
    }

    if (nextIsVisible) {
        items = nextFrame->items();
        size = items.count();
        zValue = TOP_LEVEL;
        for (int i=0; i<size; i++) {
            QGraphicsItem *item = items.at(i);
            onionSkin = nextFrame->opacity(i);
            item->setOpacity(onionSkin*onionParam);
            item->setZValue(zValue);
            scene->addItem(item);
            zValue++;
        }
    }

    /*
    items = currentFrame->items();
    size = items.count();
    zValue = CURRENT_FRAME;
    for (int i=0; i<size; i++) {
         QGraphicsItem *item = items.at(i);
         onionSkin = currentFrame->opacity(i);
         item->setOpacity(onionSkin);
         item->setZValue(zValue);
         scene->addItem(item);
         zValue++;
    }
    */
}

void TupCanvas::repaintFrame()
{
#ifdef TUP_DEBUG
    qDebug() << "TupCanvas::repaintFrame() - Tracing...";
#endif

    clearFrame(currentFrame, Current);
    clearFrame(nextFrame, Next);
    clearFrame(previousFrame, Previous);

    paintFrame(previousFrame, currentFrame, nextFrame);
}

bool TupCanvas::redoIsAvailable() 
{
    return currentFrame->redoIsAvailable();
}

bool TupCanvas::undoIsAvailable()
{
    return currentFrame->undoIsAvailable();
}

void TupCanvas::renderProject(QList<TupFrame *> newFrames)
{
#ifdef TUP_DEBUG
    qDebug() << "TupCanvas::renderProject() - Tracing...";
#endif

#if !defined(Q_OS_ANDROID)
    setCursor(QCursor(Qt::WaitCursor));
#else
    notify(TupCanvas::Info, tr("Rendering..."), 2000);
#endif

    if (photograms.count() > 0)
        photograms.clear();

    foreach(TupFrame *frame, newFrames) {
        frame->updateOpacities();
        photograms << renderFrame(frame);
    }
    removeNotification();

    #if !defined(Q_OS_ANDROID)
        setCursor(cursor);
    #endif
}

void TupCanvas::playAnimation()
{
#ifdef TUP_DEBUG
    qDebug() << "TupCanvas::playAnimation() - fps: " << fps;
#endif

   screen->setPixmap(photograms.at(0));
   scene->addItem(screen);
   screenAtScene = true;
   screenIsDisabled = true;

   currentIndex = 1;
   timer->start(1000/fps);
}

void TupCanvas::play()
{
   screen->setPixmap(photograms.at(currentIndex));
   currentIndex++;
   if (currentIndex == photograms.count())
       currentIndex = 0;
}

QPixmap TupCanvas::renderFrame(TupFrame *frame)
{
#ifdef TUP_DEBUG
    qDebug() << "TupCanvas::renderFrame() - Tracing...";
#endif

    QRect rect = scene->sceneRect().toRect();
    QImage image = QImage(rect.size(), QImage::Format_RGB32);
    image.fill(bgColor);
    QPainter painter(&image);
    painter.setRenderHint(QPainter::Antialiasing);

    TupGraphicsScene *scene = new TupGraphicsScene(frame);
    scene->render(&painter, rect, rect, Qt::IgnoreAspectRatio);

    return QPixmap::fromImage(image);
}

#if !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS)
void TupCanvas::enterEvent(QEvent *event)
{
    Q_UNUSED(event)

    setCursor(cursor);

    if (mode == Frames)
        setCursor(cursor);
    else
        setCursor(QCursor(Qt::ArrowCursor));
}

void TupCanvas::leaveEvent(QEvent *event)
{
    Q_UNUSED(event)

    if (mode == Frames)
        setCursor(QCursor(Qt::ArrowCursor));
}
#endif

bool TupCanvas::viewportEvent(QEvent *event)
{
    switch (event->type()) {
        case QEvent::MouseButtonPress:
            {
                #ifdef TUP_DEBUG
                    qDebug() << "MouseButtonPress event...";
                #endif

                if (screenIsDisabled)
                    return true;

                QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
                QPointF pt = mapToScene(mouseEvent->pos());

                firstPoint = pt;
                oldPos = pt;
                pressed = true;

                if (currentTool == PenTool) {
                    currentPath = QPainterPath();
                    currentPath.moveTo(firstPoint);
                    pathItem = new TupPathItem();

                    pathItem->setPen(pen);
                    pathItem->setBrush(fillBrush);
                    pathItem->setZValue(TOP_LEVEL + itemsCounter);
                    scene->addItem(pathItem);
                    pathAdded = true;
                } else if (currentTool == RectangleTool) {
                    rectItem = new TupRectItem(QRectF(pt, QSize(0, 0)));
                    rectItem->setPen(pen);
                    rectItem->setBrush(fillBrush);
                    rectItem->setZValue(TOP_LEVEL + itemsCounter);
                    scene->addItem(rectItem);
                    rectAdded = true;
                } else if (currentTool == EllipseTool) {
                    ellipseItem = new TupEllipseItem(QRectF(pt, QSize(0, 0)));
                    ellipseItem->setPen(pen);
                    ellipseItem->setBrush(fillBrush);
                    ellipseItem->setZValue(TOP_LEVEL + itemsCounter);
                    scene->addItem(ellipseItem);
                    ellipseAdded = true;
                } else if (currentTool == TriangleTool) {
                    pathItem = new TupPathItem();
                    pathItem->setPen(pen);
                    pathItem->setBrush(fillBrush);
                    pathItem->setZValue(TOP_LEVEL + itemsCounter + 1);
                    scene->addItem(pathItem);
                    pathAdded = true;
                }
            }
        break;
        case QEvent::MouseMove:
            {
				/*
                #ifdef TUP_DEBUG
                    qDebug() << "MouseMove event...";
                #endif
				*/

                if (pressed) {
                    QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
                    QPointF lastPoint = mapToScene(mouseEvent->pos());

                    if (currentTool == PenTool) {
                        currentPath.moveTo(oldPos);
                        currentPath.lineTo(lastPoint);
                        pathItem->setPath(currentPath);
                    } else if (currentTool == RectangleTool || currentTool == EllipseTool) {
                        QRectF rectVar;
                        if (currentTool == RectangleTool)
                            rectVar = rectItem->rect();
                        else
                            rectVar = ellipseItem->rect();

                        QPointF point = mouseEvent->pos();
                        int xMouse = static_cast<int> (point.x());
                        int yMouse = static_cast<int> (point.y());
                        int xInit = static_cast<int> (firstPoint.x());
                        int yInit = static_cast<int> (firstPoint.y());

                        if (xMouse >= xInit) {
                            if (yMouse >= yInit)
                                rectVar.setBottomRight(point);
                            else
                                rectVar.setTopRight(point);
                        } else {
                            if (yMouse >= yInit)
                                rectVar.setBottomLeft(point);
                            else
                                rectVar.setTopLeft(point);
                        }

                        if (currentTool == RectangleTool)
                            rectItem->setRect(rectVar);
                        else
                            ellipseItem->setRect(rectVar);
                    } else if (currentTool == TriangleTool) {
                        double topCornerX = firstPoint.x() + ((lastPoint.x() - firstPoint.x())/2);
                        double topCornerY = firstPoint.y();
                        QPointF topCorner(topCornerX, topCornerY);
                        double leftCornerX = firstPoint.x();
                        double leftCornerY = lastPoint.y();
                        QPointF leftCorner(leftCornerX, leftCornerY);

                        currentPath = QPainterPath();
                        currentPath.moveTo(topCorner);
                        currentPath.cubicTo(lastPoint, lastPoint, lastPoint);
                        currentPath.cubicTo(leftCorner, leftCorner, leftCorner);
                        currentPath.cubicTo(topCorner, topCorner, topCorner);

                        pathItem->setPath(currentPath);
                    }

                    oldPos = lastPoint;
                }
            }
        break;
        case QEvent::MouseButtonRelease:
            {
                #ifdef TUP_DEBUG
                    qDebug() << "MouseButtonRelease event...";
                #endif

                if (screenIsDisabled)
                    return true;

                QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
                QPointF pt = mapToScene(mouseEvent->pos());

                if (currentTool == PenTool) {
                    if (!pathItem)
                        return true;
                    if (firstPoint == pt && currentPath.elementCount() == 1) {
                        int w = pen.width();
                        int size = w/2;
                        pen.setWidth(size);
                        pathItem->setPen(pen);
                        currentPath.addEllipse(pt.x(), pt.y(), size, size);
                        pen.setWidth(w);
                    } else {
                        #ifdef TUP_DEBUG
                            qDebug() << "Smooth Level -> " << smoothLevel;
                        #endif
                        smoothPath(currentPath, smoothLevel);
                    }
                    pathItem->setPath(currentPath);
                    currentFrame->addItem(pathItem);
                } else if (currentTool == RectangleTool) {
                    currentFrame->addItem(rectItem);
                } else if (currentTool == EllipseTool) {
                    currentFrame->addItem(ellipseItem);
                } else if (currentTool == TriangleTool) {
                    pathItem->setPath(currentPath);
                    currentFrame->addItem(pathItem);
                }
                itemsCounter++;

                pressed = false;
                pathAdded = false;
                rectAdded = false;
                ellipseAdded = false;

                emit saveFrame();
                if (currentFrame->timeToUndo()) 
                    emit updateButtonState(TupCanvas::Undo, true);
            }
        break;
        case QEvent::Wheel:
            {
                #ifdef TUP_DEBUG
                    qDebug() << "Wheel event...";
                #endif

                QWheelEvent *mouseEvent = static_cast<QWheelEvent *>(event);
                QPointF center = mapToScene(mouseEvent->position().toPoint());
                centerOn(center);

                int delta = mouseEvent->angleDelta().y();
                if (delta < 0) {
                    totalScaleFactor += 0.3;
                    if (totalScaleFactor > 9)
                        totalScaleFactor = 9;
                } else {
                    totalScaleFactor -= 0.3;
                    if (totalScaleFactor < 0.8)
                        totalScaleFactor = 0.8;
                }

                setTransform(QTransform().scale(totalScaleFactor, totalScaleFactor));
            }
        break;
        case QEvent::TouchBegin:
            {
                #ifdef TUP_DEBUG
                    qDebug() << "TouchBegin event...";
                #endif

                if (screenIsDisabled)
                    return true;

                newTouchEvent = true;
            }
        break;
        case QEvent::TouchUpdate:
            {
                /*
                #ifdef TUP_DEBUG
                    qDebug() << "TouchUpdate event...";
                #endif
                */

                if (screenIsDisabled)
                    return true;

                QTouchEvent *touchEvent = static_cast<QTouchEvent *>(event);
                // QList<QTouchEvent::TouchPoint> touchPoints = touchEvent->touchPoints();
                QList<QTouchEvent::TouchPoint> touchPoints = touchEvent->points();
                if (touchPoints.size() == 2) {
                    checkPressStatus();

                    // determine scale factor
                    const QTouchEvent::TouchPoint &touchPoint0 = touchPoints.first();
                    const QTouchEvent::TouchPoint &touchPoint1 = touchPoints.last();
                    QPointF firstPos = touchPoint0.position();
                    QPointF lastPos = touchPoint1.position();

                    // qreal currentScaleFactor = QLineF(firstPos, lastPos).length() / QLineF(touchPoint0.startPos(),
                    //                                                                        touchPoint1.startPos()).length();

                    qreal currentScaleFactor = QLineF(firstPos, lastPos).length() / QLineF(touchPoint0.pressPosition(),
                                                                                           touchPoint1.pressPosition()).length();

                    if (touchEvent->touchPointStates() & Qt::TouchPointReleased) {
                        // if one of the fingers is released, remember the current scale
                        // factor so that adding another finger later will continue zooming
                        // by adding new scale factor to the existing remembered value.
                        totalScaleFactor *= currentScaleFactor;
                        currentScaleFactor = 1;
                    }

                    double factor = totalScaleFactor * currentScaleFactor;
                    QString round = QString::number(factor, 'g', 2);
                    factor = round.toDouble();

                    if (factor >= 0.8) {
                        if (factor != oldFactor) {
                            setTransform(QTransform().scale(factor, factor));
                        } else {
                            qreal x = lastPos.x();
                            if (firstPos.x() < lastPos.x())
                                x = firstPos.x();

                            qreal y = lastPos.y();
                            if (firstPos.y() < lastPos.y())
                                y = firstPos.y();

                            x += fabs(lastPos.x() - firstPos.x())/2;
                            y += fabs(lastPos.y() - firstPos.y())/2;

                            QPointF touchCenterPos = QPointF(x, y);

                            if (!newTouchEvent) {
                                qreal deltaX = touchCenterPos.x() - oldTouchPos.x();
                                qreal deltaY = touchCenterPos.y() - oldTouchPos.y();
                                deltaX /= factor*1.5;
                                deltaY /= factor*1.5;
                                QPointF centerPos = this->mapToScene(this->viewport()->rect().center());
                                centerOn(centerPos + QPointF(deltaX*(-1), deltaY*(-1)));
                            }

                            oldTouchPos = touchCenterPos;
                            newTouchEvent = false;
                        }
                    } else {
                        totalScaleFactor = 0.8;
                    }

                    oldFactor = factor;
                }
            }
        break;
        case QEvent::TouchEnd:
            {
                #ifdef TUP_DEBUG
                    qDebug() << "TouchEnd event...";
                #endif

                newTouchEvent = false;

                if (screenIsDisabled)
                    return true;

                QTouchEvent *touchEvent = static_cast<QTouchEvent *>(event);
                // QList<QTouchEvent::TouchPoint> touchPoints = touchEvent->touchPoints();
                QList<QTouchEvent::TouchPoint> touchPoints = touchEvent->points();

                if (touchPoints.size() == 2) {
                    if (pressed)
                        checkPressStatus();
                }
            }
        break;
        default:
            {
                return QGraphicsView::viewportEvent(event);
            }
        }

        return QGraphicsView::viewportEvent(event);
}

void TupCanvas::checkPressStatus()
{
    if (pathAdded) {
        scene->removeItem(pathItem);
        pressed = false;
        pathAdded = false;
    }

    if (rectAdded) {
        scene->removeItem(rectItem);
        pressed = false;
        rectAdded = false;
    }

    if (ellipseAdded) {
        scene->removeItem(ellipseItem);
        pressed = false;
        ellipseAdded = false;
    }
}

/*
void TupCanvas::inputMethodQuery(Qt::InputMethodQuery query, QVariant variant)
{
    Q_UNUSED(query);
    Q_UNUSED(variant);
}
*/

void TupCanvas::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Left) {
        emit leftArrowClicked();
    } else if (event->key() == Qt::Key_Right) {
        emit rightArrowClicked();
    }

    QGraphicsView::keyPressEvent(event);
}
