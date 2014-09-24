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

#include "about.h"
#include "ui_about.h"
#include <QDesktopServices>
#include <QUrl>

About::About(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::About)
{
    ui->setupUi(this);
}

About::~About()
{
    delete ui;
}

void About::on_license_clicked()
{
    QDesktopServices::openUrl(QUrl("http://www.gnu.org/licenses/gpl.html"));
}

void About::on_homepage_clicked()
{
    QDesktopServices::openUrl(QUrl("http://abitworld.com/projects"));
}
