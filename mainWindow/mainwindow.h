#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QListWidget>
#include <QComboBox>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QTimer>
#include "../tasks/TaskManager.h"
#include "taskItemWidget.h"
#include "taskEditorWindow.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(TaskManager& manager, QWidget *parent = nullptr);
    ~MainWindow();

    private slots:
        void addQuickTask();
    void showTaskDetails();
    void openTaskEditor();
    void onFilterChanged(int index);

    void handleTaskEdit(const Task& task);
    void handleTaskDone(const Task& task);
    void handleTaskDetails(const Task& task);
    void handleTaskDelete(const Task& task);

private:
    Ui::MainWindow *ui;
    TaskManager& taskManager;

    void loadTasks();
    void addTaskToToolBox(const Task& task);
    void clearAllLists();
};
#endif //MAINWINDOW_H
