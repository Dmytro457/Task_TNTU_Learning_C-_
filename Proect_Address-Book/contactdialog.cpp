#include "contactdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLineEdit>
#include <QTextEdit>
#include <QDateEdit>
#include <QLabel>
#include <QPushButton>
#include <QFileDialog>
#include <QMessageBox>
#include <QPixmap>
#include <QRegularExpression>
#include <QDebug>

ContactDialog::ContactDialog(Mode mode, QWidget *parent)
    : QDialog(parent)
    , m_mode(mode)
    , m_mainLayout(nullptr)
    , m_formLayout(nullptr)
    , m_firstNameEdit(nullptr)
    , m_lastNameEdit(nullptr)
    , m_phoneEdit(nullptr)
    , m_emailEdit(nullptr)
    , m_addressEdit(nullptr)
    , m_notesEdit(nullptr)
    , m_birthdayEdit(nullptr)
    , m_photoLabel(nullptr)
    , m_photoButton(nullptr)
    , m_saveButton(nullptr)
    , m_cancelButton(nullptr)
{
    setupUI();
    setWindowTitle(m_mode == Add ? "Додати контакт" : "Редагувати контакт");
}

ContactDialog::~ContactDialog()
{
}

void ContactDialog::setupUI()
{
    m_mainLayout = new QVBoxLayout(this);

    m_formLayout = new QFormLayout();

    // Ім'я
    m_firstNameEdit = new QLineEdit();
    m_firstNameEdit->setPlaceholderText("Введіть ім'я");
    m_formLayout->addRow("Ім'я*:", m_firstNameEdit);

    // Прізвище
    m_lastNameEdit = new QLineEdit();
    m_lastNameEdit->setPlaceholderText("Введіть прізвище");
    m_formLayout->addRow("Прізвище*:", m_lastNameEdit);

    // Телефон
    m_phoneEdit = new QLineEdit();
    m_phoneEdit->setPlaceholderText("+380XXXXXXXXX");
    m_formLayout->addRow("Телефон*:", m_phoneEdit);

    // Email
    m_emailEdit = new QLineEdit();
    m_emailEdit->setPlaceholderText("example@domain.com");
    m_formLayout->addRow("Email:", m_emailEdit);

    // Адреса
    m_addressEdit = new QLineEdit();
    m_formLayout->addRow("Адреса:", m_addressEdit);

    // Дата народження
    m_birthdayEdit = new QDateEdit();
    m_birthdayEdit->setCalendarPopup(true);
    m_birthdayEdit->setDate(QDate::currentDate());
    m_birthdayEdit->setDisplayFormat("dd.MM.yyyy");
    m_formLayout->addRow("Дата народження:", m_birthdayEdit);

    // Примітки
    m_notesEdit = new QTextEdit();
    m_notesEdit->setMaximumHeight(100);
    m_formLayout->addRow("Примітки:", m_notesEdit);

    // Фото
    QHBoxLayout* photoLayout = new QHBoxLayout();
    m_photoLabel = new QLabel();
    m_photoLabel->setFixedSize(120, 120);
    m_photoLabel->setStyleSheet("border: 2px solid #ccc; border-radius: 5px;");
    m_photoLabel->setAlignment(Qt::AlignCenter);
    m_photoLabel->setText("<i>Немає фото</i>");

    m_photoButton = new QPushButton("Обрати фото");
    m_photoButton->setFixedWidth(100);

    photoLayout->addWidget(m_photoLabel);
    photoLayout->addWidget(m_photoButton);
    photoLayout->addStretch();

    m_formLayout->addRow("Фото:", photoLayout);

    m_mainLayout->addLayout(m_formLayout);
    m_mainLayout->addSpacing(20);

    // Кнопки
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    m_saveButton = new QPushButton(m_mode == Add ? "Додати" : "Зберегти");
    m_saveButton->setDefault(true);
    m_saveButton->setFixedWidth(100);

    m_cancelButton = new QPushButton("Скасувати");
    m_cancelButton->setFixedWidth(100);

    buttonLayout->addStretch();
    buttonLayout->addWidget(m_saveButton);
    buttonLayout->addWidget(m_cancelButton);

    m_mainLayout->addLayout(buttonLayout);

    // Підключення сигналів
    connect(m_firstNameEdit, &QLineEdit::textChanged, this, &ContactDialog::validateForm);
    connect(m_lastNameEdit, &QLineEdit::textChanged, this, &ContactDialog::validateForm);
    connect(m_phoneEdit, &QLineEdit::textChanged, this, &ContactDialog::validateForm);
    connect(m_emailEdit, &QLineEdit::textChanged, this, &ContactDialog::validateForm);
    connect(m_photoButton, &QPushButton::clicked, this, &ContactDialog::browsePhoto);
    connect(m_saveButton, &QPushButton::clicked, this, &ContactDialog::saveContact);
    connect(m_cancelButton, &QPushButton::clicked, this, &QDialog::reject);

    validateForm();

    setLayout(m_mainLayout);
    resize(500, 600);
    setMinimumSize(500, 600);
}

