#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <QMainWindow>

#include <QWidget>
#include "internetconnection.h"
#include "listview.h"
#include "messageview.h"
#include "directory.h"
#include "treelistview.h"
#include "messagesend.h"
#include "account.h"
#include "AutorisationRequest.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:  
    void resizeEvent ( QResizeEvent * event );

    void on_tabWidget_tabCloseRequested(int index);
    void on_tableView_doubleClicked(const QModelIndex &index);
    void on_tableView_customContextMenuRequested(const QPoint &pos);

    void slot_delete_message();
    void slot_set_unread();
    void slot_set_read();

    void on_treeView_doubleClicked(const QModelIndex &index);

    void modelSort(int sec);

    void on_MessageSendButton_clicked();

    void update_required();

private:
    Ui::MainWindow *ui;
    Account *A;
    AutorisationRequest *AR;
    ListView *LV;
    //InternetConnection *IC;
    TreeModel *TVM;

};
#endif // MAINWINDOW_H
