#include "contacttablemodel.h"
#include <QIcon>
#include <QPixmap>

ContactTableModel::ContactTableModel(ContactManager* contactManager, QObject* parent)
    : QAbstractTableModel(parent)
    , m_contactManager(contactManager)
{
    refresh();
}

int ContactTableModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return m_contacts.size();
}

int ContactTableModel::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return 5; // Ім'я, Прізвище, Телефон, Email, Дата народження
}

QVariant ContactTableModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() >= m_contacts.size())
        return QVariant();

    const Contact& contact = m_contacts.at(index.row());

    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        switch (index.column()) {
        case 0: return contact.firstName();
        case 1: return contact.lastName();
        case 2: return contact.phone();
        case 3: return contact.email();
        case 4: return contact.birthday().toString("dd.MM.yyyy");
        }
    }

    if (role == Qt::DecorationRole && index.column() == 0) {
        if (!contact.photoPath().isEmpty()) {
            QPixmap photo = contact.photo();
            if (!photo.isNull()) {
                return photo.scaled(24, 24, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            }
        }
    }

    if (role == Qt::ToolTipRole) {
        QString tooltip = QString("<b>%1 %2</b><br>"
                                  "<b>Телефон:</b> %3<br>"
                                  "<b>Email:</b> %4<br>"
                                  "<b>Адреса:</b> %5<br>"
                                  "<b>Примітки:</b> %6")
                              .arg(contact.firstName())
                              .arg(contact.lastName())
                              .arg(contact.phone())
                              .arg(contact.email())
                              .arg(contact.address())
                              .arg(contact.notes());
        return tooltip;
    }

    if (role == Qt::UserRole) {
        return contact.id();
    }

    return QVariant();
}

QVariant ContactTableModel::headerData(int section, Qt::Orientation orientation,
                                       int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {
        switch (section) {
        case 0: return "Ім'я";
        case 1: return "Прізвище";
        case 2: return "Телефон";
        case 3: return "Email";
        case 4: return "Дата народження";
        }
    }

    return QVariant();
}

void ContactTableModel::sort(int column, Qt::SortOrder order)
{
    beginResetModel();

    std::sort(m_contacts.begin(), m_contacts.end(), [column, order](const Contact& a, const Contact& b) {
        bool result = false;

        switch (column) {
        case 0: // Ім'я
            result = a.firstName() < b.firstName();
            break;
        case 1: // Прізвище
            result = a.lastName() < b.lastName();
            break;
        case 2: // Телефон
            result = a.phone() < b.phone();
            break;
        case 3: // Email
            result = a.email() < b.email();
            break;
        case 4: // Дата народження
            result = a.birthday() < b.birthday();
            break;
        }

        return order == Qt::AscendingOrder ? result : !result;
    });

    endResetModel();
}

void ContactTableModel::refresh()
{
    beginResetModel();

    if (m_contactManager) {
        m_contacts = m_contactManager->getAllContacts();
    } else {
        m_contacts.clear();
    }

    endResetModel();
}

Contact ContactTableModel::getContactAt(int row) const
{
    if (row >= 0 && row < m_contacts.size()) {
        return m_contacts.at(row);
    }
    return Contact();
}
