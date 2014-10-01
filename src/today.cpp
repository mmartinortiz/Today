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

#include "today.h"
#include "ui_todaywindow.h"
#include <QMessageBox>
#include "about.h"
#include <QDebug>
#include <QCalendarWidget>
#include <QTextCharFormat>
#include "spell_checker.h"
#include "dictionary_manager.h"
#include "preferences.h"
#include "utility.h"
#include <QSettings>
#include <QDir>
#include <QProcess>
#include <QTemporaryFile>
#include <QFontDialog>

Today::Today(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::TodayWindow)
{
    ui->setupUi(this);

    // Set up calendar widget
    ui->dateSelector->setCheckable(true);
    calendar = new QCalendarWidget(this);
    calendar->setFirstDayOfWeek(Qt::Monday);
    calendar->hide();

    // Connect the calendar signals
    connect(calendar, SIGNAL(clicked(QDate)), this, SLOT(onDateChanged(QDate)));
    connect(calendar, SIGNAL(currentPageChanged(int,int)), this, SLOT(setCalendarBackground(int,int)));

    // Prepare content provider
    provider = new ContentProvider();
    provider->setContentOrigin("today");

    // Load user preferences
    loadSettings();

    // Set the focus on the main widget
    ui->content->setFocus();

    // Set the current date
    load(QDate::currentDate());

    // Some other stuff
    setLabelDate(QDate::currentDate());
    setWindowTitle(tr("Today - Write your experiencies"));
}

Today::~Today()
{
    delete ui;
}

void Today::save()
{
    // If the content is empty, the database entry is cleaned
    if (ui->content->toPlainText().length() == 0) {
        // Clean the database entry
        provider->clean(mCurrentDate);

        // Clean date background on calendar
        cleanCalendarBackground(mCurrentDate);
    } else {
        QString content = ui->content->toPlainText();
        provider->save(mCurrentDate, content);
    }
}

void Today::load(QDate date)
{
    // Set the current date
    mCurrentDate = date;

    // Get current date content
    QString content = provider->load(mCurrentDate);

    // Setting current content
    ui->content->setText(content);
    ui->content->setFocus();
    ui->content->setFont(mFont);
    ui->dateSelector->setChecked(false);

    // Setting date label
    setLabelDate(date);
}

void Today::cleanCalendarBackground(QDate date)
{
    // Restore background color
    QTextCharFormat format;
    format.setBackground(QBrush(Qt::white));

    calendar->setDateTextFormat(date, format);
}

void Today::closeEvent(QCloseEvent *)
{
    // Saving changes
    save();
}

void Today::setCalendarBackground(int year, int month)
{
    // The calendar shows the days with entries with a green background
    QTextCharFormat format;
    format.setBackground(QBrush(Qt::green));

    // Getting the entries for the current month from the provider
    for (int j = -1;j <2; j++) {
        // Take care of the previous and later month, as probably the
        // current view contains days from these months
        QList<QDate> dates = provider->getHistoryByMonth(month + j, year);
        for (int i = 0; i < dates.size(); i++) {
            calendar->setDateTextFormat(dates.at(i), format);
        }
    }

}

void Today::setLabelDate(QDate date)
{
    // Set the main window label with the selected date
    ui->labelDate->setText(date.toString("dddd '-' dd/MM/yyyy"));
}

void Today::loadSettings()
{
    // Load few settings
    QSettings s("A Bit World", "Today");

    // Load font parameters
    mFont.setFamily(s.value("font.family", "Arial").toString());
    mFont.setPointSize(s.value("font.size", 10).toInt());
    mFont.setBold(s.value("font.bold", false).toBool());
    mFont.setItalic(s.value("font.italic", false).toBool());
    ui->content->setFont(mFont);

    // Restore Geometry
    restoreGeometry(s.value("window.geometry").toByteArray());
    restoreState(s.value("window.state").toByteArray());

    // @TODO: This will be modified in future
    currentDic = "en";
    if (s.contains("SpellLanguage"))
        currentDic = s.value("SpellLanguage").toString();
    this->dictionaryChanged();
}

void Today::saveSettings()
{
    QSettings s("A Bit World", "Today");

    // Save font parameters
    s.setValue("font.family", mFont.family());
    s.setValue("font.size", mFont.pointSize());
    s.setValue("font.italic", mFont.italic());
    s.setValue("font.bold", mFont.bold());

    // Save Geometry
    s.setValue("window.geometry", saveGeometry());
    s.setValue("window.state", saveState());

    // @TODO: This will be modified in future
    s.setValue("SpellLanguage", currentDic);

}

void Today::onDateChanged(const QDate &date)
{
    // Save previous content
    save();

    // Load new content
    load(date);
}

void Today::on_exitButton_clicked()
{
    save();

    this->saveSettings();
    exit(0);
}

void Today::on_spellcheckButton_clicked()
{
    SpellChecker::checkDocument(ui->content, mDictionary);
}

void Today::on_dateSelector_toggled(bool checked)
{
    setCalendarBackground(calendar->yearShown(), calendar->monthShown());
    if (checked)
    {
        QRect calendar_geometry = calendar->geometry();

        calendar_geometry.moveTopLeft(ui->dateSelector->geometry().topRight());
        calendar_geometry.setSize(QSize(360,150));
        calendar->setGeometry(calendar_geometry);
        calendar->show();
    }
    else
    {
        calendar->hide();
    }
}

void Today::on_preferencesButton_clicked()
{
    mFont = QFontDialog::getFont(0, this);
    ui->content->setFont(mFont);

    /*
    if (currentDic != dialog->getSpellLanguage())
    {
        currentDic = dialog->getSpellLanguage();
        this->dictionaryChanged();
    }
*/
}


void Today::dictionaryChanged()
{
    DictionaryManager::instance().setDefaultLanguage(currentDic);
    mDictionary = DictionaryManager::instance().requestDictionary();
}

void Today::on_aboutButton_clicked()
{
    About *dialog = new About(this);

    dialog->setModal(true);
    dialog->exec();
}
