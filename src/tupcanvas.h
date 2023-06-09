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

#ifndef TUPCANVAS_H
#define TUPCANVAS_H

#include <QGraphicsView>
#include <QGraphicsScene> 
#include <QSettings>

#include "tupglobal.h"
#include "tupgraphicalgorithm.h"
#include "tupframe.h"

class TupCanvas : public QGraphicsView
{
    Q_OBJECT

    public:
        enum Mode { Frames = 0, Player };
        enum Type { Error = 0, Warning, Info, Console };
        enum Button { Undo = 0, Redo };
        enum FrameType { Previous = 0, Next, Current };

        TupCanvas(QGraphicsScene *newScene, QSettings *settings, const QColor &bg, QColor fillColor,
                  const QPen newPen, int newFps, QWidget *parent = nullptr);
        ~TupCanvas();

        TupFrame * frame();
        void setMode(Mode mode);
        void updatePenThickness(int size);
        void updateOnionValue(int onionSkin);

        void updateBorderColor(QColor color);
        void updateFillColor(QColor color);

        void updateOpacities(int border, int fill);
        void updateBorderOpacity(int border);
        void updateFillOpacity(int fill);

        void updateBrushes(Qt::BrushStyle, Qt::BrushStyle);
        void updateBgColor(const QColor color);
        void updateSmoothness(float level);
        void updateFramesVisibility(bool previousFlag, bool nextFlag);
        void updateFPS(int fps);
        void notify(TupCanvas::Type type, const QString &msg, int time = 0);
        void disableScreen(bool flag);

        void undo();
        void redo();

        void reset();
        void fullReset(const QColor &color = QColor("#ffffff"));
        void clearCanvas();
        void repaintFrame();
        bool isEmpty();
        void loadFrame(TupFrame *previousFrame, TupFrame *currentFrame, TupFrame *nextFrame, const QString &label);

        bool redoIsAvailable();
        bool undoIsAvailable();

        void renderProject(QList<TupFrame *> photograms);
        QPixmap renderFrame(TupFrame *frame);
        void playAnimation();

    signals:
        void saveFrame();
        void enableUI(bool flag);
        void updateButtonState(TupCanvas::Button type, bool enable);
        void cursorPositionChanged();
        void leftArrowClicked();
        void rightArrowClicked();
        // void animationStopped();

    protected:
        virtual void keyPressEvent(QKeyEvent *event);
        virtual void drawBackground(QPainter *painter, const QRectF &rect);
        bool viewportEvent(QEvent *event);

#if !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS)
        virtual void enterEvent(QEvent *event);
        virtual void leaveEvent(QEvent *event);
#endif

    public slots:
        void setCurrentTool(Tool tool);
        void removeNotification();

    private slots:
        void play();
        void setCounterLabel();
        void restoreCounterLabel(int delta);
        // void inputMethodQuery(Qt::InputMethodQuery query, QVariant variant);

    private:
        void smoothPath(QPainterPath &path, float smoothness, int from = 0, int to = -1);
        int alphaValue(double borderOpacity);
        QColor setFgColor(QColor color);
        void resetView();
        void clearFrame(TupFrame *frame, FrameType type);
        void paintFrame(TupFrame *previousFrame, TupFrame *currentFrame, TupFrame *nextFrame);
        void clearMessagesFromCanvas();
        void addMessageIntoScene(const QString &msg, TupCanvas::Type type);
        QColor messageColor(TupCanvas::Type type);
        void checkPressStatus();

        bool isCellPhone;
        QColor bgColor;
        QBrush fillBrush;
        Mode mode;
        QGraphicsScene *scene;
        TupFrame *previousFrame;
        TupFrame *currentFrame;
        TupFrame *nextFrame;
        QTimer *timer;

        QGraphicsPixmapItem *screen;
        QPixmap fabric;
        bool screenAtScene;
        QList<QPixmap> photograms;
        int currentIndex;

        TupPathItem *pathItem;
        TupRectItem *rectItem;
        TupEllipseItem *ellipseItem;

        QList<QGraphicsTextItem *> messages;
        QList<QGraphicsRectItem *> panels;
        QGraphicsTextItem *counterLabel;
        QString label;

        QPainterPath currentPath;
        QPointF firstPoint;
        QPointF oldPos;

        QPointF oldTouchPos;
        double oldFactor;
        bool newTouchEvent;

        QPen pen;
        int size;
        int fps;
        int onion;
        float smoothLevel;
        bool previousIsVisible;
        bool nextIsVisible;

        Qt::BrushStyle borderStyle;
        Qt::BrushStyle fillStyle;
        int borderOpacity;
        int fillOpacity;

        bool pressed;
        qreal totalScaleFactor;
        QCursor cursor;

        bool screenIsDisabled;
        int consoleFontSize;
        QPoint consolePos;

        QGraphicsPixmapItem *greenArrow;
        bool arrowAdded;
        int iconSize;
        Tool currentTool;

        const int BOARD_ZVALUE = 60000;
        const int TEXT_ZVALUE = 60001;
        const int BOTTOM_LEVEL = 0;
        const int MIDDLE_LEVEL = 2000;
        const int TOP_LEVEL = 4000;
        const int PAINTING_LEVEL = 6000;

        bool pathAdded;
        bool rectAdded;
        bool ellipseAdded;
        int itemsCounter;
};

#endif
