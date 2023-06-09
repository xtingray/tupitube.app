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

#include "tupopacitypanel.h"
#include "tupbutton.h"

#include <QGuiApplication>
#include <QScreen>

TupOpacityPanel::TupOpacityPanel(QSettings *config, TupPenPreviewCanvas::CanvasType type,
                                 QWidget *parent) : QWidget(parent)
{
    settings = config;
    isCellPhone = settings->value("isCellPhone").toBool();
    lowRes = settings->value("lowRes").toBool();
    titleSize = settings->value("titleSize").toInt();
    titleHeight = settings->value("titleHeight").toInt();
    borderRadius = settings->value("borderRadius").toString();
    int vSliderWidth = settings->value("vSliderWidth").toInt();
    int hSliderWidth = settings->value("hSliderWidth").toInt();
    sliderSize = QSize(vSliderWidth, hSliderWidth);
    QSize size = settings->value("opacityDialogSize").toSize();
    QString system = settings->value("system").toString();
    imgPath = config->value("imgPath").toString();

    int sliderW;
    int sliderH;
    int fillSize;
    if (isCellPhone) {
        QScreen *screen = QGuiApplication::screens().at(0);
        int width = screen->geometry().width();
        int height = screen->geometry().height();

        if (width < height) {
            height = width;
            sliderW = static_cast<int>(width * 0.8);
        } else {
            sliderW = static_cast<int>(width * 0.6);
        }
        sliderH = static_cast<int>(height * 0.15);
        fillSize = static_cast<int> (height * 0.25);
    } else {
        sliderW = static_cast<int>(size.width() * 0.7);
        sliderH = static_cast<int>(size.height() * 0.2);
        fillSize = sliderH;
    }

    canvasType = type;
    int index;
    if (canvasType == TupPenPreviewCanvas::Border) {
        penSize = settings->value("penSize").toInt();

        QString colorName = settings->value("borderColor").toString();
        color = QColor(colorName);
        opacity = settings->value("borderOpacity").toDouble();
        index = settings->value("borderBrush").toInt();
    } else {
        penSize = fillSize;

        QString colorName = settings->value("fillColor").toString();
        color = QColor(colorName);
        opacity = settings->value("fillOpacity").toDouble();
        index = settings->value("fillBrush").toInt();
    }
    style = Qt::BrushStyle(index);

/*
#ifdef TUP_DEBUG
    qDebug() << "";
    qDebug() << "tab type: " << type;
    qDebug() << "current opacity: " << opacity;
#endif
*/

    innerLayout = new QVBoxLayout;

    setLabelPanel();
    setOpacityCanvas();
    setSlider(sliderW, sliderH);

    setLayout(innerLayout);
}

TupOpacityPanel::~TupOpacityPanel()
{
}

void TupOpacityPanel::setOpacityCanvas()
{
    opacityPreview = new TupPenPreviewCanvas(canvasType, penSize, color, style, opacity, this);
    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(opacityPreview);
    innerLayout->addLayout(layout);
}

void TupOpacityPanel::setLabelPanel()
{
    QString number = QString::number(opacity);
    if (number.length() == 3)
        number = number + "0";

    if (number.compare("1") == 0)
        number = "1.00";

    QFont font = this->font();
    font.setBold(true);
    font.setPointSize(titleSize);
	
    opacityLabel = new QLabel(tr("Transparency: ") + number);
    opacityLabel->setStyleSheet("QLabel { background-color: rgb(200,200,200);"
                                "border-radius: " + borderRadius + "px; }");
    opacityLabel->setFont(font);
    opacityLabel->setFixedHeight(titleHeight);
    opacityLabel->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);

    innerLayout->addWidget(opacityLabel);
}

