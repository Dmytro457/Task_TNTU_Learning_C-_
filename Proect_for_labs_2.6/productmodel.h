#ifndef PRODUCTMODEL_H
#define PRODUCTMODEL_H

#include <QStandardItemModel>
#include <QObject>

struct Product{
    int id;
    QString Name;
    QString Category;
    double Price;
    int Count;
};

class productmodel : public QStandardItemModel
{
    Q_OBJECT
public:
    explicit productmodel(QObject *parent = nullptr);

    void loadMockData();
    void addProduct(const Product &product);
    void updateProduct(int row, const Product &product);
    void removeProduct(int row);

};

#endif // PRODUCTMODEL_H
