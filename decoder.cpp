#include "decoder.h"

Decoder::Decoder()
{

}


QString Decoder::QuotedPrintableToUTF8(const QString &Q, bool override_undercase)
{
    QByteArray byte_data_QP;
    QByteArray byte_data_UTF8;
    byte_data_QP.append(Q);
    QString data_string;
    char temp_char;
    bool converted;
    for (int i = 0; i < byte_data_QP.length(); ++i)
    {
        if ( (byte_data_QP.at(i) == '=') && ( (i + 2) < byte_data_QP.length() ) )
        {
            data_string = byte_data_QP.mid( (++i)++, 2);
            temp_char = data_string.toUInt(&converted, 16);
            if (converted)
               { byte_data_UTF8.append(temp_char); }
            else
            { byte_data_UTF8.append("=" + data_string); }
        }
        else if (byte_data_QP.at(i) == '_' && override_undercase)
        { byte_data_UTF8.append(' '); }
        else
        { byte_data_UTF8.append(byte_data_QP.at(i)); }
    }
    return QString::fromUtf8(byte_data_UTF8);
}

QString Decoder::Base64ToUTF8(const QString &B)
{
    QByteArray byte_data_UTF8;
    byte_data_UTF8.append(B);
    return (QByteArray::fromBase64(byte_data_UTF8));
}

QString Decoder::UTF8toBase64(const QString &Q)
{
    QByteArray byte_data_UTF8;
    byte_data_UTF8.append(Q);
    byte_data_UTF8 = byte_data_UTF8.toBase64();
    return (byte_data_UTF8);
}
