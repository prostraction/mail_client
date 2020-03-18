#include "listview.h"

ListView::ListView(QObject *parent, InternetConnection *ic) : IC(ic)
{
    load("INBOX");
}


ListView::~ListView()
{
    clear();
}


int ListView::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_data.count();
}

int ListView::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return 3;
}

QVariant ListView::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() > m_data.count())
        return QVariant();

    if (role == Qt::DisplayRole)
    {
        if (index.column() == 0)
            return m_data.at(index.row()).subject;
        else if (index.column() == 1)
            return m_data.at(index.row()).from;
        else if (index.column() == 2)
            return m_data.at(index.row()).date;
    }
    if (role == Qt::FontRole)
    {
        if (!m_data.at(index.row()).flags.contains("Seen"))
        {
            QFont font;
            font.setBold(true);
            return font;
        }
        if (!m_data.at(index.row()).flags.contains("Junk"))
        {
            QFont font;
            font.setItalic(true);
            return font;
        }
    }
    return QVariant();
}

QVariant ListView::headerData(const int column, Qt::Orientation orientaion, const int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();
    if (orientaion == Qt::Horizontal)
    {
        if (column == 0)
            return "Subject";
        if (column == 1)
            return "From";
        if (column == 2)
            return "Date";
    }
    return QVariant();
}

void ListView::sort(int column, Qt::SortOrder order)
{
    layoutAboutToBeChanged();
    switch (column)
    {
    case 0:
    {
        if (order == Qt::AscendingOrder)
        {
             std::sort(m_data.begin(), m_data.end(), [] (const data_table& l, const data_table &r)
             {return l.subject < r.subject;});
        }
        else
        {
            std::sort(m_data.begin(), m_data.end(), [] (const data_table& l, const data_table &r)
            {return l.subject > r.subject;});
        }
        break;
    }
    case 1:
    {
        if (order == Qt::AscendingOrder)
        {
            std::sort(m_data.begin(), m_data.end(), [] (const data_table& l, const data_table &r)
            {return l.from < r.from;});
        }
        else
        {
            std::sort(m_data.begin(), m_data.end(),[] (const data_table& l, const data_table &r)
            {return l.from > r.from;});
        }
        break;
    }
    case 2:
    {
        (order == Qt::AscendingOrder) ?
            std::sort(m_data.begin(), m_data.end(),
                [] (const data_table& l, const data_table &r)
            {return ((l.date.mid(6, 4) + l.date.mid(3, 2) + l.date.left(2) + l.date.mid(10, 8))
                              < (r.date.mid(6, 4) + r.date.mid(3, 2) + r.date.left(2) + r.date.mid(10, 8))); })
         : std::sort(m_data.begin(), m_data.end(),
            [] (const data_table& l, const data_table &r)
                    { return ((l.date.mid(6, 4) + l.date.mid(3, 2) + l.date.left(2) + l.date.mid(10, 8))
                              > (r.date.mid(6, 4) + r.date.mid(3, 2) + r.date.left(2) + r.date.mid(10, 8)));});
        break;
    }
    default:
        break;
    }
    layoutChanged();
}

QString ListView::get_data(const int &row, const int &column)
{
    if (column == 0)
        return m_data.at(row).subject;
    else if (column == 1)
        return m_data.at(row).from;
    else if (column == 2)
        return m_data.at(row).date;
    else
        return "";
}

void ListView::insert(int row, const QString &subject, const QString &from, const QString &date, const QString &UID, const QString &flags)
{
    beginInsertRows(QModelIndex(), row, row);
    m_data.insert(row, {subject, from, date, UID, flags});
    endInsertRows();
    countChanged(m_data.count());
}

void ListView::remove(int index)
{
    if (index < 0 || index > m_data.count())
        return;
    IC->delete_mail(m_data.at(index).UID);
    beginRemoveRows(QModelIndex(), index, index);
    m_data.removeAt(index);
    endRemoveRows();
    countChanged(m_data.count());
}
void ListView::clear()
{
    beginResetModel();
    m_data.clear();
    endResetModel();
}

int ListView::count() const
{
    return rowCount(QModelIndex());
}

int ListView::UID(const int &row)
{
    return (m_data.at(row).UID).toInt();
}

QString ListView::from(const int &row)
{
    return (m_data.at(row).from);
}

QString ListView::subject(const int &row)
{
    return (m_data.at(row).subject);
}

void ListView::set_mail_read(const QModelIndex &index)
{
    if (index.row() < 0 || index.row() > m_data.count() || m_data.at(index.row()).flags.contains("Seen"))
        return;
    m_data[index.row()].flags += "\\Seen";
    IC->set_mail_read(m_data.at(index.row()).UID);
}

void ListView::set_mail_unread(const QModelIndex &index)
{
    if (index.row() < 0 || index.row() > m_data.count() || !m_data.at(index.row()).flags.contains("Seen"))
        return;
    m_data[index.row()].flags.replace("\\Seen", "");
    IC->set_mail_unread(m_data.at(index.row()).UID);
}

void ListView::load(QString mailbox)
{
    current_mailbox = mailbox;
    data_table D;
    QString buf_string;
    QString message;

    if (mailbox == "\\Noselect")
        return ;
    IC->select_mailbox(mailbox);
    IC->load_all_mail_headers("messages");

    QFile downloaded_messages("messages");
    if (!downloaded_messages.open(QFile::ReadOnly | QFile::Text))
    {
       return;
    }
    QTextStream IF(&downloaded_messages);

    while (!IF.atEnd())
    {
        buf_string = IF.readLine();
        if (buf_string.contains(". OK"))  break;
        if (buf_string.contains("FETCH", Qt::CaseInsensitive))
        {
            do
            {
                buf_string += "\r\n";
                message += buf_string;
                buf_string = IF.readLine();

            } while (buf_string.indexOf(")") != 0 && !buf_string.contains("FLAGS"));
            /* some of messages probably do NOT contain "FLAGS" */
            if (buf_string.contains("FLAGS"))
            {
                buf_string += "\r\n";
                message += buf_string;
            }
            Parser::ParseHeaders(message, D.subject, D.from, D.date, D.UID, D.flags);
            insert(this->count(), D.subject, D.from, D.date, D.UID, D.flags);
            message.clear();
        }
    }

    qDebug() << this->count();
    downloaded_messages.close();

    for (int k = 0, s = m_data.size(), max = (s / 2); k < max; k++)
        m_data.swapItemsAt(k, s - (1 + k));
}


void ListView::new_message()
{
    if (current_mailbox.contains("inbox", Qt::CaseInsensitive))
    {
        clear();
        load(current_mailbox);
    }
}
