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

#include "tupgallerydialog.h"
#include "tupproject.h"

#include <QGuiApplication>
#include <QScreen>
#include <QPixmap>
#include <QScroller>
#include <QScrollBar>
#include <QStandardPaths>
#include <QDirIterator>
#include <QStringList>
#include <QTimer>

TupGalleryDialog::TupGalleryDialog(QSettings *config, QWidget *parent) : QDialog(parent)
{
    setModal(true);
    setWindowFlags(Qt::Popup);

    isCellPhone = config->value("isCellPhone").toBool();

    QString system = config->value("system").toString();
    QString imgPath = config->value("imgPath").toString();

    lowRes = config->value("lowRes").toBool();
    themeColor = config->value("themeColor").toString();
    titleSize = config->value("titleSize").toInt();
    titleHeight = config->value("titleHeight").toInt();
    borderRadius = config->value("borderRadius").toString();
    iconWidth = config->value("iconWidth").toInt();
    int dialogButtonsHeight = config->value("dialogButtonsHeight").toInt();
    rowSize = config->value("galleryRowSize").toInt();
    QSize dialogSize = config->value("galleryDialogSize").toSize();
    dialogW = dialogSize.width();
    dialogH = dialogSize.height();

    id = "";

#if defined(Q_OS_ANDROID) || defined(Q_OS_IOS)
    if (!isCellPhone && lowRes)
        setStyleSheet("* { background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
                      "stop:0 rgb(100, 100, 100), stop:1 rgb(30, 30, 30)); }");
#endif

    QFont font = this->font();
    font.setBold(true);
    font.setPointSize(titleSize);

    titleLabel = new QLabel(tr("My Projects"));
    titleLabel->setStyleSheet("QLabel { background-color: rgb(200,200,200); "
                              "color: black; border-radius: " + borderRadius + "px; }");
    titleLabel->setFixedHeight(titleHeight);
    titleLabel->setFont(font);
    titleLabel->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);

    QWidget *titleWidget = new QWidget(this);
    QBoxLayout *layout = new QHBoxLayout(titleWidget);
    layout->addWidget(titleLabel, Qt::AlignCenter);

    font.setBold(false);
    msgWidget = new QWidget;

    #if !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS)
        double value = dialogButtonsHeight * 1.2;
        msgWidget->setFixedHeight(value);
    #else
        msgWidget->setFixedHeight(dialogButtonsHeight);
    #endif

    QLabel *msgLabel = new QLabel(tr("Are you sure?"));
    msgLabel->setAlignment(Qt::AlignCenter);
    msgLabel->setFont(font);
    msgWidget->setVisible(false);

    TupButton *cancelButton = new TupButton(imgPath + "close.png", iconWidth);
    connect(cancelButton, &TupButton::clicked, this, &TupGalleryDialog::closeMessageDialog);

    TupButton *confirmButton = new TupButton(imgPath + "ok.png", iconWidth);
    connect(confirmButton, &TupButton::clicked, this, &TupGalleryDialog::removeProject);

    QHBoxLayout *msgLayout = new QHBoxLayout(msgWidget);
    msgLayout->addStretch(0);
    msgLayout->addWidget(msgLabel);
    msgLayout->addSpacing(iconWidth);
    msgLayout->addWidget(cancelButton);
    msgLayout->addWidget(confirmButton);
    msgLayout->addStretch(0);

    trashButton = new TupButton(imgPath + "trash.png", iconWidth);
    trashButton->setFixedHeight(dialogButtonsHeight);
    connect(trashButton, &TupButton::clicked, this, &TupGalleryDialog::confirmRemoveAction);
    trashButton->setEnabled(false);

    buttonSeparator = new QWidget;

    openButton = new TupButton(imgPath + "open.png", iconWidth);
    openButton->setFixedHeight(dialogButtonsHeight);
    connect(openButton, &TupButton::clicked, this, &TupGalleryDialog::openProject);
    openButton->setEnabled(false);

    /*
    shareButton = new TupButton(":images/share.png", iconWidth);
    shareButton->setFixedHeight(dialogButtonsHeight);
    connect(shareButton, &TupButton::clicked, this, &TupGalleryDialog::shareProject);
    shareButton->setEnabled(false);
    */

    closeButton = new TupButton(imgPath + "close.png", iconWidth);
    closeButton->setFixedHeight(dialogButtonsHeight);
    connect(closeButton, &TupButton::clicked, this, &QWidget::close);

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(trashButton);
    buttonLayout->addWidget(buttonSeparator);
    buttonLayout->addWidget(closeButton);
    buttonLayout->addWidget(openButton);
    // buttonLayout->addWidget(shareButton);

    buttonsWidget = new QWidget;
    buttonsWidget->setFixedHeight(dialogButtonsHeight);
    QVBoxLayout *bottomLayout = new QVBoxLayout(buttonsWidget);
    bottomLayout->addLayout(buttonLayout);
    bottomLayout->setAlignment(buttonLayout, Qt::AlignRight);

    QBoxLayout *innerLayout = new QVBoxLayout;
    workArea = new QScrollArea;
    workArea->setWidgetResizable(true);
    workArea->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);

    if (!isCellPhone) {
        workArea->setStyleSheet("QScrollArea {"
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
        workArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        workArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    }

    workArea->setAlignment(Qt::AlignHCenter);

    QScroller::grabGesture(workArea, QScroller::LeftMouseButtonGesture);

    innerLayout->addWidget(titleWidget);
    innerLayout->addWidget(workArea);
    innerLayout->addWidget(msgWidget);
    innerLayout->addWidget(buttonsWidget);

    worksPanel = new QWidget;

    if (!isCellPhone && lowRes) {
        worksPanel->setStyleSheet("* { background-color: rgb(200,200,200); }");
        innerWidget = new QWidget;

        QString style = "QWidget { background-color: rgb(" + themeColor + "); ";
        #ifdef Q_OS_ANDROID
            style += "border-radius: " + borderRadius + "px; }";
        #else
            style += "border-style: none; }";
        #endif
        innerWidget->setStyleSheet(style);

        innerWidget->setFixedSize(dialogSize);
        innerWidget->setLayout(innerLayout);

        QHBoxLayout *globalLayout = new QHBoxLayout;
        globalLayout->addWidget(innerWidget);
        setLayout(globalLayout);
    } else {
        worksPanel->setStyleSheet("* { background-color: rgb(220,220,220); }");
        setLayout(innerLayout);
    }

    contentLayout = new QVBoxLayout(worksPanel);
    contentLayout->setAlignment(Qt::AlignCenter);

    loadingLabel = new QLabel(tr("Loading Projects..."));
    loadingLabel->setFont(font);
    contentLayout->addWidget(loadingLabel);

    workArea->setWidget(worksPanel);

    setStyleSheet("* { background-color: rgb(" +  themeColor + "); }");

    if (isCellPhone)
        move(QPoint(0,0));

    QTimer::singleShot(500, this, SLOT(loadWorks()));
}

