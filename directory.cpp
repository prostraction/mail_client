#include "directory.h"

Directory::Directory()
{

}

QString Directory::GetCurrentDirectory_()
{
    //qDebug() << QDir::currentPath();
    //qDebug() << QDir::temp();
    return QDir::currentPath();
}
void Directory::CleanCache()
{

}
void Directory::DeleteMail()
{

}
