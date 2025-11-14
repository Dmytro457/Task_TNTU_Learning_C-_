#include "tablecreator.h"

bool tablecreator::createTableProduct()
{
    QSqlQuery qr;
    QString createTable =
        ("CREATE TABLE IF NOT EXISTS product ("
            "id INT AUTO_INCREMENT PRIMARY KEY, "
            "Name VARCHAR(100)"
            "Category VARCHAR(100)"
            "Price DOUBLE"
            "Count INT)");

    if(!qr.exec(createTable)){
        qDebug() << "Error create the table:" << qr.lastError().text();
        return false;
    }
    qDebug() << "The table \"Product\" successfullly are created!";
    return true;
}
