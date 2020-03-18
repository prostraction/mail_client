#ifndef INTERNETCONNECTION_H
#define INTERNETCONNECTION_H

#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#include <QtCore>       // should be reworked !!!!
#include <fstream>

#include "decoder.h"
#include "parser.h"

class InternetConnection : public QObject
{
    Q_OBJECT
public:
    /* comment demonstrates the state of rework */
    InternetConnection();   // done for now?
    ~InternetConnection();  // ?
    void setMode(int connection_type);  // OK
    int connect(const QString &server_ip, const QString &port);
    int disconnect();
    int login  (const QString &login, const QString &password);
    int logout();
    int is_authorized();
    int load_all_mailbox       (const QString &path_file);
    int select_mailbox         (const QString &name);
    int load_all_mail          (const QString &path_file);
    int load_all_mail_headers  (const QString &path_file);
    int load_mail              (const QString &UID, const QString &path_file);
    int delete_mail         (const QString &UID);
    int set_mail_read       (const QString &UID);
    int set_mail_unread     (const QString &UID);
    int send_message(const QString &mail_from, const QString &rcpt_to, const QString &message);
    QString load_list_of_mailboxes();
    static QString try_to_detect_server_ip (const QString &domain, const QString &mode);
private:
    void SendRequest(const QString &command);
    int LoadAnswer(const QString &filename);
    QString LoadAnswer();
    int initialize_OpenSSL();
    int initialize_socket();
    BIO *bio;
    SSL *ssl;
    SSL_CTX *ctx;
    char buf[1024];
    unsigned int len = 1023;
    int is_auth = 0;
    QString server;
    enum ModeSpec {imap, smtp};
    int mode;
    QString UID_validity;   // usused of not working
private slots:
    void mail_updater();
signals:
    void mail_updater_trigger();
};

#endif // INTERNETCONNECTION_H
