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

#include "tupcolordialog.h"
#include "tupbutton.h"

#include <QScrollArea>
#include <QGuiApplication>
#include <QScreen>

TupColorDialog::TupColorDialog(QSettings *config, QPixmap *paletteImg, QWidget *parent) : QDialog(parent)
{
    setModal(true);
    setWindowFlags(Qt::Popup);    

    settings = config;
    palette = paletteImg;
    isCellPhone = config->value("isCellPhone").toBool();
    QString system = config->value("system").toString();
    imgPath = config->value("imgPath").toString();

    themeColor = settings->value("themeColor").toString();
    borderRadius = config->value("borderRadius").toString();
    iconWidth = config->value("iconWidth").toInt();
    size = config->value("colorDialogSize").toSize();

#if defined(Q_OS_ANDROID) || defined(Q_OS_IOS)
    dialogButtonsHeight = config->value("dialogButtonsHeight").toInt();
    if (!isCellPhone) {
        setStyleSheet("* { background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
                      "stop:0 rgb(100, 100, 100), "
                      "stop:1 rgb(30, 30, 30)); }");
    }
#else
	dialogButtonsHeight = size.height() * 0.08;
#endif

    previousBorderColor = Qt::black;
    previousFillColor = Qt::black;

    innerLayout = new QVBoxLayout;

    tabWidget = new QTabWidget;
    tabWidget->setStyleSheet("QTabBar::tab:selected { "
                             "background-color: rgba(0, 137, 0, 100); "
                             "}"
                             "QTabBar::tab:!selected {"
                             "background-color: rgba(50, 50, 50, 50); "
                             "}");

    QString borderLabel = tr("Border Color");
    QString fillLabel = tr("Fill Color");
    QString bgLabel = tr("Background Color");

    if (isCellPhone) {
        QScreen *screen = QGuiApplication::screens().at(0);
        int width = screen->geometry().width();
        int height = screen->geometry().height();
        if (width < height) {
            borderLabel = tr("Border");
            fillLabel = tr("Fill");
            bgLabel = tr("Bg Color");
        }
    }

    addColorTab(borderLabel, Border);
    addColorTab(fillLabel, Fill);
    addColorTab(bgLabel, BgColor);

    innerLayout->addWidget(tabWidget);

    setButtonsPanel();
    connect(tabWidget, SIGNAL(currentChanged(int)), this, SLOT(updateUI(int)));

    widget = new QWidget;
    QString style = "QWidget { background-color: rgb(" + themeColor + "); ";
#ifdef Q_OS_ANDROID
	style += "border-radius: " + borderRadius + "px; }";
#else
	style += "border-style: none; }";			
#endif
						  
    widget->setStyleSheet(style);						  
						  
    if (!isCellPhone)
        widget->setFixedSize(size);
	
    widget->setLayout(innerLayout);

    // SQA: Only for debugging purposes
    // widget->setStyleSheet("* { border: 1px solid red; }");

    QHBoxLayout *global = new QHBoxLayout;
    global->addWidget(widget);
    setLayout(global);
    setStyleSheet(style);
	
    if (isCellPhone)
        move(QPoint(0, 0));
}

TupColorDialog::~TupColorDialog()
{
}

