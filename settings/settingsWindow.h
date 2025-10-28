#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H
#include <QDialog>
#include <QMediaPlayer>
#include <QAudioOutput>

namespace Ui { class SettingsWindow; }

class SettingsWindow : public QDialog {
    Q_OBJECT
public:
    explicit SettingsWindow(QWidget *parent = nullptr);
    ~SettingsWindow();

    private slots:
        void onSave();
    void onCancel();
    void onSelectSound();
    void onTestSound();
    void onAutoDeleteChanged(int state);
    void onReminderChanged(int state);

private:
    Ui::SettingsWindow *ui;
    QMediaPlayer *player = nullptr;
    QAudioOutput *audioOut = nullptr;
    void loadUiFromSettings();
    void applyVisibility();
};

#endif // SETTINGSWINDOW_H
