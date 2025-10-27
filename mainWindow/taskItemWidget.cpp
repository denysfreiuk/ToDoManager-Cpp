#include "taskitemwidget.h"
#include "ui_taskitemwidget.h"
#include <string>
#include <QHBoxLayout>
#include <QDate>
#include <QEnterEvent>
#include <QMouseEvent>
#include <QFont>
#include <QPixmap>

using namespace std;

TaskItemWidget::TaskItemWidget(const Task& t, QWidget *parent)
    : QWidget(parent),
      ui(new Ui::TaskItemWidget),
      task(t),
      btnEffect(new QGraphicsOpacityEffect(this)),
      fadeAnim(new QPropertyAnimation(btnEffect, "opacity", this))
{
    ui->setupUi(this);


    QWidget *btnContainer = new QWidget(this);
    QHBoxLayout *btnLayout = new QHBoxLayout(btnContainer);
    btnLayout->addWidget(ui->editButton);
    btnLayout->addWidget(ui->doneButton);
    btnLayout->addWidget(ui->detailsButton);
    btnLayout->addWidget(ui->deleteButton);
    btnContainer->setGraphicsEffect(btnEffect);
    ui->mainLayout->addWidget(btnContainer);

    btnEffect->setOpacity(0.0);

    connect(ui->editButton,    &QPushButton::clicked, [this]() { emit requestEdit(task); });
    connect(ui->doneButton,    &QPushButton::clicked, [this]() { emit requestDone(task); });
    connect(ui->detailsButton, &QPushButton::clicked, [this]() { emit requestDetails(task); });
    connect(ui->deleteButton,  &QPushButton::clicked, [this]() { emit requestDelete(task); });

    updateDisplay();
    setStyleSheet(qApp->styleSheet());
}

TaskItemWidget::~TaskItemWidget() {
    delete ui;
}

Task TaskItemWidget::getTask() const {
    return task;
}

void TaskItemWidget::mouseDoubleClickEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton)
        emit requestDetails(task);
    QWidget::mouseDoubleClickEvent(event);
}

void TaskItemWidget::updateDisplay() {
    ui->titleLabel->setText(task.getTitle());
    ui->titleLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    QDate deadline = task.getDeadline();
    if (deadline.isValid())
        ui->labelDeadline->setText(deadline.toString("dd MMM yyyy"));
    else
        ui->labelDeadline->setText("No deadline");

    bool done = task.isCompleted();

    if (done) {
        ui->labelCheck->setPixmap(QPixmap(":/resources/icons/check.png")
            .scaled(20, 20, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        ui->labelCheck->setVisible(true);

        ui->labelPriority->setVisible(false);

        ui->doneButton->setVisible(false);

        ui->titleLabel->setStyleSheet("color: #666; text-decoration: line-through;");
        ui->labelDeadline->setStyleSheet("color: #999;");
    } else {
        ui->labelCheck->setVisible(false);

        string pr = task.getPriority().toStdString();
        QString iconPath;
        if (pr == "Low") {
            iconPath = ":/resources/icons/priority_low.png";
        } else if (pr == "Medium") {
            iconPath = ":/resources/icons/priority_medium.png";
        } else if (pr == "High") {
            iconPath = ":/resources/icons/priority_high.png";
        }

        if (!iconPath.isEmpty()) {
            ui->labelPriority->setPixmap(QPixmap(iconPath)
                .scaled(20, 20, Qt::KeepAspectRatio, Qt::SmoothTransformation));
            ui->labelPriority->setVisible(true);
        } else {
            ui->labelPriority->setVisible(false);
        }

        ui->doneButton->setVisible(true);
        ui->doneButton->setEnabled(true);
        ui->doneButton->setToolTip("Mark task as done");

        ui->titleLabel->setStyleSheet("color: #222; text-decoration: none;");
        ui->labelDeadline->setStyleSheet("color: #555;");
    }
}


void TaskItemWidget::setButtonsVisible(bool visible) {
    fadeAnim->stop();
    fadeAnim->setDuration(250);
    fadeAnim->setStartValue(btnEffect->opacity());
    fadeAnim->setEndValue(visible ? 1.0 : 0.0);
    fadeAnim->start();
}

void TaskItemWidget::enterEvent(QEnterEvent *event) {
    setButtonsVisible(true);
    QWidget::enterEvent(event);
}

void TaskItemWidget::leaveEvent(QEvent *event) {
    setButtonsVisible(false);
    QWidget::leaveEvent(event);
}
