#include "mainwindow.h"
#include "contacttablemodel.h"
#include "contactdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTableView>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QLabel>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QMessageBox>
#include <QFileDialog>
#include <QSortFilterProxyModel>
#include <QHeaderView>
#include <QFileInfo>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_contactManager(nullptr)
    , m_contactModel(nullptr)
    , m_proxyModel(nullptr)
    , m_tableView(nullptr)
    , m_searchEdit(nullptr)
    , m_searchButton(nullptr)
    , m_clearButton(nullptr)
    , m_sortCombo(nullptr)
    , m_addButton(nullptr)
    , m_editButton(nullptr)
    , m_deleteButton(nullptr)
    , m_statusLabel(nullptr)
{
    m_contactManager = new ContactManager(this);

    setupUI();
    setupConnections();
    setupMenuBar();
    setupToolBar();
    setupTable();

    loadContacts();
    updateStatusBar();

    setWindowTitle("Менеджер контактів");
    resize(900, 600);
}

MainWindow::~MainWindow()
{
    saveContacts();
}

void MainWindow::setupUI()
{
    // Центральний віджет
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);

    // Панель пошуку та сортування
    QHBoxLayout* searchLayout = new QHBoxLayout();

    m_searchEdit = new QLineEdit();
    m_searchEdit->setPlaceholderText("Пошук контактів...");
    m_searchEdit->setClearButtonEnabled(true);

    m_searchButton = new QPushButton("Пошук");
    m_clearButton = new QPushButton("Очистити");

    m_sortCombo = new QComboBox();
    m_sortCombo->addItem("Сортування...");
    m_sortCombo->addItem("За ім'ям (А-Я)");
    m_sortCombo->addItem("За ім'ям (Я-А)");
    m_sortCombo->addItem("За датою (старші)");
    m_sortCombo->addItem("За датою (молодші)");

    searchLayout->addWidget(new QLabel("Пошук:"));
    searchLayout->addWidget(m_searchEdit);
    searchLayout->addWidget(m_searchButton);
    searchLayout->addWidget(m_clearButton);
    searchLayout->addStretch();
    searchLayout->addWidget(new QLabel("Сортування:"));
    searchLayout->addWidget(m_sortCombo);

    // Кнопки управління
    QHBoxLayout* buttonLayout = new QHBoxLayout();

    m_addButton = new QPushButton("Додати контакт");
    m_editButton = new QPushButton("Редагувати");
    m_deleteButton = new QPushButton("Видалити");

    buttonLayout->addWidget(m_addButton);
    buttonLayout->addWidget(m_editButton);
    buttonLayout->addWidget(m_deleteButton);
    buttonLayout->addStretch();

    // Таблиця
    m_tableView = new QTableView();
    m_tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    m_tableView->setAlternatingRowColors(true);

    // Статус бар
    m_statusLabel = new QLabel();

    // Додаємо все до головного лейаута
    mainLayout->addLayout(searchLayout);
    mainLayout->addLayout(buttonLayout);
    mainLayout->addWidget(m_tableView);
    mainLayout->addWidget(m_statusLabel);

    // Встановлюємо стилі
    QString buttonStyle = "QPushButton { padding: 5px 15px; }";
    m_addButton->setStyleSheet(buttonStyle + "background-color: #4CAF50; color: white;");
    m_editButton->setStyleSheet(buttonStyle);
    m_deleteButton->setStyleSheet(buttonStyle + "background-color: #f44336; color: white;");

    m_editButton->setEnabled(false);
    m_deleteButton->setEnabled(false);
}

