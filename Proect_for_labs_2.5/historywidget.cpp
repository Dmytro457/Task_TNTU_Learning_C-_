#include "historywidget.h"
#include <QString>

historywidget::historywidget(QWidget *parent)
    : QWidget(parent)
{
    historyList = new QListWidget();

    auto *layout = new QVBoxLayout(this);
    layout->addWidget(historyList);
}

void historywidget::addToHistory(const QString &record)
{
    historyList->addItem(record);
}

void historywidget::clearHistory()
{
    historyList->clear();
}
