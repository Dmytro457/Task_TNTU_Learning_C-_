#ifndef CONTACTTABLEMODEL_H
#define CONTACTTABLEMODEL_H

#include "contactmanager.h"
#include <QAbstractTableModel>

class ContactTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit ContactTableModel(ContactManager* contactManager, QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;
    void sort(int column, Qt::SortOrder order = Qt::AscendingOrder) override;

    void refresh();
    Contact getContactAt(int row) const;

private:
    ContactManager* m_contactManager;
    QList<Contact> m_contacts;
};

#endif
