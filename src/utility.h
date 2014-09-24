#include <QString>

#ifndef UTILITY_H
#define UTILITY_H

class Utility
{
public:
    Utility();
    static void log(QString LOG_TAG, QString content);
    // void log(std::string content);
};

#endif // UTILITY_H
