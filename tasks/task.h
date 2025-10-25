#ifndef TASKS_H
#define TASKS_H

#include <QString>
#include <QDate>

class Task {
private:
    QString title;
    QString description;
    QDate deadline;
    QString priority;
    bool completed;

public:
    Task();
    Task(const QString &title, const QString &description,
         const QDate &deadline, const QString &priority, bool completed = false);

    QString getTitle() const;
    QString getDescription() const;
    QDate getDeadline() const;
    QString getPriority() const;
    bool isCompleted() const;

    void setTitle(const QString &value);
    void setDescription(const QString &value);
    void setDeadline(const QDate &value);
    void setPriority(const QString &value);
    void setCompleted(bool value);

    QString toDisplayString() const;
};


#endif
