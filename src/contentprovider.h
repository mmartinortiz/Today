#include <QDate>
#include <QSqlDatabase>
#include <QString>

#ifndef CONTENTPROVIDER_H
#define CONTENTPROVIDER_H

class ContentProvider
{
public:
    ContentProvider();
    ~ContentProvider();

    bool setContentOrigin(QString origin);
    bool save(QDate date, QString content);
    QString load(QDate date);
    bool clean(QDate date);

    QList<QDate> getHistoryByMonth(int month, int year);

private:
    static const QString LOG_TAG;
    static const QString SQL_CREATE_ENTRIES;
    static const QString SQL_CREATE_SEQUENCE;

    QSqlDatabase mDataBase;
    QString mDataBaseName;

    bool createTables(QSqlDatabase database);
    bool checkTables(QSqlDatabase database);
    int createNewEntry(QDate date);
    bool clean(int id);
    bool update(int id, QString content);
    QString load(int id);
    int getIdByDate(QDate date);
};

#endif // CONTENTPROVIDER_H
