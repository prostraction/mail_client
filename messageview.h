#ifndef MESSAGEVIEW_H
#define MESSAGEVIEW_H

#include <QWidget>
#include <fstream>
#include "internetconnection.h"
#include "parser.h"
#include "directory.h"
#include "mimemessage.h"
//#include <QWebEngineView>


namespace Ui {
class MessageView;
}

class MessageView : public QWidget
{
    Q_OBJECT
public:
    MessageView(QWidget *parent, InternetConnection *ic, int number);
    void set_params(QString from, QString subj);
    ~MessageView();

private slots:
    void paintEvent(QPaintEvent *event);
private:
    Ui::MessageView *ui;
    InternetConnection *IC;
    //QWebEngineView *webview;
    MimeMessage* M;
    QUrl file;
    int number;
};

#endif // MESSAGEVIEW_H
