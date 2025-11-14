#ifndef FILTERBYCHOOSE_H
#define FILTERBYCHOOSE_H

#include <QSortFilterProxyModel>

class filterbychoose : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit filterbychoose(QObject *parent = nullptr);;

    void setFilters(const QString &category, double minPrice, double maxPrice);
protected:
     bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;

private:
    QString m_category = "All";
    double m_minPrice  = 0;
    double m_maxPrice  = 999999;

};

#endif // FILTERBYCHOOSE_H
