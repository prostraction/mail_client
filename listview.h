#ifndef MAILVIEW_H
#define MAILVIEW_H

#include <QObject>
#include <QAbstractTableModel>
#include <QtGui>
#include <QtCore>
#include <QTextStream>
#include <QTime>
#include "internetconnection.h"

#include <fstream>
#include "parser.h"





class ListView : public QAbstractTableModel
{
    Q_OBJECT
//////////////////////////////////////////////////////////////////////////////
public:
    struct data_table
    {
        QString subject;
        QString from;
        QString date;
        QString UID;
        QString flags;
    };
    ListView(QObject *parent = nullptr, InternetConnection *IC = nullptr);
    ~ListView() override;
//////////////////////////////////////////////////////////////////////////////
    virtual int rowCount    (const QModelIndex &parent) const override;
    virtual int columnCount (const QModelIndex &parent) const override;
    virtual QVariant data   (const QModelIndex &index, int role) const override;
    virtual QVariant headerData (const int column, Qt::Orientation orientaion, const int role) const override;
    void sort(int column, Qt::SortOrder order) override;
//////////////////////////////////////////////////////////////////////////////
    void insert(int row, const QString &subject, const QString &from, const QString &date, const QString &UID, const QString &flags);
    void remove(int index);
    void clear();
    int  count() const;
//////////////////////////////////////////////////////////////////////////////
    void load(QString); 
//////////////////////////////////////////////////////////////////////////////
    QString get_data(const int &row, const int &column);
    int UID(const int &row);
    QString from(const int& row);
    QString subject(const int& row);
//////////////////////////////////////////////////////////////////////////////
    void set_mail_read(const QModelIndex &index);
    void set_mail_unread(const QModelIndex &index);
    void new_message();
//////////////////////////////////////////////////////////////////////////////
signals:
    void countChanged(int arg);
//////////////////////////////////////////////////////////////////////////////
private:
    QList<data_table> m_data;
    InternetConnection *IC;
    int m_count;
    QString current_mailbox;

};
#endif // MAILVIEW_H
