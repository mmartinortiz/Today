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
#include "googlespeech.h"
#include <QSettings>
#include <QDir>
#include <QProcess>
#include <QTemporaryFile>

Today::Today(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::TodayWindow)
{
    ui->setupUi(this);
    ui->dateSelector->setCheckable(true);
    calendar = new QCalendarWidget(this);
    calendar->setFirstDayOfWeek(Qt::Monday);
    calendar->hide();
    tempFile = new QFile("temporary_file.html");

    connect(calendar, SIGNAL(clicked(QDate)), this, SLOT(onDateChanged(QDate)));

    reg = new Registry(ui->content);
    reg->openRegistry();

    loadSettings();

//    if (!reg->openRegistry())
//        QMessageBox::warning(this,
//                             tr("Today"),
//                             tr("Error opening database. Changes will be not saved"),
//                             QMessageBox::Ok);

    ui->content->setFocus();

    maybeSave = false;
    onDateChanged(QDate::currentDate());

    this->setWindowTitle(tr("Today - Write your experiencies"));
    this->setWrittenDays();
}

Today::~Today()
{
    delete ui;
}

void Today::on_saveButton_clicked()
{
    QString content = ui->content->toHtml();

    switch (currentId) {
    case 0:
        // New registry
        currentId = reg->newEntry(currentDate, content);
        maybeSave = false;
        break;
    default:
        if (reg->updateEntry(currentId, content)) maybeSave = false;
        break;
    }

    if (!maybeSave)
        ui->saveButton->setEnabled(false);

    return;
}

void Today::closeEvent(QCloseEvent *)
{
    on_exitButton_clicked();
}

void Today::maybeSaveDialog()
{
    QMessageBox *dialog = new QMessageBox();

    dialog->setText(tr("The content has been modified."));
    dialog->setInformativeText(tr("Do you want to save the changes?"));
    dialog->setStandardButtons(QMessageBox::Save | QMessageBox::Discard);
    dialog->setDefaultButton(QMessageBox::Save);
    int ret = dialog->exec();

    switch (ret) {
    case QMessageBox::Save:
        on_saveButton_clicked();
        break;
    default:
        break;
    }
}

void Today::setWrittenDays()
{
    QTextCharFormat format;
    format.setBackground(QBrush(Qt::green));
    QList<QDate> dates = reg->getHistoryByMonth(calendar->monthShown(), calendar->yearShown());
    for (int i = 0; i < dates.size(); i++)
    {
        calendar->setDateTextFormat(dates.at(i), format);
    }

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

//    QString dbName = QDir::currentPath() + "today.db";
//    if (settings.contains("DataBase"))
//        dbName = settings.value("DataBase").toString();
//    if (!reg->setDatabase(dbName))
//        QMessageBox::warning(this,
//                             tr("Today"),
//                             tr("Error opening database. Changes will be not saved"),
//                             QMessageBox::Ok);;

}

void Today::saveSettings()
{
    QSettings settings("A Bit World", "Today");

    settings.setValue("FontSize", ui->content->fontPointSize());
    settings.setValue("SpellLanguage", currentDic);

}

void Today::onDateChanged(const QDate &date)
{
    if (maybeSave)
        maybeSaveDialog();

    currentId = reg->loadEntry(date);
    currentDate = calendar->selectedDate();
    ui->content->setFocus();
    maybeSave = false;
    ui->dateSelector->setChecked(false);
}

void Today::on_exitButton_clicked()
{
    // Save Dialog
    if (maybeSave)
        maybeSaveDialog();

    reg->close();
    this->saveSettings();
    tempFile->remove();
    exit(0);
}

void Today::on_content_textChanged()
{
    if (!ui->saveButton->isEnabled())
        ui->saveButton->setEnabled(true);
    maybeSave = true;
}

void Today::on_spellcheckButton_clicked()
{
    SpellChecker::checkDocument(ui->content, mDictionary);
}

void Today::on_dateSelector_toggled(bool checked)
{
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

    // Update values
    int newSize = dialog->getFontSize();
    if (newSize != ui->content->fontPointSize())
    {
        ui->content->selectAll();
        ui->content->setFontPointSize(newSize);
        QTextCursor cursor = ui->content->textCursor();
        cursor.setPosition(QTextCursor::End);
        ui->content->setTextCursor(cursor);
    }

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

void Today::on_textToSpeech_clicked()
{
    GoogleSpeech *speech = new GoogleSpeech("english");
    speech->speech(ui->content->toPlainText());

//    if (tempFile->open(QIODevice::ReadWrite)) {
//        QTextStream out(tempFile);
//        out << ui->content->toHtml();
//        tempFile->close();
//    }
//    QString chrome = "/usr/bin/chromium-browser";
//    QStringList arguments;
//    arguments << "";
//    arguments << tempFile->fileName();

//    QProcess *chromeProcess = new QProcess(this);
//    chromeProcess->start(chrome, arguments);
}
