#ifndef TASKITEMWIDGET_H
#define TASKITEMWIDGET_H
#include <QWidget>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include "../tasks/task.h"

QT_BEGIN_NAMESPACE
namespace Ui { class TaskItemWidget; }
QT_END_NAMESPACE

class TaskItemWidget : public QWidget {
    Q_OBJECT

public:
    explicit TaskItemWidget(const Task& task, QWidget *parent = nullptr);
    ~TaskItemWidget();

    Task getTask() const;
    void updateStyle();

    signals:
        void requestEdit(const Task& task);
    void requestDone(const Task& task);
    void requestDetails(const Task& task);
    void requestDelete(const Task& task);

protected:
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;

private:
    Ui::TaskItemWidget *ui;
    Task task;
    QGraphicsOpacityEffect *btnEffect;
    QPropertyAnimation *fadeAnim;
    void setButtonsVisible(bool visible);
};

#endif //TASKITEMWIDGET_H
