#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QListWidget>
#include <QComboBox>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QTimer>
#include <QMap>
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
    struct Section {
        QWidget* page = nullptr;
        QListWidget* list = nullptr;
        int index = -1;
        QString baseTitle;
    };

    Ui::MainWindow *ui;
    TaskManager& taskManager;

    void loadTasks();
    void addTaskToToolBox(const Task& task);
    void updateToolBoxTitles();
    void clearAllLists();
    QVector<Section> sections;
    void initToolBoxSections();
    void updateToolBoxTitleFor(QListWidget* list);
    QWidget* toolBoxPageFor(QWidget* child) const;
};
#endif //MAINWINDOW_H
