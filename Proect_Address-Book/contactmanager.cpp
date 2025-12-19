#include "contactmanager.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QDir>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QFileInfo>
#include <algorithm>

ContactManager::ContactManager(QObject *parent)
    : QObject(parent)
{
    m_dataDir = "data";
    m_photosDir = m_dataDir + "/photos";
    ensureDirectories();
}

void ContactManager::ensureDirectories()
{
    QDir dir;
    if (!dir.exists(m_dataDir)) {
        dir.mkpath(m_dataDir);
    }
    if (!dir.exists(m_photosDir)) {
        dir.mkpath(m_photosDir);
    }
}

bool ContactManager::addContact(const Contact& contact)
{
    if (contact.isEmpty()) return false;

    QString id = contact.id();
    m_contacts.insert(id, contact);
    emit dataChanged();
    return true;
}

bool ContactManager::updateContact(const QString& id, const Contact& contact)
{
    if (!m_contacts.contains(id) || contact.isEmpty()) return false;

    m_contacts[id] = contact;
    emit dataChanged();
    return true;
}

bool ContactManager::removeContact(const QString& id)
{
    if (!m_contacts.contains(id)) return false;

    // Видаляємо фото контакта
    removePhoto(id);

    m_contacts.remove(id);
    emit dataChanged();
    return true;
}

Contact ContactManager::getContact(const QString& id) const
{
    return m_contacts.value(id, Contact());
}

QList<Contact> ContactManager::getAllContacts() const
{
    return m_contacts.values();
}

QList<Contact> ContactManager::search(const QString& keyword) const
{
    if (keyword.isEmpty()) {
        return m_contacts.values();
    }

    QList<Contact> results;
    QString keywordLower = keyword.toLower();

    for (const Contact& contact : m_contacts) {
        if (contact.firstName().toLower().contains(keywordLower) ||
            contact.lastName().toLower().contains(keywordLower) ||
            contact.phone().contains(keyword) ||
            contact.email().toLower().contains(keywordLower) ||
            contact.address().toLower().contains(keywordLower) ||
            contact.notes().toLower().contains(keywordLower)) {
            results.append(contact);
        }
    }

    return results;
}

void ContactManager::sortByName(bool ascending)
{
    QList<Contact> contacts = m_contacts.values();

    std::sort(contacts.begin(), contacts.end(), [ascending](const Contact& a, const Contact& b) {
        QString nameA = a.lastName() + a.firstName();
        QString nameB = b.lastName() + b.firstName();
        return ascending ? (nameA < nameB) : (nameA > nameB);
    });

    // Оновлюємо порядок (хоча в QMap це не зберігається)
    emit dataChanged();
}

void ContactManager::sortByDate(bool ascending)
{
    QList<Contact> contacts = m_contacts.values();

    std::sort(contacts.begin(), contacts.end(), [ascending](const Contact& a, const Contact& b) {
        return ascending ? (a.birthday() < b.birthday()) : (a.birthday() > b.birthday());
    });

    emit dataChanged();
}

bool ContactManager::exportToCSV(const QString& filename) const
{
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Cannot open file for writing:" << filename;
        return false;
    }

    QTextStream stream(&file);

// Для Qt 6 використовуємо QTextCodec або явно вказуємо кодування
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    stream.setCodec("UTF-8");
#else
    stream.setEncoding(QStringConverter::Utf8);
#endif

    // Заголовок
    stream << "ID,FirstName,LastName,Phone,Email,Address,Notes,Birthday,PhotoPath\n";

    // Дані
    for (const Contact& contact : m_contacts.values()) {
        stream << contact.toCSV() << "\n";
    }

    file.close();
    return true;
}

bool ContactManager::importFromCSV(const QString& filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Cannot open file for reading:" << filename;
        return false;
    }

    QTextStream stream(&file);

// Для Qt 6
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    stream.setCodec("UTF-8");
#else
    stream.setEncoding(QStringConverter::Utf8);
