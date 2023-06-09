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

#include "tupcolortab.h"

#include <QGuiApplication>
#include <QScreen>

TupColorTab::TupColorTab(QSettings *settings, QPixmap *palette, ColorTab tab,
                         QColor colorVar, Qt::BrushStyle brushStyle, double opacityVal,
                         QWidget *parent) : QWidget(parent)
{
    isCellPhone = settings->value("isCellPhone").toBool();
    lowRes = settings->value("lowRes").toBool();    
    borderRadius = settings->value("borderRadius").toString();
    titleSize = settings->value("titleSize").toInt();
    titleHeight = settings->value("titleHeight").toInt();
    headerHeight = settings->value("boldTitleHeight").toInt();
    iconWidth = settings->value("iconWidth").toInt();
    dialogButtonsHeight = settings->value("dialogButtonsHeight").toInt();
    colorCellSize = settings->value("colorCellSize").toSize();

    baseColorHeight = settings->value("baseColorHeight").toInt();
    baseColorCellSize = settings->value("baseColorCellSize").toSize();
    baseSpacing = settings->value("baseColorSpacing").toInt();
    buttonParams = settings->value("colorButtonParams").toString();
    colorSliderHeight = settings->value("colorSliderHeight", colorSliderHeight).toInt();

    QString system = settings->value("system").toString();
    imgPath = settings->value("imgPath").toString();

    int vSliderWidth = settings->value("vSliderWidth").toInt();
    int hSliderWidth = settings->value("hSliderWidth").toInt();
    sliderSize = QSize(vSliderWidth, hSliderWidth);

    colorTab = tab;
    color = colorVar;

    colorBrush = QBrush(colorVar);
    colorBrush.setStyle(brushStyle);
    opacity = opacityVal;
    if (opacityVal == 0.0)
        baseOpacity = 1;
    else
        baseOpacity = opacityVal;

    int alpha = static_cast<int>(opacity * 255.0);
    opacityLabel = " - " + tr("Alpha: ") + QString::number(alpha);

    innerLayout = new QVBoxLayout;
    optionsLayout = new QVBoxLayout;
    historyLayout = new QVBoxLayout;

    setLabelPanel();

    tabWidget = new QTabWidget;
    tabWidget->setStyleSheet("QTabWidget { border: none; }"
                             "QTabBar::tab:selected { "
                             "background-color: rgba(0, 137, 0, 100); "
                             "}"
                             "QTabBar::tab:!selected {"
                             "background-color: rgba(50, 50, 50, 50); "
                             "}");
    setPickerArea(palette);    
    setBaseColorsPanel();
    setSliderPanel();

    QWidget *optionsWidget = new QWidget;
    optionsWidget->setLayout(optionsLayout);
    tabWidget->addTab(optionsWidget, tr("Options"));

    historyWidget = new TupColorHistory(tab, settings);
    connect(historyWidget, SIGNAL(colorClicked(const QString &)), this, SLOT(setColor(const QString &)));

    tabWidget->addTab(historyWidget, tr("History"));

    innerLayout->addWidget(tabWidget);
    setLayout(innerLayout);
	
    if (isCellPhone)
        move(QPoint(0, 0));
}

TupColorTab::~TupColorTab()
{
}

void TupColorTab::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event)

    if (isCellPhone) {
        QScreen *screen = QGuiApplication::screens().at(0);
        int width = screen->geometry().width();
        int height = screen->geometry().height();

        QString text = colorToString(color);
        bool isVisible = true;
        if ((width > height) && (colorTab != BgColor)) {
            text = colorToString(color) + opacityLabel;
            isVisible = false;
        }
        sliderWidget->setVisible(isVisible);
        colorLabel->setText(text);
    }
}

void TupColorTab::initSlider()
{
    if (color != Qt::black)
        colorSlider->setValue(100);
}

