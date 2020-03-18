#ifndef PARSER_H
#define PARSER_H
#include <QtCore/QString>
#include <QtCore/QFile>
#include <QDebug>
#include <map>
#include "decoder.h"
#include "mimemessage.h"
#include <QImage>


class Parser
{
public:
    Parser();
    static void ParseHeaders(
            const QString &buf,
            QString &subject,
            QString &from,
            QString &date,
            QString &UID,
            QString &flags);
    /////////////////////////////////////////////
    static void ParseSelect(
            const std::string &buf,
            int &CountOfMail,
            int &CountOfRecent,
            int &CountOfUnseen,
            std::string &UIDVALIDITY,
            int &UIDNEXT);
    /////////////////////////////////////////////
    static MimeMessage* ParseMail(const QString &Q);
    /////////////////////////////////////////////
    static QString ParseIP(const QString &Q);

private:
    static QString ParseCID(const QString &Q);
    inline static QString ParseDate(const QString &Q);

    // text fetching
    inline static void ParseAttribute(const QString &data, const QString &search_keyword, QString &returnable);
    inline static void ParseMultilineAttribute(const QString &data, const QString &search_keyword, QString &returnable);
    inline static void CreateText(QString &part, const QString &filename, const QString &charset, const QString &content_transer_encoding);

    inline static void CreateFile(const QString &Q, const QString &filename, const QString &content_type, const QString &content_transfer_encoding);

    // ?
    inline static QString PartHandle(const QString &Q, QString &part, const QString &content_type, const QString &charset, const QString &content_transfer_encoding, const QString &filename);
    // multimedia fetching
    static void replaceCID(std::map <QString, QString> &M, const QString &filename);


};

#endif // PARSER_H
