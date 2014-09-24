/***********************************************************************
 *
 * Copyright (C) 2009, 2010, 2011, 2012, 2013 Manuel Martín <mmartinortiz@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#ifndef TODAYWINDOW_H
#define TODAYWINDOW_H

#include <QMainWindow>
#include <QSqlDatabase>
#include <QDate>
#include <QCalendarWidget>
#include <QFile>
#include "registry.h"
#include "dictionary.h"

namespace Ui {
class TodayWindow;
}

class Today : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit Today(QWidget *parent = 0);
    ~Today();
    
private slots:
    void on_saveButton_clicked();

    void onDateChanged(const QDate &date);

    void on_exitButton_clicked();

    void on_content_textChanged();

    void on_spellcheckButton_clicked();

    void on_dateSelector_toggled(bool checked);

    void on_preferencesButton_clicked();

    void on_aboutButton_clicked();

protected:
    void closeEvent(QCloseEvent *);

private:
    Ui::TodayWindow *ui;
    Registry *reg;
    int currentId;
    QDate currentDate;
    QString currentDic;
    bool maybeSave;

    Dictionary mDictionary;
    QCalendarWidget *calendar;
    QFile *tempFile;

    void maybeSaveDialog();
    void setWrittenDays();

    void loadSettings();
    void saveSettings();

    void dictionaryChanged();
};

#endif // TODAYWINDOW_H