void TupColorTab::setLabelPanel()
{
    QWidget *labelWidget = new QWidget(this);
    labelWidget->setStyleSheet("QWidget { background-color: rgb(232, 232, 232); }");
    labelWidget->setFixedHeight(headerHeight);

    QHBoxLayout *layout = new QHBoxLayout(labelWidget);
    QString colorText = colorToString(color);

    if (!isCellPhone && (colorTab != BgColor)) {
        colorText += opacityLabel;
    } else {
        QScreen *screen = QGuiApplication::screens().at(0);
        int width = screen->geometry().width();
        int height = screen->geometry().height();

        if ((width > height) && (colorTab != BgColor))
            colorText += opacityLabel;
    }

    colorCell = new TupCurrentColor(colorBrush, opacity, colorCellSize);
    colorCell->setFixedSize(colorCellSize);
    layout->addWidget(colorCell, Qt::AlignVCenter);
    layout->addSpacing(5);

    QFont font = this->font();
    font.setBold(true);
    font.setPointSize(titleSize);

    colorLabel = new QLabel(colorText);
    colorLabel->setStyleSheet("QLabel { background-color: rgb(190, 190, 190); "
                              "border-radius: " + borderRadius + "px; }");
    colorLabel->setFont(font);
    colorLabel->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    colorLabel->setFixedHeight(titleHeight);

    layout->addWidget(colorLabel, Qt::AlignVCenter);

    innerLayout->addWidget(labelWidget);
}

void TupColorTab::setPickerArea(QPixmap *palette)
{
    pickerArea = new TupColorPicker(palette, this);
    connect(pickerArea, &TupColorPicker::newColor, this, &TupColorTab::updateFromPicker);

    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(pickerArea);

    optionsLayout->addLayout(layout);
}

void TupColorTab::setBaseColorsPanel()
{
    buttonPanel = new TupColorButtonPanel(color, colorBrush.style(), baseColorCellSize,
                                          baseSpacing, buttonParams, this);
    connect(buttonPanel, &TupColorButtonPanel::clickColor, this, &TupColorTab::updateFromPanel);
    buttonPanel->setFixedHeight(baseColorHeight);
	
    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(buttonPanel);

    optionsLayout->addLayout(layout);
}

void TupColorTab::setSliderPanel()
{
    sliderWidget = new QWidget();
    sliderWidget->setStyleSheet("QWidget { background-color: rgb(232, 232, 232); }");
    sliderWidget->setFixedHeight(colorSliderHeight);

    QBoxLayout *sliderLayout = new QVBoxLayout(sliderWidget);
    sliderLayout->setAlignment(Qt::AlignVCenter);

    if (color != Qt::black) {
        colorSlider = new TupSlider(Qt::Horizontal, imgPath, sliderSize, TupSlider::Color, QColor(0, 0, 0), color);
        colorSlider->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        colorSlider->setRange(0, 100);
        colorSlider->setValue(100);
        topColor = color;
    } else {
        colorSlider = new TupSlider(Qt::Horizontal, imgPath, sliderSize, TupSlider::Color, QColor(0, 0, 0), Qt::white);
        colorSlider->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        colorSlider->setRange(0, 100);
        colorSlider->setValue(0);
        topColor = Qt::white;
    }

    sliderLayout->addWidget(colorSlider);
    connect(colorSlider, &TupSlider::valueChanged, this, &TupColorTab::updateFromSlider);

    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(sliderWidget);

    optionsLayout->addLayout(layout);
}

void TupColorTab::updateColorLabel(const QColor &color)
{
    QString colorText = colorToString(color);
    if (isCellPhone) {
        QScreen *screen = QGuiApplication::screens().at(0);
        int width = screen->geometry().width();
        int height = screen->geometry().height();

        QString text0 = colorText;
        if ((width > height) && (colorTab != BgColor))
            text0 = colorText + opacityLabel;
        colorLabel->setText(text0);
    } else {
        colorLabel->setText(colorText + opacityLabel);
    }
}

void TupColorTab::updateFromPanel(QColor newColor)
{
    if (colorTab == Border || colorTab == Fill) {
        if (opacity == 0.0)
            opacity = baseOpacity;
        newColor.setAlpha(static_cast<int>(opacity * 255.0));
    }

    updateColorLabel(newColor);
    colorBrush.setColor(newColor);
    colorCell->setParameters(colorBrush, opacity);

    if (newColor != Qt::black) {
        colorSlider->setColors(Qt::black, newColor);
        colorSlider->setValue(100);
        topColor = newColor;
    } else {
        colorSlider->setColors(Qt::black, Qt::white);
        colorSlider->setValue(0);
        topColor = Qt::white;
    }

    color = newColor;
    emit colorUpdated(colorTab, color);
}

void TupColorTab::updateFromPicker(QColor newColor)
{
    if (colorTab == Border || colorTab == Fill) {
        if (opacity == 0.0)
            opacity = baseOpacity;
        newColor.setAlpha(static_cast<int>(opacity * 255.0));
    }

    updateColorLabel(newColor);

    colorBrush.setColor(newColor);
    colorCell->setParameters(colorBrush, opacity);
    buttonPanel->resetPanel();

    if (newColor != Qt::black) {
        colorSlider->setColors(Qt::black, newColor);
        colorSlider->setValue(100);
        topColor = newColor;
    } else {
        colorSlider->setColors(Qt::black, Qt::white);
        colorSlider->setValue(0);
        topColor = Qt::white;
    }

    color = newColor;
    emit colorUpdated(colorTab, color);
}