void MainWindow::setupConnections()
{
    // Використовуємо новий синтаксис для Qt 6
    connect(m_addButton, &QPushButton::clicked, this, &MainWindow::onAddContact);
    connect(m_editButton, &QPushButton::clicked, this, &MainWindow::onEditContact);
    connect(m_deleteButton, &QPushButton::clicked, this, &MainWindow::onDeleteContact);

    // Для searchButton - без параметра
    connect(m_searchButton, &QPushButton::clicked, this, &MainWindow::onSearch);

    // Для clearButton - без параметра
    connect(m_clearButton, &QPushButton::clicked, this, &MainWindow::onClearSearch);

    // Для searchEdit - з параметром
    connect(m_searchEdit, &QLineEdit::textChanged, this, &MainWindow::onSearchTextChanged);

    // Для sortCombo
    connect(m_sortCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onSortChanged);

    // Для tableView
    connect(m_tableView, &QTableView::doubleClicked,
            this, &MainWindow::onContactDoubleClicked);

    // Для contactManager
    connect(m_contactManager, &ContactManager::dataChanged,
            this, &MainWindow::onDataChanged);
}

void MainWindow::setupMenuBar()
{
    QMenu* fileMenu = menuBar()->addMenu("Файл");

    QAction* importCSVAction = new QAction("Імпорт з CSV...", this);
    connect(importCSVAction, &QAction::triggered, this, &MainWindow::onImportCSV);
    fileMenu->addAction(importCSVAction);

    QAction* importJSONAction = new QAction("Імпорт з JSON...", this);
    connect(importJSONAction, &QAction::triggered, this, &MainWindow::onImportJSON);
    fileMenu->addAction(importJSONAction);

    fileMenu->addSeparator();

    QAction* exportCSVAction = new QAction("Експорт у CSV...", this);
    connect(exportCSVAction, &QAction::triggered, this, &MainWindow::onExportCSV);
    fileMenu->addAction(exportCSVAction);

    QAction* exportJSONAction = new QAction("Експорт у JSON...", this);
    connect(exportJSONAction, &QAction::triggered, this, &MainWindow::onExportJSON);
    fileMenu->addAction(exportJSONAction);

    fileMenu->addSeparator();

    QAction* exitAction = new QAction("Вихід", this);
    connect(exitAction, &QAction::triggered, this, &QWidget::close);
    fileMenu->addAction(exitAction);

    QMenu* helpMenu = menuBar()->addMenu("Допомога");

    QAction* aboutAction = new QAction("Про програму", this);
    connect(aboutAction, &QAction::triggered, this, &MainWindow::onAbout);
    helpMenu->addAction(aboutAction);
}

void MainWindow::setupToolBar()
{
    QToolBar* toolBar = addToolBar("Панель інструментів");

    QAction* addAction = new QAction("Додати", this);
    connect(addAction, &QAction::triggered, this, &MainWindow::onAddContact);
    toolBar->addAction(addAction);

    QAction* editAction = new QAction("Редагувати", this);
    connect(editAction, &QAction::triggered, this, &MainWindow::onEditContact);
    toolBar->addAction(editAction);

    QAction* deleteAction = new QAction("Видалити", this);
    connect(deleteAction, &QAction::triggered, this, &MainWindow::onDeleteContact);
    toolBar->addAction(deleteAction);

    toolBar->addSeparator();

    toolBar->addWidget(new QLabel("Пошук:"));
    m_searchEdit->setMaximumWidth(200);
    toolBar->addWidget(m_searchEdit);

    // Додаємо кнопку пошуку на тулбар
    QAction* searchAction = new QAction("Пошук", this);
    connect(searchAction, &QAction::triggered, this, &MainWindow::onSearch);
    toolBar->addAction(searchAction);
}

