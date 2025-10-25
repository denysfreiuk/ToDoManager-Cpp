#include "taskeditorwindow.h"
#include "ui_taskeditorwindow.h"
#include <QMessageBox>
#include <QDate>

TaskEditorWindow::TaskEditorWindow(QWidget *parent)
    : QDialog(parent),
      ui(new Ui::TaskEditorWindow)
{
    ui->setupUi(this);
    setWindowTitle("New Task");
    resize(400, 300);

    ui->deadlineEdit->setDate(QDate::currentDate());
    ui->priorityBox->addItems({"Low", "Medium", "High"});

    connect(ui->saveButton, &QPushButton::clicked, this, &TaskEditorWindow::saveTask);
    connect(ui->cancelButton, &QPushButton::clicked, this, &TaskEditorWindow::reject);
}

TaskEditorWindow::~TaskEditorWindow() {
    delete ui;
}

void TaskEditorWindow::saveTask() {
    QString title = ui->titleEdit->text().trimmed();
    if (title.isEmpty()) {
        QMessageBox::warning(this, "Warning", "Task title cannot be empty!");
        return;
    }
    accept();
}

Task TaskEditorWindow::getTask() const {
    return Task(
        ui->titleEdit->text(),
        ui->descriptionEdit->toPlainText(),
        ui->deadlineEdit->date(),
        ui->priorityBox->currentText(),
        false
    );
}

void TaskEditorWindow::setTask(const Task& task) {
    ui->titleEdit->setText(task.getTitle());
    ui->descriptionEdit->setText(task.getDescription());
    ui->deadlineEdit->setDate(task.getDeadline());
    int index = ui->priorityBox->findText(task.getPriority());
    if (index >= 0) ui->priorityBox->setCurrentIndex(index);
}
