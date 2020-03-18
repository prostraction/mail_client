#include "messageview.h"
#include "ui_messageview.h"

MessageView::MessageView(QWidget *parent, InternetConnection *ic, int num) : QWidget(parent), ui(new Ui::MessageView), number(num)
{
    IC = ic;
    IC->load_mail(QString::number(number), QString::number(number));
    M = new MimeMessage;
    bool text_only = true;
    M = Parser::ParseMail(QString::number(number));
    file.setScheme("file://");
    QList <std::pair <QString, QString> > files;
    files = M->display();
    auto it = files.begin();
    for (int i = 0; it != files.end(); ++it, i++)
    {
        if (files.at(i).first.contains("html"))
        {
            file = Directory::GetCurrentDirectory_() + "/" + files.at(i).second;
            text_only = false;
        }
    }
    if (text_only)
    {
        it = files.begin();
        for (int i = 0; it != files.end(); ++it, i++)
        {
            if (files.at(i).first.contains("plain"))
            {
                file = Directory::GetCurrentDirectory_() + "/" + files.at(i).second;
            }
        }
    }
    ui->setupUi(this);
    webview = new QWebEngineView(ui->frame);
    webview->load(file);
}

void MessageView::set_params(QString from, QString subj)
{
    ui->from_edit->setText(from);
    ui->subject_edit->setText(subj);
}

MessageView::~MessageView()
{
    webview->close();
    remove(QString::number(number).toStdString().c_str());
    delete webview;
    delete M;
    delete ui;
}

void MessageView::paintEvent(QPaintEvent *event)
{
    ui->frame->resize(width(), height() - 30);
    ui->frame_info->resize(width(), ui->frame_info->height());
    /* fixing the QT bag */
    webview->resize(ui->frame->size());
}