void TupColorTab::updateFromSlider(int index)
{    
    int r = topColor.red();
    int g = topColor.green();
    int b = topColor.blue();

    r = (r*index) / 100;
    g = (g*index) / 100;
    b = (b*index) / 100;

    color = QColor(r, g, b);

    if (colorTab == Border || colorTab == Fill) {
        if (opacity == 0.0)
            opacity = baseOpacity;
        color.setAlpha(static_cast<int>(opacity * 255.0));
    }

    updateColorLabel(color);
    colorBrush.setColor(color);
    colorCell->setParameters(colorBrush, opacity);
    buttonPanel->resetPanel();

    emit colorUpdated(colorTab, color);
}

void TupColorTab::setColor(const QColor newColor)
{
    #ifdef TUP_DEBUG
        qDebug() << "TupMainWindow::setColor() - New color: " << newColor;
    #endif

    color = newColor;

    if (colorTab == Border || colorTab == Fill) {
        if (color.alpha() == 0) {
            opacity = 0;
        } else {
            opacity = baseOpacity;
            color.setAlpha(static_cast<int>(opacity * 255.0));
        }
    }

    updateColorLabel(color);
    colorBrush.setColor(color);
    colorCell->setParameters(colorBrush, opacity);
    buttonPanel->resetPanel();

    if (newColor != Qt::black) {
        colorSlider->setColors(Qt::black, color);
        colorSlider->setValue(100);
        topColor = color;
    } else {
        colorSlider->setColors(Qt::black, Qt::white);
        colorSlider->setValue(0);
        topColor = Qt::white;
    }
}

QString TupColorTab::colorToString(const QColor color)
{
    if (colorTab == Border || colorTab == Fill) {
        int alpha = static_cast<int>(opacity * 255.0);
        opacityLabel = " - " + tr("Alpha: ") + QString::number(alpha);
    }

    QString text = "RGB: " + QString::number(color.red()) + ", "
                   + QString::number(color.green()) + ", "
                   + QString::number(color.blue());
    return text;
}

QColor TupColorTab::currentColor() const
{
    #ifdef TUP_DEBUG
        qDebug() << "TupColorTab::currentColor() - color type: " << colorTab;
        qDebug() << "TupColorTab::currentColor() - color: " << color;
        qDebug() << "TupColorTab::currentColor() - alpha: " << color.alpha();
        qDebug() << "";
    #endif

    return color;
}

double TupColorTab::baseOpacityValue()
{
    return baseOpacity;
}

void TupColorTab::setParameters(Qt::BrushStyle brushStyle, double transp)
{
    #ifdef TUP_DEBUG
        qDebug() << "---";
        qDebug() << "TupColorTab::setParameters(() - color: " << color.name();
        qDebug() << "TupColorTab::setParameters(() - opacity: " << transp;
        qDebug() << "TupColorTab::setParameters(() - brush style: " << brushStyle;
    #endif

    opacity = transp;
    int alpha = static_cast<int>(opacity * 255.0);

    colorBrush.setStyle(brushStyle);
    QColor c = colorBrush.color();
    c.setAlpha(alpha);
    colorBrush.setColor(c);

    opacityLabel = " - " + tr("Alpha: ") + QString::number(alpha);

    updateColorLabel(c);
    colorCell->setParameters(colorBrush, opacity);
    buttonPanel->setBrushStyle(brushStyle);

    color = c;
}

void TupColorTab::selectOptionsTab()
{
    historyWidget->clearSelection();
    tabWidget->setCurrentIndex(0);
}

void TupColorTab::setColor(const QString &color)
{
    #ifdef TUP_DEBUG
        qDebug() << "TupColorTab::setColor() - color -> " << color;
    #endif

    updateFromPanel(color);
}

void TupColorTab::updateHistory(const QString &color)
{
    historyWidget->appendHistoryRecord(color);
}

void TupColorTab::clearHistorySelection()
{
    historyWidget->clearSelection();
}

void TupColorTab::resetColorHistory()
{
    historyWidget->resetColorList();
}

void TupColorTab::loadColorHistory()
{
    historyWidget->loadColorHistory(colorTab);
}
