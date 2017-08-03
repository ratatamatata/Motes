#ifndef SETTINGSWIDGET_H
#define SETTINGSWIDGET_H
#include "ui_settings_form.h"
#include <QSettings>
#include <QString>

class MotesSettings : public QSettings
{
    Q_OBJECT
public:
    explicit MotesSettings(const QString &organization, const QString &application = QString(), QSettings *parent = 0);
    ~MotesSettings();
    QString* getHome_folder();
    QString* getEditor_path();
    void changeWorkfolder(QString& str);
    void changeEditor(QString& str);
private:
    QString home_folder;
    QString editor_path;
};

class SettingsWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SettingsWidget(MotesSettings* set, QWidget *parent = 0);
    ~SettingsWidget();
private slots:
    void on_Cancel_released();
    void on_editorButton_released();
    void on_workfolderButton_released();
    void on_Save_released();

private:
    Ui::Settings *settings_ui;
    MotesSettings* settings;
    QString temp_home_folder;
    QString temp_editor_path;
};

#endif // SETTINGSWIDGET_H
