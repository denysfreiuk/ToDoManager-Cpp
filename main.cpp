#include "mainWindow/mainwindow.h"
#include "databaseManager/accountRepository.h"
#include "logger/globalLogger.h"
#include "databaseManager/TaskRepository.h"
#include "tasks/TaskManager.h"
#include <QApplication>
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

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    DatabaseManager db("todo.db");
    AccountRepository accRepo(db);
    AuthManager auth(accRepo);

    auto loggedUser = auth.authenticate(nullptr);
    if (!loggedUser.has_value()) {
        return 0;
    }

    TaskRepository taskRepo(db);
    TaskManager taskManager(taskRepo);
    taskManager.setCurrentUser(loggedUser.value());

    MainWindow mainWin(taskManager);
    mainWin.show();

    return app.exec();
}