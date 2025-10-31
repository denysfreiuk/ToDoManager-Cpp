#ifndef TASKMANAGER_H
#define TASKMANAGER_H

#include <optional>
#include <string>
#include "../databaseManager/taskRepository.h"
#include "../logger/globalLogger.h"
#include "../tasks/task.h"
#include <QWidget>
using namespace std;
class TaskManager {
private:
    TaskRepository& repo;
    string currentUser;

public:
    explicit TaskManager(TaskRepository& repository);

    void setCurrentUser(const ::string& username);

    bool addTask(QWidget* parent, const Task& task);
    bool removeTask(QWidget* parent, const ::string& title);
    bool markCompleted(QWidget* parent, const ::string& title);
    bool updateTask(QWidget* parent, const Task& task);
    vector<Task> tasksForToday(bool includeCompleted = true);
    vector<Task> loadTasks();
};

#endif //TASKMANAGER_H
