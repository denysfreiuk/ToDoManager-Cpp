#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QDate>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QGraphicsDropShadowEffect>
#include <QPropertyAnimation>

MainWindow::MainWindow(TaskManager& manager, QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      taskManager(manager)
{
    ui->setupUi(this);
    initToolBoxSections();
    updateToolBoxTitles();

    for (auto button : findChildren<QPushButton*>()) {
        auto *shadow = new QGraphicsDropShadowEffect(button);
        shadow->setBlurRadius(15);
        shadow->setOffset(0, 3);
        shadow->setColor(QColor(201, 66, 119, 80));
        button->setGraphicsEffect(shadow);
    }

    setWindowTitle("ToDo Manager");
    QFile file(":/styles/main.qss");
    if (file.open(QFile::ReadOnly)) {
        QString style = QLatin1String(file.readAll());
        setStyleSheet(style);
    } else {
        qWarning() << "Could not open style file!";
    }
    setAttribute(Qt::WA_Hover, true);
    setMouseTracking(true);

    ui->todayList->setSelectionMode(QAbstractItemView::NoSelection);
    ui->weekList->setSelectionMode(QAbstractItemView::NoSelection);
    ui->monthList->setSelectionMode(QAbstractItemView::NoSelection);
    ui->laterList->setSelectionMode(QAbstractItemView::NoSelection);
    ui->otherList->setSelectionMode(QAbstractItemView::NoSelection);

    connect(ui->quickAddButton, &QPushButton::clicked, this, &MainWindow::addQuickTask);
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
    updateToolBoxTitles();
}

void MainWindow::addTaskToToolBox(const Task& task) {
    QDate today = QDate::currentDate();
    QDate deadline = task.getDeadline();

    QListWidget *targetList = nullptr;

    if (deadline < today)                      {targetList = ui->otherList; }
    else if (deadline == today)                targetList = ui->todayList;
    else if (deadline <= today.addDays(7))     targetList = ui->weekList;
    else if (deadline <= today.addMonths(1))   targetList = ui->monthList;
    else if (deadline.isValid())               targetList = ui->laterList;
    else                                       targetList = ui->otherList;

    auto *item = new QListWidgetItem(targetList);
    auto *taskWidget = new TaskItemWidget(task);

    taskWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    taskWidget->setMinimumHeight(68);
    item->setSizeHint(QSize(0, taskWidget->minimumHeight() + 8));

    targetList->addItem(item);
    targetList->setItemWidget(item, taskWidget);

    targetList->setUniformItemSizes(false);
    targetList->setResizeMode(QListView::Adjust);
    targetList->setSelectionMode(QAbstractItemView::NoSelection);
    targetList->setFocusPolicy(Qt::NoFocus);
    targetList->setSpacing(6);
    targetList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    targetList->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

    connect(taskWidget, &TaskItemWidget::requestEdit,    this, &MainWindow::handleTaskEdit);
    connect(taskWidget, &TaskItemWidget::requestDone,    this, &MainWindow::handleTaskDone);
    connect(taskWidget, &TaskItemWidget::requestDetails, this, &MainWindow::handleTaskDetails);
    connect(taskWidget, &TaskItemWidget::requestDelete,  this, &MainWindow::handleTaskDelete);

    updateToolBoxTitleFor(targetList);
    taskWidget->setStyleSheet(qApp->styleSheet());
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
            updateToolBoxTitles();
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

void MainWindow::initToolBoxSections() {
    sections.clear();

    const QVector<QPair<QWidget*, QListWidget*>> pairs = {
        { ui->todayPage,  ui->todayList  },
        { ui->weekPage,   ui->weekList   },
        { ui->monthPage,  ui->monthList  },
        { ui->laterPage,  ui->laterList  },
        { ui->otherPage,  ui->otherList  }
    };

    for (const auto& p : pairs) {
        Section s;
        s.page  = p.first;
        s.list  = p.second;
        s.index = ui->toolBox->indexOf(s.page);
        if (s.index < 0) continue;

        s.baseTitle = ui->toolBox->itemText(s.index).trimmed();
        if (s.baseTitle.isEmpty()) {
            if (s.page == ui->todayPage) s.baseTitle = "Today";
            else if (s.page == ui->weekPage) s.baseTitle = "This Week";
            else if (s.page == ui->monthPage) s.baseTitle = "This Month";
            else if (s.page == ui->laterPage) s.baseTitle = "Later";
            else if (s.page == ui->otherPage) s.baseTitle = "Other";
        }

        sections.push_back(s);

        if (s.list && s.list->model()) {
            auto mdl = s.list->model();
            connect(mdl, &QAbstractItemModel::rowsInserted, this,
                    [this, lw=s.list](const QModelIndex&, int, int){ updateToolBoxTitleFor(lw); });
            connect(mdl, &QAbstractItemModel::rowsRemoved, this,
                    [this, lw=s.list](const QModelIndex&, int, int){ updateToolBoxTitleFor(lw); });
            connect(mdl, &QAbstractItemModel::modelReset,  this,
                    [this, lw=s.list](){ updateToolBoxTitleFor(lw); });
        }
    }
}

void MainWindow::updateToolBoxTitles() {
    for (const auto& s : sections) {
        if (!s.page || !s.list || s.index < 0) continue;
        const int count = s.list->count();
        ui->toolBox->setItemText(s.index, QString("%1 (%2)").arg(s.baseTitle).arg(QString::number(count)));
    }
}

void MainWindow::updateToolBoxTitleFor(QListWidget* list) {
    for (const auto& s : sections) {
        if (s.list == list && s.index >= 0) {
            ui->toolBox->setItemText(s.index, QString("%1 (%2)").arg(s.baseTitle).arg(QString::number(list->count())));
            break;
        }
    }
}

QWidget* MainWindow::toolBoxPageFor(QWidget* child) const {
    QWidget* p = child;
    while (p && p->parentWidget() != ui->toolBox) p = p->parentWidget();
    return p;
}

