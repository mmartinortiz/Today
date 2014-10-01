#include "contentprovider.h"
#include "utility.h"

#include <QApplication>
#include <QDir>
#include <QSqlError>
#include <QSqlQuery>

/*
 * This class (mainly the name) is inspired by the Android's
 * patter ContentProvider.
 *
 * This class will provide access to the database, trying to do
 * more simple some common tasks
 **/

const QString ContentProvider::LOG_TAG = "ContentProvider";
const QString ContentProvider::SQL_CREATE_ENTRIES = "CREATE TABLE Entries \
        (id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, \
         date TEXT UNIQUE NOT NULL, \
         content TEXT) \
        ON CONFLICT REPLACE";
const QString ContentProvider::SQL_CREATE_SEQUENCE = "CREATE TABLE sqlite_sequence(name,seq);";

ContentProvider::ContentProvider()
{

}

ContentProvider::~ContentProvider()
{
    if (mDataBase.isValid()) {
        mDataBase.commit();
        mDataBase.close();
    }
}

bool ContentProvider::setContentOrigin(QString origin)
{
    // Set database name and close the current database if open
    mDataBaseName = origin;
    if (mDataBase.isOpen())
        mDataBase.close();

    // Set database
    // @TODO: Now the database is located in the same application directory. It should be asked
    // to the user where to locate the database or the database to be opened
    mDataBase = QSqlDatabase::addDatabase("QSQLITE");
    mDataBase.setDatabaseName(QApplication::applicationDirPath() + QDir::separator() + origin + ".db");

    // Open the database
    if (!mDataBase.open()) {
        Utility::log(LOG_TAG, "Error opening database");
        Utility::log(LOG_TAG, mDataBase.lastError().databaseText());
        return false;
    } else {
//        Utility::log(LOG_TAG, "Database opened");
    }

    if (mDataBase.tables().isEmpty()) {
        // Create tables
        createTables(mDataBase);
    } else {
        // Check database integrity
        checkTables(mDataBase);
    }

    return true;
}

bool ContentProvider::save(QDate date, QString content)
{
    bool success;

    // Does exist the entry?
    int id = getIdByDate(date);

    if ( id < 0 && content.length() > 0) {
        // A new entry has to be created
        id = createNewEntry(date);
    }

    success = update(id, content);
    mDataBase.commit();

    return success;
}

QString ContentProvider::load(QDate date)
{
    QString content;

    // Does exist the entry?
    int id = getIdByDate(date);

    if ( id < 0) {
        // A new entry has to be created
        createNewEntry(date);
        content = "";
    } else {
        content = load(id);
    }

    return content;
}

bool ContentProvider::clean(QDate date)
{
    bool success;

    // Does exist the entry?
    int id = getIdByDate(date);

    if ( id < 0) {
        // The entry does not exist, so it can not be removed
        success = false;
    }

    success = clean(id);
    mDataBase.commit();

    return success;
}

QList<QDate> ContentProvider::getHistoryByMonth(int month, int year)
{
    // months are numbered from 1 to 12
    QList<QDate> dates;
    QSqlQuery query;

    dates.clear();

    query.prepare("SELECT date FROM Entries WHERE strftime('%Y-%m',date) = :date");
    query.bindValue(":date", QString::number(year) + "-" + QString().sprintf("%02d", month));

    if (!query.exec())
    {
        QString str = query.lastQuery();
        QMapIterator<QString, QVariant> it(query.boundValues());
        while (it.hasNext())
        {
            it.next();
            str.replace(it.key(),it.value().toString());
        }

        Utility::log(LOG_TAG, "Could not execute this query:" + str);
        Utility::log(LOG_TAG, query.lastError().driverText());
        Utility::log(LOG_TAG, query.lastError().databaseText());

        return dates;
    }

    while (query.next())
    {
        QStringList d = query.value(0).toString().split("-");
        dates.append(QDate(d.at(0).toInt(), d.at(1).toInt(), d.at(2).toInt()));
    }

    return dates;
}