TupGalleryDialog::~TupGalleryDialog()
{
}

void TupGalleryDialog::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
	
#ifdef Q_OS_ANDROID	
    if (isCellPhone) {
        QScreen *displayDevice = QGuiApplication::screens().at(0);
        int w = displayDevice->geometry().width();
        int h = displayDevice->geometry().height();

        if (w > h)
            buttonSeparator->setFixedWidth(w*0.75);
        else
            buttonSeparator->setFixedWidth(w*0.55);
    } else {
        buttonSeparator->setFixedWidth(dialogH*0.75);
    }
#else
	buttonSeparator->setFixedWidth(dialogW*0.9);
#endif
}

void TupGalleryDialog::loadWorks()
{
    if (isCellPhone || !lowRes)
        smallGalleryPage();
    else
        wideGalleryPage();

    titleLabel->setText(tr("My Projects") + " (" + QString::number(worksTotal) + ")");
}

QStringList TupGalleryDialog::projectsList(const QString &dir)
{
#ifdef TUP_DEBUG
    if (!QDir(dir).exists())
        qDebug() << "TupGalleryDialog::projectsList() - Fatal error: Gallery path doesn't exist! -> " << dir;
#endif

    QDirIterator iterator(dir, QDirIterator::NoIteratorFlags);
    QStringList items;
    while (iterator.hasNext()) {
           iterator.next();
           QString record = iterator.fileName();
           if (record.startsWith("project_")) {
               if (QFile::exists(dir + "/" + record + "/config.xml"))
                   items << record;
           }
    }
    items.sort();

    return items;
}