void TupColorDialog::addColorTab(const QString &label, ColorTab tab)
{
    QScrollArea *scrollArea = new QScrollArea;
    scrollArea->setStyleSheet("QScrollArea {"
                            "background-color: rgb(" + themeColor + ");"
                            "}"
                            "QScrollBar:vertical {"
                            "background-color: rgb(200, 200, 200);"
                            "}"
                            "QScrollBar::handle:vertical {"
                            "background-color: rgb(130, 130, 130);"
                            "}");
    scrollArea->setWidgetResizable(true);

    if (tab == Border) {
        double opacity = settings->value("borderOpacity").toDouble();
        QColor color = getColor("borderColor", opacity);

        int index = settings->value("borderBrush").toInt();
        Qt::BrushStyle style = Qt::BrushStyle(index);

        borderOpacityFlag = false;
        if (opacity == 0.0)
            borderOpacityFlag = true;

        borderTab = new TupColorTab(settings, palette, tab, color, style, opacity, this);
        connect(borderTab, &TupColorTab::colorUpdated, this, &TupColorDialog::updateOpacityStatus);

        scrollArea->setWidget(borderTab);
        scrollArea->setAlignment(Qt::AlignHCenter);
        tabWidget->addTab(scrollArea, label);
    } else if (tab == Fill) {
        double opacity = settings->value("fillOpacity").toDouble();
        QColor color = getColor("fillColor", opacity);

        int index = settings->value("fillBrush").toInt();
        Qt::BrushStyle style = Qt::BrushStyle(index);

        fillOpacityFlag = false;
        if (opacity == 0.0)
            fillOpacityFlag = true;

        fillTab = new TupColorTab(settings, palette, tab, color, style, opacity, this);
        connect(fillTab, &TupColorTab::colorUpdated, this, &TupColorDialog::updateOpacityStatus);

        scrollArea->setWidget(fillTab);
        scrollArea->setAlignment(Qt::AlignHCenter);
        tabWidget->addTab(scrollArea, label);
    } else {
        QColor color = getColor("bgColor", 1.0);
        Qt::BrushStyle style = Qt::BrushStyle(1);

        bgTab = new TupColorTab(settings, palette, tab, color, style, 1.0, this);
        scrollArea->setWidget(bgTab);
        scrollArea->setAlignment(Qt::AlignHCenter);
        tabWidget->addTab(scrollArea, label);
    }
}

void TupColorDialog::updateUI(int index)
{
    if (index == BgColor) {
        if (opacityWidget->isVisible())
            opacityWidget->setVisible(false);
    } else {
        if (index == Border) {
            updateOpacityCheckBox(borderOpacityFlag);
            opacityLabel->setText(tr("Transparent Border"));
        } else { // Fill
            updateOpacityCheckBox(fillOpacityFlag);
            opacityLabel->setText(tr("Transparent Fill"));
        }

        if (!opacityWidget->isVisible())
            opacityWidget->setVisible(true);
    }
}

void TupColorDialog::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event)

    if (isCellPhone) {
        QScreen *screen = QGuiApplication::screens().at(0);
        int width = screen->geometry().width();
        int height = screen->geometry().height();

        QString borderLabel = tr("Border");
        QString fillLabel = tr("Fill");
        QString bgLabel = tr("Bg Color");

        if (width > height) {
            borderLabel = tr("Border Color");
            fillLabel = tr("Fill Color");
            bgLabel = tr("Background Color");
        }

        tabWidget->setTabText(Border, borderLabel);
        tabWidget->setTabText(Fill, fillLabel);
        tabWidget->setTabText(BgColor, bgLabel);

        setFixedSize(width, height);
    }
}

void TupColorDialog::initSliders()
{
    tabWidget->setCurrentIndex(Border);

    borderTab->initSlider();
    fillTab->initSlider();
    bgTab->initSlider();
}

