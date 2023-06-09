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

#include "tupdrawtoolsmenu.h"
#include "tupbutton.h"

#include <QDialog>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>

TupDrawToolsMenu::TupDrawToolsMenu(QSettings *config, QWidget *parent) : QDialog(parent)
{
    setModal(false);
    setWindowFlags(Qt::Popup);

    QString bgColor = config->value("themeColor").toString();
    setStyleSheet("QDialog { background-color: rgb(" + bgColor + "); }");

    QString system = config->value("system").toString();
    QString imgPath = config->value("imgPath").toString();

    int menuTitleSize = config->value("menuTitleSize").toInt();
    int menuTextSize = config->value("menuTextSize").toInt();

    borderRadius = config->value("borderRadius").toString();
    iconWidth = config->value("iconWidth").toInt();
    size = config->value("actionDialogSize").toSize();

    textFont = this->font();
    textFont.setBold(true);
    textFont.setPointSize(menuTitleSize);

    QLabel *titleLabel = new QLabel(tr("Draw Tools"));
    titleLabel->setStyleSheet("QLabel { background-color: rgb(200,200,200); "
                              "border-radius: " + borderRadius + "px; }");
    titleLabel->setFont(textFont);
    titleLabel->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);

    TupButton *penButton = new TupButton(imgPath + "pen.png", iconWidth, tr("Pen"), menuTextSize);
    connect(penButton, &TupButton::clicked, this, &TupDrawToolsMenu::callPenTool);

    TupButton *squareButton = new TupButton(imgPath + "rectangle.png", iconWidth, tr("Rectangle"), menuTextSize);
    connect(squareButton, &TupButton::clicked, this, &TupDrawToolsMenu::callRectangleTool);

    TupButton *ellipseButton = new TupButton(imgPath + "ellipse.png", iconWidth, tr("Ellipse"), menuTextSize);
    connect(ellipseButton, &TupButton::clicked, this, &TupDrawToolsMenu::callEllipseTool);

    TupButton *triangleButton = new TupButton(imgPath + "triangle.png", iconWidth, tr("Triangle"), menuTextSize);
    connect(triangleButton, &TupButton::clicked, this, &TupDrawToolsMenu::callTriangleTool);

    QGridLayout *gridLayout = new QGridLayout;

    gridLayout->addWidget(penButton, 0, 0);
    gridLayout->addWidget(squareButton, 0, 1);
    gridLayout->addWidget(ellipseButton, 0, 2);
    gridLayout->addWidget(triangleButton, 0, 3);

    QVBoxLayout *layout = new QVBoxLayout(this);

    layout->addWidget(titleLabel);
    layout->addLayout(gridLayout);
}

TupDrawToolsMenu::~TupDrawToolsMenu()
{
}

void TupDrawToolsMenu::keyReleaseEvent(QKeyEvent *event)
{
#if defined(Q_OS_ANDROID) || defined(Q_OS_IOS)
    if (event->key() == Qt::Key_Back)
        closeMenu();
#else
    /*
    if (event->key() == Qt::Key_Left) {
        emit leftArrowClicked();
    } else if (event->key() == Qt::Key_Right) {
        emit rightArrowClicked();
    }
    */

    if (event->key() == Qt::Key_Back) {
        closeMenu();
    } else if (event->key() == Qt::Key_Z) {
        emit undoRequested();
    } else if (event->key() == Qt::Key_Y) {
        emit redoRequested();
    }

#endif

    QDialog::keyPressEvent(event);
}

void TupDrawToolsMenu::closeMenu()
{
    emit drawMenuClosed();
    activateWindow();
    raise();
    accept();
}

void TupDrawToolsMenu::callPenTool()
{
    emit callTool(PenTool);
    close();
}

void TupDrawToolsMenu::callRectangleTool()
{
    emit callTool(RectangleTool);
    close();
}

void TupDrawToolsMenu::callEllipseTool()
{
    emit callTool(EllipseTool);
    close();
}

void TupDrawToolsMenu::callTriangleTool()
{
    emit callTool(TriangleTool);
    close();
}
