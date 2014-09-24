#include <QDate>
#include <QSqlDatabase>
#include <QString>

#ifndef CONTENTPROVIDER_H
#define CONTENTPROVIDER_H

class ContentProvider
{
public:
    ContentProvider();

    bool setContentOrigin(QString origin);
    bool save();
    bool load(QDate date);

    QList<QDate> getHistoryByMonth(int month, int year);

private:
    const QString LOG_TAG = "ContentProvider";
    const QString SQL_CREATE_ENTRIES = "CREATE TABLE Entries (id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, date TEXT NOT NULL, content TEXT)";
    const QString SQL_CREATE_SEQUENCE = "CREATE TABLE sqlite_sequence(name,seq);";

    QSqlDatabase mDataBase;
    QString mDataBaseName;
};

#endif // CONTENTPROVIDER_H
