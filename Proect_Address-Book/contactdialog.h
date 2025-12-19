#ifndef CONTACTDIALOG_H
#define CONTACTDIALOG_H

#include "contact.h"
#include <QDialog>

class QLineEdit;
class QTextEdit;
class QDateEdit;
class QLabel;
class QPushButton;
class QFormLayout;
class QVBoxLayout;
class QHBoxLayout;

class ContactDialog : public QDialog
{
    Q_OBJECT

public:
    enum Mode { Add, Edit };

    explicit ContactDialog(Mode mode, QWidget *parent = nullptr);
    ~ContactDialog();

    void setContact(const Contact& contact);
    Contact getContact() const;

private slots:
    void validateForm();
    void browsePhoto();
    void saveContact();

private:
    void setupUI();
    void showPhoto(const QString& path);

    Mode m_mode;
    Contact m_contact;
    QString m_currentPhotoPath;

    QVBoxLayout* m_mainLayout;
    QFormLayout* m_formLayout;
    QLineEdit* m_firstNameEdit;
    QLineEdit* m_lastNameEdit;
    QLineEdit* m_phoneEdit;
    QLineEdit* m_emailEdit;
    QLineEdit* m_addressEdit;
    QTextEdit* m_notesEdit;
    QDateEdit* m_birthdayEdit;
    QLabel* m_photoLabel;
    QPushButton* m_photoButton;
    QPushButton* m_saveButton;
    QPushButton* m_cancelButton;
};

#endif
