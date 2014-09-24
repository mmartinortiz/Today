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

#include "registry.h"
#include <QDebug>
#include <QDir>
#include <QSqlError>
#include <QSqlQuery>
#include <QApplication>

Registry::Registry(QTextEdit *editor, QObject *parent) :
    QObject(parent)
{
    mEditor = editor;
    dbName = "";
}

Registry::~Registry()
{
    this->close();
}

bool Registry::setDatabase(QString database)
{
    dbName = database;
    if (db.isOpen())
        db.close();
    return this->openRegistry();
}

bool Registry::openRegistry()
{
    // Database
    db = QSqlDatabase::addDatabase("QSQLITE");
//    if (dbName.isEmpty())
//        return false;

    db.setDatabaseName(QApplication::applicationDirPath() + QDir::separator() + "today.db");

    if (!db.open())
    {
        qDebug() << "Error opening database";
        qDebug() << db.lastError().databaseText();
        return false;
    }
    else
    {
        qDebug() << "Database opened";
    }

    if (db.tables().isEmpty())
    {
        qDebug() << "Creating tables...";
        // Database empty, no tables.
        QStringList queries;
        queries << "CREATE TABLE Entries (id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, date TEXT NOT NULL, content TEXT)";
        queries << "CREATE TABLE sqlite_sequence(name,seq);";
        QSqlQuery query;

        for (int i = 0; i < queries.size(); i++)
        {
            query.prepare(queries.at(i));
            query.exec();
        }
    }
    else
    {
        //check integrity
        QSqlQuery query;
        query.prepare("pragma table_info(Entries)");
        if (query.exec())
        {
            bool correct = false;
            query.seek(0);
            if ((query.value(1).toString() == "id") &&
                (query.value(2).toString() == "INTEGER") &&
                (query.value(3).toInt() == 1) &&
                (query.value(5).toInt() == 1))
            {
                query.seek(1);
                if ((query.value(1).toString() == "date") &&
                    (query.value(2).toString() == "TEXT") &&
                    (query.value(3).toInt() == 1) &&
                    (query.value(5).toInt() == 0))
                {
                    query.seek(2);
                    if ((query.value(1).toString() == "content") &&
                        (query.value(2).toString() == "TEXT") &&
                        (query.value(3).toInt() == 0) &&
                        (query.value(5).toInt() == 0))
                    {
                        correct = true;
                    }
                }
            }

            if (!correct)
                return false;
        }
        else
        {
            return false;
        }
    }

    return true;
}

int Registry::loadEntry(QDate date)
{
    QSqlQuery query;
    query.prepare("SELECT id FROM Entries WHERE date = :date");
    query.bindValue(":date", date.toString("yyyy-MM-dd"));

    if (!query.exec())
    {
        qDebug() << "Could not execute this query:" << query.lastQuery();
        qDebug() << query.lastError().driverText();
        qDebug() << query.lastError().databaseText();
    }

    int i = 0;
    while (query.next()) i++;
    query.first();

    switch (i) {
    case 1:
        // 1 registry found, load the 'id' found
        return this->loadEntry(query.value(0).toInt());
        break;

    case 0:
        // No registry founds, clear editor for new registry
        mEditor->clear();
        return 0;
        break;

    default:
        break;
    }

    return 0;
}

int Registry::loadEntry(int id)
{
    QSqlQuery query;
    query.prepare("SELECT content FROM Entries WHERE id = :id");
    query.bindValue(":id", id);

    if (!query.exec())
    {
        qDebug() << "Could not execute this query:" << query.lastQuery();
        qDebug() << query.lastError().driverText();
        qDebug() << query.lastError().databaseText();
    }

    int i = 0;
    while (query.next()) i++;
    query.first();

    switch (i) {
    case 0:
        // No entry found, clear editor
        mEditor->clear();
        return 0;
        break;

    case 1:
        // Entry found, load content and return 'id'
        mEditor->setHtml(query.value(0).toString());
        return id;

    default:
        break;
    }

    return 0;
}

bool Registry::updateEntry(int id, QString content)
{
    QSqlQuery query;

    query.prepare("UPDATE Entries SET content = :content WHERE id = :id");
    query.bindValue(":content", content);
    query.bindValue(":id", id);

    if (!query.exec())
    {
        qDebug() << "Could not execute this query:" << query.lastQuery();
        qDebug() << query.lastError().driverText();
        qDebug() << query.lastError().databaseText();

        return false;
    }
    else
    {
        return true;
    }
}

int Registry::newEntry(QDate date, QString content)
{
    QSqlQuery query;

    query.prepare("INSERT INTO Entries (date, content) VALUES (:date, :content)");
    query.bindValue(":date", date.toString("yyyy-MM-dd"));
    query.bindValue(":content", content);

    if (!query.exec())
    {
        qDebug() << "Could not execute this query:" << query.lastQuery();
        qDebug() << query.lastError().driverText();
        qDebug() << query.lastError().databaseText();

        return 0;
    }
    else
    {
        return  query.lastInsertId().toInt();
    }
}

void Registry::close()
{
    db.commit();
    db.close();
}

QList<QDate> Registry::getHistoryByMonth(int month, int year)
{
    // months are numbered from 1 to 12
    QList<QDate> dates;
    QSqlQuery query;

    dates.clear();

    query.prepare("SELECT date FROM Entries WHERE strftime('%Y-%m',date) = :date");
    query.bindValue(":date", QString::number(year) + "-" + QString().sprintf("%02d", month));

    if (!query.exec())
    {
        qDebug() << "Could not execute this query:" << this->getLastExecutedQuery(query);
        qDebug() << query.lastError().driverText();
        qDebug() << query.lastError().databaseText();

        return dates;
    }

    while (query.next())
    {
        QStringList d = query.value(0).toString().split("-");
        dates.append(QDate(d.at(0).toInt(), d.at(1).toInt(), d.at(2).toInt()));
    }

    return dates;
}

QString Registry::getLastExecutedQuery(const QSqlQuery& query)
{
    QString str = query.lastQuery();
    QMapIterator<QString, QVariant> it(query.boundValues());
    while (it.hasNext())
    {
        it.next();
        str.replace(it.key(),it.value().toString());
    }
    return str;
}
