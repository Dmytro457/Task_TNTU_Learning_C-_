#include "resultdisplaywidget.h"
#include <QString>

resultdisplaywidget::resultdisplaywidget(QWidget *parent)
    : QWidget(parent)
{
    resultLabel = new QLabel("0");

    auto *layout = new QVBoxLayout(this);
    layout->addWidget(resultLabel);
}

void resultdisplaywidget::setResult(double value)
{
    resultLabel->setText(QString::number(value));
}

void resultdisplaywidget::setResult(const QString &text)
{
    resultLabel->setText(text);
}

QString resultdisplaywidget::result() const {
    return resultLabel->text();
}


