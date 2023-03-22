#include "account.h"

Account::Account()
{
    imap_foreground = new InternetConnection;
    imap_background = new InternetConnection;
    smtp = new InternetConnection;
    imap_foreground->setMode(0); 
    imap_background->setMode(0);
    smtp->setMode(1);
    smtp_configured = false; // ?????????
}

Account::~Account()
{
    thread->exit();
    delete imap_foreground;
    delete smtp;
}

int Account::run()
{
    imap_foreground->set_login_with_domain(login_with_domain());
    imap_foreground->set_password(_password);
    imap_background->set_login_with_domain(login_with_domain());
    imap_background->set_password(_password);
    smtp->set_login_with_domain(login_with_domain());
    smtp->set_password(_password);

    QThread *thread_smtp = new QThread;
    QThread *thread_imap_foreground = new QThread;
    QThread *thread_imap_background = new QThread;

    imap_foreground->moveToThread(thread_imap_foreground);
    imap_background->moveToThread(thread_imap_background);
    smtp->moveToThread(thread_smtp);


    QObject::connect(thread_imap_foreground, SIGNAL(started()), imap_foreground, SLOT(auth()));
    QObject::connect(thread_smtp, SIGNAL(started()), smtp, SLOT(auth()));
    QObject::connect(thread_imap_background, SIGNAL(started()), imap_background, SLOT(auth()));
    QObject::connect(thread_imap_background, SIGNAL(started()), imap_background, SLOT(mail_updater()));

    thread_imap_foreground->start();
    thread_imap_background->start();
    thread_smtp->start();

    qDebug() << "log ended";
    Sleep(4000);


    return 1;
}

int Account::smtp_login()
{
    if (smtp->connect(ip_smtp, port_smtp) <= 0) return 0;
    if (smtp->login(_login_with_domain, _password) <= 0) return 0;
    else
        return 1;
}