void TupColorDialog::setButtonsPanel()
{
    opacityWidget = new QWidget;
    opacityWidget->setFixedHeight(dialogButtonsHeight);

    opacityCheckBox = new TupCheckBox(0, imgPath + "unchecked.png", iconWidth, themeColor, borderRadius);
    connect(opacityCheckBox, &TupCheckBox::clicked, this, &TupColorDialog::updateOpacitySelection);

    double opacity = settings->value("borderOpacity").toDouble();

    borderOpacityFlag = false;
    if (opacity == 0.0)
        borderOpacityFlag = true;
    updateOpacityCheckBox(borderOpacityFlag);

    opacityLabel = new TupLabel(0, tr("Transparent Border"));
    connect(opacityLabel, &TupLabel::clicked, this, &TupColorDialog::updateOpacitySelection);

    int textSize = settings->value("textSize").toInt();
    QFont textFont = this->font();
    textFont.setPointSize(textSize);
    opacityLabel->setFont(textFont);

    QHBoxLayout *opacityLayout = new QHBoxLayout(opacityWidget);
    opacityLayout->addWidget(opacityCheckBox);
    opacityLayout->addWidget(opacityLabel);
    opacityLayout->addStretch();

    QWidget *buttonsPanel = new QWidget;
    buttonsPanel->setFixedHeight(dialogButtonsHeight);

    TupButton *okButton = new TupButton(imgPath + "ok.png", iconWidth);
    okButton->setFixedHeight(dialogButtonsHeight);
    connect(okButton, &TupButton::clicked, this, &TupColorDialog::closeDialog);

    TupButton *closeButton = new TupButton(imgPath + "close.png", iconWidth);
    closeButton->setFixedHeight(dialogButtonsHeight);
    connect(closeButton, &TupButton::clicked, this, &TupColorDialog::cancel);

    QHBoxLayout *rightLayout = new QHBoxLayout(buttonsPanel);
    rightLayout->addStretch();
    rightLayout->addWidget(closeButton);
    rightLayout->addWidget(okButton);

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(opacityWidget, Qt::AlignVCenter|Qt::AlignLeft);
    buttonLayout->addStretch();
    buttonLayout->addWidget(buttonsPanel, Qt::AlignVCenter|Qt::AlignRight);

    innerLayout->addLayout(buttonLayout);
}

// Enable/disable opacity controls and UI
void TupColorDialog::updateOpacitySelection(int index)
{
    Q_UNUSED(index)

    bool isChecked = false;
    if (!opacityCheckBox->isChecked())
        isChecked = true;

    #ifdef TUP_DEBUG
        qDebug() << "TupColorDialog::updateOpacitySelection() - isChecked? -> " << isChecked;
        qDebug() << "TupColorDialog::updateOpacitySelection() - current index -> " << tabWidget->currentIndex();
    #endif

    updateOpacityCheckBox(isChecked);
    if (isChecked) {
        if (tabWidget->currentIndex() == Border) {
            borderOpacityFlag = true;
            borderTab->setColor(Qt::transparent);
            if (fillOpacityFlag) {
                fillTab->setColor(Qt::black);
                fillOpacityFlag = false;
            }
        } else { // Fill
            fillOpacityFlag = true;
            fillTab->setColor(Qt::transparent);
            if (borderOpacityFlag) {
                borderTab->setColor(Qt::black);
                borderOpacityFlag = false;
            }
        }
    } else {
        if (tabWidget->currentIndex() == Border) {
            borderOpacityFlag = false;
            borderTab->setColor(previousBorderColor);
        } else {
            fillOpacityFlag = false;
            fillTab->setColor(previousFillColor);
        }
    }
}

// Update opacity controls when user choose a new color
void TupColorDialog::updateOpacityStatus(ColorTab tab, const QColor newColor)
{
    if (tab == Border) {
        borderOpacityFlag = false;
        previousBorderColor = newColor;
    } else if (tab == Fill) {
        fillOpacityFlag = false;
        previousFillColor = newColor;
    }

    updateOpacityCheckBox(false);
}

void TupColorDialog::updateOpacityCheckBox(bool flag)
{
    opacityCheckBox->setChecked(flag);

    if (flag) {
        opacityCheckBox->setStyleSheet("* { background-color: rgb(200,200,200);"
                                       " border-radius: " + borderRadius + "px; }");
        opacityCheckBox->updateImage(imgPath + "checked.png");
    } else {
        opacityCheckBox->setStyleSheet("* { background-color: rgb(232,232,232); "
                                       "border-radius: " + borderRadius + "px; }");
        opacityCheckBox->updateImage(imgPath + "unchecked.png");
    }
}

