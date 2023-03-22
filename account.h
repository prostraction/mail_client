#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <QObject>
#include "internetconnection.h"

class Account
{
public:
    Account();
    ~Account();
    InternetConnection* imap_foreground;
    InternetConnection* imap_background;
    InternetConnection* smtp;

    int run();
    int smtp_login();

    void set_login(QString log) {_login = log.left(log.indexOf("@"));}
    void set_password(QString pass) {_password = pass;}
    void set_login_with_domain(QString log_w_d) {_login_with_domain = log_w_d;}

    QString login_with_domain() {return _login_with_domain;}
private:
    QString _login;
    QString _password;
    QString _login_with_domain;

    QString ip_imap;
    QString port_imap;

    QString ip_smtp;
    QString port_smtp;

    QThread* thread;

    bool smtp_configured;
};

#endif // ACCOUNT_H
