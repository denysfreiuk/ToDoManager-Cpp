#include "taskitemwidget.h"
#include "ui_taskitemwidget.h"
#include <QFont>
#include <QEnterEvent>
#include <QPalette>

TaskItemWidget::TaskItemWidget(const Task& t, QWidget *parent)
    : QWidget(parent),
      ui(new Ui::TaskItemWidget),
      task(t),
      btnEffect(new QGraphicsOpacityEffect(this)),
      fadeAnim(new QPropertyAnimation(btnEffect, "opacity", this))
{
    ui->setupUi(this);
    setMinimumHeight(70);
    setFocusPolicy(Qt::NoFocus);
    setAttribute(Qt::WA_Hover, true);
    setAutoFillBackground(true);

    QPalette pal = palette();
    pal.setColor(QPalette::Window, QColor(245, 245, 245));
    setPalette(pal);

    ui->mainLayout->setAlignment(Qt::AlignVCenter);
    ui->titleLabel->setText(task.toDisplayString());
    ui->titleLabel->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    ui->mainLayout->setContentsMargins(8, 4, 8, 4);
    ui->mainLayout->setSpacing(8);

    QWidget *btnContainer = new QWidget(this);
    QHBoxLayout *btnLayout = new QHBoxLayout(btnContainer);
    btnLayout->setContentsMargins(0, 0, 0, 0);
    btnLayout->setSpacing(6);
    btnLayout->addWidget(ui->editButton);
    btnLayout->addWidget(ui->doneButton);
    btnLayout->addWidget(ui->detailsButton);
    btnLayout->addWidget(ui->deleteButton);
    btnContainer->setGraphicsEffect(btnEffect);
    ui->mainLayout->addWidget(btnContainer);

    btnEffect->setOpacity(0.0);

    connect(ui->editButton, &QPushButton::clicked, [this]() { emit requestEdit(task); });
    connect(ui->doneButton, &QPushButton::clicked, [this]() { emit requestDone(task); });
    connect(ui->detailsButton, &QPushButton::clicked, [this]() { emit requestDetails(task); });
    connect(ui->deleteButton, &QPushButton::clicked, [this]() {emit requestDelete(task);});

    setFocusPolicy(Qt::NoFocus);
    setAttribute(Qt::WA_Hover, true);

    updateStyle();
}

TaskItemWidget::~TaskItemWidget() {
    delete ui;
}

Task TaskItemWidget::getTask() const {
    return task;
}

void TaskItemWidget::updateStyle() {
    QFont f = ui->titleLabel->font();

    if (task.isCompleted()) {
        f.setStrikeOut(true);
        ui->titleLabel->setFont(f);
        ui->titleLabel->setStyleSheet("color: darkgreen;");
        ui->doneButton->setEnabled(false);
        ui->doneButton->setToolTip("Already completed");
    }
    else if (task.getDeadline() < QDate::currentDate()) {
        ui->titleLabel->setStyleSheet("color: red;");
        ui->doneButton->setEnabled(true);
        ui->doneButton->setToolTip("Mark task as done");
    }
    else {
        ui->titleLabel->setStyleSheet("color: black;");
        ui->doneButton->setEnabled(true);
        ui->doneButton->setToolTip("Mark task as done");
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

