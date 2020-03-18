#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "treelistview.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->resize(1280, 720);
    this->setWindowTitle("Mail client.");


    A = new Account;
    AR = new AutorisationRequest(this, A);
    AR->exec();

    LV = new ListView(this, A->imap_foreground);
    ui->tableView->setModel(LV);

    ui->tableView->horizontalHeader()->setVisible(true);

    for (int i = 0; i < LV->count(); i++)
    { ui->tableView->setRowHeight(i, 10); }

    ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tableView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slotCustomMenuRequested()));
    ui->tableView->setColumnWidth(0, width() - 5 - 250 - 450 - 30);
    ui->tableView->setColumnWidth(1, 300);
    ui->tableView->setColumnWidth(2, 150);

    TVM = new TreeModel(this, A->imap_foreground, A->login_with_domain());
    ui->treeView->setModel(TVM);
    ui->treeView->expandAll();

    connect(ui->tableView->horizontalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(modelSort(int)));
    connect(A->imap_background, SIGNAL(mail_updater_trigger()), this, SLOT(update_required()));

}

MainWindow::~MainWindow()
{
    delete AR;
    delete LV;
    delete TVM;
    delete ui;
}


void MainWindow::on_tableView_doubleClicked(const QModelIndex &index)
{
    ui->statusbar->showMessage(QString::number(index.row()) + " clicked. " + QString::number(LV->UID(index.row())) + " called." );
    MessageView *MV = new MessageView(this, A->imap_foreground, LV->UID(index.row()));
    ui->tabWidget->addTab(MV, LV->get_data(index.row(), 0));
    MV->set_params(LV->from(index.row()), LV->subject(index.row()));
    MV->show();
    LV->set_mail_read(index);
}

void MainWindow::modelSort(int section)
{
    LV->sort(section, ui->tableView->horizontalHeader()->sortIndicatorOrder());
}

void MainWindow::resizeEvent ( QResizeEvent * event )
{
    ui->tableView->setColumnWidth(0, width() - 5 - 250 - ui->tableView->columnWidth(1) - ui->tableView->columnWidth(2) - 30);
    ui->tableView->resize(width() - 5 - 250, height() - 55);
    ui->treeView->resize(ui->treeView->width(), height());

}

void MainWindow::on_tabWidget_tabCloseRequested(int index)
{
    int save_current_index = ui->tabWidget->currentIndex();
    ui->tabWidget->setCurrentIndex(index);
    delete ui->tabWidget->currentWidget();
    ui->tabWidget->setCurrentIndex(save_current_index);

    /* removeTab does not remove content of Tab (documentation) and
        calls automaticlly in destructor of currentWidget() (observation).
       The line below will close (but not delete) next tab before current tab.
       This code remains to prevent this mistake. */
    //ui->tabWidget->removeTab(index);
}

void MainWindow::on_tableView_customContextMenuRequested(const QPoint &pos)
{
    QMenu *menu = new QMenu(this);

    QAction *delete_message = new QAction(("Delete"), this);
    QAction *set_unread = new QAction(("Set unread"), this);
    QAction *set_read   = new QAction (("Set read"), this);

    connect(delete_message, SIGNAL(triggered()), this, SLOT(slot_delete_message()));
    connect(set_unread,     SIGNAL(triggered()), this, SLOT(slot_set_unread()));
    connect(set_read,       SIGNAL(triggered()), this, SLOT(slot_set_read()));

    menu->addAction(delete_message);
    menu->addAction(set_unread);
    menu->addAction(set_read);

    /* pos returns position     (of menu) of the screen.
       new_pos returns position (of menu) of the program. */
    QPoint new_pos;
    new_pos.setX(this->x() + pos.x() + 5 + 250); // 250 is treeView
    new_pos.setY(this->y() + pos.y() + 92);

    menu->popup(ui->tableView->viewport()->mapToGlobal(pos));
    menu->exec(new_pos);

    delete delete_message;
    delete set_unread;
    delete set_read;
    delete menu;
}

void MainWindow::slot_delete_message()
{
    LV->remove(ui->tableView->selectionModel()->currentIndex().row());
}
void MainWindow::slot_set_unread()
{
    LV->set_mail_unread(ui->tableView->selectionModel()->currentIndex());
}
void MainWindow::slot_set_read()
{
    LV->set_mail_read(ui->tableView->selectionModel()->currentIndex());
}

void MainWindow::on_treeView_doubleClicked(const QModelIndex &index)
{
     LV->clear();
     LV->load(TVM->select_mailbox(index));

     for (int i = 0; i < LV->count(); i++)
     { ui->tableView->setRowHeight(i, 10); }
}

void MainWindow::on_MessageSendButton_clicked()
{
    MessageSend *MS = new MessageSend(this, A);
    ui->tabWidget->addTab(MS, "New message: ");
    MS->show();
}

void MainWindow::update_required()
{
    LV->new_message();
    for (int i = 0; i < LV->count(); i++)
    { ui->tableView->setRowHeight(i, 10); }
}
