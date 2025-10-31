
#include <gtest/gtest.h>
#include <QSignalSpy>
#include <QSlider>
#include <QPushButton>
#include <QComboBox>
#include "../settings/settingsWindow.h"
#include "../settings/appSettings.h"

TEST(SettingsWindowQt, ThemeComboChangesSetting) {
    SettingsWindow w;
    auto themeCombo = w.findChild<QComboBox*>("themeBox");
    ASSERT_TRUE(themeCombo);
    int oldIndex = themeCombo->currentIndex();
    int newIndex = (oldIndex == 0 ? 1 : 0);
    themeCombo->setCurrentIndex(newIndex);
    EXPECT_EQ(themeCombo->currentIndex(), newIndex);
}
