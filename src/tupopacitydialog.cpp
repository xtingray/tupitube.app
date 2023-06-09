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

#include "tupopacitydialog.h"
#include "tupbutton.h"
#include "tupsettings.h"

#include <QScrollArea>

TupOpacityDialog::TupOpacityDialog(QSettings *config, QWidget *parent) : QDialog(parent)
{
    setModal(true);
    setWindowFlags(Qt::Popup);

    settings = config;
    isCellPhone = config->value("isCellPhone").toBool();

    QString system = config->value("system").toString();
    imgPath = config->value("imgPath").toString();

    QString bgColor = config->value("themeColor").toString();
    borderRadius = config->value("borderRadius").toString();
    dialogButtonsHeight = config->value("dialogButtonsHeight").toInt();
    iconWidth = config->value("iconWidth").toInt();
    size = config->value("opacityDialogSize").toSize();

#if defined(Q_OS_ANDROID) || defined(Q_OS_IOS)
    if (!isCellPhone) {
        setStyleSheet("* { background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
                      "stop:0 rgb(100, 100, 100), "
                      "stop:1 rgb(30, 30, 30)); }");
    }
#endif

    innerLayout = new QVBoxLayout;

    tabWidget = new QTabWidget;
    tabWidget->setStyleSheet("QTabBar::tab:selected { "
                             "background-color: rgba(0, 137, 0, 100); "
                             "}"
                             "QTabBar::tab:!selected {"
                             "background-color: rgba(50, 50, 50, 50); "
                             "}");

    addOpacityTab(tr("Border Properties"));
    addOpacityTab(tr("Fill Properties"));

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

TupOpacityDialog::~TupOpacityDialog()
{
}

void TupOpacityDialog::addOpacityTab(const QString &label)
{
    QScrollArea *scrollArea = new QScrollArea;
    scrollArea->setStyleSheet("QScrollArea {"
                            "background-color: rgb(232, 232, 232);"
                            "}"
                            "QScrollBar:vertical {"
                            "background-color: rgb(200, 200, 200);"
                            "}"
                            "QScrollBar::handle:vertical {"
                            "background-color: rgb(130, 130, 130);"
                            "}");
    scrollArea->setWidgetResizable(true);

    if (label.compare(tr("Border Properties")) == 0) {
        borderPanel = new TupOpacityPanel(settings, TupPenPreviewCanvas::Border, this);
        connect(borderPanel, &TupOpacityPanel::opacityZero, this, &TupOpacityDialog::checkOpacityStatus);

        scrollArea->setWidget(borderPanel);
        scrollArea->setAlignment(Qt::AlignHCenter);
        tabWidget->addTab(scrollArea, label);
    }

    if (label.compare(tr("Fill Properties")) == 0) {
        fillPanel = new TupOpacityPanel(settings, TupPenPreviewCanvas::Fill, this);
        connect(fillPanel, &TupOpacityPanel::opacityZero, this, &TupOpacityDialog::checkOpacityStatus);

        scrollArea->setWidget(fillPanel);
        scrollArea->setAlignment(Qt::AlignHCenter);
        tabWidget->addTab(scrollArea, label);
    }
}

void TupOpacityDialog::setButtonsPanel()
{
    TupButton *okButton = new TupButton(imgPath + "ok.png", iconWidth);
    okButton->setFixedHeight(dialogButtonsHeight);
    connect(okButton, &TupButton::clicked, this, &TupOpacityDialog::closeDialog);

    TupButton *closeButton = new TupButton(imgPath + "close.png", iconWidth);
    closeButton->setFixedHeight(dialogButtonsHeight);
    connect(closeButton, &TupButton::clicked, this, &QWidget::close);

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(closeButton);
    buttonLayout->addWidget(okButton);

    innerLayout->addLayout(buttonLayout);
    innerLayout->setAlignment(buttonLayout, Qt::AlignRight);
}

void TupOpacityDialog::closeDialog()
{
    double border = borderPanel->opacityValue();
    double fill = fillPanel->opacityValue();
    if (border == 0.0 && fill == 0.0) {
        if (tabWidget->currentIndex() == Fill)
            border = 1.0;
        else
            fill = 1.0;
    }

    emit opacitiesUpdated(border, fill);
    accept();
}

void TupOpacityDialog::setParameters(const QPen pen, const QColor fillColor, Qt::BrushStyle brushStyle,
                                     double borderOpacity, double fillOpacity)
{
#ifdef TUP_DEBUG
    qDebug() << "Current border opacity: " << borderOpacity;
    qDebug() << "Current border color: " << pen.color();
    qDebug() << "Current fill opacity: " << fillOpacity;
    qDebug() << "Current fill color: " << fillColor;
#endif

    borderPanel->setParameters(pen);
    borderPanel->setOpacityValue(borderOpacity * 100);

    fillPanel->setParameters(fillColor, brushStyle);
    fillPanel->setOpacityValue(fillOpacity * 100);
}

void TupOpacityDialog::mousePressEvent(QMouseEvent *event)
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

void TupOpacityDialog::checkOpacityStatus(TupPenPreviewCanvas::CanvasType panelType)
{
    if (panelType == TupPenPreviewCanvas::Border) {
        double fOpacity = fillPanel->opacityValue();
        if (fOpacity == 0.0)
            fillPanel->resetOpacityValue();
    } else {
        double bOpacity = borderPanel->opacityValue();
        if (bOpacity == 0.0)
            borderPanel->resetOpacityValue();
    }
}
