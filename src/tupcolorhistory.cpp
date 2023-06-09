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

#include "tupcolorhistory.h"
#include "tuplabel.h"

#include <QScrollArea>
#include <QScroller>
#include <QListWidget>

TupColorHistory::TupColorHistory(ColorTab tab, QSettings *config, QWidget *parent) : QWidget(parent)
{    
    noRecords = true;
    previousItem = nullptr;
    bgDefaultStyle = "QWidget { background-color: rgb(255,255,255); }";
    settings = config;
    bool isCellPhone = config->value("isCellPhone").toInt();
    QString themeColor = settings->value("themeColor").toString();
    iconWidth = config->value("iconWidth").toInt();
    textSize = config->value("helpTextSize").toInt();

    listArea = new QScrollArea;
    listArea->setWidgetResizable(true);
    listArea->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);

    if (!isCellPhone) {
        listArea->setStyleSheet("QScrollArea {"
            "background-color: rgb(" + themeColor + ");"
            "}"
            "QScrollBar:vertical {"
            "background-color: rgb(200,200,200);"
            "}"
            "QScrollBar::handle:vertical {"
            "background-color: rgb(130,130,130);"
            "}"
            "QScrollBar:horizontal {"
            "background-color: rgb(200,200,200);"
            "}"
            "QScrollBar::handle:horizontal {"
            "background-color: rgb(130,130,130);"
            "}"
        );
    } else {
        listArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        listArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    }

    listArea->setWidget(colorsPage(tab));
    QScroller::grabGesture(listArea, QScroller::LeftMouseButtonGesture);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(listArea);
}

TupColorHistory::~TupColorHistory()
{
}

QWidget * TupColorHistory::colorsPage(ColorTab tab)
{
    #ifdef TUP_DEBUG
        qDebug() << "TupColorHistory::colorsPage() - Loading color records for tab -> " << tab;
    #endif

    QWidget *widget = new QWidget;
    widget->setStyleSheet(bgDefaultStyle);

    contentLayout = new QVBoxLayout;

    QStringList colorHistoryList;
    colorHistoryList << "borderHistory" << "fillHistory" << "bgHistory";
    QString colors = settings->value(colorHistoryList.at(tab)).toString();

    listWidget = new QListWidget;
    listWidget->setStyleSheet("QListWidget::item:selected {"
                              "background-color: rgba(255, 255, 255, 0);"
                              "}");

    if (!colors.isEmpty()) {
        noRecords = false;
        QStringList colorsList = colors.split(";");
        if (colorsList.size() > 0) {
            int i = 0;
            foreach (QString color, colorsList) {
                if (!color.isEmpty()) {
                    addHistoryRecord(i, color);
                    i++;
                }
            }
        }
    } else {
        addHistoryRecord(0, tr("No Colors"));
    }

    contentLayout->addWidget(listWidget);
    widget->setLayout(contentLayout);

    return widget;
}

QWidget * TupColorHistory::addColorItem(int index, const QString &color)
{
    #ifdef TUP_DEBUG
        qDebug() << "TupColorHistory::addColorItem() - color -> " << color;
    #endif

    QHBoxLayout *itemLayout = new QHBoxLayout;

    if (color.compare(tr("No Colors")) != 0) {
        QPixmap pix(iconWidth, iconWidth);
        pix.fill(QColor(color));

        TupLabel *itemIcon = new TupLabel(color, index, pix);
        itemIcon->setFixedWidth(iconWidth);
        connect(itemIcon, &TupLabel::colorClicked, this, &TupColorHistory::setSelectedItem);

        itemLayout->addWidget(itemIcon);
    }

    QFont textFont = this->font();
    textFont.setBold(false);
    textFont.setPointSize(textSize);

    TupLabel *itemLabel = new TupLabel(color, index);
    itemLabel->setFont(textFont);
    if (color.compare(tr("No Colors")) != 0)
        connect(itemLabel, &TupLabel::colorClicked, this, &TupColorHistory::setSelectedItem);
    else
        itemLabel->setAlignment(Qt::AlignCenter);

    itemLayout->addWidget(itemLabel);

    QWidget *item = new QWidget;
    item->setLayout(itemLayout);
    item->setFixedHeight(static_cast<int>(iconWidth * 1.5));

    return item;
}

void TupColorHistory::addHistoryRecord(int index, const QString &color)
{
    #ifdef TUP_DEBUG
        qDebug() << "TupColorHistory::addHistoryRecord() - index -> " << index;
        qDebug() << "TupColorHistory::addHistoryRecord() - color -> " << color;
    #endif

    QListWidgetItem *colorItem = new QListWidgetItem;
    colorItem->setSizeHint(QSize(static_cast<int>(0.8 * width()), static_cast<int>(iconWidth * 1.5)));
    listWidget->addItem(colorItem);
    listWidget->setItemWidget(colorItem, addColorItem(index, color));
}

void TupColorHistory::appendHistoryRecord(const QString &color)
{
    #ifdef TUP_DEBUG
        qDebug() << "TupColorHistory::appendHistoryRecord() - color -> " << color;
    #endif

    if (noRecords) {
        listWidget->takeItem(0);
        noRecords = false;
    }

    addHistoryRecord(listWidget->count(), color);
}

void TupColorHistory::setSelectedItem(int index, const QString &color)
{
    #ifdef TUP_DEBUG
        qDebug() << "TupColorHistory::setSelectedItem() - index -> " << index;
    #endif

    if (previousItem)
        previousItem->setStyleSheet(bgDefaultStyle);

    QListWidgetItem *item = listWidget->item(index);
    if (item) {
        QWidget *widget = static_cast<QWidget *>(listWidget->itemWidget(item));
        if (widget) {
            widget->setStyleSheet("QWidget { background-color: rgba(205, 230, 205, 255); }");
            previousItem = widget;
        }
    }

    emit colorClicked(color);
}

void TupColorHistory::clearSelection()
{
    if (previousItem) {
        previousItem->setStyleSheet(bgDefaultStyle);
        previousItem = nullptr;
    }

    listWidget->clearSelection();
}

void TupColorHistory::resetColorList()
{
    listWidget->clear();
    addHistoryRecord(0, tr("No Colors"));
}

void TupColorHistory::loadColorHistory(ColorTab tab)
{
    listArea->setWidget(colorsPage(tab));
}
