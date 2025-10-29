#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "../windowEdit/framelesswindow.h"
#include "../tasks/taskmanager.h"
#include "../settings/appSettings.h"
#include <QTimer>
#include <QSoundEffect>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QSet>
#include <QListWidget>
#include <QLabel>
#include <QPushButton>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include <QIcon>
#include <QApplication>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public FramelessWindow {
    Q_OBJECT

public:
    explicit MainWindow(TaskManager &manager, QWidget *parent = nullptr);
    ~MainWindow();

    private slots:
        void onThemeButtonClicked();
    void addQuickTask();
    void openTaskEditor();
    void handleTaskEdit(const Task &task);
    void handleTaskDone(const Task &task);
    void handleTaskDelete(const Task &task);
    void handleTaskDetails(const Task &task);
    void onFilterChanged(int index);
    void onSettingsClicked();
    void checkRemindersTick();

private:
    Ui::MainWindow *ui;
    TaskManager &taskManager;
    QSystemTrayIcon *trayIcon;
    QMenu *trayMenu;

    QTimer *reminderTimer = nullptr;
    QMediaPlayer *reminderPlayer = nullptr;
    QAudioOutput *audioOut = nullptr;
    QSoundEffect reminderSound;
    QSet<QString> remindedKeys;
    QTimer *autoDeleteTimer = nullptr;

    struct Section {
        QWidget *page = nullptr;
        QListWidget *list = nullptr;
        int index = -1;
        QString baseTitle;
    };
    QVector<Section> sections;

    void applyTrayTheme();
    void updateTrayTooltip();
    void updateMaximizeIcon(bool maxed);
    void initToolBoxSections();
    void clearAllLists();
    void loadTasks();
    void addTaskToToolBox(const Task &task);
    void updateToolBoxTitles();
    void updateToolBoxTitleFor(QListWidget *list);
    void applySettings();
    void enforceAutoDelete();
    void startOrStopReminders();
    void closeEvent(QCloseEvent *event);

    void setupTitleBar();

    signals:
    void themeChanged(AppSettings::Theme newTheme);

};

#endif // MAINWINDOW_H
