#include "messagesend.h"
#include "ui_messagesend.h"

MessageSend::MessageSend(QWidget *parent, Account* a) :
    QWidget(parent),
    ui(new Ui::MessageSend)
{
    A = a;
    A->smtp_login();

    ui->setupUi(this);
    //ui->ToEdit->setText(Decoder::UTF8toBase64(ui->ToEdit->toPlainText()));
    ui->MessageEdit->setText(Decoder::Base64ToUTF8(ui->MessageEdit->toPlainText()));

}

MessageSend::~MessageSend()
{
    A->smtp->disconnect();
    delete ui;
}

void MessageSend::resizeEvent ( QResizeEvent * event )
{
    Q_UNUSED(event)
    ui->ToEdit->setFixedWidth(width() - ui->ToEdit->x() - 5);
    ui->SubjectEdit->setFixedWidth(width() - ui->SubjectEdit->x() - 5);
    ui->MessageEdit->setFixedWidth(width() - ui->MessageEdit->x());
    ui->MessageEdit->setFixedHeight(height() - 124);
    ui->frame->resize(width(), ui->frame->height());
}

void MessageSend::on_SendButton_clicked()
{
    remove("file.txt");
    MIME_Message M;
    M.set_subject(ui->SubjectEdit->toPlainText());
    M.set_text_plain(ui->MessageEdit->toPlainText());
    // !!!!!!!!!!
    // toHTML() if no <html>
    M.set_text_html(ui->MessageEdit->toPlainText());
    //qDebug() << M.message();
    A->smtp->send_message(A->login_with_domain(), ui->ToEdit->toPlainText(), "M.message()");
}
