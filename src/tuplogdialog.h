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

#ifndef TUPLOGDIALOG_H
#define TUPLOGDIALOG_H

#include <QDialog>
#include <QDebug>
#include <QSettings>
#include <QKeyEvent>
#include <QResizeEvent>
#include <QBoxLayout>
#include <QLabel>
#include <QTextEdit>

class TupLogDialog : public QDialog
{
    Q_OBJECT

    public:
        TupLogDialog(QSettings *settings, QWidget *parent = nullptr);
        ~TupLogDialog();

        void appendRecord(const QString &record);
        void loadJavaLog(const QString &log);

    protected:
        void resizeEvent(QResizeEvent *);
        void mousePressEvent(QMouseEvent *);

    private slots:
        void closeDialog();

    private:
        void setLabelPanel();
        void setButtonsPanel();

        QVBoxLayout *innerLayout;
        QLabel *sizeLabel;

        bool isCellPhone;
        QString imgPath;
        QWidget *widget;

        QTextEdit *qtTextArea;
        QTextEdit *javaTextArea;

        int titleSize;
        int titleHeight;
        QString borderRadius;
        int iconWidth;
        int dialogButtonsHeight;
};

#endif
