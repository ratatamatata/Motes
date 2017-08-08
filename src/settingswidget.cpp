#include "settingswidget.h"
#include <QDebug>
#include <QFileDialog>

SettingsWidget::SettingsWidget(MotesSettings* set, QWidget *parent):
    QWidget(parent),
    settings_ui(new Ui::Settings),
    settings(set)
{
    settings_ui->setupUi(this);
    this->setFixedSize(this->width(), this->height());
    this->show();
    connect(settings_ui->Cancel, &QAbstractButton::released, this, &SettingsWidget::on_Cancel_released);
    temp_editor_path = *settings->getEditor_path();
    temp_home_folder = *settings->getHome_folder();
    settings_ui->editorLine->setText(temp_editor_path);
    settings_ui->workfolderLine->setText(temp_home_folder);
}

SettingsWidget::~SettingsWidget()
{
    delete settings_ui;
}

void SettingsWidget::on_Cancel_released()
{
    temp_editor_path = *settings->getEditor_path();
    temp_home_folder = *settings->getHome_folder();
    this->~SettingsWidget();
}

void SettingsWidget::on_Save_released()
{
    settings->changeWorkfolder(temp_home_folder);
    settings->changeEditor(temp_editor_path);
    this->~SettingsWidget();
}

void SettingsWidget::on_editorButton_released()
{
    auto editorPath = QFileDialog::getOpenFileName(nullptr, tr("Choose editor"), tr("/"));
    if(!editorPath.isEmpty())
    {
        temp_editor_path = editorPath;
        settings_ui->editorLine->setText(editorPath);
    }
}

void SettingsWidget::on_workfolderButton_released()
{
    auto workfolderPath = QFileDialog::getOpenFileName(nullptr, tr("Choose workfolder"), tr("/"));
    if(!workfolderPath.isEmpty())
    {
        temp_home_folder = workfolderPath;
        settings_ui->workfolderLine->setText(*settings->getHome_folder());
    }
}

MotesSettings::MotesSettings(const QString &organization, const QString &application, QSettings *parent): QSettings(organization, application)
{
    if(!this->value("default_editor").isValid()) this->setValue("default_editor", "/usr/bin/subl3");
    if(!this->value("work_folder").isValid()) this->setValue("work_folder", QDir::homePath() + "/.local/share/Todoom/");
    qDebug() << this->fileName();
    home_folder = this->value("work_folder").toString();
    editor_path = this->value("default_editor").toString();
}

MotesSettings::~MotesSettings()
{
}

QString* MotesSettings::getHome_folder()
{
    return &home_folder;
}

QString* MotesSettings::getEditor_path()
{
    return &editor_path;
}

void MotesSettings::changeWorkfolder(QString &str)
{
    this->home_folder = str;
    this->setValue("work_folder", str);
}

void MotesSettings::changeEditor(QString &str)
{
    this->editor_path = str;
    this->setValue("default_editor", str);
}
