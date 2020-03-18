#ifndef DIRECTORY_H
#define DIRECTORY_H

#include <QDebug>
#include <QDir>

class Directory
{
public:
    Directory();
    static QString GetCurrentDirectory_();
    static void CleanCache();
    static void DeleteMail();
};

#endif // DIRECTORY_H
