#include "parser.h"

Parser::Parser()
{

}

void Parser::ParseHeaders(
            const QString &buf,
            QString &subject,
            QString &from,
            QString &date,
            QString &UID,
            QString &flags)

{
    int t;
    int x;

    x = buf.indexOf("(UID");
    t = buf.indexOf(" ", x + 6);
    UID = buf.mid(x + 5, t - x - 5);
    x = 0;

    t = buf.indexOf("\nDate: ") + 7;
    date = buf.mid(t, (buf.indexOf("\r", t) - t));
    date = ParseDate(date);

    t = buf.indexOf("FLAGS", Qt::CaseInsensitive);
    x = buf.indexOf(")", t);
    if (x == -1)
        x = buf.indexOf("\r", t);
    flags = buf.mid (buf.indexOf("(", t) + 1,  (x - buf.indexOf("(", t)) - 1);

    ParseMultilineAttribute(buf, "\nFrom: ", from);
    ParseMultilineAttribute(buf, "\nSubject: ", subject);
}

/////////////////////////////////////////////
void Parser::ParseSelect(
            const std::string &buf,
            int &CountOfMail,
            int &CountOfRecent,
            int &CountOfUnseen,
            std::string &UIDVALIDITY,
            int &UIDNEXT)

{
    size_t i = buf.find("EXISTS") - 2;
    CountOfMail = int(buf[i]) - '0'; i--;
    for (int q = 10; buf[i] != ' '; i--, q += 10)
    {
        CountOfMail += (int(buf[i]) - '0') * 10;
    }

    i = buf.find("RECENT") - 2;
    CountOfRecent = int(buf[i]) - '0'; i--;
    for (int q = 10; buf[i] != ' '; i--, q += 10)
    {
        CountOfRecent += (int(buf[i]) - '0') * 10;
    }

    i = buf.find("UNSEEN") + 7;
    CountOfUnseen = int(buf[i]) - '0'; i++;
    for (int q = 10; buf[i] != ']'; i++, q += 10)
    {
        CountOfUnseen += (int(buf[i]) - '0') * 10;
    }

    i = buf.find("UIDVALIDITY") + 12;
    UIDVALIDITY = buf.substr(i, (buf.find("]", i) - i));

    i = buf.find("UIDNEXT") + 8;
    UIDNEXT = int(buf[i]) - '0'; i++;
    for (int q = 10; buf[i] != ']'; i++, q += 10)
    {
        UIDNEXT += (int(buf[i]) - '0') * 10;
    }
}

