#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QWebEngineView>
#include <QModelIndex>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    model(new QFileSystemModel)
{
    ui->setupUi(this);
    ui->webView->setUrl((QUrl("file:/home/sabbat/.local/share/Todoom/.converted/markdown.html")));
    QStringList filters;
    filters.append("*.md");
    model->setNameFilters(filters);
    model->setRootPath("/home/sabbat/.local/share/Todoom");
    model->setNameFilterDisables(false);
    ui->treeView->setModel(model);
    ui->treeView->hideColumn(1);
    ui->treeView->hideColumn(2);
    ui->treeView->setRootIndex(model->index("/home/sabbat/.local/share/Todoom"));
    ui->treeView->setContextMenuPolicy(Qt::CustomContextMenu);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_treeView_doubleClicked(const QModelIndex &index)
{
    if(!model->isDir(index))
    {
        auto file_name = index.data().toString();
        auto file_path = model->filePath(index);
        QString path_to_html_add = "/.converted/" + file_name.replace(file_name.size()-3, 3, ".html");
        file_path.replace(file_path.size()-file_name.size()+1, file_name.size(), path_to_html_add);
        qDebug() << file_path;
        ui->webView->setUrl((QUrl("file:" + file_path)));
    }
}

void MainWindow::on_treeView_customContextMenuRequested(const QPoint &pos)
{
    QPoint globalpos = ui->treeView->mapToGlobal(pos);
    QMenu menu;
    QAction* actionEditElement = menu.addAction("Edit");
    connect(actionEditElement, &QAction::triggered, this, &MainWindow::edit);
    menu.exec(globalpos);
}

void MainWindow::edit()
{
    auto currentIndex = this->ui->treeView->currentIndex();
    if(!model->isDir(currentIndex))
    {
        auto file_path = model->filePath(currentIndex);
        qDebug() << file_path ;
        auto exec = "/usr/bin/subl3 " + file_path.toStdString();
        system(exec.c_str());
    }
}
