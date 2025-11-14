#include "productmodel.h"
#include <QStandardItem>

productmodel::productmodel(QObject *parent)
    : QStandardItemModel(parent)
{
    setColumnCount(5);
    setHorizontalHeaderLabels({"ID","Name","Category","Price","Count"});
}

void productmodel::loadMockData() {
    QList<Product> products = {
        {1, "Laptop", "Electronics", 23000.0, 6},
        {2, "Mouse", "Electronics", 355.0, 23},
        {3, "Book", "Printed product", 420.0, 20},
        {4, "Pen", "Stationery", 20.0, 44},
        {5, "Notebook", "Stationery", 15.0, 25}
    };

    for (const auto &p : products) { addProduct(p); }
}

void productmodel::addProduct(const Product &p)
{
    QList<QStandardItem*> row;
    row << new QStandardItem(QString::number(p.id))
        << new QStandardItem(p.Name)
        << new QStandardItem(p.Category)
        << new QStandardItem(QString::number(p.Price))
        << new QStandardItem(QString::number(p.Count));
    appendRow(row);
 }

void productmodel::updateProduct(int row, const Product &p)
{
    if(row<0 || row >= rowCount()) return;
    setItem(row, 1, new QStandardItem(p.Name));
    setItem(row, 2, new QStandardItem(p.Category));
    setItem(row, 3, new QStandardItem(QString::number(p.Price)));
    setItem(row, 4, new QStandardItem(QString::number(p.Count)));
}

void productmodel::removeProduct(int row)
{
    if(row<0 || row >= rowCount()) return;
    removeRow(row);
}
