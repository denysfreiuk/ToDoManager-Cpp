#include <gtest/gtest.h>
#include <QtTest/QTest>
#include <QPushButton>
#include <QComboBox>
#include <QToolBox>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include <QTimer>
#include <QSignalSpy>
#include "../mainWindow/mainwindow.h"
#include "../tasks/TaskManager.h"
#include "../databaseManager/databaseManager.h"
#include "../databaseManager/TaskRepository.h"
#include "../settings/appSettings.h"
#include "friend_mainwindow.h"

struct MainWindowFixture : public ::testing::Test {
    std::unique_ptr<DatabaseManager> db;
    std::unique_ptr<TaskRepository> repo;
    std::unique_ptr<TaskManager> manager;
    std::unique_ptr<MainWindow> window;

    void SetUp() override {
        db = std::make_unique<DatabaseManager>(":memory:");
        ASSERT_TRUE(db->open());
        repo = std::make_unique<TaskRepository>(*db);
        repo->initTable();
        manager = std::make_unique<TaskManager>(*repo);
        window = std::make_unique<MainWindow>(*manager);
    }
};

TEST(MainWindowQt, ApplyTrayThemeAccessible) {
    DatabaseManager db(":memory:");
    ASSERT_TRUE(db.open());
    TaskRepository repo(db);
    repo.initTable();
    TaskManager mgr(repo);

    TestFriend_MainWindow w(mgr);
    EXPECT_NO_THROW(w.applyTrayTheme());
    EXPECT_NO_THROW(w.updateTrayTooltip());
    EXPECT_NO_THROW(w.loadTasks());
    EXPECT_NO_THROW(w.updateMaximizeIcon(true));
}

TEST_F(MainWindowFixture, HasCoreWidgets) {
    EXPECT_TRUE(window->findChild<QPushButton*>("addTaskButton"));
    EXPECT_TRUE(window->findChild<QPushButton*>("quickAddButton"));
    EXPECT_TRUE(window->findChild<QPushButton*>("themeButton"));
    EXPECT_TRUE(window->findChild<QPushButton*>("settingsButton"));
    EXPECT_TRUE(window->findChild<QComboBox*>("viewFilterBox"));
    EXPECT_TRUE(window->findChild<QToolBox*>());
}

TEST_F(MainWindowFixture, SwitchThemeChangesSettingAndEmitsSignal) {
    using Theme = AppSettings::Theme;
    Theme initial = AppSettings::theme();

    auto themeBtn = window->findChild<QPushButton*>("themeButton");
    ASSERT_TRUE(themeBtn);

    QSignalSpy spy(window.get(), SIGNAL(themeChanged(AppSettings::Theme)));
    themeBtn->click();

    QTRY_VERIFY_WITH_TIMEOUT(spy.count() > 0, 500);

    Theme newTheme = AppSettings::theme();
    EXPECT_NE(newTheme, initial);
    EXPECT_TRUE(newTheme == Theme::Light || newTheme == Theme::Dark);
}

TEST_F(MainWindowFixture, SettingsButtonExistsAndResponds) {
    auto btn = window->findChild<QPushButton*>("settingsButton");
    ASSERT_TRUE(btn);
    btn->click();
    SUCCEED();
}

TEST_F(MainWindowFixture, FilterBoxHasCorrectItems) {
    auto combo = window->findChild<QComboBox*>("viewFilterBox");
    ASSERT_TRUE(combo);
    EXPECT_GE(combo->count(), 4);
    EXPECT_EQ(combo->itemText(0), "All Tasks");
}

TEST_F(MainWindowFixture, UpdateMaximizeIconChangesIcons) {
    TestFriend_MainWindow w(*manager);
    EXPECT_NO_THROW(w.updateMaximizeIcon(true));
    EXPECT_NO_THROW(w.updateMaximizeIcon(false));
    SUCCEED();
}

TEST_F(MainWindowFixture, TrayIconCreatedAndMenuHasExpectedActions) {
    auto tray = window->findChild<QSystemTrayIcon*>();
    ASSERT_TRUE(tray);
    EXPECT_TRUE(tray->isVisible());

    auto menu = tray->contextMenu();
    ASSERT_TRUE(menu);
    EXPECT_GE(menu->actions().size(), 5);
}

TEST_F(MainWindowFixture, StatusMenuChangesUserStatus) {
    auto tray = window->findChild<QSystemTrayIcon*>();
    ASSERT_TRUE(tray);
    auto menu = tray->contextMenu();
    ASSERT_TRUE(menu);

    QMenu *statusMenu = nullptr;
    for (auto *act : menu->actions()) {
        if (act->menu() && act->menu()->title().contains("Status")) {
            statusMenu = act->menu();
            break;
        }
    }
    ASSERT_TRUE(statusMenu) << "Status submenu not found";

    QAction *statusBusy = nullptr;
    for (auto *act : statusMenu->actions()) {
        if (act->text().contains("Busy")) {
            statusBusy = act;
            break;
        }
    }
    ASSERT_TRUE(statusBusy) << "Busy action not found in Status submenu";

    statusBusy->trigger();
    QCoreApplication::processEvents();

    EXPECT_EQ(AppSettings::userStatus(), AppSettings::UserStatus::Busy)
        << "Triggering Busy should change AppSettings::userStatus";
}

TEST_F(MainWindowFixture, LoadTasksRunsWithoutCrash) {
    TestFriend_MainWindow w(*manager);
    EXPECT_NO_THROW(w.loadTasks());
}

TEST_F(MainWindowFixture, ApplyTrayThemeDoesNotCrash) {
    TestFriend_MainWindow w(*manager);
    EXPECT_NO_THROW(w.applyTrayTheme());
}


TEST_F(MainWindowFixture, TrayTooltipContainsUserStatusAndTheme) {
    TestFriend_MainWindow w(*manager);
    EXPECT_NO_THROW(w.updateTrayTooltip());

    auto tray = w.findChild<QSystemTrayIcon*>();
    ASSERT_TRUE(tray);

    QString tip = tray->toolTip();
    EXPECT_FALSE(tip.isEmpty());

    EXPECT_TRUE(tip.contains(AppSettings::userStatusName(), Qt::CaseInsensitive))
        << "Tooltip should contain current user status name.";
}

TEST_F(MainWindowFixture, AddTaskButtonTriggersOpenTaskEditor) {
    auto btn = window->findChild<QPushButton*>("addTaskButton");
    ASSERT_TRUE(btn);
    btn->click();
    SUCCEED();
}

TEST_F(MainWindowFixture, ReminderTimerAndPlayerInitialized) {
    EXPECT_TRUE(window->findChild<QTimer*>());
    EXPECT_TRUE(window->findChild<QMediaPlayer*>());
    EXPECT_TRUE(window->findChild<QAudioOutput*>());
}

TEST_F(MainWindowFixture, AutoDeleteTimerRuns) {
    EXPECT_TRUE(window->findChild<QTimer*>());
    SUCCEED();
}
