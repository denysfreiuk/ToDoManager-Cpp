#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QDialog>
#include <QSettings>

namespace Ui {
    class SettingsWindow;
}

class SettingsWindow : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsWindow(QWidget *parent = nullptr);
    ~SettingsWindow();

    void loadSettings();
    void saveSettings();

    private slots:
        void onSaveClicked();
    void onCancelClicked();
    void onSelectSoundClicked();

private:
    Ui::SettingsWindow *ui;
    QSettings settings;
};

#endif // SETTINGSWINDOW_H
