#ifndef CONTACTMANAGER_H
#define CONTACTMANAGER_H

#include "contact.h"
#include <QObject>
#include <QList>
#include <QMap>

class ContactManager : public QObject
{
    Q_OBJECT

private:
    QMap<QString, Contact> m_contacts;
    QString m_dataDir;
    QString m_photosDir;

    void ensureDirectories();
    QString copyPhotoToStorage(const QString& sourcePath, const QString& contactId);

public:
    explicit ContactManager(QObject *parent = nullptr);

    // CRUD операції
    bool addContact(const Contact& contact);
    bool updateContact(const QString& id, const Contact& contact);
    bool removeContact(const QString& id);
    Contact getContact(const QString& id) const;
    QList<Contact> getAllContacts() const;
    int count() const { return m_contacts.size(); }

    // Пошук
    QList<Contact> search(const QString& keyword) const;

    // Сортування
    void sortByName(bool ascending = true);
    void sortByDate(bool ascending = true);

    // Експорт/імпорт
    bool exportToCSV(const QString& filename) const;
    bool importFromCSV(const QString& filename);
    bool exportToJSON(const QString& filename) const;
    bool importFromJSON(const QString& filename);

    // Робота з фото
    QString addPhoto(const QString& sourcePath, const QString& contactId);
    bool removePhoto(const QString& contactId);
    QString getPhotoPath(const QString& contactId) const;

    // Збереження/завантаження
    bool saveToFile();
    bool loadFromFile();

signals:
    void dataChanged();
};

#endif
