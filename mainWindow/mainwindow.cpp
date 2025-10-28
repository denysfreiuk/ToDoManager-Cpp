#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "taskitemwidget.h"
#include "taskeditorwindow.h"
#include "../settings/settingswindow.h"
#include "../settings/appsettings.h"

#include <QMessageBox>
#include <QDate>
#include <QFile>
#include <QDebug>
#include <QTimer>
#include <QSoundEffect>
#include <QFileInfo>

MainWindow::MainWindow(TaskManager &manager, QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      taskManager(manager)
{
    ui->setupUi(this);
    initToolBoxSections();
    updateToolBoxTitles();
    setWindowTitle("ToDo Manager");

    QMediaPlayer *test = new QMediaPlayer(this);
    QAudioOutput *out = new QAudioOutput(this);
    test->setAudioOutput(out);
    test->setSource(QUrl("qrc:/resources/sounds/ping.wav"));
    test->play();

    ui->todayList->setSelectionMode(QAbstractItemView::NoSelection);
    ui->weekList->setSelectionMode(QAbstractItemView::NoSelection);
    ui->monthList->setSelectionMode(QAbstractItemView::NoSelection);
    ui->laterList->setSelectionMode(QAbstractItemView::NoSelection);
    ui->otherList->setSelectionMode(QAbstractItemView::NoSelection);

    connect(ui->quickAddButton, &QPushButton::clicked, this, &MainWindow::addQuickTask);
    connect(ui->addTaskButton,  &QPushButton::clicked, this, &MainWindow::openTaskEditor);
    connect(ui->settingsButton, &QPushButton::clicked, this, &MainWindow::onSettingsClicked);

    ui->viewFilterBox->clear();
    ui->viewFilterBox->addItems({"All Tasks", "Overdue", "Completed", "Uncompleted"});
    connect(ui->viewFilterBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onFilterChanged);


    reminderTimer = new QTimer(this);
    connect(reminderTimer, &QTimer::timeout, this, &MainWindow::checkRemindersTick);

    audioOut = new QAudioOutput(this);
    audioOut->setVolume(0.9);
    reminderPlayer = new QMediaPlayer(this);
    reminderPlayer->setAudioOutput(audioOut);

    autoDeleteTimer = new QTimer(this);
    connect(autoDeleteTimer, &QTimer::timeout, this, [this](){ enforceAutoDelete(); loadTasks(); });

    startOrStopReminders();
    applySettings();
    loadTasks();
    enforceAutoDelete();

    autoDeleteTimer->start(1 * 60 * 1000);
}

MainWindow::~MainWindow() {
    delete ui;
}


void MainWindow::onSettingsClicked() {
    SettingsWindow dlg(this);
    if (dlg.exec() == QDialog::Accepted) {
        applySettings();
        enforceAutoDelete();
        startOrStopReminders();
        loadTasks();
    }
}

void MainWindow::applySettings() {
    QFile f;
    if (AppSettings::theme() == AppSettings::Theme::Dark)
        f.setFileName(":/styles/main_dark.qss");
    else
        f.setFileName(":/styles/main.qss");

    if (f.open(QFile::ReadOnly)) {
        qApp->setStyleSheet(QLatin1String(f.readAll()));
        f.close();
    }
}

void MainWindow::startOrStopReminders() {
    remindedKeys.clear();
    if (AppSettings::reminderEnabled()) {
        reminderTimer->start(60 * 1000);
        const QString path = AppSettings::reminderSound();
        if (!path.isEmpty() && QFileInfo::exists(path)) {
            reminderPlayer->setSource(QUrl::fromLocalFile(path));
        } else {
            reminderPlayer->setSource(QUrl());
        }
    } else {
        reminderTimer->stop();
    }
}


void MainWindow::checkRemindersTick() {
    if (!AppSettings::reminderEnabled())
        return;

    const int remindBeforeMin = AppSettings::reminderMinutes();
    const QDateTime now = QDateTime::currentDateTime();

    auto tasks = taskManager.loadTasks();
    for (const auto &task : tasks) {
        if (task.isCompleted())
            continue;

        const QDateTime deadline = task.getDeadline();
        if (!deadline.isValid())
            continue;

        const qint64 minutesLeft = now.secsTo(deadline) / 60;

        if (minutesLeft < 0)
            continue;

        const QString key = task.getTitle() + "|" + deadline.toString(Qt::ISODate);

        if (minutesLeft <= remindBeforeMin && !remindedKeys.contains(key)) {
            remindedKeys.insert(key);

            if (reminderPlayer && !reminderPlayer->source().isEmpty()) {
                reminderPlayer->stop();
                reminderPlayer->play();
            }
            QTimer::singleShot(300, this, [=]() {
                const QString msg = QString(
                        "Task: <b>%1</b><br>"
                        "Deadline: %2<br>"
                        "Time left: %3 min")
                        .arg(task.getTitle())
                        .arg(deadline.toString("dd.MM.yyyy hh:mm"))
                        .arg(minutesLeft);

                QMessageBox::information(this, "Reminder", msg);
            });
        }

        if (minutesLeft > remindBeforeMin) {
            remindedKeys.remove(key);
        }
    }
}

void MainWindow::enforceAutoDelete() {
    if (!AppSettings::autoDelete()) return;

    const auto mode = AppSettings::deletePeriod();
    const QDateTime now = QDateTime::currentDateTime();
    const auto tasks = taskManager.loadTasks();

    for (const auto &t : tasks) {
        if (!t.isCompleted()) continue;

        const QDateTime dl = t.getDeadline();
        bool del = false;

        switch (mode) {
            case AppSettings::AutoDeletePeriod::Immediately:
                del = true;
            break;
            case AppSettings::AutoDeletePeriod::After1Day:
                if (dl.isValid()) del = dl.addDays(1) <= now;
            break;
            case AppSettings::AutoDeletePeriod::After1Week:
                if (dl.isValid()) del = dl.addDays(7) <= now;
            break;
            case AppSettings::AutoDeletePeriod::AfterDeadline:
                if (dl.isValid()) del = dl <= now;
            break;
        }

        if (del) {
            taskManager.deleteTask(this, t.getTitle().toStdString());
        }
    }
}

void MainWindow::clearAllLists() {
    auto clear = [](QListWidget *list) {
        while (list->count() > 0) {
            QListWidgetItem *item = list->takeItem(0);
            QWidget *w = list->itemWidget(item);
            if (w) w->deleteLater();
            delete item;
        }
    };
    clear(ui->todayList);
    clear(ui->weekList);
    clear(ui->monthList);
    clear(ui->laterList);
    clear(ui->otherList);
}

void MainWindow::loadTasks() {
    clearAllLists();

    QDateTime today = QDateTime::currentDateTime();
    auto tasks = taskManager.loadTasks();
    int filter = ui->viewFilterBox->currentIndex();

    for (const auto &t : tasks) {
        bool overdue = (t.getDeadline() < today && !t.isCompleted());
        bool completed = t.isCompleted();
        bool uncompleted = !t.isCompleted();

        if (filter == 1 && !overdue) continue;
        if (filter == 2 && !completed) continue;
        if (filter == 3 && !uncompleted) continue;

        addTaskToToolBox(t);
    }
    updateToolBoxTitles();
}

void MainWindow::addTaskToToolBox(const Task &task) {
    QDateTime today = QDateTime::currentDateTime();
    QDateTime deadline = task.getDeadline();
    QListWidget *target = ui->otherList;

    if (deadline.date() < today.date()) target = ui->otherList;
    else if (deadline.date() == today.date()) target = ui->todayList;
    else if (deadline.date() <= today.date().addDays(7)) target = ui->weekList;
    else if (deadline.date() <= today.date().addMonths(1)) target = ui->monthList;
    else if (deadline.date().isValid()) target = ui->laterList;

    auto *item = new QListWidgetItem(target);
    auto *taskWidget = new TaskItemWidget(task);
    taskWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    taskWidget->setMinimumHeight(68);
    item->setSizeHint(QSize(0, taskWidget->minimumHeight() + 8));

    target->addItem(item);
    target->setItemWidget(item, taskWidget);

    connect(taskWidget, &TaskItemWidget::requestEdit,    this, &MainWindow::handleTaskEdit);
    connect(taskWidget, &TaskItemWidget::requestDone,    this, &MainWindow::handleTaskDone);
    connect(taskWidget, &TaskItemWidget::requestDetails, this, &MainWindow::handleTaskDetails);
    connect(taskWidget, &TaskItemWidget::requestDelete,  this, &MainWindow::handleTaskDelete);

    updateToolBoxTitleFor(target);
    taskWidget->setStyleSheet(qApp->styleSheet());
}

void MainWindow::updateToolBoxTitles() {
    for (const auto &s : sections) {
        if (!s.page || !s.list || s.index < 0) continue;
        ui->toolBox->setItemText(s.index,
                                 QString("%1 (%2)").arg(s.baseTitle).arg(s.list->count()));
    }
}

void MainWindow::updateToolBoxTitleFor(QListWidget *list) {
    for (const auto &s : sections) {
        if (s.list == list && s.index >= 0) {
            ui->toolBox->setItemText(s.index,
                                     QString("%1 (%2)").arg(s.baseTitle).arg(list->count()));
            break;
        }
    }
}

void MainWindow::addQuickTask() {
    QString text = ui->taskInput->text().trimmed();
    if (text.isEmpty()) {
        QMessageBox::warning(this, "Warning", "Please enter a task title!");
        return;
    }
    QDateTime defDeadline = AppSettings::computeQuickAddDeadline(QDateTime::currentDateTime());
    Task quickTask(text, "Quick added task", defDeadline, "Medium", false);
    if (taskManager.addTask(this, quickTask)) {
        enforceAutoDelete();
        ui->taskInput->clear();
        loadTasks();
    }
}

void MainWindow::openTaskEditor() {
    TaskEditorWindow editor(this);
    if (editor.exec() == QDialog::Accepted) {
        Task t = editor.getTask();
        if (taskManager.addTask(this, t)) {
            QMessageBox::information(this, "Success", "Task added successfully!");
            enforceAutoDelete();
            loadTasks();
        } else {
            QMessageBox::critical(this, "Error", "Failed to add task!");
        }
    }
}

void MainWindow::handleTaskEdit(const Task &task) {
    TaskEditorWindow ed(this);
    ed.setTask(task);
    if (ed.exec() == QDialog::Accepted) {
        Task upd = ed.getTask();
        if (taskManager.updateTask(this, upd)) {
            QMessageBox::information(this, "Updated", "Task updated successfully!");
            enforceAutoDelete();
            loadTasks();
        }
    }
}

void MainWindow::handleTaskDone(const Task &task) {
    if (task.isCompleted()) {
        QMessageBox::information(this, "Info", "Task already completed.");
        return;
    }
    if (!taskManager.markCompleted(this, task.getTitle().toStdString())) {
        QMessageBox::warning(this, "Error", "Failed to update status!");
        return;
    }
    enforceAutoDelete();
    loadTasks();
}

void MainWindow::handleTaskDelete(const Task &task) {
    if (QMessageBox::question(this, "Delete", "Delete \"" + task.getTitle() + "\"?")
        == QMessageBox::Yes) {
        if (taskManager.deleteTask(this, task.getTitle().toStdString()))
            loadTasks();
    }
}

void MainWindow::handleTaskDetails(const Task &task) {
    QString info = QString("Title: %1\nDescription: %2\nDeadline: %3\nPriority: %4\nCompleted: %5")
            .arg(task.getTitle())
            .arg(task.getDescription())
            .arg(task.getDeadline().toString("dd.MM.yyyy hh:mm"))
            .arg(task.getPriority())
            .arg(task.isCompleted() ? "Yes" : "No");
    QMessageBox::information(this, "Details", info);
}

void MainWindow::onFilterChanged(int) { loadTasks(); }

void MainWindow::initToolBoxSections() {
    sections.clear();
    const QVector<QPair<QWidget*, QListWidget*>> pairs = {
        {ui->todayPage,  ui->todayList},
        {ui->weekPage,   ui->weekList},
        {ui->monthPage,  ui->monthList},
        {ui->laterPage,  ui->laterList},
        {ui->otherPage,  ui->otherList}
    };

    for (const auto &p : pairs) {
        Section s;
        s.page = p.first;
        s.list = p.second;
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
    }
}
