#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QMessageBox>

#include <QSqlRecord>
#include <QSqlQuery>
#include <QSortFilterProxyModel>
#include <QSqlTableModel>
#include <QSqlDatabase>
#include <QRegularExpression>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , model(nullptr)
{
    ui->setupUi(this);

    m_dbConnect = new databaseconnect(this);
    m_dbConnect->setHost("127.0.0.1");
    m_dbConnect->setPort(3306);
    m_dbConnect->setdatabasename("Mydatabase");
    m_dbConnect->setUser("whoUser");
    m_dbConnect->setPassword("Security_db");

    connect(m_dbConnect, &databaseconnect::connected, this, [](){qDebug() << "db connected"; });
    connect(m_dbConnect, &databaseconnect::errorOccurred, this, [](const QString &err){ qDebug() << " db error:" << err; });

    if(!m_dbConnect->connect()){
        qWarning() << "Could not connect: " << m_dbConnect->lastError();
    } else {
        QSqlDatabase db = m_dbConnect->database();
        QSqlQuery q(db);

        q.exec("CREATE TABLE IF NOT EXISTS Product("
                "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                "Name TEXT,"
                "Category TEXT,"
                "Price REAL, "
                "Counts INTEGER)");
    }

    model = new productmodel(this);
    model->loadMockData();

    ui->comboBox_Category->addItem("All");
    ui->lineEdit_Price_from->setText("");
    ui->lineEdit_Price_to->setText("");
    QSet<QString> uniCategories;
    for(int i = 0; i < model->rowCount(); i++) {
        QString uniCategory = model->data(model->index(i,2)).toString();
        uniCategories.insert(uniCategory);
    }
    for(auto &c : uniCategories)
        ui->comboBox_Category->addItem(c);

    proxyModel = new filterbychoose(this);
    proxyModel->setSourceModel(model);
    proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    ui->tableView_->setModel(proxyModel);
    ui->tableView_->setSortingEnabled(true);

    connect(ui->pushButton_Add, &QPushButton::clicked, this, & MainWindow::onAddClicked);
    connect(ui->pushButton_Delete, &QPushButton::clicked, this, & MainWindow::onDeleteClicked);
    connect(ui->pushButton_Update, &QPushButton::clicked, this, & MainWindow::onUpdateClicked);
    connect(ui->pushButton_ResetFilter, &QPushButton::clicked, this, & MainWindow::onResetFilterClicked);
    connect(ui->tableView_, &QTableView::doubleClicked, this, &MainWindow::onTableDoubleClicked);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updateCategoryFilter()
{
    ui->comboBox_Category->clear();
    ui->comboBox_Category->addItem("All");

    QSet<QString> uniCategories;
    for(int i = 0; i < model->rowCount(); i++) {
        QString uniCategory = model->data(model->index(i, 2)).toString();
        uniCategories.insert(uniCategory);
    }

    for(auto &c : uniCategories)
        ui->comboBox_Category->addItem(c);
}

void MainWindow::onAddClicked(){
    if(ui->field_Name->text().isEmpty() ||
        ui->field_Category->text().isEmpty() ||
        ui->field_Price->text().isEmpty() ||
        ui->field_Count->text().isEmpty()) {
        QMessageBox::warning(this, "Помилка", "Заповніть всі поля!");
        return;
    }

    bool ok;
    double price = ui->field_Price->text().toDouble(&ok);
    if(!ok || price < 0) {
        QMessageBox::warning(this, "Помилка", "Невірна ціна!");
        return;
    }

    int count = ui->field_Count->text().toInt(&ok);
    if(!ok || count < 0) {
        QMessageBox::warning(this, "Помилка", "Невірна кількість!");
        return;
    }

    Product p;
    p.id = model->rowCount() + 1;
    p.Name = ui->field_Name->text();
    p.Category = ui->field_Category->text();
    p.Price = ui->field_Price->text().toDouble();
    p.Count = ui->field_Count->text().toInt();

    model->addProduct(p);

    ui->field_Name->clear();
    ui->field_Category->clear();
    ui->field_Price->clear();
    ui->field_Count->clear();

    updateCategoryFilter();
}

void MainWindow::onDeleteClicked(){
    QModelIndex proxyIndex = ui->tableView_->currentIndex();
    if(!proxyIndex.isValid()) return;

    QModelIndex sourceIndex = proxyModel->mapToSource(proxyIndex);
    if(!sourceIndex.isValid()) return;
    model->removeRow(sourceIndex.row());

    updateCategoryFilter();
}

void MainWindow::onUpdateClicked(){
    QModelIndex proxyIndex = ui->tableView_->currentIndex();
    if(!proxyIndex.isValid()){
        QMessageBox::warning(this, "Помилка", "Виберіть товар для оновлення!");
        return;
    }

    QModelIndex sourceIndex = proxyModel->mapToSource(proxyIndex);
    if(!sourceIndex.isValid()) return;

    if(ui->field_Name->text().isEmpty() ||
        ui->field_Category->text().isEmpty() ||
        ui->field_Price->text().isEmpty() ||
        ui->field_Count->text().isEmpty()) {
        QMessageBox::warning(this, "Помилка", "Заповніть всі поля!");
        return;
    }

    bool ok;
    double price = ui->field_Price->text().toDouble(&ok);
    if(!ok || price < 0) {
        QMessageBox::warning(this, "Помилка", "Невірна ціна!");
        return;
    }

    int count = ui->field_Count->text().toInt(&ok);
    if(!ok || count < 0) {
        QMessageBox::warning(this, "Помилка", "Невірна кількість!");
        return;
    }

    Product p;
    p.Name = ui->field_Name->text();
    p.Category = ui->field_Category->text();
    p.Price = price;
    p.Count = count;

    model->updateProduct(sourceIndex.row(), p);
    updateCategoryFilter();
}

void MainWindow::on_pushButton_Filter_clicked()
{
    QString category = ui->comboBox_Category->currentText();

    double minPrice = ui->lineEdit_Price_from->text().toDouble();
    double maxPrice = ui->lineEdit_Price_to->text().toDouble();

    proxyModel->setFilters(category, minPrice, maxPrice);
}

void MainWindow::onResetFilterClicked()
{
    ui->comboBox_Category->setCurrentText("All");
    ui->lineEdit_Price_from->setText("");
    ui->lineEdit_Price_to->setText("");
    proxyModel->setFilters("All", 0, std::numeric_limits<double>::max());
}

void MainWindow::onTableDoubleClicked(const QModelIndex &index)
{
    if(!index.isValid()) return;

    QModelIndex sourceIndex = proxyModel->mapToSource(index);
    if(!sourceIndex.isValid()) return;

    int row = sourceIndex.row();

    ui->field_Name->setText(model->data(model->index(row, 1)).toString());
    ui->field_Category->setText(model->data(model->index(row, 2)).toString());
    ui->field_Price->setText(model->data(model->index(row, 3)).toString());
    ui->field_Count->setText(model->data(model->index(row, 4)).toString());
}
