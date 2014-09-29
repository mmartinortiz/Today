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
        provider->clean(mCurrentDate);
    } else {
        QString content = ui->content->toHtml();
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
    ui->dateSelector->setChecked(false);

    // Setting date label
    setLabelDate(date);
}

void Today::closeEvent(QCloseEvent *)
{
    // Saving changes
    save();
}

void Today::setWrittenDays()
{
    // The calendar shows the days with entries with a green background
    QTextCharFormat format;
    format.setBackground(QBrush(Qt::green));

    // Getting the entries for the current month from the provider
    QList<QDate> dates = provider->getHistoryByMonth(calendar->monthShown(), calendar->yearShown());
    for (int i = 0; i < dates.size(); i++) {
        calendar->setDateTextFormat(dates.at(i), format);
    }

}

void Today::setLabelDate(QDate date)
{
    ui->labelDate->setText(date.toString("dddd '-' dd/MM/yyyy"));
}

void Today::loadSettings()
{
    QSettings settings("A Bit World", "Today");

    int fs = 12;
    if (settings.contains("FontSize"))
        fs = settings.value("FontSize").toInt();
    ui->content->setFontPointSize(fs);

    currentDic = "en";
    if (settings.contains("SpellLanguage"))
        currentDic = settings.value("SpellLanguage").toString();
    this->dictionaryChanged();
}

void Today::saveSettings()
{
    QSettings settings("A Bit World", "Today");

    settings.setValue("FontSize", ui->content->fontPointSize());
    settings.setValue("SpellLanguage", currentDic);

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
    setWrittenDays();
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
    Preferences *dialog = new Preferences(this);

    // Load current values
    dialog->setFontSize(ui->content->fontPointSize());
    dialog->setDictionaries(DictionaryManager::instance().availableDictionaries());
    dialog->setCurrentDic(currentDic);

    // Show dialog
    dialog->exec();

    // Update font values
    int currentPosition = ui->content->textCursor().position();

    QFont newFont = dialog->getFontFamily();
    newFont.setPointSize(dialog->getFontSize());

    ui->content->selectAll();
    ui->content->setFont(newFont);
    ui->content->textCursor().setPosition(currentPosition);

    // Font size
    /*int newSize = dialog->getFontSize();
    if (newSize != ui->content->fontPointSize())
    {
        int currentPosition = ui->content->textCursor().position();

        ui->content->selectAll();
        ui->content->setFontPointSize(newSize);
        ui->content->textCursor().setPosition(currentPosition);
//        cursor.setPosition(QTextCursor::End);
//        ui->content->setTextCursor(cursor);
    }

    // Font Family
    QFont newFont = dialog->getFontFamily();
    if (newFont != ui->content->currentFont()) {
        int currentPosition = ui->content->textCursor().position();

        ui->content->selectAll();
        ui->content->setCurrentFont(newFont);
        ui->content->textCursor().setPosition(currentPosition);
    }*/

    if (currentDic != dialog->getSpellLanguage())
    {
        currentDic = dialog->getSpellLanguage();
        this->dictionaryChanged();
    }

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
