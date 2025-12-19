#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "contactmanager.h"
#include <QMainWindow>

QT_BEGIN_NAMESPACE
class QTableView;
class QLineEdit;
class QPushButton;
class QComboBox;
class QLabel;
class ContactTableModel;
class QSortFilterProxyModel;
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onAddContact();
    void onEditContact();
    void onDeleteContact();
    void onSearch();  // Без параметра
    void onSearchTextChanged(const QString& text);
    void onClearSearch();
    void onContactDoubleClicked(const QModelIndex &index);
    void onSortChanged(int index);
    void onImportCSV();
    void onImportJSON();
    void onExportCSV();
    void onExportJSON();
    void onAbout();
    void onDataChanged();

private:
    void setupUI();
    void setupConnections();
    void setupMenuBar();
    void setupToolBar();
    void setupTable();
    void updateStatusBar();
    void loadContacts();
    void saveContacts();

    ContactManager* m_contactManager;
    ContactTableModel* m_contactModel;
    QSortFilterProxyModel* m_proxyModel;

    // UI елементи
    QTableView* m_tableView;
    QLineEdit* m_searchEdit;
    QPushButton* m_searchButton;
    QPushButton* m_clearButton;
    QComboBox* m_sortCombo;
    QPushButton* m_addButton;
    QPushButton* m_editButton;
    QPushButton* m_deleteButton;
    QLabel* m_statusLabel;
};

#endif
