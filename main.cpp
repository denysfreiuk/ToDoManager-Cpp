#include "mainWindow/mainwindow.h"
#include "databaseManager/accountRepository.h"
#include "logger/globalLogger.h"
#include "logger/logger.h"
#ifdef _WIN32
#include <windows.h>
void enableANSI() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
}
#endif

#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    DatabaseManager db("toDo.db");
    AccountRepository accRepo(db);
    AuthManager auth(accRepo);

    while (true) {
        auto session = auth.authenticate(nullptr);
        if (!session.has_value()) break;

        MainWindow w;
        w.setWindowTitle(QString::fromStdString("Welcome, " + session.value()));
        w.show();

        a.exec();
        break;
    }

    return 0;
}
