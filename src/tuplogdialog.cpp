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

#include "tuplogdialog.h"
#include "tupbutton.h"

#include <QTabWidget>
#include <QFile>

TupLogDialog::TupLogDialog(QSettings *config, QWidget *parent) : QDialog(parent)
{
    setModal(true);
    setWindowFlags(Qt::Popup);

    isCellPhone = config->value("isCellPhone").toBool();

    QString system = config->value("system").toString();
    imgPath = config->value("imgPath").toString();

    QString bgColor = config->value("themeColor").toString();
    titleSize = config->value("titleSize").toInt();
    titleHeight = config->value("titleHeight").toInt();
    borderRadius = config->value("borderRadius").toString();
    iconWidth = config->value("iconWidth").toInt();
    dialogButtonsHeight = config->value("dialogButtonsHeight").toInt();

    if (!isCellPhone)
        setStyleSheet("* { background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
                      "stop:0 rgb(100, 100, 100), stop:1 rgb(30, 30, 30)); }");

    innerLayout = new QVBoxLayout;
    setLabelPanel();

    QTabWidget *tabWidget = new QTabWidget;
    tabWidget->setStyleSheet("QTabBar::tab:selected { "
                             "background-color: rgba(0, 137, 0, 100); "
                             "}"
                             "QTabBar::tab:!selected {"
                             "background-color: rgba(50, 50, 50, 50); "
                             "}");


    QString textAreaStyle = "QTextEdit { background-color: rgb(255, 255, 255); }"
                            "QScrollArea {"
                                           "background-color: rgb(255,255,255);"
                                           "}"
                                           "QScrollBar:vertical {"
                                           "background-color: rgb(200,200,200);"
                                           "}"
                                           "QScrollBar::handle:vertical {"
                                           "background-color: rgb(130,130,130);"
                                           "}";
    qtTextArea = new QTextEdit("");
    qtTextArea->setStyleSheet(textAreaStyle);

    javaTextArea = new QTextEdit("");
    javaTextArea->setStyleSheet(textAreaStyle);

    tabWidget->addTab(qtTextArea, "Qt Log");
    tabWidget->addTab(javaTextArea, "Java Log");

    innerLayout->addWidget(tabWidget);
    setButtonsPanel();

    widget = new QWidget;
    QString style = "QWidget { background-color: rgb(" + bgColor + "); ";
    style += "border-radius: " + borderRadius + "px; }";
    widget->setStyleSheet(style);

    widget->setLayout(innerLayout);

    QHBoxLayout *global = new QHBoxLayout;
    global->addWidget(widget);
    setLayout(global);
    setStyleSheet(style);
}

TupLogDialog::~TupLogDialog()
{
}

void TupLogDialog::setLabelPanel()
{
	QFont font = this->font();
    font.setBold(true);
    font.setPointSize(titleSize);
	
    sizeLabel = new QLabel(tr("App Log"));
    sizeLabel->setStyleSheet("QLabel { background-color: rgb(200,200,200); "
                                "border-radius: " + borderRadius + "px; }");
    sizeLabel->setFixedHeight(titleHeight);
    sizeLabel->setFont(font);
    sizeLabel->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);

    innerLayout->addWidget(sizeLabel);
}

void TupLogDialog::setButtonsPanel()
{
    TupButton *okButton = new TupButton(imgPath + "ok.png", iconWidth);
    okButton->setFixedHeight(dialogButtonsHeight);
    connect(okButton, &TupButton::clicked, this, &TupLogDialog::closeDialog);

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(okButton);

    innerLayout->addLayout(buttonLayout);
    innerLayout->setAlignment(buttonLayout, Qt::AlignRight);
}

void TupLogDialog::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event)
}

void TupLogDialog::closeDialog()
{
    accept();
}

void TupLogDialog::mousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
}

void TupLogDialog::appendRecord(const QString &record)
{
    qtTextArea->append(record);
}

void TupLogDialog::loadJavaLog(const QString &log)
{
    QFile file(log);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "";
        qDebug() << "TupLogDialog::loadJavaLog() - Can't read JAVA log file!";
        qDebug() << "TupLogDialog::loadJavaLog() - Path: " << log;
        return;
    }

    qDebug() << "TupLogDialog::loadJavaLog() - JAVA DEBUG: ";
    javaTextArea->clear();

    QTextStream in(&file);
    QString data = "";
    while (!in.atEnd()) {
        data = in.readLine();
        qDebug() << data;
        javaTextArea->append(data + "<br/>");
    }

    qDebug() << "";
    file.close();
}
