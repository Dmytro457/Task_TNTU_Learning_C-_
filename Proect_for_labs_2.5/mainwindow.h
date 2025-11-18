#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include "resultdisplaywidget.h"
#include "historywidget.h"
#include "ui_mainwindow.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    historywidget *historyWidget;
    resultdisplaywidget *resultWidget;

    double num_first = 0;
    QString currentInput = "0";

    bool useDegrees = true;

private slots:
    QString formatDouble(double value, int precision = 3);
    void digit_numbers();
    void on_pushButton_dot_clicked();

    void operations();
    void on_pushButton_AC_clicked();
    void on_pushButton_equal_clicked();
    void math_operations();
    void advanced_operations();
    void on_pushButton_Clear_clicked();
};
#endif // MAINWINDOW_H