void MainWindow::setupTable()
{
    m_contactModel = new ContactTableModel(m_contactManager, this);
    m_proxyModel = new QSortFilterProxyModel(this);
    m_proxyModel->setSourceModel(m_contactModel);
    m_proxyModel->setFilterKeyColumn(-1); // Фільтрувати по всіх колонках
    m_proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);

    m_tableView->setModel(m_proxyModel);
    m_tableView->setSortingEnabled(true);

    // Налаштування заголовків колонок
    m_tableView->horizontalHeader()->setStretchLastSection(true);
    m_tableView->verticalHeader()->setVisible(false);
    m_tableView->setColumnWidth(0, 150); // Ім'я
    m_tableView->setColumnWidth(1, 150); // Прізвище
    m_tableView->setColumnWidth(2, 150); // Телефон
    m_tableView->setColumnWidth(3, 200); // Email

    // Підключення виділення
    connect(m_tableView->selectionModel(), &QItemSelectionModel::selectionChanged,
            [this]() {
                bool hasSelection = !m_tableView->selectionModel()->selectedRows().isEmpty();
                m_editButton->setEnabled(hasSelection);
                m_deleteButton->setEnabled(hasSelection);
            });
}

void MainWindow::loadContacts()
{
    m_contactManager->loadFromFile();
    m_contactModel->refresh();
}

void MainWindow::saveContacts()
{
    m_contactManager->saveToFile();
}

void MainWindow::updateStatusBar()
{
    int total = m_contactManager->count();
    int visible = m_proxyModel->rowCount();

    QString status;
    if (visible == total) {
        status = QString("Контактів: %1").arg(total);
    } else {
        status = QString("Контактів: %1 (показано: %2)").arg(total).arg(visible);
    }

    m_statusLabel->setText(status);
}

// =============== SLOTS ===============

void MainWindow::onAddContact()
{
    ContactDialog dialog(ContactDialog::Add, this);
    if (dialog.exec() == QDialog::Accepted) {
        Contact contact = dialog.getContact();

        // Копіюємо фото у сховище
        QString photoPath = contact.photoPath();
        if (!photoPath.isEmpty()) {
            QString newPhotoPath = m_contactManager->addPhoto(photoPath, contact.id());
            contact.setPhotoPath(newPhotoPath);
        }

        if (m_contactManager->addContact(contact)) {
            updateStatusBar();
        }
    }
}

void MainWindow::onEditContact()
{
    QModelIndexList selected = m_tableView->selectionModel()->selectedRows();
    if (selected.isEmpty()) return;

    QModelIndex proxyIndex = selected.first();
    QModelIndex sourceIndex = m_proxyModel->mapToSource(proxyIndex);
    Contact contact = m_contactModel->getContactAt(sourceIndex.row());

    ContactDialog dialog(ContactDialog::Edit, this);
    dialog.setContact(contact);

    if (dialog.exec() == QDialog::Accepted) {
        Contact updatedContact = dialog.getContact();

        // Обробка фото
        QString newPhotoPath = dialog.getContact().photoPath();
        if (newPhotoPath != contact.photoPath()) {
            // Видаляємо старе фото
            m_contactManager->removePhoto(contact.id());

            // Копіюємо нове фото
            if (!newPhotoPath.isEmpty()) {
                QString storedPhotoPath = m_contactManager->addPhoto(newPhotoPath, contact.id());
                updatedContact.setPhotoPath(storedPhotoPath);
            }
        }

        if (m_contactManager->updateContact(contact.id(), updatedContact)) {
            updateStatusBar();
        }
    }
}

void MainWindow::onDeleteContact()
{
    QModelIndexList selected = m_tableView->selectionModel()->selectedRows();
    if (selected.isEmpty()) return;

    QModelIndex proxyIndex = selected.first();
    QModelIndex sourceIndex = m_proxyModel->mapToSource(proxyIndex);
    Contact contact = m_contactModel->getContactAt(sourceIndex.row());

    int answer = QMessageBox::question(this, "Підтвердження",
                                       QString("Видалити контакт <b>%1 %2</b>?").arg(contact.firstName()).arg(contact.lastName()),
                                       QMessageBox::Yes | QMessageBox::No);

    if (answer == QMessageBox::Yes) {
        if (m_contactManager->removeContact(contact.id())) {
            updateStatusBar();
        }
    }
}

void MainWindow::onSearch()
{
    QString searchText = m_searchEdit->text().trimmed();
    m_proxyModel->setFilterFixedString(searchText);
    updateStatusBar();
}