QString Parser::ParseDate(const QString &Q)
{
    //qDebug() << Q;
    QString returnable = Q;
    QString bufer;

    /* remove "Day, " in front of date */
    returnable.replace("Mon", "");
    returnable.replace("Tue", "");
    returnable.replace("Wed", "");
    returnable.replace("Thu", "");
    returnable.replace("Fri", "");
    returnable.replace("Sat", "");
    returnable.replace("Sun", "");
    if (returnable.startsWith(","))
    { returnable = returnable.right(returnable.size() - 1); }
    while (returnable.startsWith(" "))
    { returnable = returnable.right(returnable.size() - 1);}
    bufer = (returnable.startsWith("0")) ? returnable.mid(1, returnable.indexOf(" ")) : returnable.mid(0, returnable.indexOf(" "));
    int day = bufer.toInt();

    returnable = returnable.right(returnable.size() - returnable.indexOf(" ") - 1);
    bufer = returnable.left(returnable.indexOf(" "));

    int month = 0;
    if (bufer.contains("Jan")) month = 1;
    else if (bufer.contains("Feb")) month = 2;
    else if (bufer.contains("Mar")) month = 3;
    else if (bufer.contains("Apr")) month = 4;
    else if (bufer.contains("May")) month = 5;
    else if (bufer.contains("Jun")) month = 6;
    else if (bufer.contains("Jul")) month = 7;
    else if (bufer.contains("Aug")) month = 8;
    else if (bufer.contains("Sep")) month = 9;
    else if (bufer.contains("Oct")) month = 10;
    else if (bufer.contains("Nov")) month = 11;
    else if (bufer.contains("Dec")) month = 12;

    returnable = returnable.right(returnable.size() - returnable.indexOf(" ") - 1);
    int year;
    bufer = returnable.left(returnable.indexOf(" "));
    year = bufer.toInt();
    if (year < 100) year += 2000;

    returnable = returnable.right(returnable.size() - returnable.indexOf(" ") - 1);
    bufer = (returnable.startsWith("0")) ? returnable.mid(1, 1) : returnable.mid(0, 2);
    returnable = returnable.right(returnable.size() - returnable.indexOf(":") - 1);
    int hour = bufer.toInt();

    bufer = (returnable.startsWith("0")) ? returnable.mid(1, 1) : returnable.mid(0, 2);
    returnable = returnable.right(returnable.size() - returnable.indexOf(":") - 1);
    int min = bufer.toInt();

    bufer = (returnable.startsWith("0")) ? returnable.mid(1, 1) : returnable.mid(0, 2);
    returnable = returnable.right(returnable.size() - returnable.indexOf(" ") - 1);
    int sec = bufer.toInt();

    bool plus;
    bufer = returnable.left(returnable.indexOf(" "));
    if (bufer.startsWith("+")) {plus = true; bufer = bufer.right(bufer.size() - 1);}
    if (bufer.startsWith("-")) {plus = false; bufer = bufer.right(bufer.size() - 1);}
    else {plus = true;}

    plus ? hour -= 10 * (bufer[0].unicode()  -48) : hour += 10 * (bufer[0].unicode() - 48);
    plus ? hour -=  1 * (bufer[1].unicode() - 48) : hour +=  1 * (bufer[1].unicode() - 48);
    plus ? min  -=  6 * (bufer[2].unicode() - 48) : min  +=  6 * (bufer[2].unicode() - 48);

    /* local machine utc zone */
    if ((1 * Qt::OffsetFromUTC) != 0)
        hour += (1 * Qt::OffsetFromUTC) + 1;

    if (min > 59)
    { hour++; min -= 60; }
    else if (min < 0)
    { hour--; min += 60; }
    if (hour > 23)
    { day++; hour -= 24; }
    else if (hour < 0)
    { day--; hour += 24; }

    returnable.clear();
    returnable += (day < 10)   ? ("0" + QString::number(day))   : (QString::number(day));
    returnable += "/";
    returnable += (month < 10) ? ("0" + QString::number(month)) : (QString::number(month));
    returnable += "/";
    returnable += QString::number(year);

    returnable += " ";
    returnable += (hour < 10) ? ("0" + QString::number(hour)) : (QString::number(hour));
    returnable += ":";
    returnable += (min < 10)  ? ("0" + QString::number(min))  : (QString::number(min));
    returnable += ":";
    returnable += (sec < 10)  ? ("0" + QString::number(sec))  : (QString::number(sec));
    bufer.clear();
    return returnable;
}

