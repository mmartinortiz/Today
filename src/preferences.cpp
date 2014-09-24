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

#include "preferences.h"
#include "ui_preferences.h"
#include <QFileDialog>
#include <QDir>
#include <QDebug>

Preferences::Preferences(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Preferences)
{
    ui->setupUi(this);
    this->setModal(true);
    this->setWindowTitle(tr("Preferences"));
}

Preferences::~Preferences()
{
    delete ui;
}

int Preferences::getFontSize()
{
    return ui->fontSize->value();
}

void Preferences::setFontSize(int value)
{
    ui->fontSize->setValue(value);
}

QString Preferences::getSpellLanguage()
{
    return ui->dic->currentText();
}

void Preferences::setDictionaries(QStringList dics)
{
    ui->dic->addItems(dics);
}

void Preferences::setCurrentDic(QString dic)
{
    int current = 0;
    for (int i = 0; i < ui->dic->count(); i++)
    {
        if (ui->dic->itemText(i) == dic)
            current = i;
    }

    ui->dic->setCurrentIndex(current);
}

