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

    setStyleSheet(R"(

/* === Повна ізоляція від глобальних стилів === */
* {
    font-family: 'Segoe UI', 'Roboto', sans-serif;
    font-weight: 500;
    color: #1e293b;
    background: transparent;
    border: none;
}

/* === Контейнер TaskItemWidget === */
TaskItemWidget {
    background-color: #ffffff;
    border: 1px solid #d0d7e2;
    border-radius: 8px;
    padding: 8px 12px;
}

/* === Hover на весь віджет === */
TaskItemWidget:hover {
    background-color: #eaf2ff;
    border: 1px solid #aac4ff;
}

/* === Основний текст === */
#titleLabel {
    font-weight: 600;
    font-size: 15px;
    color: #1e293b;
    qproperty-alignment: 'AlignVCenter | AlignLeft';
}

/* === Completed === */
.completed #titleLabel {
    color: #2e7d32;
    text-decoration: line-through;
    font-weight: 600;
}

/* === Overdue === */
.overdue #titleLabel {
    color: #b71c1c;
    font-weight: 600;
}

/* === Кнопки === */
QPushButton {
    font-size: 13px;
    font-weight: 500;
    color: #1f2937;
    background-color: #f8fafc;
    border: 1px solid #d0d7e2;
    border-radius: 6px;
    padding: 4px 10px;
}

/* 🚫 Вимикаємо hover та pressed для кнопок */
QPushButton:hover,
QPushButton:pressed,
QPushButton:focus {
    background-color: #f8fafc;
    border: 1px solid #d0d7e2;
    color: #1f2937;
}

/* === Без фокусу/selection для самого TaskItemWidget === */
TaskItemWidget:focus,
TaskItemWidget:selected {
    outline: none;
    background-color: #eaf2ff;
}

QListWidget::item:selected {
    background: transparent;
    border: none;
}
QListWidget::item:focus {
    outline: none;
}

)");

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
    setStyleSheet("background-color: #e8f3ff; border-radius: 6px;");
    QWidget::enterEvent(event);
}

void TaskItemWidget::leaveEvent(QEvent *event) {
    setButtonsVisible(false);
    setStyleSheet("background-color: #f5f5f5;");
    QWidget::leaveEvent(event);
}

