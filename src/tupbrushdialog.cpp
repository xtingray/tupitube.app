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

#include "tupbrushdialog.h"
#include "tupbutton.h"
#include "tupsettings.h"

#include <QLabel>
#include <QTextBrowser>

TupBrushDialog::TupBrushDialog(QSettings *config, QWidget *parent) : QDialog(parent)
{
#ifdef TUP_DEBUG
    qDebug() << "TupBrushDialog() - Tracing...";
#endif

    setModal(true);
    setWindowFlags(Qt::Popup);

    borderPanelAdded = false;
    fillPanelAdded = false;
    borderMsgAdded = false;
    fillMsgAdded = false;

    settings = config;
    isCellPhone = config->value("isCellPhone").toBool();

    QString system = config->value("system").toString();
    imgPath = config->value("imgPath").toString();

    QString bgColor = settings->value("themeColor").toString();
    borderRadius = config->value("borderRadius").toString();
    iconWidth = config->value("iconWidth").toInt();
    textSize = settings->value("textSize").toInt();
    dialogButtonsHeight = config->value("dialogButtonsHeight").toInt();
    size = config->value("brushesDialogSize").toSize();

    int index = config->value("borderBrush").toInt();
    borderStyle = Qt::BrushStyle(index);
    index = config->value("fillBrush").toInt();
    fillStyle = Qt::BrushStyle(index);

    borderColor = getColor("borderColor");
    fillColor = getColor("fillColor");

    borderOpacity = config->value("borderOpacity").toDouble();
    fillOpacity = config->value("fillOpacity").toDouble();

#if defined(Q_OS_ANDROID) || defined(Q_OS_IOS)
    if (!isCellPhone)
        setStyleSheet("* { background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
                           "stop:0 rgb(100, 100, 100), stop:1 rgb(30, 30, 30)); }");
#endif

    innerLayout = new QVBoxLayout;

    borderScrollArea = new QScrollArea;
    borderMsgWidget = new QWidget;
    fillScrollArea = new QScrollArea;
    fillMsgWidget = new QWidget;

    tabWidget = new QTabWidget;
    tabWidget->setStyleSheet("QTabBar::tab:selected { "
                             "background-color: rgba(0, 137, 0, 100); "
                             "}"
                             "QTabBar::tab:!selected {"
                             "background-color: rgba(50, 50, 50, 50); "
                             "}");

    setBrushTab(Border, borderScrollArea);
    setBrushTab(Fill, fillScrollArea);
    QString msg = tr("Current <b>border color</b> is transparent.<br/><br/>"
                     "Please, pick a <b>border color</b> to enable this tab.");
    setMessagePanel(borderMsgWidget, msg);
    msg = tr("Current <b>fill color</b> is transparent.<br/><br/>"
                     "Please, pick a <b>fill color</b> to enable this tab.");
    setMessagePanel(fillMsgWidget, msg);

    if (borderOpacity > 0)
        addBrushTab(Border);
    else
        addMsgPanel(Border);

    if (fillOpacity > 0)
        addBrushTab(Fill);
    else
        addMsgPanel(Fill);

    innerLayout->addWidget(tabWidget);
    setButtonsPanel();

    widget = new QWidget;
    QString style = "QWidget { background-color: rgb(" + bgColor + "); ";
    #ifdef Q_OS_ANDROID
        style += "border-radius: " + borderRadius + "px; }";
    #else
        style += "border-style: none; }";
    #endif
    widget->setStyleSheet(style);

    if (!isCellPhone)
        widget->setFixedSize(size);
    widget->setLayout(innerLayout);

    QHBoxLayout *global = new QHBoxLayout;
    global->addWidget(widget);
    setLayout(global);
    setStyleSheet(style);

    if (isCellPhone)
        move(QPoint(0, 0));
}

TupBrushDialog::~TupBrushDialog()
{
}

void TupBrushDialog::setBrushTab(ColorType colorType, QScrollArea *scrollArea)
{
    scrollArea->setStyleSheet("QScrollArea {"
                            "background-color: rgb(255, 255, 255);"
                            "}"
                            "QScrollBar:vertical {"
                            "background-color: rgb(200, 200, 200);"
                            "}"
                            "QScrollBar::handle:vertical {"
                            "background-color: rgb(130, 130, 130);"
                            "}");
    scrollArea->setWidgetResizable(true);
    scrollArea->setAlignment(Qt::AlignHCenter);

    if (colorType == Border) {
        borderPanel = new TupBrushesPanel(borderColor, borderStyle, borderOpacity, this);
        connect(borderPanel, &TupBrushesPanel::styleClick, this, &TupBrushDialog::setBorderStyle);

        scrollArea->setWidget(borderPanel);
    } else if (colorType == Fill) {
        fillPanel = new TupBrushesPanel(fillColor, fillStyle, fillOpacity, this);
        connect(fillPanel, &TupBrushesPanel::styleClick, this, &TupBrushDialog::setFillStyle);

        scrollArea->setWidget(fillPanel);
    }
}