bool ContentProvider::createTables(QSqlDatabase database)
{
    if (!database.isOpen())
        return false;

    // Create the tables
//    Utility::log(LOG_TAG, "Creating tables...");
    QStringList queries;
    queries << SQL_CREATE_ENTRIES;
    queries << SQL_CREATE_SEQUENCE;
    QSqlQuery query;

    for (int i = 0; i < queries.size(); i++)
    {
        query.prepare(queries.at(i));
        query.exec();
    }

    return true;
}

bool ContentProvider::checkTables(QSqlDatabase database)
{
//    Utility::log(LOG_TAG, "Checking database...");
    bool success = true;

    if(!database.isOpen())
        return false;

    QSqlQuery query;
    query.prepare("pragma table_info(Entries)");
    if (query.exec()) {
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
            success = false;
    } else {
        success = false;
    }

//    Utility::log(LOG_TAG, "Database check result: " + QString::number(success));
    return success;
}

int ContentProvider::createNewEntry(QDate date)
{
    int id = -1;
    if (!mDataBase.isOpen())
        return id;

    QSqlQuery query;

    query.prepare("INSERT INTO Entries (date) VALUES (:date)");
    query.bindValue(":date", date.toString("yyyy-MM-dd"));

    if (!query.exec()) {
        Utility::log(LOG_TAG, "Could not execute this query:" + query.lastQuery());
        Utility::log(LOG_TAG, query.lastError().driverText());
        Utility::log(LOG_TAG, query.lastError().databaseText());

        id = -1;
    } else {
        id = query.lastInsertId().toInt();
    }

    return id;
}

bool ContentProvider::clean(int id)
{
//    Utility::log(LOG_TAG, "Cleaning entry");
    bool success = false;
    if (!mDataBase.isOpen())
        return success;

    QSqlQuery query;

    query.prepare("DELETE FROM Entries WHERE id = :id");
    query.bindValue(":id", id);

    if (!query.exec()) {
        Utility::log(LOG_TAG, "Could not execute this query:" + query.lastQuery());
        Utility::log(LOG_TAG, query.lastError().driverText());
        Utility::log(LOG_TAG, query.lastError().databaseText());

        success = false;
    } else {
        success = true;
    }
    mDataBase.commit();

    return success;
}

bool ContentProvider::update(int id, QString content)
{
    bool success = false;
    if (!mDataBase.isOpen())
        return success;

    QSqlQuery query;

    query.prepare("UPDATE Entries SET content = :content WHERE id = :id");
    query.bindValue(":content", content);
    query.bindValue(":id", id);

    if (!query.exec()) {
        Utility::log(LOG_TAG, "Could not execute this query:" + query.lastQuery());
        Utility::log(LOG_TAG, query.lastError().driverText());
        Utility::log(LOG_TAG, query.lastError().databaseText());

        success = false;
    } else {
        success = true;
    }
    mDataBase.commit();

    return success;
}

QString ContentProvider::load(int id)
{
    QString content;

    QSqlQuery query;
    query.prepare("SELECT content FROM Entries WHERE id = :id");
    query.bindValue(":id", id);

    if (!query.exec())
    {
        Utility::log(LOG_TAG, "Could not execute this query:" + query.lastQuery());
        Utility::log(LOG_TAG, query.lastError().driverText());
        Utility::log(LOG_TAG, query.lastError().databaseText());
    }

    int i = 0;
    while (query.next()) i++;
    query.first();

    switch (i) {
    case 0:
        // No entry found
        content = "";
        break;

    case 1:
        // Entry found, load content and return 'id'
        content = query.value(0).toString();
        break;
    }

    return content;
}

int ContentProvider::getIdByDate(QDate date)
{
    QSqlQuery query;
    query.prepare("SELECT id FROM Entries WHERE date = :date");
    query.bindValue(":date", date.toString("yyyy-MM-dd"));

    if (!query.exec())
    {
        Utility::log(LOG_TAG, "Could not execute this query:" + query.lastQuery());
        Utility::log(LOG_TAG, query.lastError().driverText());
        Utility::log(LOG_TAG, query.lastError().databaseText());
    }

    int i = 0;
    int id = 0;
    while (query.next()) i++;
    query.first();

    switch (i) {
    case 1:
        // 1 registry found, return its id
        id = query.value(0).toInt();
        break;

    case 0:
    default:
        // No registry founds, return a negative value
        id = -1;
        break;
    }

    return id;
}
