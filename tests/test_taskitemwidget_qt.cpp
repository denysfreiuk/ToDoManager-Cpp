
#include <gtest/gtest.h>
#include <QSignalSpy>
#include <QPushButton>
#include "../mainWindow/taskItemWidget.h"
#include "../tasks/task.h"


TEST(TaskItemWidgetQt, EmitsEditAndDeleteRequests) {
    Task model("A", "B", QDateTime::currentDateTime(), "Medium", false);
    TaskItemWidget w(model);
    QSignalSpy editSpy(&w, &TaskItemWidget::requestEdit);
    QSignalSpy delSpy (&w, &TaskItemWidget::requestDelete);

    auto btnEdit = w.findChild<QPushButton*>("editButton");
    auto btnDel  = w.findChild<QPushButton*>("deleteButton");

    ASSERT_TRUE(btnEdit) << "editButton not found in UI";
    ASSERT_TRUE(btnDel)  << "deleteButton not found in UI";

    btnEdit->click();
    btnDel->click();

    QCoreApplication::processEvents();

    EXPECT_GE(editSpy.count(), 1) << "Edit button should emit requestEdit()";
    EXPECT_GE(delSpy.count(), 1)  << "Delete button should emit requestDelete()";
}
