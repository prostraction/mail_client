#ifndef AUTORISATIONREQUEST_H
#define AUTORISATIONREQUEST_H

#include <QDialog>
#include "account.h"

namespace Ui {
class AutorisationRequest;
}

class AutorisationRequest : public QDialog
{
    Q_OBJECT

public:
    explicit AutorisationRequest(QWidget *parent = nullptr, Account *A = nullptr);
    ~AutorisationRequest();

private slots:
    void on_connect_button_clicked();

private:
    Ui::AutorisationRequest *ui;
    Account *A;
    bool not_expanded;
    bool not_expanded_settings;
};

#endif // AUTORISATIONREQUEST_H
