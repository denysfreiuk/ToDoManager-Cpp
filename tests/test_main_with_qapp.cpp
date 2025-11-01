
#include <gtest/gtest.h>
#include <QApplication>

int main(int argc, char **argv) {
    qputenv("TEST_MODE", "1");
    qputenv("QT_QPA_PLATFORM", "offscreen");
    QApplication app(argc, argv);
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
