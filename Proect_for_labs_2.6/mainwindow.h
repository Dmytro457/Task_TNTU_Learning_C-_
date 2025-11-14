#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "databaseconnect.h"
#include "productmodel.h"
#include "filterbychoose.h"
#include <QSqlTableModel>
#include <QSortFilterProxyModel>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void updateCategoryFilter();
    void onAddClicked();
    void onDeleteClicked();
    void onUpdateClicked();
    void on_pushButton_Filter_clicked();
    void onResetFilterClicked();
    void onTableDoubleClicked(const QModelIndex &index);

private:
    Ui::MainWindow *ui;
    databaseconnect *m_dbConnect;
    productmodel *model;
    filterbychoose *proxyModel;


};
#endif // MAINWINDOW_H