#endif

    // Пропускаємо заголовок
    if (!stream.atEnd()) {
        stream.readLine();
    }

    int importedCount = 0;
    while (!stream.atEnd()) {
        QString line = stream.readLine();
        QStringList fields;

        // Розбираємо CSV з урахуванням лапок
        QString field;
        bool inQuotes = false;

        for (int i = 0; i < line.length(); ++i) {
            QChar ch = line[i];

            if (ch == '"') {
                inQuotes = !inQuotes;
            } else if (ch == ',' && !inQuotes) {
                fields.append(field);
                field.clear();
            } else {
                field.append(ch);
            }
        }
        fields.append(field);

        if (fields.size() >= 9) {
            Contact contact;

            // Для Qt 6 потрібно обробити екрановані лапки
            for (int i = 0; i < fields.size(); ++i) {
                QString& field = fields[i];
                if (field.startsWith('"') && field.endsWith('"')) {
                    field = field.mid(1, field.length() - 2);
                    field.replace("\"\"", "\"");
                }
            }

            // Перше поле - ID, але ми генеруємо новий
            contact.setFirstName(fields[1]);
            contact.setLastName(fields[2]);
            contact.setPhone(fields[3]);
            contact.setEmail(fields[4]);
            contact.setAddress(fields[5]);
            contact.setNotes(fields[6]);

            QDate date = QDate::fromString(fields[7], "yyyy-MM-dd");
            if (date.isValid()) {
                contact.setBirthday(date);
            }

            contact.setPhotoPath(fields[8]);

            m_contacts.insert(contact.id(), contact);
            importedCount++;
        }
    }

    file.close();

    if (importedCount > 0) {
        emit dataChanged();
    }

    qDebug() << "Imported" << importedCount << "contacts from CSV";
    return importedCount > 0;
}

bool ContactManager::exportToJSON(const QString& filename) const
{
    QJsonArray contactsArray;

    for (const Contact& contact : m_contacts.values()) {
        contactsArray.append(contact.toJson());
    }

    QJsonObject root;
    root["contacts"] = contactsArray;
    root["count"] = m_contacts.size();

    QJsonDocument doc(root);

    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }

    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();
    return true;
}

bool ContactManager::importFromJSON(const QString& filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull()) {
        return false;
    }

    QJsonObject root = doc.object();
    QJsonArray contactsArray = root["contacts"].toArray();

    int importedCount = 0;
    for (const QJsonValue& value : contactsArray) {
        Contact contact = Contact::fromJson(value.toObject());
        m_contacts.insert(contact.id(), contact);
        importedCount++;
    }

    if (importedCount > 0) {
        emit dataChanged();
    }

    return importedCount > 0;
}

QString ContactManager::addPhoto(const QString& sourcePath, const QString& contactId)
{
    if (sourcePath.isEmpty() || contactId.isEmpty()) {
        return QString();
    }

    return copyPhotoToStorage(sourcePath, contactId);
}

bool ContactManager::removePhoto(const QString& contactId)
{
    QString photoPath = getPhotoPath(contactId);
    if (!photoPath.isEmpty() && QFile::exists(photoPath)) {
        return QFile::remove(photoPath);
    }
    return false;
}

QString ContactManager::getPhotoPath(const QString& contactId) const
{
    Contact contact = getContact(contactId);
    return contact.photoPath();
}

QString ContactManager::copyPhotoToStorage(const QString& sourcePath, const QString& contactId)
{
    QFileInfo sourceInfo(sourcePath);
    if (!sourceInfo.exists()) {
        return QString();
    }

    QString extension = sourceInfo.suffix().toLower();
    QString newFileName = contactId + "." + extension;
    QString newPath = m_photosDir + "/" + newFileName;

    // Видаляємо старе фото якщо існує
    if (QFile::exists(newPath)) {
        QFile::remove(newPath);
    }

    // Копіюємо нове фото
    if (QFile::copy(sourcePath, newPath)) {
        return newPath;
    }

    return QString();
}

bool ContactManager::saveToFile()
{
    return exportToJSON(m_dataDir + "/contacts.json");
}

bool ContactManager::loadFromFile()
{
    QString jsonFile = m_dataDir + "/contacts.json";
    if (QFile::exists(jsonFile)) {
        return importFromJSON(jsonFile);
    }

    // Спробуємо завантажити з CSV для зворотної сумісності
    QString csvFile = m_dataDir + "/contacts.csv";
    if (QFile::exists(csvFile)) {
        return importFromCSV(csvFile);
    }

    return false;
}
