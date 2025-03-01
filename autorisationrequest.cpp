#include "autorisationrequest.h"
#include "ui_autorisationrequest.h"

AutorisationRequest::AutorisationRequest(QWidget *parent, Account *a) : QDialog(parent), ui(new Ui::AutorisationRequest), A(a)
{
    //A = a;
    Qt::WindowFlags flags = Qt::Window;
    setWindowFlags(flags);
    not_expanded = true;
    not_expanded_settings = true;
    //this->setGeometry(this->x(), this->y(), this->width(), this->height());
    ui->setupUi(this);
    QFile logindata("userpass");
    if (logindata.exists())
    {
        logindata.open(QFile::ReadOnly);
        QTextStream Q(&logindata);
        QString bufer = Q.readLine();
        ui->login_edit->setText(bufer);
        bufer = Q.readLine();
        ui->password_edit->setText(bufer);
        logindata.close();
        ui->remember_me_checkbox->setChecked(true);
        ui->connect_button->pressed();
    }
}

AutorisationRequest::~AutorisationRequest()
{
    delete A;
    delete ui;
}

void AutorisationRequest::on_connect_button_clicked()
{
    if (ui->remember_me_checkbox->isChecked())
    {
        QFile logindata("userpass");
        logindata.open(QFile::WriteOnly);
        QTextStream Q(&logindata);
        QString bufer = Q.readLine();
        Q << ui->login_edit->text() << "\r\n";
        Q << ui->password_edit->text() << "\r\n";
        logindata.close();
    }
    if (!ui->login_edit->text().isEmpty() && !ui->password_edit->text().isEmpty())
    {
        if (!not_expanded)
        {
            this->setGeometry(this->x() + 2, this->y() + 32, this->width(), this->height() - 30);
            not_expanded = true;
        }
        if (!not_expanded_settings)
        {
            this->setGeometry(this->x() + 2, this->y() + 32, this->width(), this->height() - 140);
            not_expanded_settings = true;
        }
        A->set_login_with_domain(ui->login_edit->text());
        A->set_login(ui->login_edit->text());
        A->set_password(ui->password_edit->text());
        int error = A->run();
        if (error == 0)
        {
            if (not_expanded)
            {
                this->setGeometry(this->x(), this->y() + 30, this->width(), this->height() + 30);
                not_expanded = false;
            }
            ui->error_label->setText("Your login or password is incorrect.");
        }
        else if (error == -1)
        {
            if (not_expanded_settings)
            {
                this->setGeometry(this->x(), this->y() + 30, this->width(), this->height() + 140);
                not_expanded_settings = false;
            }
            ui->error_label->setText("Program can't define the server IP. Please specify it.");
        }
        else if (error == -2)
        {
            if (not_expanded_settings)
            {
                this->setGeometry(this->x(), this->y() + 30, this->width(), this->height() + 140);
                not_expanded_settings = false;
            }
            ui->error_label->setText("The server uses unusual port, which program can't define. Please specify it.");
        }
        else
        {
            close();
        }
    }
}