void Parser::ParseAttribute(const QString &Q, const QString &search_keyword, QString &returnable)
{
    /*
        Parses Content-type, Charset, CID and other values in string Q like:
        "search_keyword_1: value_1, search_keyword_2=value_2;"

        if ( value exists ) { returnable = value }
        else { returnable does not changed }

        example:
            bufer = "\tcharset=\"utf-8\"";
            ParseAttribute(bufer, "charset", charset) --> charset == "UTF-8"
    */

    /* fixing the QT bag */
    QString Q_upper = Q.toUpper();
    QString seach_keyword_upper = search_keyword.toUpper();

    int pos_of_word;
    int min_pos_of_first_symbol  = Q.length();
    int min_pos_of_second_symbol = Q.length();
    if (Q_upper.contains(seach_keyword_upper, Qt::CaseInsensitive))
    {
        pos_of_word = Q_upper.indexOf(seach_keyword_upper, Qt::CaseInsensitive);
        if ( (Q.indexOf(",", pos_of_word) != -1) &&
             (Q.indexOf(",", pos_of_word) < min_pos_of_second_symbol) )
            min_pos_of_second_symbol = Q.indexOf(",", pos_of_word);

        if ( (Q.indexOf(";", pos_of_word) != -1) &&
             (Q.indexOf(";", pos_of_word) < min_pos_of_second_symbol) )
            min_pos_of_second_symbol = Q.indexOf(";", pos_of_word);

        if ( (Q.indexOf("=", pos_of_word) != -1) &&
             (Q.indexOf("=", pos_of_word) < min_pos_of_first_symbol) )
            min_pos_of_first_symbol = Q.indexOf("=", pos_of_word) + 1;

        if ( (Q.indexOf(":", pos_of_word) != -1) &&
             (Q.indexOf(":", pos_of_word) < min_pos_of_first_symbol) )
            min_pos_of_first_symbol = Q.indexOf(":", pos_of_word) + 1;

        returnable = Q.mid(
                    min_pos_of_first_symbol,
                    min_pos_of_second_symbol -  min_pos_of_first_symbol);

        if (returnable.startsWith('\"') && returnable.endsWith('\"'))
            returnable = returnable.mid(1, returnable.size() - 2);


        while(returnable.startsWith(" "))
        {
            returnable = returnable.right(returnable.size() - 1);
        }
        while(returnable.endsWith(" "))
        {
            returnable = returnable.left(returnable.size() - 1);
        }
    }
}
/////////////////////////////////////////////
void Parser::ParseMultilineAttribute(const QString &Q, const QString &search_keyword, QString &returnable)
{
    int t = Q.indexOf(search_keyword);
    int x;
    if (t == -1)
    {
        qDebug() << ("ParseMultilineAttribute(<bufer>, " + search_keyword + ", " + returnable + " failed\n");
        return;
    }
    t += search_keyword.length();
    QString source = "";
    QString bufer = "";
    do
    {
        source += Q.mid(t, ( (x = Q.indexOf("\r", t)) - t) );
    } while( (t = Q.indexOf(" ", x)) == (x + 2));

    t = 0; x = 0;
    returnable.clear();

    /* for the last word */
    source.append(" ");

    /* Fixing broken string like "data<data>" instead of "data <data>" */
    if (search_keyword.contains("From", Qt::CaseInsensitive))
    {
        if (source.lastIndexOf(" ") + 1 != source.lastIndexOf("<"))
        {
            QString save_space = source.mid( source.lastIndexOf("<"),
                                                 source.length() - source.lastIndexOf("<"));
            source = source.left(source.lastIndexOf("<"));
            source += " ";
            source += save_space;
        }
    }

    /* One word will be exists */
    t = (source.indexOf(" ", x + 1) != -1) ? source.indexOf(" ", x + 1) : source.indexOf("\r", x + 1);
    do
    {
        bufer = source.mid(x , t - x);
        if (bufer.contains("?utf-8?B", Qt::CaseInsensitive))
        {  returnable += Decoder::Base64ToUTF8 (bufer.mid(10, bufer.length() - 10 - 2));  }
        else if (bufer.contains("?utf-8?Q", Qt::CaseInsensitive))
        {  returnable += Decoder::QuotedPrintableToUTF8 (bufer.mid(10, bufer.length() - 10 - 2),  true); }
        else
        {
            if (!returnable.endsWith(" "))
               { returnable += " "; }
            returnable += bufer;
            returnable += " ";
        }
        x = t + 1;
        t = source.indexOf(" ", x + 1);
    } while (t > 0);

    if (returnable.startsWith(" "))
    { returnable = returnable.right(returnable.size() - 1); }
    if (returnable.endsWith(" "))
    { returnable = returnable.left(returnable.size() - 1);  }


    if (search_keyword.contains("From", Qt::CaseInsensitive))
    {
        /* removing "<mail@domen>" */
        t = returnable.lastIndexOf("<");
        x = returnable.lastIndexOf(">");
        if (x != -1 && t != -1 && x > t)
        { returnable = returnable.left(t - 1); }

        /* removing repetitive "mail@domen" */
        else
        {
            x = returnable.lastIndexOf(" ");
            returnable = returnable.left(x - 1);
        }

        /* fixing several exceptions */
        if (returnable.endsWith(" "))
        { returnable = returnable.left(returnable.size() - 1); }
        if (returnable.startsWith("\"") && !returnable.endsWith("\""))
        {  returnable = returnable.right(returnable.size() - 1); }
        if (returnable.startsWith("?") && returnable.endsWith("?"))
        { returnable = returnable.mid(1, returnable.lastIndexOf("?") - 1); }
    }

    /* using name instead of "name" */
    if (returnable.startsWith("\"") && returnable.endsWith("\""))
    { returnable = returnable.mid(1, returnable.size() - 2); }
}
/////////////////////////////////////////////
QString Parser::ParseCID(const QString &Q)
{
    QString returnable = "\0";
    if (Q.contains("Content-ID", Qt::CaseInsensitive))
    {
        // 13 is "Content-ID: <"
        qDebug() << (returnable = Q.mid(
                    (Q.indexOf("Content-ID:", Qt::CaseInsensitive) + 13),
                    (Q.length()  - 14 - (Q.indexOf("Content-ID:", Qt::CaseInsensitive)))));
    }
    return returnable;
}
/////////////////////////////////////////////
void Parser::replaceCID(std::map <QString, QString> &M, const QString &filename)
{
    QFile file(filename);
    if (!file.open(QFile::ReadOnly | QFile::Text))
        qDebug() << "Something wrong in replaceCID (" << filename << ")";
    QTextStream input(&file);

    QFile file_rewrited(filename + "updated");
    QTextStream output(&file_rewrited);
    if (!file_rewrited.open(QFile::WriteOnly | QFile::Text))
    {qDebug() << "Something wrong in replaceCID (" << filename + "updated" << ")"; return;}
    QString bufer;
    QString CID;
    while (!input.atEnd())
    {
        bufer = input.readLine();
        if (bufer.contains("cid:", Qt::CaseInsensitive))
        {
            // 5 is "stc=\"
             CID = bufer.mid(
                        (bufer.indexOf("src=") + 5),
                        (bufer.indexOf("\"", (bufer.indexOf("src=") + 5)) - (bufer.indexOf("src=") + 5)));
            QString CID_to_find = CID;
            CID_to_find.replace("cid:", "<");
            CID_to_find.append(">");
            auto it = M.find(CID_to_find);
            if (it != M.end())
            {
                bufer.replace(CID, it->second);
                //qDebug() << "REPLACING " << CID << " WITH " << it->second;
            }
            else
            {
                qDebug() << "NO FIND" << CID;
            }
        }
        output << bufer << "\n";
    }
    file.close();
    file_rewrited.close();
    remove(filename.toStdString().c_str());
    rename((filename + "updated").toStdString().c_str(), filename.toStdString().c_str());
    M.clear();
}