void TupOpacityPanel::setSlider(int sliderW, int sliderH)
{
    slider = new TupSlider(Qt::Horizontal, imgPath, sliderSize, TupSlider::Opacity, color, color);
    slider->setFixedWidth(sliderW);
    slider->setRange(0, 100);
    slider->setParameters(style, opacity);

    slider->setValue(static_cast<int>(opacity * 100));
    connect(slider, &TupSlider::valueChanged, this, &TupOpacityPanel::modifyOpacity);

    QWidget *widget = new QWidget;
    widget->setFixedHeight(sliderH);

    QBoxLayout *layout = new QHBoxLayout(widget);

    QWidget *space1 = new QWidget(this);
    layout->addWidget(space1);
    layout->addWidget(slider);

    QWidget *space2 = new QWidget(this);
    layout->addWidget(space2);

    innerLayout->addWidget(widget);
}

void TupOpacityPanel::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event)

    if (isCellPhone) {
        QScreen *screen = QGuiApplication::screens().at(0);
        int width = screen->geometry().width();
        int height = screen->geometry().height();

        if (width > height)
            slider->setFixedWidth(static_cast<int>(width * 0.6));
        else
            slider->setFixedWidth(static_cast<int>(width * 0.8));
    }
}

void TupOpacityPanel::modifyOpacity(int value)
{
    opacity = value / 100.0;

#ifdef TUP_DEBUG
    qDebug() << "TupOpacityPanel::modifyOpacity() - tab: " << canvasType;
    qDebug() << "TupOpacityPanel::modifyOpacity() - opacity: " << opacity;
#endif

    if (opacity == 0.0) {
        opacityLabel->setText(tr("Transparency: ") + "0.00");
        emit opacityZero(canvasType);
    } else if (opacity == 1.0) {
        opacityLabel->setText(tr("Transparency: ") + "1.00");
    } else {
        QString number = QString::number(opacity);
        if (number.length() == 3)
            number = number + "0";
        opacityLabel->setText(tr("Transparency: ") + number);
    }

    opacityPreview->updateOpacity(opacity);
}

void TupOpacityPanel::setParameters(QPen pen)
{
#ifdef TUP_DEBUG
    qDebug() << "";
    qDebug() << "TupOpacityPanel::setParameters() - tab: " << canvasType;
    qDebug() << "TupOpacityPanel::setParameters() - pen: " << pen;
    qDebug() << "TupOpacityPanel::setParameters() - opacity: " << opacity;
#endif

    QColor color = pen.color();
    if (color.alpha() == 0) {
        color.setAlpha(255);
        pen.setColor(color);
    }

    opacityPreview->setParameters(pen);
    slider->setColors(color, color);
}

void TupOpacityPanel::setParameters(QColor color, Qt::BrushStyle brushStyle)
{
#ifdef TUP_DEBUG
    qDebug() << "";
    qDebug() << "TupOpacityPanel::setParameters() - tab: " << canvasType;
    qDebug() << "TupOpacityPanel::setParameters() - color: " << color;
    qDebug() << "TupOpacityPanel::setParameters() - opacity: " << opacity;
    qDebug() << "TupOpacityPanel::setParameters() - Qt::BrushStyle: " << brushStyle;
#endif

    if (color.alpha() == 0)
        color.setAlpha(255);

    opacityPreview->setParameters(color, brushStyle);
    slider->setColors(color, color);
}

void TupOpacityPanel::setOpacityValue(double value)
{
#ifdef TUP_DEBUG
    qDebug() << "";
    qDebug() << "TupOpacityPanel::setOpacityValue() - value: " << value;
#endif

    slider->setValue(static_cast<int>(value));
    modifyOpacity(static_cast<int>(value));
}

void TupOpacityPanel::resetOpacityValue()
{
#ifdef TUP_DEBUG
    qDebug() << "";
    qDebug() << "Tracing...";
#endif

    opacityLabel->setText(tr("Transparency: ") + "1.00");
    slider->blockSignals(true);
    slider->setValue(100);
    slider->blockSignals(false);

    opacityPreview->updateOpacity(1.0);
}

double TupOpacityPanel::opacityValue()
{
    return opacity;
}
