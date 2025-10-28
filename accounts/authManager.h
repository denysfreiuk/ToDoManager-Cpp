#ifndef AUTHMANAGER_H
#define AUTHMANAGER_H

#include "account.h"
#include <QObject>
#include <optional>
#include <string>
#include <vector>

using namespace std;

class AccountRepository;

class AuthManager : public QObject {
    Q_OBJECT
private:
    AccountRepository& repo;

public:
    explicit AuthManager(AccountRepository& repository);

    optional<string> authenticate(QWidget *parent = nullptr);
};

#endif // AUTHMANAGER_H
