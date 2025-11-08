#ifndef HISTORYWIDGET_H
#define HISTORYWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QListWidget>
#include <QVBoxLayout>

class historywidget : public QWidget
{
    Q_OBJECT
public:
    explicit historywidget(QWidget *parent = nullptr);

    void addToHistory(const QString &record);
    void clearHistory();
private:
    QListWidget *historyList;
};

#endif // HISTORYWIDGET_H
