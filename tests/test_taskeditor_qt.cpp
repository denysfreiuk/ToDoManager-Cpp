
#include <gtest/gtest.h>
#include <QSignalSpy>
#include <QLineEdit>
#include <QTextEdit>
#include <QDateTimeEdit>
#include <QComboBox>
#include <QPushButton>
#include "../mainWindow/taskEditorWindow.h"
#include "../tasks/task.h"

TEST(TaskEditorWindowQt, EmitsAcceptedWhenSaving) {
    TaskEditorWindow w;
    QSignalSpy spy(&w, &QDialog::accepted);

    auto title  = w.findChild<QLineEdit*>("titleEdit");
    auto desc   = w.findChild<QTextEdit*>("descriptionEdit");
    auto dt     = w.findChild<QDateTimeEdit*>("deadlineEdit");
    auto prio   = w.findChild<QComboBox*>("priorityBox");
    auto btn    = w.findChild<QPushButton*>("saveButton");

    ASSERT_TRUE(title);
    ASSERT_TRUE(desc);
    ASSERT_TRUE(dt);
    ASSERT_TRUE(prio);
    ASSERT_TRUE(btn);

    title->setText("Test Task");
    desc->setPlainText("Desc");
    dt->setDateTime(QDateTime::fromString("2025-10-30T12:00:00", Qt::ISODate));
    int idx = prio->findText("High");
    if (idx < 0) idx = 0;
    prio->setCurrentIndex(idx);

    btn->click();
    QCoreApplication::processEvents();

    EXPECT_GE(spy.count(), 1) << "Dialog should emit accepted() after valid save";
}
