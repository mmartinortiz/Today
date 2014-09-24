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

ContentProvider::ContentProvider()
{
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
    if (!mDataBase.open())
    {
        Utility::log(LOG_TAG, "Error opening database");
        Utility::log(LOG_TAG, mDataBase.lastError().databaseText());
        return false;
    }
    else
    {
        Utility::log(LOG_TAG, "Database opened");
    }

    if (mDataBase.tables().isEmpty())
    {
        // Create the tables
        Utility::log(LOG_TAG, "Creating tables...");
        QStringList queries;
        queries << SQL_CREATE_ENTRIES;
        queries << SQL_CREATE_SEQUENCE;
        QSqlQuery query;

        for (int i = 0; i < queries.size(); i++)
        {
            query.prepare(queries.at(i));
            query.exec();
        }
    }
    else
    {
        // Check database integrity
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