void MainWindow::onSearchTextChanged(const QString& text)
{
    Q_UNUSED(text);
    onSearch(); // Викликаємо той самий метод
}

void MainWindow::onClearSearch()
{
    m_searchEdit->clear();
    m_proxyModel->setFilterFixedString("");
    updateStatusBar();
}

void MainWindow::onContactDoubleClicked(const QModelIndex &index)
{
    Q_UNUSED(index);
    onEditContact();
}

void MainWindow::onSortChanged(int index)
{
    switch (index) {
    case 1: m_contactManager->sortByName(true); break;
    case 2: m_contactManager->sortByName(false); break;
    case 3: m_contactManager->sortByDate(true); break;
    case 4: m_contactManager->sortByDate(false); break;
    default: return;
    }

    m_contactModel->refresh();
}

void MainWindow::onImportCSV()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    "Імпорт з CSV", "",
                                                    "CSV Files (*.csv);;All Files (*)");

    if (!fileName.isEmpty()) {
        if (m_contactManager->importFromCSV(fileName)) {
            m_contactModel->refresh();
            updateStatusBar();
            QMessageBox::information(this, "Імпорт", "Контакти успішно імпортовано з CSV.");
        } else {
            QMessageBox::warning(this, "Помилка", "Не вдалося імпортувати контакти з CSV.");
        }
    }
}

void MainWindow::onImportJSON()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    "Імпорт з JSON", "",
                                                    "JSON Files (*.json);;All Files (*)");

    if (!fileName.isEmpty()) {
        if (m_contactManager->importFromJSON(fileName)) {
            m_contactModel->refresh();
            updateStatusBar();
            QMessageBox::information(this, "Імпорт", "Контакти успішно імпортовано з JSON.");
        } else {
            QMessageBox::warning(this, "Помилка", "Не вдалося імпортувати контакти з JSON.");
        }
    }
}

void MainWindow::onExportCSV()
{
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    "Експорт у CSV", "contacts.csv",
                                                    "CSV Files (*.csv);;All Files (*)");

    if (!fileName.isEmpty()) {
        if (!fileName.endsWith(".csv", Qt::CaseInsensitive)) {
            fileName += ".csv";
        }

        if (m_contactManager->exportToCSV(fileName)) {
            QMessageBox::information(this, "Експорт", "Контакти успішно експортовано у CSV.");
        } else {
            QMessageBox::warning(this, "Помилка", "Не вдалося експортувати контакти у CSV.");
        }
    }
}

void MainWindow::onExportJSON()
{
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    "Експорт у JSON", "contacts.json",
                                                    "JSON Files (*.json);;All Files (*)");

    if (!fileName.isEmpty()) {
        if (!fileName.endsWith(".json", Qt::CaseInsensitive)) {
            fileName += ".json";
        }

        if (m_contactManager->exportToJSON(fileName)) {
            QMessageBox::information(this, "Експорт", "Контакти успішно експортовано у JSON.");
        } else {
            QMessageBox::warning(this, "Помилка", "Не вдалося експортувати контакти у JSON.");
        }
    }
}

void MainWindow::onAbout()
{
    QMessageBox::about(this, "Про програму",
                       "<h2>Менеджер контактів</h2>"
                       "<p><b>Версія 2.0</b></p>"
                       "<p>Програма для управління контактами з повним функціоналом:</p>"
                       "<ul>"
                       "<li>Додавання, редагування, видалення контактів</li>"
                       "<li>Фотографії контактів</li>"
                       "<li>Пошук та сортування</li>"
                       "<li>Експорт/імпорт у CSV та JSON</li>"
                       "<li>Валідація даних</li>"
                       "</ul>"
                       "<p>Розроблено з використанням Qt та принципів ООП</p>");
}

void MainWindow::onDataChanged()
{
    m_contactModel->refresh();
    updateStatusBar();
}
