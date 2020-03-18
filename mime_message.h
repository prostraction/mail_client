#ifndef MIME_MESSAGE_H
#define MIME_MESSAGE_H

#include <QtCore>
#include "decoder.h"

class MIME_Message
{
public:
    MIME_Message() {_mail_boundary = "--=GdgGGGgggGggGGggGGggGG";}
    QString text_plain() {return _text_plain;}
    QString text_html()  {return _text_html;}
    void set_text_plain(const QString &txt)
    {
        _text_plain = "Content-Transfer-Encoding: base64\r\nContent-Type: text/plain; charset=UTF-8\r\n\r\n";

        QString bufer = txt;
        int t = 0;
        int t_last;
        while (t != -1)
        {
            t = bufer.indexOf('<');
            t_last = bufer.indexOf('>', t + 1);
            qDebug() << t << " " << t_last;
            bufer = bufer.remove(t, t_last - t + 1);
        }

        QFile message_file("file.txt");
        message_file.open(QFile::WriteOnly | QFile::Append);
        QTextStream message_ofstream(&message_file);

        set_text_data(bufer, _text_plain);

        message_ofstream << "--" << _mail_boundary + "\r\n" + _text_plain;
    }
    void set_text_html(const QString &html)
    {
        _text_html = "Content-Transfer-Encoding: base64\r\nContent-Type: text/html; charset=\"UTF-8\"\r\n\r\n";

        QFile message_file("file.txt");
        message_file.open(QFile::WriteOnly | QFile::Append);
        QTextStream message_ofstream(&message_file);

        set_text_data(html, _text_html);

        message_ofstream << "--" << _mail_boundary + "\r\n" + _text_html + _mail_boundary + "--\r\n";
    }
    void set_subject(const QString &sub)
    {
        _subject = sub;
        QFile message_file("file.txt");
        message_file.open(QFile::WriteOnly | QFile::Append);
        QTextStream message_ofstream(&message_file);
        message_ofstream << "Subject: " + _subject + "\r\n";
        message_ofstream << "Content-Type: " +_mode + "; boundary=\"" + _mail_boundary + "\"\r\n" + "\r\n";
    }
    QString message()
    {
        return "";
    }

private:
    QString _subject;
    QString _mail_boundary;
    QString _text_plain;
    QString _text_html;
    QString _bufer;
    QString _mode = "multipart/alternative";
    QString message_name;

    void set_text_data(const QString &text, QString &variable_data)
    {
        _bufer = Decoder::UTF8toBase64(text);
        for (int i = 0; i < _bufer.size(); i += 76)
        {
            QString bufer;
            if (i + 76 <= _bufer.size())
            {
               variable_data += _bufer.mid(i, 76);
            }
            else
            {
               variable_data += _bufer.mid(i, _bufer.size() - i);
            }
            variable_data += "\r\n";
        }
        variable_data += "\r\n";
    }
};

#endif // MIME_MESSAGE_H
