#include "contact.h"
#include <QUuid>
#include <QRegularExpression>
#include <QFileInfo>
#include <QDir>
#include <QDebug>

Contact::Contact()
{
    m_id = generateId();
    m_birthday = QDate::currentDate();
}

Contact::Contact(const QString& firstName, const QString& lastName,
                 const QString& phone, const QString& email)
    : m_id(generateId())
    , m_firstName(firstName)
    , m_lastName(lastName)
    , m_phone(phone)
    , m_email(email)
    , m_birthday(QDate::currentDate())
{
}

QPixmap Contact::photo() const
{
    if (!m_photoPath.isEmpty() && QFile::exists(m_photoPath)) {
        QPixmap pixmap;
        if (pixmap.load(m_photoPath)) {
            return pixmap;
        }
    }
    return QPixmap();
}

bool Contact::setFirstName(const QString& firstName)
{
    if (firstName.trimmed().isEmpty()) {
        return false;
    }
    m_firstName = firstName.trimmed();
    return true;
}

bool Contact::setLastName(const QString& lastName)
{
    if (lastName.trimmed().isEmpty()) {
        return false;
    }
    m_lastName = lastName.trimmed();
    return true;
}

bool Contact::setPhone(const QString& phone)
{
    if (!isValidPhone(phone.trimmed())) {
        return false;
    }
    m_phone = phone.trimmed();
    return true;
}

bool Contact::setEmail(const QString& email)
{
    if (!email.trimmed().isEmpty() && !isValidEmail(email.trimmed())) {
        return false;
    }
    m_email = email.trimmed();
    return true;
}

QString Contact::toCSV() const
{
    QStringList fields;
    fields << m_id
           << m_firstName
           << m_lastName
           << m_phone
           << m_email
           << m_address
           << m_notes
           << m_birthday.toString("yyyy-MM-dd")
           << m_photoPath;

    // Екрануємо поля з комами або лапками
    for (int i = 0; i < fields.size(); ++i) {
        QString& field = fields[i];
        if (field.contains(',') || field.contains('"') || field.contains('\n')) {
            field = '"' + field.replace('"', "\"\"") + '"';
        }
    }

    return fields.join(',');
}

QJsonObject Contact::toJson() const
{
    QJsonObject obj;
    obj["id"] = m_id;
    obj["firstName"] = m_firstName;
    obj["lastName"] = m_lastName;
    obj["phone"] = m_phone;
    obj["email"] = m_email;
    obj["address"] = m_address;
    obj["notes"] = m_notes;
    obj["birthday"] = m_birthday.toString("yyyy-MM-dd");
    obj["photoPath"] = m_photoPath;
    return obj;
}

Contact Contact::fromJson(const QJsonObject& json)
{
    Contact contact;
    contact.m_id = json["id"].toString();
    contact.m_firstName = json["firstName"].toString();
    contact.m_lastName = json["lastName"].toString();
    contact.m_phone = json["phone"].toString();
    contact.m_email = json["email"].toString();
    contact.m_address = json["address"].toString();
    contact.m_notes = json["notes"].toString();
    contact.m_birthday = QDate::fromString(json["birthday"].toString(), "yyyy-MM-dd");
    contact.m_photoPath = json["photoPath"].toString();

    if (contact.m_id.isEmpty()) {
        contact.m_id = generateId();
    }

    return contact;
}

QString Contact::generateId()
{
    return QUuid::createUuid().toString(QUuid::WithoutBraces);
}

bool Contact::isValidEmail(const QString& email)
{
    QRegularExpression regex("^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}$");
    return regex.match(email).hasMatch();
}

bool Contact::isValidPhone(const QString& phone)
{
    // Дозволяємо формати: +380501234567, 0501234567, (050)123-45-67
    QRegularExpression regex("^[+]?[0-9\\s\\-\\(\\)]{10,}$");
    return regex.match(phone).hasMatch();
}
