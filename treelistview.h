#ifndef TREELISTVIEW_H
#define TREELISTVIEW_H
#include <QAbstractItemModel>
#include "internetconnection.h"
#include <QFont>

class TreeModel : public QAbstractItemModel
{
private:
    InternetConnection *IC;
    QString mail_box_name;
public:
    struct m_data
    {
      m_data* parent;
      QList<m_data> children;

      QString name;          // displayble
      QString parent_name;   //
      QString real_name;     // used by IMAP commands
      QString flags;
    };

    QList<m_data> nodes; // root
    TreeModel()
    {
        fetch();
    }
    TreeModel(QWidget *parent, InternetConnection *ic, QString mbn) : IC(ic), mail_box_name(mbn)
    {
        fetch();
    }
    void fetch()
    {
        IC->load_all_mailbox("mailboxes.txt");
        QFile mailboxes("mailboxes.txt");
        mailboxes.open(QFile::ReadOnly);
        QTextStream file(&mailboxes);

        QString bufer;
        QString flags;
        QString name;
        QString parent_name;

        bufer = file.readLine();
        while (!bufer.contains(". OK", Qt::CaseInsensitive))
        {
            if (bufer.indexOf("(") != -1 && bufer.lastIndexOf(")") != -1)
            {
                flags = bufer.mid(
                        bufer.indexOf("(") + 1,
                        bufer.lastIndexOf(")") - bufer.indexOf("(") - 1);
            }
            else
            {
                flags.clear();
            }

            int t = bufer.lastIndexOf("\"");
            int k = 0;
            int k1 = -1;
            while (k1 != t)
            {
                k = k1 + 1;
                k1 = bufer.indexOf("\"", k);
            }

            qDebug() << bufer.mid(k, t - k) << t << k << k1;

            bool has_children = false;
            bool is_children  = false;
            if (!flags.isEmpty())
            {

                if (flags.contains("\\HasNoChildren", Qt::CaseInsensitive))
                {
                    flags.replace("\\HasNoChildren", "");
                    has_children = false;
                }
                if (flags.contains("\\HasChildren", Qt::CaseInsensitive))
                {
                    flags.replace("\\HasChildren", "");
                    has_children = true;
                }
                //qDebug() << flags << ((has_children) ? " has children" : "with no children");
            }

            /* parse for parent */
            name = bufer.mid(k, t - k);

            if (true) /////////////
            {
                if (name.indexOf("/") == -1)
                {
                    m_data m;
                    m.name = name;
                    m.parent_name = "/";
                    m.real_name = name;
                    m.flags = flags;
                    m.parent = nullptr;
                    nodes.append(m);
                }
                else
                {
                    //is_children = true;
                    int k = 0; int k1 = name.indexOf("/");
                    QString catalog = name.mid(k,k1 - k);
                    qDebug() << "CATALOG: " << catalog;
                    k = k1 + 1;

                    m_data *current_element;
                    //current_element->parent = nullptr;

                    int z = 0;
                    for (int i = 0; i < nodes.size(); i++)
                    {
                        if (nodes[i].name.contains(catalog))
                        {
                            //current_element = &nodes[i];
                            //current_element->parent = nodes[i].parent;
                            z = i;
                            //qDebug() << "SIZE: " << current_element->children.size() << " VS " << nodes[i].children.size();
                        }
                    }

                    //m_data m;
                    //m.name = name;
                    //m.parent = new_child;

                   /*
                    while ((k1 = name.indexOf("/", k)) != -1)
                    {
                         catalog = name.mid(k,k1 - k);
                         qDebug() << "CATALOG: " << catalog;
                         for (int i = 0; i < current_element.children.size(); i++)
                         {
                             qDebug() << i << ": " << &current_element.children.at(i).name;
                             //if (current_element->children[i].name.contains(catalog))
                             //{
                               //  qDebug() << "ok";
                                 //m.parent = &new_child->children[i];
                                 //m_data *temp;
                                 //temp = &new_child->children[i];
                                 //new_child = temp;
                                 //m_data temp = new_child->children[i];
                                 //new_child = &new_child->children[i];
                                 //new_child->parent = &temp;
                                 //new_child->parent->children.append(temp);
                                 //qDebug() << "new_child->parent->name == " << new_child->parent->name;
                             //}
                         }
                         k = k1 + 1;
                    }
                    */

                    m_data m;
                    m.parent = &nodes[z];
                    flags.replace(" ", "");
                    flags.replace("\\", "");
                    m.name = flags;
                    m.flags = flags;
                    m.real_name = name;
                    //if (current_element->parent == nullptr)
                    //{
                       // nodes[z].children.append(*current_element);
                    //}
                    //qDebug()<< "current_elem: " << current_element;
                    //qDebug() << &nodes[z];
                    nodes[z].children.append(m);
                    //current_element->parent->children.append(m);


                    //qDebug() << "!!!! : " << m.parent->name << ", " << m.name;
                    //new_child->children.append(m);
                }
            }
                bufer = file.readLine();
        }
    }
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override
    {
        if (!hasIndex(row, column, parent))
        {
            return QModelIndex();
        }
        if (!parent.isValid())
        {
            return createIndex(row, column, const_cast<m_data*>(&nodes[row]));
        }
        m_data* parent_info = static_cast<m_data*>(parent.internalPointer());
        Q_ASSERT(parent_info->children.size() > row);
        return createIndex(row, column, &parent_info->children[row]);
    }
    QModelIndex parent(const QModelIndex &child) const override
    {
        if (!child.isValid()) {
            return QModelIndex();
        }
        m_data* child_info = static_cast<m_data*>(child.internalPointer());
        m_data* parent_info = child_info->parent;
        if (parent_info != 0)
        {
            return createIndex(findRow(parent_info), 0, parent_info);
        }
        else
        {
            return QModelIndex();
        }
    }
    int findRow(m_data *node) const
    {
        //return nodes.size();
        //const QList<m_data> parent_info_children = (node->parent != 0) ? node->parent->children : nodes;
        //QList<m_data>::const_iterator position = qFind(parent_info_children, *node);
        //return std::distance(parent_info_children.begin(), position);
        return 0;
    }
    int rowCount(const QModelIndex &parent = QModelIndex()) const override
    {
        if (!parent.isValid())
        {
            return nodes.size();
        }
        const m_data* parent_info = static_cast<const m_data*>(parent.internalPointer());
        Q_ASSERT(parent_info != 0);
        return parent_info->children.size();
    }
    int columnCount(const QModelIndex &parent = QModelIndex()) const override
    {
        return 1;
    }
    QVariant data(const QModelIndex &index, int role) const override
    {
        if (!index.isValid())
        {
            return QVariant();
        }
        const m_data* node = static_cast<m_data*>(index.internalPointer());
        if (node == nullptr)
        {return QVariant();}
        if (role == Qt::DisplayRole)
            return node->name;
        return QVariant();
    }
    QVariant headerData(const int column, Qt::Orientation orientaion, const int role) const override
    {
        if (role == Qt::FontRole)
        {
            QFont font;
            font.setPointSize(10);
            font.setFamily("Arial");
            font.setBold(true);
            return font;
        }
        if (role != Qt::DisplayRole)
            return QVariant();
        if (orientaion == Qt::Horizontal)
        {
            if (column == 0)
                return mail_box_name;
        }
        return QVariant();
    }
    QString select_mailbox(const QModelIndex &index) const
    {
        //qDebug() << index.parent().n
        m_data* info = static_cast<m_data*>(index.internalPointer());
        qDebug() << info->real_name;
        return (info->flags.contains("\\Noselect")) ?  "\\Noselect" : ("\"" + info->real_name + "\"");
    }
};

#endif // TREELISTVIEW_H
