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

#include "tuptoolstopmenu.h"
#include "tupbutton.h"

#include <QGridLayout>

TupToolsTopMenu::TupToolsTopMenu(QSettings *config, QWidget *parent) : QDialog(parent)
{
    setModal(false);
    setWindowFlags(Qt::Popup);

    QString bgColor = config->value("themeColor").toString();
    setStyleSheet("QDialog { background-color: rgb(" + bgColor + "); }");

    QString system = config->value("system").toString();
    QString imgPath = config->value("imgPath").toString();

    int iconWidth = config->value("iconWidth").toInt();
    int menuTitleSize = config->value("menuTitleSize").toInt();
    int menuTextSize = config->value("menuTextSize").toInt();
    QString borderRadius = config->value("borderRadius").toString();

    QFont textFont = this->font();
    textFont.setBold(true);
    textFont.setPointSize(menuTitleSize);

    QLabel *titleLabel = new QLabel(tr("Tools"));
    titleLabel->setStyleSheet("QLabel { background-color: rgb(200,200,200); "
                              "border-radius: " + borderRadius + "px; }");
    titleLabel->setFont(textFont);
    titleLabel->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);

    TupButton *penButton = new TupButton(imgPath + "pen.png", iconWidth, tr("Pen"), menuTextSize);
    connect(penButton, SIGNAL(clicked()), this, SLOT(callPenTool()));

    TupButton *rectButton = new TupButton(imgPath + "rectangle.png", iconWidth, tr("Rectangle"), menuTextSize);
    connect(rectButton, SIGNAL(clicked()), this, SLOT(callRectangleTool()));

    TupButton *ellipseButton = new TupButton(imgPath + "ellipse.png", iconWidth, tr("Ellipse"), menuTextSize);
    connect(ellipseButton, SIGNAL(clicked()), this, SLOT(callEllipseTool()));

    TupButton *triangleButton = new TupButton(imgPath + "triangle.png", iconWidth, tr("Triangle"), menuTextSize);
    connect(triangleButton, SIGNAL(clicked()), this, SLOT(callTriangleTool()));

    TupButton *paletteButton = new TupButton(imgPath + "palette.png", iconWidth, tr("Color"), menuTextSize);
    connect(paletteButton, SIGNAL(clicked()), this, SIGNAL(color()));

    TupButton *cloneButton = new TupButton(imgPath + "clone.png", iconWidth, tr("Clone"), menuTextSize);
    connect(cloneButton, SIGNAL(clicked()), this, SIGNAL(cloneRequested()));

    undoButton = new TupButton(imgPath + "undo.png", iconWidth, tr("Eraser"), menuTextSize);
    connect(undoButton, SIGNAL(clicked()), this, SIGNAL(undo()));

    redoButton = new TupButton(imgPath + "redo.png", iconWidth, tr("Redo"), menuTextSize);
    connect(redoButton, SIGNAL(clicked()), this, SIGNAL(redo()));

    TupButton *sizeButton = new TupButton(imgPath + "thickness.png", iconWidth, tr("Thickness"), menuTextSize);
    connect(sizeButton, SIGNAL(clicked()), this, SIGNAL(stroke()));

    TupButton *opacityButton = new TupButton(imgPath + "opacity.png", iconWidth, tr("Transp."), menuTextSize);
    connect(opacityButton, SIGNAL(clicked()), this, SIGNAL(opacity()));

    TupButton *brushesButton = new TupButton(imgPath + "brush.png", iconWidth, tr("Brushes"), menuTextSize);
    connect(brushesButton, SIGNAL(clicked()), this, SIGNAL(brushes()));

    TupButton *actionsButton = new TupButton(imgPath + "frames.png", iconWidth, tr("Frames"), menuTextSize);
    connect(actionsButton, SIGNAL(clicked()), this, SIGNAL(actions()));

    QGridLayout *gridLayout = new QGridLayout;
    // row 0
    gridLayout->addWidget(penButton, 0, 0);
    gridLayout->addWidget(rectButton, 0, 1);
    gridLayout->addWidget(ellipseButton, 0, 2);
    gridLayout->addWidget(triangleButton, 0, 3);

    // row 1
    gridLayout->addWidget(undoButton, 1, 0);
    gridLayout->addWidget(redoButton, 1, 1);
    gridLayout->addWidget(cloneButton, 1, 2);
    gridLayout->addWidget(actionsButton, 1, 3);

    // row 2
    gridLayout->addWidget(paletteButton, 2, 0);
    gridLayout->addWidget(sizeButton, 2, 1);
    gridLayout->addWidget(opacityButton, 2, 2);
    gridLayout->addWidget(brushesButton, 2, 3);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(titleLabel);
    layout->addLayout(gridLayout);
}

TupToolsTopMenu::~TupToolsTopMenu()
{
}

void TupToolsTopMenu::keyReleaseEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Back) {
        emit toolsMenuClosed();

        activateWindow();
        raise();
        accept();
    }
}

void TupToolsTopMenu::enableButton(TupCanvas::Button type, bool flag)
{
    if (type == TupCanvas::Redo)
        redoButton->setEnabled(flag);

    if (type == TupCanvas::Undo)
        undoButton->setEnabled(flag);
}

void TupToolsTopMenu::callPenTool()
{
    emit callTool(PenTool);
    close();
}

void TupToolsTopMenu::callRectangleTool()
{
    emit callTool(RectangleTool);
    close();
}

void TupToolsTopMenu::callEllipseTool()
{
    emit callTool(EllipseTool);
    close();
}

void TupToolsTopMenu::callTriangleTool()
{
    emit callTool(TriangleTool);
    close();
}