void TupGalleryDialog::smallGalleryPage()
{
    QString dir = TupProject::projectsDir();
    QStringList items = projectsList(dir);
    worksTotal = items.count();

    for (int i=(worksTotal-1); i >= 0; i--) {
        QString record = items.at(i);
        QPixmap pixmap;
        pixmap.load(dir + "/" + record + "/preview.png");
        if (!pixmap.isNull()) {
            TupCheckBox *work = new TupCheckBox(record, pixmap, themeColor, borderRadius);
            connect(work, &TupCheckBox::selected, this, &TupGalleryDialog::updateSelection);
            connect(work, &TupCheckBox::opened, this, &TupGalleryDialog::openSelectedProject);

            QWidget *widget = new QWidget;
            QHBoxLayout *itemLayout = new QHBoxLayout(widget);
            itemLayout->setAlignment(Qt::AlignCenter);
            itemLayout->addWidget(work);
            contentLayout->addWidget(widget);

            pixPreviewList[record] = pixmap;
            checkBoxList[record] = work;
            widgetList[record] = widget;
        } else {
            #ifdef TUP_DEBUG
                qDebug() << "TupGalleryDialog::smallGalleryPage() - Preview pixmap is NULL in project: " << record;
            #endif
        }
    }

    loadingLabel->setVisible(false);
    contentLayout->removeWidget(loadingLabel);
    contentLayout->addStretch();
}

void TupGalleryDialog::wideGalleryPage()
{
    QString dir = TupProject::projectsDir();
    QStringList items = projectsList(dir);
    worksTotal = items.count();

    worksTotal = items.count();
    int rows = worksTotal/rowSize;
    int mod = worksTotal % rowSize;

    QMargins margins(0, 0, 0, 0);

    // Completed rows
    int index = worksTotal - 1;
    for (int i=0; i<rows; i++) {
         QWidget *worksRow = new QWidget;
         QHBoxLayout *rowLayout = new QHBoxLayout(worksRow);
         rowLayout->setAlignment(Qt::AlignCenter);
         rowLayout->setSpacing(0);
         // rowLayout->setMargin(0);
         rowLayout->setContentsMargins(margins);

         for (int j=0; j<rowSize; j++) {
              QString record = items.at(index);
              QPixmap pixmap;
              pixmap.load(dir + "/" + record + "/preview.png");
              if (!pixmap.isNull()) {
                  TupCheckBox *work = new TupCheckBox(record, pixmap, themeColor, 0);
                  work->setStyleSheet("* { background-color: rgb(200,200,200); }");
                  connect(work, &TupCheckBox::selected, this, &TupGalleryDialog::updateSelection);
                  connect(work, &TupCheckBox::opened, this, &TupGalleryDialog::openSelectedProject);

                  QWidget *workWidget = new QWidget(worksRow);
                  QHBoxLayout *itemLayout = new QHBoxLayout(workWidget);
                  itemLayout->setSpacing(0);
                  // itemLayout->setMargin(0);
                  itemLayout->setContentsMargins(margins);
                  itemLayout->setAlignment(Qt::AlignCenter);
                  itemLayout->addWidget(work, Qt::AlignHCenter);
                  rowLayout->addWidget(workWidget, Qt::AlignHCenter);

                  pixPreviewList[record] = pixmap;
                  checkBoxList[record] = work;
                  widgetList[record] = workWidget;

                  index--;
			  } else {
                  #ifdef TUP_DEBUG
                      qDebug() << "TupGalleryDialog::wideGalleryPage() - Preview pixmap is NULL in project: " << record;
                  #endif
              }
         }
         rowLayout->addStretch();
         contentLayout->addWidget(worksRow);
    }

    // Last (incomplete) row
    if (mod > 0) {
        QWidget *worksRow = new QWidget;
        QHBoxLayout *rowLayout = new QHBoxLayout(worksRow);
        rowLayout->setAlignment(Qt::AlignCenter);
        rowLayout->setSpacing(0);
        // rowLayout->setMargin(0);
        rowLayout->setContentsMargins(margins);

        for (int i=0; i<mod; i++) {
             QString record = items.at(index);
             QPixmap pixmap;
             pixmap.load(dir + "/" + record + "/preview.png");
             if (!pixmap.isNull()) {
                 TupCheckBox *work = new TupCheckBox(record, pixmap, themeColor, 0);
                 work->setStyleSheet("* { background-color: rgb(200,200,200); }");
                 connect(work, &TupCheckBox::selected, this, &TupGalleryDialog::updateSelection);
                 connect(work, &TupCheckBox::opened, this, &TupGalleryDialog::openSelectedProject);

                 QWidget *workWidget = new QWidget;
                 QHBoxLayout *itemLayout = new QHBoxLayout(workWidget);
                 itemLayout->setSpacing(0);
                 // itemLayout->setMargin(0);
                 itemLayout->setContentsMargins(margins);
                 itemLayout->setAlignment(Qt::AlignCenter);
                 itemLayout->addWidget(work, Qt::AlignHCenter);
                 rowLayout->addWidget(workWidget, Qt::AlignHCenter);

                 pixPreviewList[record] = pixmap;
                 checkBoxList[record] = work;
                 widgetList[record] = workWidget;

                 index--;
             } else {
                 #ifdef TUP_DEBUG
                     qDebug() << "TupGalleryDialog::wideGalleryPage() - Preview pixmap is NULL in project: " << record;
                 #endif
             }
        }

        int spaces = rowSize - mod;
        for (int i=0; i<spaces; i++) {
            QWidget *workWidget = new QWidget;
            QHBoxLayout *itemLayout = new QHBoxLayout(workWidget);
            itemLayout->setSpacing(0);
            // itemLayout->setMargin(0);
            itemLayout->setContentsMargins(margins);
            itemLayout->setAlignment(Qt::AlignCenter);
            itemLayout->addWidget(new QWidget, Qt::AlignHCenter);
            rowLayout->addWidget(workWidget, Qt::AlignHCenter);
        }

        rowLayout->addStretch();
        contentLayout->addWidget(worksRow);
    }

    contentLayout->addStretch();
    loadingLabel->setVisible(false);
    contentLayout->removeWidget(loadingLabel);
}

