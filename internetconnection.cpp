#include "internetconnection.h"

InternetConnection::InternetConnection()
{
    bio = nullptr;
    ssl = nullptr;
    ctx = nullptr;
    if (initialize_OpenSSL() <= 0)
        qDebug() << "Critical error in initialize_OpenSSL()";
    if (initialize_socket() <= 0)
        qDebug() << "Critical error in initialize_socket()";
}
int InternetConnection::initialize_OpenSSL()
{
    if (SSL_library_init() <= 0)
    { return 0; }
    SSL_load_error_strings();
    ERR_load_BIO_strings();
    OpenSSL_add_all_algorithms();
    return 1;
}
void InternetConnection::setMode(int connection_type)
{
    switch(connection_type)
    {
      case imap: {mode = imap; break;}
      case smtp: {mode = smtp; break;}
      default: break;
    }
}
int InternetConnection::initialize_socket()
{
    ctx = SSL_CTX_new(SSLv23_client_method());
    if (ctx == nullptr)
    { return 0; }
    if (!SSL_CTX_load_verify_locations(ctx, "C:\\OpenSSL-Win32\\bin\\cert.pem", nullptr))
    { return 0; }
    bio = BIO_new_ssl_connect(ctx);
    BIO_set_nbio(bio, 1);
    if (BIO_get_ssl(bio, &ssl) <= 0)
    { return 0; }
    return 1;
}
InternetConnection::~InternetConnection()
{
    BIO_free_all(bio);
}
QString InternetConnection::try_to_detect_server_ip (const QString &domain, const QString &mode)
{
/* returns:
     - "no ip" with failure (by Parser::ParseIP())
     - ipv4 address in QString, containing only dec numbers and 4 dots (like "111.111.111.111") */
#ifdef _WIN32
    QString command = "nslookup " + mode + "." + domain + " >> temp_of_try_to_detect_ip" + mode;
    qDebug() << command;
    system(command.toStdString().c_str());
    QString temp = Parser::ParseIP("temp_of_try_to_detect_ip" + mode);
    qDebug() << temp;
    command = "temp_of_try_to_detect_ip" + mode;
    //remove(command.toStdString().c_str());
    return temp;
#endif

}
int InternetConnection::connect(const QString &server_ip, const QString &port)
{
    server = server_ip + ":" + port;
    BIO_set_conn_hostname(bio, server.toStdString().c_str());
    int tries = 0;
    while (true)
    {
        if (BIO_do_connect(bio) <= 0 && BIO_should_retry(bio))
        {
            qDebug() << "BIO connection failed";
            tries++;
            /* 5 seconds without connection */
            if (tries > 500)
                return 0;
            Sleep(10);
        }
        else
        {
            qDebug() << "BIO connection success";
            break;
        }
    }
    memset(buf, 0, len); tries = 0;

    /* Handle the server's welcome */
    while (BIO_read(bio, buf, int(len)) <= 0 && tries < 500)
    {
        tries++;
        Sleep(10);
    }
    memset(buf, 0, len);
    /* 5 seconds without answer */
    return (tries == 500) ? 0 : 1;
}
int InternetConnection::disconnect()
{
    is_auth = 0;
    BIO_reset(bio);
    return 1;
}
int InternetConnection::login (const QString &login, const QString &password)
{
    if (mode == imap)
    {
        QString command = ". login " + login + " " + password + "\r\n";
        SendRequest(command);
        QString temp = LoadAnswer();
        is_auth = (temp.contains(". NO")) ? false : true;
        qDebug() << "auth: " << is_auth;
    }
    else if (mode == smtp)
    {
        QString command = "helo domain\r\n";
        SendRequest(command);
        qDebug() << LoadAnswer();
        command = "auth login\r\n";
        SendRequest(command);
        qDebug() << LoadAnswer();
        command = Decoder::UTF8toBase64(login) + "\r\n";
        SendRequest(command);
        qDebug() << LoadAnswer();
        command = Decoder::UTF8toBase64(password) + "\r\n";
        SendRequest(command);
        qDebug() << LoadAnswer();
        QString temp =  LoadAnswer();
        is_auth = (temp.contains("235")) ? true : false;
    }
    else
    {
        return 0;
    }
    return is_auth;
}
int InternetConnection::logout()
{
    if (mode == imap)
    {
        QString command = ". logout\r\n";
        SendRequest(command);
    }
    is_auth = 0;
    return 1;
}
int InternetConnection::is_authorized()
{
    return is_auth;
}
int InternetConnection::select_mailbox (const QString &name)
{
    if (mode == imap)
    {
        QString command = ". select " + name + "\r\n";
        SendRequest(command);
        return (LoadAnswer().contains("\r\n. NO")) ?  1 : 0;
    }
    return 0;
}
int InternetConnection::load_all_mailbox (const QString &path_file)
{
    if (mode == imap)
    {
        QString command = ". list \"/\" *\r\n";
        SendRequest(command);
        LoadAnswer(path_file);
    }
    return 0;
}
int InternetConnection::load_all_mail (const QString &path_file)
{
    if (mode == imap)
    {
        QString command = ". fetch 1:* (UID FLAGS RFC822)\r\n";
        SendRequest(command);
        return LoadAnswer(path_file);
    }
    return 0;
}
int InternetConnection::load_all_mail_headers (const QString &path_file)
{
    if (mode == imap)
    {
        QString command = ". fetch 1:* (UID FLAGS RFC822.HEADER)\r\n";
        SendRequest(command);
        return LoadAnswer(path_file);
    }
    return 0;
}
int InternetConnection::load_mail(const QString &UID, const QString &path_file)
{
    if (mode == imap)
    {
        QString command = ". UID fetch " + UID + " (RFC822)\r\n";
        SendRequest(command);
        return LoadAnswer(path_file);
    }
    return 0;
}
int InternetConnection::delete_mail(const QString &UID)
{
    if (mode == imap)
    {
        QString command = ". UID store " + UID + " +flags \\Deleted\r\n";
        SendRequest(command);
        return 1;
    }
    return 0;
}
int InternetConnection::set_mail_read(const QString &UID)
{
    if (mode == imap)
    {
        QString command = ". UID store " + UID + " +flags \\Seen\r\n";
        SendRequest(command);
        return 1;
    }
    return 0;
}
int InternetConnection::set_mail_unread(const QString &UID)
{
    if (mode == imap)
    {
        QString command = ". UID store " + UID + " -flags \\Seen\r\n";
        SendRequest(command);
        return 1;
    }
    return 0;
}
QString InternetConnection::load_list_of_mailboxes()
{
    if (mode == imap)
    {
        SendRequest(". list \"/\" *\r\n");
        return LoadAnswer();
    }
    return "";
}
void InternetConnection::SendRequest(const QString &command)
{
    BIO_write(bio, command.toStdString().c_str(), int(command.length()));
}
int InternetConnection::LoadAnswer(const QString &filename)
{
    std::ofstream reply;
    reply.open(filename.toStdString(), std::ios::binary);
    QString buf_string;
    int tries = 0;
    do
    {
        memset(buf, 0, len);
        if ((BIO_read(bio, buf, int (len - 1))) > 0)
        {
            tries = 0;
            reply << buf;
            reply.flush();
            buf_string = buf;
            if (mode == imap && buf_string.contains("\r\n. NO"))
            {
                memset(buf, 0, len);
                reply.close();
                return 0;
            }
            if (mode == imap && buf_string.contains("\r\n. OK"))
            {
                memset(buf, 0, len);
                reply.close();
                return 1;
            }
        }
        else
        {
            tries++;
            Sleep(5);
        }

    } while (tries < 100);
    memset(buf, 0, len);
    reply.close();
    return 0;
}
QString InternetConnection::LoadAnswer()
{
    QString reply;
    if (mode == imap)
    {
        QString buf_string;
        QString buf_string1;
        QString buf_string2;
        do
        {
            memset(buf, 0, len);
            if (BIO_read(bio, buf, int (len - 1)) > 0)
            {
                if (buf_string1.isEmpty())
                {
                    buf_string1 = buf;
                    reply += buf;
                    buf_string = buf_string2 + buf_string1;
                    buf_string2.clear();
                }
                else if (buf_string2.isEmpty())
                {
                    buf_string2 = buf;
                    reply += buf;
                    buf_string = buf_string1 + buf_string2;
                    buf_string1.clear();
                }
                if (buf_string.startsWith(". NO", Qt::CaseInsensitive))
                {
                    return ". NO";
                }
            }
            else if (!BIO_should_retry(bio))
            {
                /* connection lost */
                return "";
            }

        } while (!buf_string.contains(". OK", Qt::CaseInsensitive));
        /* prevent "success" or other words to be written in socket after error code */
        while (BIO_read(bio, buf, int (len - 1)) != -1) continue;
    }
    else if (mode == smtp)
    {
        memset(buf, 0, len);
        do
        {
            Sleep(100);
            reply += buf;
        } while (BIO_read(bio, buf, int (len - 1)) > 0);
    }
    return reply;
}
int InternetConnection::send_message(const QString &mail_from, const QString &rcpt_to, const QString &message)
{
    if (mode != smtp)
        return 0;
    SendRequest("ehlo domain.local\r\n");
    Sleep(100);
    SendRequest("mail from: <" + mail_from + ">\r\n");
    Sleep(100);
    SendRequest("rcpt to: <" + rcpt_to + ">\r\n");
    Sleep(100);
    SendRequest("data\r\n");
    Sleep(100);

    QFile message_file(message);
    message_file.open(QFile::ReadOnly);
    QTextStream message_ofstream(&message_file);
    QString bufer;
    while (!message_ofstream.atEnd())
    {
        bufer = message_ofstream.readLine();
        SendRequest(bufer + "\r\n");
    }

    SendRequest("\r\n.\r\n");
    Sleep(100);
    return 1;
}
void InternetConnection::mail_updater()
{
    int t = 0;
    SendRequest(". select inbox\r\n");
    QString bufer = LoadAnswer();
    t = bufer.indexOf("UIDNEXT");
    QString temp = bufer.mid(
                t + 8, (bufer.indexOf("]", t) - t - 8));
    UID_validity = temp;
    while (true)
    {
        Sleep(5000);
        int t = 0;
        SendRequest(". select inbox\r\n");
        QString bufer = LoadAnswer();
        t = bufer.indexOf("UIDNEXT");
        QString temp = bufer.mid(
                    t + 8, (bufer.indexOf("]", t) - t - 8));
        if (temp != UID_validity)
        {
            qDebug() << "new message";
            UID_validity = temp;
            mail_updater_trigger();
        }
    }
}
int InternetConnection::auth()
{
    QString server_ip;
    qDebug() << "auth() called";
    if (mode == imap)
    {
        server_ip = try_to_detect_server_ip(login_with_domain.right(login_with_domain.size() - login_with_domain.indexOf("@") - 1), "imap");
        qDebug() << login_with_domain;
        qDebug() << login_with_domain.right(login_with_domain.size() - login_with_domain.indexOf("@") - 1);
        if (server_ip != "no ip")
        {
            if (connect(server_ip, "993"))
            {
                login(login_with_domain, password);
            }
            else
            {
                disconnect();
                return -1;
            }
        }
        else
        {
            return -1;
        }
    }
    else if (mode == smtp)
    {
        server_ip = try_to_detect_server_ip(login_with_domain.right(login_with_domain.size() - login_with_domain.indexOf("@") - 1), "smtp");
        if (server_ip != "no ip")
        {
            if (connect(server_ip, "465"))
            {
                login(login_with_domain, password);
            }
            else
            {
                disconnect();
                return -2;
            }
        }
        else
        {
            return -2;
        }
    }

    else
    {
        qDebug() << "mode is not specified.";
        return 0;
    }
    return 1;
}
