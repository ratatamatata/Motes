#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileSystemModel>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_treeView_doubleClicked(const QModelIndex &index);
    void on_treeView_customContextMenuRequested(const QPoint &pos);
    void edit();

private:
    Ui::MainWindow *ui;
    QFileSystemModel *model;
};

#endif // MAINWINDOW_H
