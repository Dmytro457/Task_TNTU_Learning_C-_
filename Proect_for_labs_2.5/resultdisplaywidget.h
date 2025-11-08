#ifndef RESULTDISPLAYWIDGET_H
#define RESULTDISPLAYWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QListWidget>
#include <QVBoxLayout>

class resultdisplaywidget : public QWidget
{
    Q_OBJECT
public:
    explicit resultdisplaywidget(QWidget *parent = nullptr);

    void setResult(double value);
    void setResult(const QString &text);
    QString result() const;

private:
    QLabel *resultLabel;
};

#endif // RESULTDISPLAYWIDGET_H
