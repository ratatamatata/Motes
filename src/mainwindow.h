#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileSystemModel>
#include <QSettings>
#include <thread>
#include "settingswidget.h"
#include "CloudControl.h"

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
    void editTreeElement();
    void deleteTreeElement();
    void newFile();
    void settingsWidget();

private:
    Ui::MainWindow *ui;
    QFileSystemModel *model;
    MotesSettings* settings;
    YandexCloudControl* yandexDisk;
    QString* home_folder;
    QString* editor_path;
    std::thread* getTokenThr;

};

#endif // MAINWINDOW_H
