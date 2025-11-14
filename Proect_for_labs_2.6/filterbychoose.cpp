#include "filterbychoose.h"

filterbychoose::filterbychoose(QObject *parent)
    : QSortFilterProxyModel(parent) {}

void filterbychoose::setFilters(const QString &category, double minPrice, double maxPrice)
{
    m_category = category;
    m_minPrice = (minPrice > 0) ? minPrice : 0;
    m_maxPrice = (maxPrice > 0) ? maxPrice : std::numeric_limits<double>::max();
    invalidate();
}

bool filterbychoose::filterAcceptsRow(int row, const QModelIndex &parent) const {
    QString rowCategory = sourceModel()->index(row, 2, parent).data().toString();
    double rowPrice = sourceModel()->index(row, 3, parent).data().toDouble();

    bool matchCat = (m_category == "All" || rowCategory == m_category);
    bool matchPrice = (rowPrice >= m_minPrice && rowPrice <= m_maxPrice);

    return matchCat && matchPrice;
}
