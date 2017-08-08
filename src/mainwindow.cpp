#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QWebEngineView>
#include <QWebEngineSettings>
#include <QModelIndex>
#include <QFileDialog>
#include <QSettings>
#include <QMessageBox>

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
    //Configure webView
    ui->webView->setUrl((QUrl("file:" + *home_folder + "/.converted/markdown.html")));
    auto webEngineSettings = ui->webView->settings();
    webEngineSettings->setAttribute(QWebEngineSettings::JavascriptEnabled, false);
    webEngineSettings->setAttribute(QWebEngineSettings::WebGLEnabled, false);
    webEngineSettings->setAttribute(QWebEngineSettings::Accelerated2dCanvasEnabled, false);
    //Configure treeView
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
    ui->treeView->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
    connect(ui->actionNew_File, &QAction::triggered, this, &MainWindow::newFile);
    connect(ui->actionSettings, &QAction::triggered, this, &MainWindow::settingsWidget);
    // Connect to cloud
    yandexDisk = new YandexCloudControl(home_folder->toStdString());
    ui->webView->setUrl(QUrl("https://oauth.yandex.ru/authorize?response_type=code&client_id=cb5b4cad0f46478c9dd05becdfd6ba6b"));
    auto cloudFunc = [this](std::string home_folder)
    {
        yandexDisk->getToken();
        yandexDisk->syncWithCloud();
        yandexDisk->watchFolder(home_folder);
    };
    getTokenThr = new std::thread(cloudFunc, home_folder->toStdString());
}

MainWindow::~MainWindow()
{
    delete ui;
    delete model;
    delete settings;
    delete yandexDisk;
    getTokenThr->join();
    delete getTokenThr;

}

void MainWindow::on_treeView_doubleClicked(const QModelIndex &index)
{
    if(!model->isDir(index))
    {
        auto file_name = index.data().toString();
        auto file_path = model->filePath(index);
        QString path_to_html_add = "/.converted/" + file_name.replace(file_name.size()-3, 3, ".html");
        file_path.replace(file_path.size()-file_name.size()+1, file_name.size(), path_to_html_add);
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
    if (QMessageBox::Yes == QMessageBox::question(this, "Delete this file?", "Are you sure?",
                          QMessageBox::Yes|QMessageBox::No))
    {
        auto currentIndex = this->ui->treeView->currentIndex();
        auto file_path = model->filePath(currentIndex);
        if(!model->isDir(currentIndex))
        {
            QFile file(file_path);
            file.remove();
        }
        else
        {
            QDir dir(file_path);
            bool result = dir.removeRecursively();
            qDebug() << "The directory remove operation " << (result ? "finished successfully" : "failed");
        }
    }
}

void MainWindow::newFile()
{
    QFileDialog newFile;
    auto fileName = QFileDialog::getSaveFileName(nullptr,
        tr("New File"), *home_folder, tr("Markdown Files (*.md)"));
    if(!fileName.isEmpty())
    {
        if(!fileName.endsWith(".md")) fileName += ".md";
        QFile file(fileName);
        file.open(QIODevice::WriteOnly);
        file.close();
        auto exec = editor_path->toStdString() + " " + fileName.toStdString();
        system(exec.c_str());
    }
}

void MainWindow::settingsWidget()
{
    auto settingsWidget = new SettingsWidget(settings);
}