void TupBrushDialog::addBrushTab(ColorType colorType)
{
    if (colorType == Border) {
        tabWidget->insertTab(0, borderScrollArea, tr("Border Brush"));
        borderPanelAdded = true;
    } else {
        tabWidget->insertTab(1, fillScrollArea, tr("Fill Brush"));
        fillPanelAdded = true;
    }
}

void TupBrushDialog::removeBrushTab(ColorType colorType)
{
    if (colorType == Border) {
        tabWidget->removeTab(0);
        borderPanelAdded = false;
    } else {
        tabWidget->removeTab(1);
        fillPanelAdded = false;
    }
}

void TupBrushDialog::setMessagePanel(QWidget *msgWidget, const QString &msg)
{
    msgWidget->setStyleSheet("* { background-color: rgb(255,255,255); }");
    QVBoxLayout *msgLayout = new QVBoxLayout(msgWidget);
    QTextBrowser *textBlock = new QTextBrowser;

#if defined(Q_OS_WIN) || defined(Q_OS_MAC) || defined(Q_OS_LINUX)
    QFont font = QFont();
    font.setPointSize(textSize);
    textBlock->setFont(font);
#endif

    textBlock->setText(msg);
    msgLayout->addWidget(textBlock);
}

void TupBrushDialog::addMsgPanel(ColorType colorType)
{
    if (colorType == Border) {
        tabWidget->insertTab(0, borderMsgWidget, tr("Border Brush"));
        borderMsgAdded = true;
    } else {
        tabWidget->insertTab(1, fillMsgWidget, tr("Fill Brush"));
        fillMsgAdded = true;
    }
}

void TupBrushDialog::removeMsgPanel(ColorType colorType)
{
    if (colorType == Border) {
        tabWidget->removeTab(0);
        borderMsgAdded = false;
    } else {
        tabWidget->removeTab(1);
        fillMsgAdded = false;
    }
}

void TupBrushDialog::setButtonsPanel()
{
    TupButton *okButton = new TupButton(imgPath + "ok.png", iconWidth);
    okButton->setFixedHeight(dialogButtonsHeight);
    connect(okButton, &TupButton::clicked, this, &TupBrushDialog::closeDialog);

    TupButton *closeButton = new TupButton(imgPath + "close.png", iconWidth);
    closeButton->setFixedHeight(dialogButtonsHeight);
    connect(closeButton, &TupButton::clicked, this, &QWidget::close);

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(closeButton);
    buttonLayout->addWidget(okButton);

    innerLayout->addLayout(buttonLayout);
    innerLayout->setAlignment(buttonLayout, Qt::AlignRight);
}

void TupBrushDialog::closeDialog()
{
    emit brushesUpdated(borderStyle, fillStyle);
    accept();
}

void TupBrushDialog::setBorderStyle(Qt::BrushStyle style)
{
    borderStyle = style;
}

void TupBrushDialog::setFillStyle(Qt::BrushStyle style)
{
    fillStyle = style;
}

QColor TupBrushDialog::getColor(const QString &param)
{
    QString colorName = settings->value(param).toString();
    QColor color;
    if (colorName.isEmpty())
        color = Qt::transparent;
    else
        color = QColor(colorName);

    return color;
}

void TupBrushDialog::mousePressEvent(QMouseEvent *event)
{
    if (!isCellPhone) {
        QPoint point = event->pos();
        int x0 = widget->pos().x();
        int y0 = widget->pos().y();
        int x1 = x0 + size.width();
        int y1 = y0 + size.height();
        if ((point.x() < x0 || point.x() > x1) || (point.y() < y0 || point.y() > y1))
            close();
    }
}

void TupBrushDialog::setParameters(QColor borderColor, double borderOpacity, QColor fillColor, double fillOpacity)
{
    if (borderOpacity > 0) {
        if (!borderPanelAdded) {
            if (borderMsgAdded)
                removeMsgPanel(Border);
            addBrushTab(Border);
        }
        borderPanel->setParameters(borderColor, borderOpacity);
    } else {
        if (!borderMsgAdded) {
            if (borderPanelAdded)
                removeBrushTab(Border);
            addMsgPanel(Border);
        }
    }

    if (fillOpacity > 0) {
        if (!fillPanelAdded) {
            if (fillMsgAdded)
                removeMsgPanel(Fill);
            addBrushTab(Fill);
        }
        fillPanel->setParameters(fillColor, fillOpacity);
    } else {
        if (!fillMsgAdded) {
            if (fillPanelAdded)
                removeBrushTab(Fill);
            addMsgPanel(Fill);
        }
    }
}
