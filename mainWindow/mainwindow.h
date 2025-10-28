#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QSoundEffect>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QSet>
#include <QListWidget>
#include "../tasks/taskmanager.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(TaskManager &manager, QWidget *parent = nullptr);
    ~MainWindow();

    private slots:
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

    void initToolBoxSections();
    void clearAllLists();
    void loadTasks();
    void addTaskToToolBox(const Task &task);
    void updateToolBoxTitles();
    void updateToolBoxTitleFor(QListWidget *list);
    QWidget* toolBoxPageFor(QWidget *child) const;

    void applySettings();
    void enforceAutoDelete();
    void startOrStopReminders();
};

#endif // MAINWINDOW_H
