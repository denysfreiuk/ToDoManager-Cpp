#include "authManager.h"
#include "../authWindow/accountWindow.h"
#include "../logger/globalLogger.h"
#include "../databaseManager/accountRepository.h"
#include <QMessageBox>
#include <QTimer>
#include <functional>
#include <algorithm>

using namespace std;

AuthManager::AuthManager(AccountRepository& repository)
    : repo(repository) {
    logger.debug("Initializing AuthManager (Qt GUI mode)");
    repo.initTable();

    auto all = repo.getAllAccounts();
    if (all.empty()) {
        logger.warn("No accounts found. Creating default admin...");
        repo.addAccount("admin", hash<string>{}("admin"));
        logger.info("Default account 'admin' created.");
    }
}


optional<string> AuthManager::authenticate(QWidget *parent) {
    AccountWindow dialog(parent);
    optional<string> loggedUser = nullopt;

    QObject::connect(&dialog, &AccountWindow::loginRequested,
                     [&](const string &user, const string &pass) {
        size_t h = hash<string>{}(pass);
        auto accOpt = repo.getAccount(user);

        if (accOpt.has_value() && accOpt->getPasswordHash() == h) {
            dialog.setStatus(QString::fromStdString("Welcome, " + user + "!"), false);
            logger.info("Login success: " + user);
            loggedUser = user;

            QTimer::singleShot(500, &dialog, &QDialog::accept);
        } else {
            dialog.setStatus("Incorrect username or password", true);
            logger.warn("Login failed for user: " + user);
        }
    });

    QObject::connect(&dialog, &AccountWindow::registerRequested,
                 [&](const string &user, const string &pass) {

    if (user.empty() || pass.empty()) {
        dialog.setStatus("Username and password cannot be empty!", true);
        logger.warn("Empty username or password during registration attempt.");
        return;
    }

    if (user.length() < 3) {
        dialog.setStatus("Username must be at least 3 characters long.", true);
        logger.warn("Too short username during registration: " + user);
        return;
    }
    if (pass.length() < 4) {
        dialog.setStatus("Password must be at least 4 characters long.", true);
        logger.warn("Too short password for user: " + user);
        return;
    }

    if (repo.accountExists(user)) {
        dialog.setStatus("This username already exists!", true);
        logger.warn("Registration failed: username already exists (" + user + ")");
        return;
    }

    size_t h = hash<string>{}(pass);
    bool ok = repo.addAccount(user, h);

    if (ok) {
        dialog.setStatus(QString::fromStdString("Account created: " + user), false);
        logger.info("New account registered successfully: " + user);
    } else {
        dialog.setStatus("Error creating account in database!", true);
        logger.error("Account registration failed in DB: " + user);
    }
});

    dialog.exec();
    return loggedUser;
}