#ifndef TASKMANAGER_H
#define TASKMANAGER_H

#include <optional>
#include <string>
#include "../databaseManager/taskRepository.h"
#include "../logger/globalLogger.h"
#include "../tasks/task.h"
#include <QWidget>

class TaskManager {
private:
    TaskRepository& repo;
    std::string currentUser;

public:
    explicit TaskManager(TaskRepository& repository);

    void setCurrentUser(const std::string& username);

    bool addTask(QWidget* parent, const Task& task);
    bool removeTask(QWidget* parent, const std::string& title);
    bool markCompleted(QWidget* parent, const std::string& title);
    bool updateTask(QWidget* parent, const Task& task);
    bool deleteTask(QWidget* parent, const std::string& title);
    std::vector<Task> loadTasks();
};

#endif //TASKMANAGER_H