void Parser::CreateText(QString &part, const QString &filename, const QString &charset, const QString &content_transer_encoding)
{
    QFile log("loger_create_text.txt");
    log.open(QFile::WriteOnly | QFile::Append);
    QTextStream logg(&log);
    logg.setCodec("UTF-8");

    QFile Q(filename);
    Q.open(QFile::WriteOnly | QFile::Text);
    QTextStream output(&Q);
    // !!!!!

    // !!!!!
    logg << "\r\n######################################################\r\n";
    logg << "\r\n-------------------------------------------\r\n";
    logg << "part_source: " << part;
    logg << "\r\n-------------------------------------------\r\n";
    if (content_transer_encoding.contains("base64", Qt::CaseInsensitive))
    {
        output.setCodec("UTF-8");
        part.replace("\r\n", "");
        part = (Decoder::Base64ToUTF8(part));
        //output << part;
    }
    else if (content_transer_encoding.contains("quoted", Qt::CaseInsensitive))
    {
        output.setCodec("UTF-8");
        part.replace("=\r\n", "");
        QString new_part;
        for (int i = 0; i < part.length(); i++)
        {
            /* is quoted-printable */
            if (part[i] == "=" && ((i + 3) <= part.length()) )
            {
                if (part[i+1] != "=" && part[i+1] != " ")
                {
                    new_part += (part.mid(i, 3));
                    i = i + 2;
                }
            }
            else
            {
                new_part += (QString(part[i]));
            }
        }
        new_part = Decoder::QuotedPrintableToUTF8(new_part);
        part = new_part;
        logg << "new_part: " << new_part << "\n";
    }
    output << part;
    logg << "\r\n-------------------------------------------\r\n";
    logg << "\r\n-------------------------------------------\r\n";
    logg << "\r\n######################################################\r\n";
    log.close();

}

void Parser::CreateFile(const QString &Q, const QString &filename, const QString &content_type, const QString &content_transfer_encoding)
{
    QByteArray base64Data = Q.toUtf8();
    QString type = content_type.mid((content_type.indexOf("/") + 1), (content_type.size() - content_type.indexOf("/")));
    if (content_type.contains("image"))
    {
        QImage image;
        image.loadFromData(QByteArray::fromBase64(base64Data), "PNG");
        image.save(filename, "PNG");
    }
}