void TupGalleryDialog::openSelectedProject(const QString &code)
{
    emit openCall(code);
    accept();
}

void TupGalleryDialog::openProject()
{
    emit openCall(id);
    accept();
}

/*
void TupGalleryDialog::shareProject()
{
    emit shareCall(id);
    accept();
}
*/

void TupGalleryDialog::confirmRemoveAction()
{
    enableButtonsPanel(false);
}

void TupGalleryDialog::closeMessageDialog()
{
    enableButtonsPanel(true);
}

void TupGalleryDialog::enableButtonsPanel(bool flag)
{
    trashButton->setEnabled(flag);
    closeButton->setEnabled(flag);
    openButton->setEnabled(flag);

    buttonsWidget->setVisible(flag);
    msgWidget->setVisible(!flag);
}

void TupGalleryDialog::removeProject()
{
    if (!TupProject::remove(id)) {
        #ifdef TUP_DEBUG
            qDebug() << "TupGalleryDialog::removeProject() - Fatal error: Can't remove project dir -> " << id;
        #endif
        enableButtonsPanel(true);
        return;
    }

    if (widgetList[id]) {
        pixPreviewList.remove(id);
        checkBoxList.remove(id);
        widgetList[id]->setVisible(false);

        if (isCellPhone) {
            contentLayout->removeWidget(widgetList[id]);
        } else {
            QWidget *row = widgetList[id]->parentWidget();
            QLayout *layout = row->layout();
            if (layout->isEmpty()) {
                row->setVisible(false);
                contentLayout->removeWidget(row);
            }
        }

        widgetList.remove(id);
        worksTotal--;
        titleLabel->setText(tr("My Projects") + " (" + QString::number(worksTotal) + ")");
        emit remove(id);
    } else {
        #ifdef TUP_DEBUG
            qDebug() << "TupGaleryDialog::removeProject() - Fatal error: Project widget isn't available' -> " << id;
        #endif
    }

#ifdef TUP_DEBUG
    qDebug() << "TupGaleryDialog::removeProject() - Project " << id << " removed successfully! :)";
#endif

    msgWidget->setVisible(false);
    buttonsWidget->setVisible(true);
    trashButton->setEnabled(false);
    openButton->setEnabled(false);
    closeButton->setEnabled(true);

    if (checkBoxList.count() == 0)
        accept();
}

void TupGalleryDialog::updateSelection(const QString &project)
{
    if (id.compare(project) == 0)
        return;

    QHashIterator<QString, TupCheckBox *> iterator(checkBoxList);
    while (iterator.hasNext()) {
        iterator.next();
        TupCheckBox *control = iterator.value();

        if (iterator.key().compare(project) == 0) {
            control->updateSelection(true);
        } else {
            if (iterator.key().compare(id) == 0)
                control->updateSelection(false);
        }
    }

    if (!openButton->isEnabled())
        openButton->setEnabled(true);

    /*
    if (!shareButton->isEnabled())
        shareButton->setEnabled(true);
    */

    if (!trashButton->isEnabled())
        trashButton->setEnabled(true);

    id = project;
}

void TupGalleryDialog::mousePressEvent(QMouseEvent *event)
{
    if (!isCellPhone) {
        QPoint point = event->pos();
        int x0 = innerWidget->pos().x();
        int y0 = innerWidget->pos().y();
        int x1 = x0 + dialogW;
        int y1 = y0 + dialogH;
        if ((point.x() < x0 || point.x() > x1) || (point.y() < y0 || point.y() > y1))
            close();
    }
}
