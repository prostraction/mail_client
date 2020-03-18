#ifndef MESSAGESEND_H
#define MESSAGESEND_H

#include <QWidget>
#include "decoder.h"
#include "internetconnection.h"
#include "mime_message.h"
#include "account.h"

namespace Ui {
class MessageSend;
}

class MessageSend : public QWidget
{
    Q_OBJECT

public:
    explicit MessageSend(QWidget *parent = nullptr, Account *a = nullptr);
    ~MessageSend();
private slots:
    void resizeEvent ( QResizeEvent * event );
    void on_SendButton_clicked();

private:
    Ui::MessageSend *ui;
    Account *A;
};

#endif // MESSAGESEND_H