void ContactDialog::validateForm()
{
    bool isValid = true;

    // Очищаємо стилі
    m_firstNameEdit->setStyleSheet("");
    m_lastNameEdit->setStyleSheet("");
    m_phoneEdit->setStyleSheet("");
    m_emailEdit->setStyleSheet("");

    // Перевірка обов'язкових полів
    QString firstName = m_firstNameEdit->text().trimmed();
    QString lastName = m_lastNameEdit->text().trimmed();
    QString phone = m_phoneEdit->text().trimmed();
    QString email = m_emailEdit->text().trimmed();

    if (firstName.isEmpty()) {
        m_firstNameEdit->setStyleSheet("border: 1px solid red;");
        isValid = false;
    }

    if (lastName.isEmpty()) {
        m_lastNameEdit->setStyleSheet("border: 1px solid red;");
        isValid = false;
    }

    if (phone.isEmpty()) {
        m_phoneEdit->setStyleSheet("border: 1px solid red;");
        isValid = false;
    } else {
        // Перевірка формату телефону
        QRegularExpression phoneRegex("^[+]?[0-9\\s\\-\\(\\)]{10,}$");
        if (!phoneRegex.match(phone).hasMatch()) {
            m_phoneEdit->setStyleSheet("border: 1px solid orange;");
            // Не робимо недійсною форму через неправильний формат телефону,
            // але показуємо попередження
        }
    }

    // Валідація email (якщо введено)
    if (!email.isEmpty()) {
        QRegularExpression emailRegex("^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}$");
        if (!emailRegex.match(email).hasMatch()) {
            m_emailEdit->setStyleSheet("border: 1px solid orange;");
            // Не робимо недійсною форму через неправильний email,
            // але показуємо попередження
        }
    }

    m_saveButton->setEnabled(isValid);

    // Якщо поля заповнені, але є попередження, змінюємо текст кнопки
    if (isValid) {
        bool hasWarnings = !m_phoneEdit->styleSheet().isEmpty() || !m_emailEdit->styleSheet().isEmpty();
        if (hasWarnings) {
            m_saveButton->setText(m_mode == Add ? "Додати (є попередження)" : "Зберегти (є попередження)");
            m_saveButton->setStyleSheet("background-color: #FFA500; color: white;");
        } else {
            m_saveButton->setText(m_mode == Add ? "Додати" : "Зберегти");
            m_saveButton->setStyleSheet("");
        }
    }
}


void ContactDialog::browsePhoto()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    "Оберіть фото", "",
                                                    "Зображення (*.png *.jpg *.jpeg *.bmp)");

    if (!fileName.isEmpty()) {
        showPhoto(fileName);
        m_currentPhotoPath = fileName;
    }
}

void ContactDialog::showPhoto(const QString& path)
{
    QPixmap photo(path);
    if (!photo.isNull()) {
        m_photoLabel->setPixmap(photo.scaled(120, 120, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    } else {
        m_photoLabel->setText("<i>Помилка завантаження фото</i>");
        m_currentPhotoPath.clear();
    }
}

void ContactDialog::setContact(const Contact& contact)
{
    m_contact = contact;
    m_currentPhotoPath = contact.photoPath();

    m_firstNameEdit->setText(contact.firstName());
    m_lastNameEdit->setText(contact.lastName());
    m_phoneEdit->setText(contact.phone());
    m_emailEdit->setText(contact.email());
    m_addressEdit->setText(contact.address());
    m_notesEdit->setText(contact.notes());
    m_birthdayEdit->setDate(contact.birthday().isValid() ? contact.birthday() : QDate::currentDate());

    if (!contact.photoPath().isEmpty()) {
        showPhoto(contact.photoPath());
    }
}

Contact ContactDialog::getContact() const
{
    Contact contact = m_contact;

    contact.setFirstName(m_firstNameEdit->text().trimmed());
    contact.setLastName(m_lastNameEdit->text().trimmed());
    contact.setPhone(m_phoneEdit->text().trimmed());
    contact.setEmail(m_emailEdit->text().trimmed());
    contact.setAddress(m_addressEdit->text().trimmed());
    contact.setNotes(m_notesEdit->toPlainText().trimmed());
    contact.setBirthday(m_birthdayEdit->date());

    // Оновлюємо шлях до фото тільки якщо вибрано нове
    if (!m_currentPhotoPath.isEmpty() && m_currentPhotoPath != contact.photoPath()) {
        contact.setPhotoPath(m_currentPhotoPath);
    }

    return contact;
}

void ContactDialog::saveContact()
{
    // Додаткова валідація перед збереженням
    if (m_firstNameEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Помилка", "Ім'я є обов'язковим полем!");
        m_firstNameEdit->setFocus();
        return;
    }

    if (m_lastNameEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Помилка", "Прізвище є обов'язковим полем!");
        m_lastNameEdit->setFocus();
        return;
    }

    if (m_phoneEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Помилка", "Телефон є обов'язковим полем!");
        m_phoneEdit->setFocus();
        return;
    }

    accept();
}
