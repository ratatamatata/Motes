#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QWebEngineView>
#include <QModelIndex>
#include <QFileDialog>
#include <QSettings>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    model(new QFileSystemModel)
{
    ui->setupUi(this);
    //Load configuration
    settings = new MotesSettings("SmallFlyes", "Motes");
    home_folder = settings->getHome_folder();
    editor_path = settings->getEditor_path();

    ui->webView->setUrl((QUrl("file:" + *home_folder + "/.converted/markdown.html")));
    qDebug() << *home_folder;
    QStringList filters;
    filters.append("*.md");
    model->setNameFilters(filters);
    model->setRootPath(*home_folder);
    model->setNameFilterDisables(false);
    ui->treeView->setModel(model);
    ui->treeView->hideColumn(1);
    ui->treeView->hideColumn(2);
    ui->treeView->setRootIndex(model->index(*home_folder));
    ui->treeView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->actionNew_File, &QAction::triggered, this, &MainWindow::newFile);
    connect(ui->actionSettings, &QAction::triggered, this, &MainWindow::settingsWidget);
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
        qDebug() << *editor_path;
        ui->webView->setUrl((QUrl("file:" + file_path)));
    }
}

void MainWindow::on_treeView_customContextMenuRequested(const QPoint &pos)
{
    QPoint globalpos = ui->treeView->mapToGlobal(pos);
    QMenu menu;
    QAction* actionEditElement = menu.addAction("Edit");
    connect(actionEditElement, &QAction::triggered, this, &MainWindow::editTreeElement);
    QAction* actionDeleteElement = menu.addAction("Delete");
    connect(actionDeleteElement, &QAction::triggered, this, &MainWindow::deleteTreeElement);
    menu.exec(globalpos);
}

void MainWindow::editTreeElement()
{
    auto currentIndex = this->ui->treeView->currentIndex();
    if(!model->isDir(currentIndex))
    {
        auto file_path = model->filePath(currentIndex);
        auto exec = editor_path->toStdString() + " " + file_path.toStdString();
        system(exec.c_str());
    }
}

void MainWindow::deleteTreeElement()
{
    auto currentIndex = this->ui->treeView->currentIndex();
    if(!model->isDir(currentIndex))
    {
        auto file_path = model->filePath(currentIndex);
        QFile file(file_path);
        file.remove();
    }
}

void MainWindow::newFile()
{
    QFileDialog newFile;
    auto fileName = QFileDialog::getSaveFileName(nullptr,
        tr("New File"), *home_folder, tr("Markdown Files (*.md)"));
    if(!fileName.endsWith(".md")) fileName += ".md";
    QFile file(fileName);
    file.open(QIODevice::WriteOnly);
    file.close();
    auto exec = editor_path->toStdString() + " " + fileName.toStdString();
    system(exec.c_str());
}

void MainWindow::settingsWidget()
{
    auto settingsWidget = new SettingsWidget(settings);
}
