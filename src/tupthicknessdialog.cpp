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

#include "tupthicknessdialog.h"
#include "tupslider.h"
#include "tupbutton.h"

#include <QGuiApplication>
#include <QScreen>

TupThicknessDialog::TupThicknessDialog(QSettings *config, QPen penVar,
                                       double transparency, QWidget *parent) : QDialog(parent)
{
    setModal(true);
    setWindowFlags(Qt::Popup);

    isCellPhone = config->value("isCellPhone").toBool();

    QString system = config->value("system").toString();
    imgPath = config->value("imgPath").toString();

    lowRes = config->value("lowRes").toBool();
    QString bgColor = config->value("themeColor").toString();
    titleSize = config->value("titleSize").toInt();
    titleHeight = config->value("titleHeight").toInt();
    borderRadius = config->value("borderRadius").toString();
    iconWidth = config->value("iconWidth").toInt();
    dialogButtonsHeight = config->value("dialogButtonsHeight").toInt();

    int vSliderWidth = config->value("vSliderWidth").toInt();
    int hSliderWidth = config->value("hSliderWidth").toInt();
    sliderSize = QSize(vSliderWidth, hSliderWidth);
    size = config->value("strokeDialogSize").toSize();

#if defined(Q_OS_ANDROID) || defined(Q_OS_IOS)
    if (!isCellPhone)
        setStyleSheet("* { background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
                      "stop:0 rgb(100, 100, 100), stop:1 rgb(30, 30, 30)); }");
#endif

    pen = penVar;
    opacity = transparency;
    currentSize = pen.width();

    int sliderW;
    int sliderH;
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
    } else {
        sliderW = static_cast<int>(size.width() * 0.7);
        sliderH = static_cast<int>(size.height() * 0.2);
    }

    innerLayout = new QVBoxLayout;
    setLabelPanel();
    setBrushCanvas();
    setSlider(sliderW, sliderH);
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

TupThicknessDialog::~TupThicknessDialog()
{
}

void TupThicknessDialog::setBrushCanvas()
{
    thickPreview = new TupPenPreviewCanvas(TupPenPreviewCanvas::Border, pen.width(), pen.color(),
                                           pen.brush().style(), opacity, this);
    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(thickPreview);
    innerLayout->addLayout(layout);
}

void TupThicknessDialog::setLabelPanel()
{
	QFont font = this->font();
    font.setBold(true);
    font.setPointSize(titleSize);
	
    sizeLabel = new QLabel(tr("Thickness: ") + QString::number(currentSize));
    sizeLabel->setStyleSheet("QLabel { background-color: rgb(200,200,200); "
                                "border-radius: " + borderRadius + "px; }");
    sizeLabel->setFixedHeight(titleHeight);
    sizeLabel->setFont(font);
    sizeLabel->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);

    innerLayout->addWidget(sizeLabel);
}

void TupThicknessDialog::setSlider(int sliderW, int sliderH)
{
    slider = new TupSlider(Qt::Horizontal, imgPath, sliderSize, TupSlider::Size, pen.color(), pen.color());
    slider->setFixedWidth(sliderW);
    slider->setParameters(pen.brush().style(), opacity);
    slider->setRange(1, 100);
    connect(slider, &TupSlider::valueChanged, this, &TupThicknessDialog::modifySize);

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

void TupThicknessDialog::modifySize(int value)
{
    currentSize = value;
    sizeLabel->setText(tr("Thickness: ") + QString::number(currentSize));
    QColor color = pen.color();

#ifdef TUP_DEBUG
    qDebug() << "Thickness: " << currentSize;
    qDebug() << "Color: " << color;
    qDebug() << "Opacity: " << opacity;
#endif

    thickPreview->setParameters(color, opacity, pen.brush().style());
    thickPreview->updatePenSize(currentSize);

    slider->setParameters(pen.brush().style(), opacity);
    slider->setColors(color, color);
}

void TupThicknessDialog::setButtonsPanel()
{
    TupButton *okButton = new TupButton(imgPath + "ok.png", iconWidth);
    okButton->setFixedHeight(dialogButtonsHeight);
    connect(okButton, &TupButton::clicked, this, &TupThicknessDialog::closeDialog);

    TupButton *closeButton = new TupButton(imgPath + "close.png", iconWidth);
    closeButton->setFixedHeight(dialogButtonsHeight);
    connect(closeButton, &TupButton::clicked, this, &QWidget::close);

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(closeButton);
    buttonLayout->addWidget(okButton);

    innerLayout->addLayout(buttonLayout);
    innerLayout->setAlignment(buttonLayout, Qt::AlignRight);
}

void TupThicknessDialog::resizeEvent(QResizeEvent *event)
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

void TupThicknessDialog::setParameters(int size, QPen penVar, double transparency)
{
    slider->setValue(size);
    pen = penVar;
    opacity = transparency;

    modifySize(size);
}

void TupThicknessDialog::closeDialog()
{
    emit penUpdated(currentSize);
    accept();
}

void TupThicknessDialog::mousePressEvent(QMouseEvent *event)
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
