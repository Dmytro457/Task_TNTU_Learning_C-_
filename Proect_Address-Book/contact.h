#ifndef CONTACT_H
#define CONTACT_H

#include <QString>
#include <QPixmap>
#include <QDate>
#include <QUuid>
#include <QJsonObject>

class Contact
{
private:
    QString m_id;
    QString m_firstName;
    QString m_lastName;
    QString m_phone;
    QString m_email;
    QString m_address;
    QString m_notes;
    QDate m_birthday;
    QString m_photoPath; // Зберігаємо шлях до фото, а не саме фото

public:
    Contact();
    Contact(const QString& firstName, const QString& lastName,
            const QString& phone, const QString& email = "");

    // Гетери
    QString id() const { return m_id; }
    QString firstName() const { return m_firstName; }
    QString lastName() const { return m_lastName; }
    QString phone() const { return m_phone; }
    QString email() const { return m_email; }
    QString address() const { return m_address; }
    QString notes() const { return m_notes; }
    QDate birthday() const { return m_birthday; }
    QString photoPath() const { return m_photoPath; }
    QPixmap photo() const;

    // Сетери з валідацією
    bool setFirstName(const QString& firstName);
    bool setLastName(const QString& lastName);
    bool setPhone(const QString& phone);
    bool setEmail(const QString& email);
    void setAddress(const QString& address) { m_address = address; }
    void setNotes(const QString& notes) { m_notes = notes; }
    void setBirthday(const QDate& birthday) { m_birthday = birthday; }
    void setPhotoPath(const QString& path) { m_photoPath = path; }

    // Утиліти
    QString fullName() const { return m_firstName + " " + m_lastName; }
    bool isEmpty() const { return m_firstName.isEmpty() && m_lastName.isEmpty(); }

    // Серіалізація
    QString toCSV() const;
    QJsonObject toJson() const;
    static Contact fromJson(const QJsonObject& json);

    // Статичні методи
    static QString generateId();
    static bool isValidEmail(const QString& email);
    static bool isValidPhone(const QString& phone);
};

#endif