void TupColorDialog::closeDialog()
{
    #ifdef TUP_DEBUG
        qDebug() << "[TupColorDialog::closeDialog()] - Tracing...";
    #endif

    QColor border = borderTab->currentColor();
    QColor fill = fillTab->currentColor();
    QColor bgColor = bgTab->currentColor();

    if (borderOpacityFlag && fillOpacityFlag) { // Both colors are transparent. Restoring default values
        if (tabWidget->currentIndex() == Border || tabWidget->currentIndex() == BgColor)
            fill.setAlpha(static_cast<int>(fillTab->baseOpacityValue() * 255.0));
        else
            border.setAlpha(static_cast<int>(borderTab->baseOpacityValue() * 255.0));
    }

    storeColor(Border, border.name());
    storeColor(Fill, fill.name());
    storeColor(BgColor, bgColor.name());

    emit colorsUpdated(border, fill, bgColor);

    orderInterface();
    accept();
}

void TupColorDialog::cancel()
{
    orderInterface();
    close();
}

void TupColorDialog::orderInterface()
{
    borderTab->selectOptionsTab();
    fillTab->selectOptionsTab();
    bgTab->selectOptionsTab();
    tabWidget->setCurrentIndex(Border);
}

QColor TupColorDialog::getColor(const QString &param, double opacity)
{
    QString colorName = settings->value(param).toString();
    QColor color;

    if (colorName.isEmpty())
        color = Qt::transparent;
    else
        color = QColor(colorName);
    color.setAlpha(static_cast<int>(opacity * 255.0));

    return color;
}

void TupColorDialog::mousePressEvent(QMouseEvent *event)
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

void TupColorDialog::setParameters(Qt::BrushStyle borderStyle, double borderOpacity,
                                   Qt::BrushStyle fillStyle, double fillOpacity)
{
    if (borderOpacity == 0.0 && !opacityCheckBox->isChecked()) {
        updateOpacitySelection(0);
    } else if (borderOpacity == 0.0 && opacityCheckBox->isChecked()) {
        updateOpacitySelection(0);
    } else {
        borderTab->setParameters(borderStyle, borderOpacity);
    }

    fillOpacityFlag = false;

    if (fillOpacity == 0.0)
        fillOpacityFlag = true;

    fillTab->setParameters(fillStyle, fillOpacity);
}

void TupColorDialog::resetBorderColor()
{
    borderTab->setColor(Qt::black);
}

void TupColorDialog::storeColor(ColorTab tab, const QString &color)
{
    #ifdef TUP_DEBUG
        qDebug() << "[TupColorDialog::storeColor()] - tab -> " << tab;
        qDebug() << "[TupColorDialog::storeColor()] - color -> " << color;
    #endif

    QStringList colorHistoryList;
    colorHistoryList << "borderHistory" << "fillHistory" << "bgHistory";
    QString key = colorHistoryList.at(tab);
    QString history = settings->value(key, "").toString();

    if (!history.contains(color)) {
        QStringList colors = history.split(";");
        if (QColor(color).alpha() > 0) {
            if (colors.size() > 10)
                colors.removeFirst();
            colors << color;
            history = "";
            foreach (QString item, colors) {
                if (!item.isEmpty())
                    history += item + ";";
            }
            history.chop(1);
            settings->setValue(key, history);
            switch (tab) {
                case Border:
                    borderTab->updateHistory(color);
                break;
                case Fill:
                    fillTab->updateHistory(color);
                break;
                case BgColor:
                    bgTab->updateHistory(color);
                break;
            }
        }
    }
}

void TupColorDialog::resetColorHistory()
{
    borderTab->resetColorHistory();
    fillTab->resetColorHistory();
    bgTab->resetColorHistory();
}

void TupColorDialog::loadHistory()
{
    borderTab->loadColorHistory();
    fillTab->loadColorHistory();
    bgTab->loadColorHistory();
}