/////////////////////////////////////////////
QString Parser::PartHandle(const QString &Q, QString &part, const QString &content_type, const QString &charset, const QString &content_transfer_encoding, const QString &filename)
{
    /* returns the file name */
    if (content_type.contains("text/plain", Qt::CaseInsensitive))
    {
        CreateText(part, (Q + ".txt"),  charset, content_transfer_encoding);
        return Q + ".txt";
    }
    if (content_type.contains("text/html", Qt::CaseInsensitive))
    {
         CreateText(part, (Q + ".html"), charset, content_transfer_encoding);
         return Q + ".html";
    }
    else
    {
        CreateFile(part, filename, content_type, content_transfer_encoding);
        return filename;
    }
}
/////////////////////////////////////////////
MimeMessage* Parser::ParseMail(const QString &Q)
{
    MimeMessage* M = new MimeMessage;

    QFile log("loger_parse_mail" + Q + ".txt");
    log.open(QFile::WriteOnly);
    QTextStream logg(&log);
    logg << "\r\n######################################################\r\n";
    logg << "\r\nFile is: " << Q << "\r\n";
    logg << "\r\n-------------------------------------------\r\n";


    QFile file(Q);
    QString bufer;
    QString part;
    QString boundary;
    QString boundary_mail; //?
    QString content_type;
    QString charset;
    QString content_transfer_encoding;
    QString filename;
    QString content_id;
    std::map <QString, QString> CIDs;

    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        qDebug() << "Something went wrong in ParseMail(" << Q << ")";
        exit(1);
    }
    QTextStream input(&file);

    /*
        Start of parsing. No check for corrupted file.
    */
    bufer = input.readLine();

    /*
         Keywords should be initialiazed with zero byte.
         Algorithm will check the "\0" value.
    */
    part = "";
    boundary = "\0";
    boundary_mail = "\0";
    content_type = "\0";
    charset = "\0";
    content_transfer_encoding = "\0";

    /*
        Parsing the header.
        Header will be ended in empty CR LF line.
    */
    logg << "\r\n <--- START FETCHING HEADER --->\r\n";
    while (!bufer.isEmpty())
    {
        ParseAttribute(bufer, "boundary",     boundary_mail);
        ParseAttribute(bufer, "Content-Type", content_type);
        ParseAttribute(bufer, "charset",      charset);
        ParseAttribute(bufer, "Content-Transfer-Encoding", content_transfer_encoding);
        bufer = input.readLine();
    }

    logg << "boundary:      "  << boundary_mail << "\r\n";
    logg << "boundary:      "  << boundary      << "\r\n";
    logg << "content_type:  "  << content_type  << "\r\n";
    logg << "charset:       "  << charset       << "\r\n";
    logg << "contentTEnc:   "  << content_transfer_encoding << "\r\n";
    logg << "\r\n <--- END FETCHING HEADER --->\r\n";

    /*
        Header`s parse done.
        Now we have:
            boundary of whole mail or no boundary
            content_type = "multipart/alternative" or no content_type
    */

    QStringList boundaries;
    QString boundaty_mail_end = boundary_mail + "--";

    //qDebug() << content_type;
    if (content_type.contains("multipart", Qt::CaseInsensitive))
    {

        M->add_multipart(content_type);
        M->multipart_begin();
        boundaries.append(boundary_mail);
        if (boundary != boundary_mail && boundary.size() > 1)
        {
            boundaries.append(boundary);
        }
        /* EOF will be at boundary_mail_end string */
        while (!bufer.contains(boundaty_mail_end))
        {
            QString q = (boundaries.last());
            while (!bufer.contains(boundaries.last()))
            {
                bufer = input.readLine();
            }

            if (bufer.contains(boundaty_mail_end))
                break;

            /*  Parse keywords  */
            while (!bufer.isEmpty())
            {
                ParseAttribute(bufer, "boundary",     boundary);
                ParseAttribute(bufer, "Content-Type", content_type);
                ParseAttribute(bufer, "charset",      charset);
                ParseAttribute(bufer, "Content-Transfer-Encoding", content_transfer_encoding);
                ParseAttribute(bufer, "Content-ID", content_id);
                ParseAttribute(bufer, "filename", filename);
                bufer = input.readLine();
            }
            if (boundary != boundaries.last() && boundary.size() > 1)
            {
                boundaries.append(boundary);
            }
            //qDebug() << filename;
            //qDebug() << content_id;

            CIDs.insert(std::make_pair(content_id, filename));

            /* The next line will be data of part. Grab all until the boundary line */
            logg << "boundary:     " << boundary     << "\r\n";
            logg << "content_type: "  << content_type << "\r\n";
            logg << "charset:      "  << charset      << "\r\n";
            logg << "contentTEnc:  "  << content_transfer_encoding << "\r\n";
            logg << "Content-ID:   " << content_id << "\r\n";
            logg << "filename:     " << filename << "\r\n";
            //qDebug() << " " << content_type;
            logg << "<-- PART START --->" << "\r\n";

            bufer = input.readLine();
            qDebug() << "::::::::::::::::" << content_type;
            qDebug() << "BOUNDARIES:";
            auto it = boundaries.begin();
            for (int i = 0; it != boundaries.end(); ++it, i++)
            {
                qDebug() << boundaries.at(i);
            }
            qDebug() << "BOUNDARIES END";
            while (!bufer.contains(boundaries.last()))
            {
                part += bufer;
                part += "\r\n";
                bufer = input.readLine();
            }

            /* Parse the part */
            QString name = PartHandle(Q, part, content_type, charset, content_transfer_encoding, filename);

            if (content_type.contains("multipart"))
            {
                M->add_multipart(content_type);
                M->multipart_begin();
            }
            else
            {
                /* fix */
                if (content_type.contains("text/plain"))
                    M->add_part(content_type, content_transfer_encoding, part, content_id, Q + ".txt");
                else
                {

                    if (content_type.contains("text/html"))
                        M->add_part_file(content_type, content_transfer_encoding, name, content_id, Q + ".html");
                    else
                        M->add_part_file(content_type, content_transfer_encoding, name, content_id, filename);
                }
            }
            part.clear();

            qDebug() << "BUFER:::" << bufer;
            if (bufer.contains(boundaries.last() + "--"))
            {
                if (bufer.contains(boundary_mail))
                { M->multipart_stop(); boundaries.clear(); break; }
                boundaries.removeLast();
                if (content_type.contains("multipart"))
                {
                    M->multipart_stop();
                }
            }

            content_id = "\0";
            boundary = "\0";
            content_type = "\0";
            charset = "\0";
            content_transfer_encoding = "\0";
            filename = "\0";
            logg << "<-- PART END --->" << "\r\n";
            if (boundaries.empty())
                {break;}
            //bufer = input.readLine();
        }

     }

    else
    {
        bufer = input.readLine();
        logg << "<-- PART START --->" << "\r\n";
        while (!bufer.contains(". OK", Qt::CaseInsensitive))
        {
            if (bufer.startsWith(" FLAGS")) break;
            logg << bufer << "\r\n";
            part += bufer;
            part += "\r\n";
            bufer = input.readLine();
        }
        logg << "<-- PART END --->" << "\r\n";
        logg << "<-- END FETCHING BODY --->\r\n";
        part = part.left(part.size() - 3);
        filename = PartHandle(Q, part, content_type, charset, content_transfer_encoding, filename);
        M->add_part_file(content_type, content_transfer_encoding, part, content_id, filename);
        file.close();
        log.close();
    }

    if (!CIDs.empty())
    {
        replaceCID(CIDs, Q + ".html");
    }
    qDebug() << "parseMail()";
    return M;
}

