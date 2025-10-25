#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QDate>

MainWindow::MainWindow(TaskManager& manager, QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      taskManager(manager)
{
    ui->setupUi(this);
    setWindowTitle("ToDo Manager");

    ui->todayList->setSelectionMode(QAbstractItemView::NoSelection);
    ui->weekList->setSelectionMode(QAbstractItemView::NoSelection);
    ui->monthList->setSelectionMode(QAbstractItemView::NoSelection);
    ui->laterList->setSelectionMode(QAbstractItemView::NoSelection);
    ui->otherList->setSelectionMode(QAbstractItemView::NoSelection);

    connect(ui->quickAddButton, &QPushButton::clicked, this, &MainWindow::addQuickTask);
    connect(ui->detailsButton,  &QPushButton::clicked, this, &MainWindow::showTaskDetails);
    connect(ui->addTaskButton,  &QPushButton::clicked, this, &MainWindow::openTaskEditor);

    ui->viewFilterBox->clear();
    ui->viewFilterBox->addItems({"All Tasks", "Overdue", "Completed", "Uncompleted"});
    connect(ui->viewFilterBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onFilterChanged);

    loadTasks();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::clearAllLists() {
    auto clearList = [](QListWidget* list){
        if (!list) return;
        while (list->count() > 0) {
            QListWidgetItem *item = list->takeItem(0);
            QWidget *widget = list->itemWidget(item);
            if (widget) {
                list->removeItemWidget(item);
                widget->deleteLater();
            }
            delete item;
        }
        list->update();
    };

    clearList(ui->todayList);
    clearList(ui->weekList);
    clearList(ui->monthList);
    clearList(ui->laterList);
    clearList(ui->otherList);
}

void MainWindow::loadTasks() {
    clearAllLists();

    QDate today = QDate::currentDate();
    auto tasks = taskManager.loadTasks();
    int filter = ui->viewFilterBox->currentIndex();

    for (const auto& t : tasks) {
        bool overdue   = (t.getDeadline() < today && !t.isCompleted());
        bool completed = t.isCompleted();
        bool uncompleted = !t.isCompleted();

        if (filter == 1 && !overdue)     continue;
        if (filter == 2 && !completed)   continue;
        if (filter == 3 && !uncompleted) continue;

        addTaskToToolBox(t);
    }
    ui->toolBox->update();
}

void MainWindow::addTaskToToolBox(const Task& task) {
    QDate today = QDate::currentDate();
    QDate deadline = task.getDeadline();

    QListWidget *targetList = nullptr;
    if (deadline < today) targetList = ui->otherList;
    else if (deadline == today) targetList = ui->todayList;
    else if (deadline <= today.addDays(7)) targetList = ui->weekList;
    else if (deadline <= today.addMonths(1)) targetList = ui->monthList;
    else if (deadline.isValid()) targetList = ui->laterList;
    else targetList = ui->otherList;

    QListWidgetItem *item = new QListWidgetItem();
    auto *taskWidget = new TaskItemWidget(task);
    item->setSizeHint(taskWidget->sizeHint());
    targetList->addItem(item);
    targetList->setItemWidget(item, taskWidget);

    connect(taskWidget, &TaskItemWidget::requestEdit,    this, &MainWindow::handleTaskEdit);
    connect(taskWidget, &TaskItemWidget::requestDone,    this, &MainWindow::handleTaskDone);
    connect(taskWidget, &TaskItemWidget::requestDetails, this, &MainWindow::handleTaskDetails);
    connect(taskWidget, &TaskItemWidget::requestDelete, this, &MainWindow::handleTaskDelete);

}

void MainWindow::addQuickTask() {
    QString text = ui->taskInput->text().trimmed();
    if (text.isEmpty()) {
        QMessageBox::warning(this, "Warning", "Please enter a task title!");
        return;
    }

    Task quickTask(text, "Quick added task", QDate::currentDate(), "Medium", false);
    if (taskManager.addTask(this, quickTask)) {
        loadTasks();
        ui->taskInput->clear();
    }
}

void MainWindow::showTaskDetails() {
    QListWidget *list = qobject_cast<QListWidget*>(ui->toolBox->currentWidget());
    if (!list || !list->currentItem()) {
        QMessageBox::information(this, "Info", "Select a task first!");
        return;
    }

    QString details = list->currentItem()->text();
    QMessageBox::information(this, "Task Details", details);
}

void MainWindow::openTaskEditor() {
    TaskEditorWindow editor(this);
    if (editor.exec() == QDialog::Accepted) {
        Task newTask = editor.getTask();
        if (taskManager.addTask(this, newTask)) {
            loadTasks();
            QMessageBox::information(this, "Success", "Task added successfully!");
        } else {
            QMessageBox::critical(this, "Error", "Failed to add task!");
        }
    }
}

void MainWindow::handleTaskDone(const Task& task) {
    if (task.isCompleted()) {
        QMessageBox::information(this, "Info", "Task is already completed.");
        return;
    }

    if (!taskManager.markCompleted(this, task.getTitle().toStdString())) {
        QMessageBox::warning(this, "Error", "Failed to update task status!");
        return;
    }

    if (auto *senderWidget = qobject_cast<TaskItemWidget*>(sender())) {
        auto *fade = new QGraphicsOpacityEffect(senderWidget);
        senderWidget->setGraphicsEffect(fade);

        auto *anim = new QPropertyAnimation(fade, "opacity");
        anim->setDuration(250);
        anim->setStartValue(1.0);
        anim->setEndValue(0.0);

        connect(anim, &QPropertyAnimation::finished, this, [this]() {
            QTimer::singleShot(50, this, [this]() { loadTasks(); });
        });

        anim->start(QAbstractAnimation::DeleteWhenStopped);
    } else {
        loadTasks();
    }
}

void MainWindow::handleTaskEdit(const Task& task) {
    TaskEditorWindow editor(this);

    editor.setTask(task);

    if (editor.exec() == QDialog::Accepted) {
        Task updated = editor.getTask();
        if (taskManager.updateTask(this, updated)) {
            QMessageBox::information(this, "Updated", "Task updated successfully!");
            loadTasks();
        } else {
            QMessageBox::warning(this, "Error", "Failed to update task in DB!");
        }
    }
}

void MainWindow::handleTaskDetails(const Task& task) {
    QString details = QString("Title: %1\nDescription: %2\nDeadline: %3\nPriority: %4\nCompleted: %5")
            .arg(task.getTitle())
            .arg(task.getDescription())
            .arg(task.getDeadline().toString("dd.MM.yyyy"))
            .arg(task.getPriority())
            .arg(task.isCompleted() ? "Yes" : "No");

    QMessageBox::information(this, "Task Details", details);
}

void MainWindow::handleTaskDelete(const Task& task) {
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Delete Task",
                                  "Are you sure you want to delete \"" + task.getTitle() + "\"?",
                                  QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        if (taskManager.deleteTask(this, task.getTitle().toStdString())) {
            QMessageBox::information(this, "Deleted", "Task deleted successfully!");
            loadTasks();
        } else {
            QMessageBox::warning(this, "Error", "Failed to delete task!");
        }
    }
}

void MainWindow::onFilterChanged(int index) {
    Q_UNUSED(index);
    loadTasks();
}
