#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QPushButton>
#include <QDebug>
#include <QtMath>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    historyWidget = new historywidget(this);
    ui->verticalLayout_2->addWidget(historyWidget);

    resultWidget = new resultdisplaywidget(this);
    ui->displayResult_->addWidget(resultWidget);
    resultWidget->setResult("0");

    ui->comboUnits_2->addItem("Radians");
    ui->comboUnits_2->addItem("Degrees");
    connect(ui->pushButton_0, SIGNAL(clicked()), this, SLOT(digit_numbers()));
    connect(ui->pushButton_1, SIGNAL(clicked()), this, SLOT(digit_numbers()));
    connect(ui->pushButton_2, SIGNAL(clicked()), this, SLOT(digit_numbers()));
    connect(ui->pushButton_3, SIGNAL(clicked()), this, SLOT(digit_numbers()));
    connect(ui->pushButton_4, SIGNAL(clicked()), this, SLOT(digit_numbers()));
    connect(ui->pushButton_5, SIGNAL(clicked()), this, SLOT(digit_numbers()));
    connect(ui->pushButton_6, SIGNAL(clicked()), this, SLOT(digit_numbers()));
    connect(ui->pushButton_7, SIGNAL(clicked()), this, SLOT(digit_numbers()));
    connect(ui->pushButton_8, SIGNAL(clicked()), this, SLOT(digit_numbers()));
    connect(ui->pushButton_9, SIGNAL(clicked()), this, SLOT(digit_numbers()));

    connect(ui->pushButton_negative, SIGNAL(clicked()), this, SLOT(operations()));
    connect(ui->pushButton_percent, SIGNAL(clicked()), this, SLOT(operations()));
    connect(ui->pushButton_divide, SIGNAL(clicked()), this, SLOT(math_operations()));
    connect(ui->pushButton_plus, SIGNAL(clicked()), this, SLOT(math_operations()));
    connect(ui->pushButton_diff, SIGNAL(clicked()), this, SLOT(math_operations()));
    connect(ui->pushButton_multi, SIGNAL(clicked()), this, SLOT(math_operations()));

    connect(ui->pushButton_root, SIGNAL(clicked()), this, SLOT(advanced_operations()));
    connect(ui->pushButton_sin, SIGNAL(clicked()), this, SLOT(advanced_operations()));
    connect(ui->pushButton_cos, SIGNAL(clicked()), this, SLOT(advanced_operations()));
    connect(ui->pushButton_tg, SIGNAL(clicked()), this, SLOT(advanced_operations()));
    connect(ui->pushButton_log, SIGNAL(clicked()), this, SLOT(advanced_operations()));

    ui->pushButton_divide->setCheckable(true);
    ui->pushButton_plus->setCheckable(true);
    ui->pushButton_diff->setCheckable(true);
    ui->pushButton_multi->setCheckable(true);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::digit_numbers()
{
    QPushButton *button = (QPushButton *)sender();
    QString digit = button->text();

    if(currentInput.length() >= 15)
        return;
    if (currentInput == "0" && digit != ".") {
        currentInput = digit;
    } else {
        currentInput += digit;
    }
    resultWidget->setResult(currentInput);
}

 void MainWindow::on_pushButton_dot_clicked()
 {
     if(ui->comboBox->currentText() == "Whole numbers") {
         return;
     }

     if(!currentInput.contains('.')) {
         currentInput += '.';
         resultWidget->setResult(currentInput);
     }
 }

 void MainWindow::operations()
 {
     QPushButton *button = (QPushButton*)sender();
     double val = currentInput.toDouble();

     if(button->text() == "+/-") {
         val *= -1;
     } else if(button->text() == "%") {
         val *= 0.01;
     }
     currentInput = QString::number(val, 'g', 15);
     resultWidget->setResult(currentInput);
 }

 void MainWindow::math_operations()
 {
     QPushButton *button = (QPushButton*)sender();
     num_first = currentInput.toDouble();
     currentInput = "0";
     resultWidget->setResult("0");
     button->setChecked(true);
 }

 void MainWindow::on_pushButton_AC_clicked()
 {
     ui->pushButton_plus->setChecked(false);
     ui->pushButton_diff->setChecked(false);
     ui->pushButton_divide->setChecked(false);
     ui->pushButton_multi->setChecked(false);

     historyWidget->clearHistory();
     currentInput = "0";
     num_first = 0;
     resultWidget->setResult("0");
 }


 void MainWindow::on_pushButton_equal_clicked()
 {
     double labelNumber = 0.0;
     double num_second = currentInput.toDouble();
     QString operation;
     QString result;

     if (ui->pushButton_plus->isChecked()) {
         labelNumber = num_first + num_second;
         operation = "+";
         ui->pushButton_plus->setChecked(false);
     }
     else if (ui->pushButton_diff->isChecked()) {
         labelNumber = num_first - num_second;
         operation = "-";
         ui->pushButton_diff->setChecked(false);
     }
     else if(ui->pushButton_divide->isChecked()){
         operation = "/";
         if(num_second == 0){
             resultWidget->setResult("Error");
             historyWidget->addToHistory(QString("%1 / %2 = Error")
                 .arg(num_first).arg(num_second));
              ui->pushButton_divide->setChecked(false);
             return;
         } else {
             labelNumber = num_first / num_second;
             ui->pushButton_divide->setChecked(false);
         }
     } else if(ui->pushButton_multi->isChecked()){
         labelNumber = num_first * num_second;
         operation = "*";
         ui->pushButton_multi->setChecked(false);
     }

     if(operation.isEmpty())
         return;

     QString type = ui->comboBox->currentText();

     if (type == "Whole numbers") {
         labelNumber = qRound(labelNumber);
         result = QString::number(labelNumber, 'f', 0);
     }else{
         result = QString::number(labelNumber, 'g', 15);
     }
     resultWidget->setResult(result);
     currentInput = result;

     QString record = QString("%1 %2 %3 = %4")
                          .arg(num_first)
                          .arg(operation)
                          .arg(num_second)
                          .arg(currentInput);
     historyWidget->addToHistory(record);

 }

 void MainWindow::advanced_operations(){
     QPushButton *button = (QPushButton *)sender();
     double value = currentInput.toDouble();
     double value_copy = currentInput.toDouble();
     double result = 0.0;
     QString operation;
     QString resText;

     QString mode = ui->comboUnits_2->currentText();

     if (button->text() == "√") {
         if (value < 0) {
             resultWidget->setResult("Error"); return; }
             operation = "√";
             result = qSqrt(value);
     }
     else if (button->text() == "sin") {
         value = (mode == "Degrees" ? qDegreesToRadians(value) : value);
         operation = "sin";
         result = qSin(value);
     }
     else if (button->text() == "cos") {
         value = (mode == "Degrees" ? qDegreesToRadians(value) : value);
         operation = "cos";
         result = qCos(value);
     }
     else if (button->text() == "tg") {
         value = (mode == "Degrees" ? qDegreesToRadians(value) : value);
         operation = "tg";
         result = qTan(value);
     }
     else if (button->text() == "log") {
         if (value <= 0) {
             resultWidget->setResult("Error"); return; }
             operation = "log";
             result = qLn(value) / qLn(10);
     }
     QString type = ui->comboBox->currentText();
     if (type == "Whole numbers") {
         resText = QString::number(qRound(result), 'f', 0);
     } else {
         resText = QString::number(result, 'g', 15);
     }
     currentInput = resText;
     resultWidget->setResult(currentInput);

     QString numbersSymbol = QString::number(value_copy);
     if(mode == "Degrees") {
        numbersSymbol = QString::number(value_copy) +  "°";
     }

     QString record = QString("%1(%2) = %3")
                          .arg(operation)
                              .arg(numbersSymbol)
                          .arg(resText);

     historyWidget->addToHistory(record);
 }


 void MainWindow::on_pushButton_Clear_clicked()
 {
    currentInput = "0";
    resultWidget->setResult("0");

 }

