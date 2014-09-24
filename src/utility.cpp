#include "utility.h"
#include <QDebug>

Utility::Utility()
{
}

void Utility::log(QString LOG_TAG, QString content)
{
    qDebug() << LOG_TAG + ": " + content;
}
