#ifndef MIMEMESSAGE_H
#define MIMEMESSAGE_H

#include <QtCore>

class MimeMessage
{
public:
    struct Part
    {
        QString content_transfer_encoding;
        QString content_type;
        QString content_id;
        QString filename;
        QString data;
    };
    struct Multipart
    {
        Multipart* parent;
        QString content_type;
        QList<Multipart*> MP;
        QList<Part*> P;
        Multipart()
        {
            parent = nullptr;
        }
    }; 
    MimeMessage();
    void add_multipart(QString content_type);
    void multipart_begin();
    void multipart_stop();
    void add_part(QString content_type, QString content_transfer_encoding, QString data, QString content_id, QString filename);
    void add_part_file(QString content_type, QString content_transfer_encoding, QString file, QString content_id, QString filename);
    void clear(Multipart* current);
    void createMessage(QString name);
    ~MimeMessage();
    void print(Multipart* current);
    QList < std::pair<QString, QString> > display();
    void set_searchable_type(QString type);
    void set_subject();
    void set_from();
private:
    Multipart *home;
    Multipart *current;
    Multipart *last_added;
    QString searchable_type = "html";
    QList<std::pair<QString, QString>> returnable;
    QString subject;
    QString from;

};

#endif // MIMEMESSAGE_H
