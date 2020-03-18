#include "mimemessage.h"


MimeMessage::MimeMessage()
{
    home = nullptr;
    current = nullptr;
    last_added = nullptr;
}
void MimeMessage::add_multipart(QString content_type)
{
    if (current)
    {
        Multipart* new_multipart = new Multipart;
        new_multipart->content_type = content_type;
        new_multipart->parent = current;
        current->MP.append(new_multipart);
        last_added = new_multipart;
    }
    else
    {
        home = new Multipart;
        home->content_type = content_type;
        current = home;
        last_added = home;
    }
}
void MimeMessage::multipart_begin()
{
    if (current && last_added)
        current = last_added;
    else
        if (last_added)
        {
            qDebug() << "multipart_begin(): (current) == null";
            throw "multipart_begin(): (current) == null";
        }
        else
        {
            qDebug() << "multipart_begin(): (last_added) == null";
            throw "multipart_begin(): (last_added) == null";
        }
}
void MimeMessage::multipart_stop()
{
    if (current && current->parent)
        current = current->parent;
    else
        if (current == home)
            return; // no error: at home dir
        else if (current)
        {
            qDebug() << "multipart_stop(): (current->parent) == null";
            throw "multipart_stop(): (current->parent) == null";
        }
        else
        {
            qDebug() << "multipart_stop(): (current) == null";
            throw "multipart_stop(): (current) == null";
        }
}
void MimeMessage::add_part(QString content_type, QString content_transfer_encoding, QString data, QString content_id, QString filename)
{
    Part* new_part = new Part;
    new_part->data = data;
    new_part->content_transfer_encoding = content_transfer_encoding;
    new_part->content_type = content_type;
    new_part->content_id = content_id;
    new_part->filename = filename;
    if (current)
        current->P.append(new_part);
    else
    {
        qDebug() << "add_part(): current == null";
        throw "add_part(): current == null";
    }
}
void MimeMessage::add_part_file(QString content_type, QString content_transfer_encoding, QString file, QString content_id, QString filename)
{
    Part* new_part = new Part;
    new_part->data = file;
    new_part->content_transfer_encoding = content_transfer_encoding;
    new_part->content_type = content_type;
    new_part->content_id = content_id;
    new_part->filename = filename;
    if (current)
        current->P.append(new_part);
    else
    {
        qDebug() << "add_part(): current == null";
        add_multipart("not a multipart");
        multipart_begin();
        current->P.append(new_part);
        multipart_stop();
        //throw "add_part(): current == null";
    }
}
void MimeMessage::clear(Multipart* current)
{
    if (!current->MP.isEmpty())
    {
        auto it = current->MP.begin();
        for (int i = 0; it != current->MP.end(); ++it, i++)
        {
            clear(current->MP.at(i));
            if (current->MP.at(i))
                delete (current->MP.at(i));
        }
    }
    if (!current->P.isEmpty())
    {
        auto it = current->P.begin();
        for (int i = 0; it != current->P.end(); ++it, i++)
        {
            if (!current->P.at(i)->filename.isEmpty())
            {
                remove(current->P.at(i)->filename.toStdString().c_str());
                delete current->P.at(i);
            }
        }
    }
}

void MimeMessage::createMessage(QString name)
{
    QFile message_file(name);
    QTextStream out(&message_file);
    //out <<
}
MimeMessage::~MimeMessage()
{
    if (home)
    {
        clear(home);
        delete home;
    }
    home = current = nullptr;
}
void MimeMessage::print(Multipart* current)
{
    if (current->content_type.contains("alternative"))
    {
        bool type_found = false;
        /* Multipart takes precedence. (it can be related HTML with pictures, as example). */
        if (current->MP.size() != 0)
            print(current->MP.first());
        if (!type_found)
        {
            /* it a choice between several Parts */
            if (current->P.size() != 0)
            {
                auto it = current->P.begin();
                for (int i = 0; it != current->P.end(); ++it, i++)
                {
                    /* searching for "text/html" by default of other type (user can define "text/plain") */
                    if (current->P.at(i)->content_type.contains(searchable_type))
                    {
                        returnable.append(std::make_pair(current->P.at(i)->content_type, current->P.at(i)->filename));
                        type_found = true;
                    }
                }
                /* "text/html" as default (or other type) had not been found. Searching for "text/plain" */
                if (!type_found)
                {
                    it = current->P.begin();
                    for (int i = 0; it != current->P.end(); ++it, i++)
                    {
                        if (current->P.at(i)->content_type.contains("text/plain"))
                        {
                            returnable.append(std::make_pair(current->P.at(i)->content_type, current->P.at(i)->filename));
                            type_found = true;
                        }
                    }
                }
            }
        }
    }
    else if (current->content_type.contains("related"))
    {
        qDebug() << "is related";
        if (current->MP.size() != 0)
        {
            auto it = current->MP.begin();
            for (int i = 0; it != current->MP.end(); ++it, i++)
            {
                print(current->MP.at(i));
            }
        }
        auto it = current->P.begin();
        for (int i = 0; it != current->P.end(); ++it, i++)
        {
            if (current->P.at(i)->content_id.isEmpty())
            {
                returnable.append(std::make_pair(current->P.at(i)->content_type, current->P.at(i)->filename));
            }
            qDebug() << current->P.at(i)->content_type << (current->P.at(i)->content_id.isEmpty() ? "" : "(is CID)");
        }
    }
    else if (current->content_type.contains("not a multipart"))
    {
         returnable.append(std::make_pair(current->P.at(0)->content_type, current->P.at(0)->filename));
         qDebug() << current->P.at(0)->content_type << (current->P.at(0)->content_id.isEmpty() ? "" : "(is CID)");
    }
    else
    {
        qDebug() << "Unknown MIME part";
    }
}
QList < std::pair<QString, QString> > MimeMessage::display()
{
    if (home)
        print(home);
    return returnable;
}
void MimeMessage::set_searchable_type(QString type) {searchable_type = type;}
void MimeMessage::set_subject() {}
void MimeMessage::set_from() {}

