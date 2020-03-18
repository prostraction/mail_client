#ifndef DECODER_H
#define DECODER_H

#include <QtCore>

class Decoder
{
public:
    Decoder();
    // Text:
    static QString QuotedPrintableToUTF8(const QString &Q, bool override_undercase = false);
    static QString Base64ToUTF8         (const QString &B);
    static QString UTF8toBase64         (const QString &Q);

    // Multimedia content:
    static void Base64ToPNG(QByteArray B, QString filename);
};

#endif // DECODER_H