QString Parser::ParseIP(const QString &Q)
{
    QFile File(Q);
    File.open(QFile::ReadOnly);
    QTextStream input(&File);

    QString bufer;
    QString temp;
    QChar c;
    int dot_counter = 0;
    bool ok_word = true;
    while (!input.atEnd())
    {
        bufer = input.readLine();
        dot_counter = 0;
        ok_word = true;
        for (int i = 0; i < bufer.size(); i++)
        {
            if ((c = bufer[i]) == ".")
                dot_counter++;
        }
        if (dot_counter == 3)
        {
            temp = bufer.mid(bufer.indexOf(".") - 3, 3);
            for (int i = 0; i < 3; i++)
            {
                if (!((temp[i] >= "0" && temp[i] <= "9") || temp != " "))
                {
                    ok_word = false;
                }
            }
            if (temp == "192" || temp == "172" || temp == "100" || temp == "10")
                ok_word = false;
            if (ok_word)
            {
                while ((bufer.size()) > 0 && (!(bufer[0] >= "0" && bufer[0] <= "9")))
                {
                    bufer = bufer.right(bufer.size() - 1);
                }
                while ((bufer.size()) > 0 && (!(bufer[bufer.size() - 1] >= "0" && bufer[bufer.size() - 1] <= "9")))
                {
                    bufer = bufer.left(bufer.size() - 1);
                }
                if (bufer.size() > 7)
                {
                    File.close();
                    return bufer;
                }
            }
        }
    }
    return "no ip";
}
