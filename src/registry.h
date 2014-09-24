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

#ifndef REGISTRY_H
#define REGISTRY_H

#include <QObject>
#include <QDate>
#include <QSqlDatabase>
#include <QTextEdit>

class Registry : public QObject
{
    Q_OBJECT
public:
    explicit Registry(QTextEdit *editor, QObject *parent = 0);
    ~Registry();
    bool setDatabase(QString database);
    bool openRegistry();
    int loadEntry(QDate date);
    int loadEntry(int id);
    bool updateEntry(int id, QString content);
    int newEntry(QDate date, QString content);
    void close();
    QList<QDate> getHistoryByMonth(int month, int year);
    QString getLastExecutedQuery(const QSqlQuery& query);

signals:
    
public slots:
    
private:
    QSqlDatabase db;
    QTextEdit *mEditor;
    QString dbName;
};

#endif // REGISTRY_H
