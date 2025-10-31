#ifndef AUTHMANAGER_H
#define AUTHMANAGER_H

#include <optional>
#include <QString>

class AccountRepository;
class QWidget;

class AuthManager {
public:
    explicit AuthManager(AccountRepository& repository);
    std::optional<QString> authenticate(QWidget *parent = nullptr);

private:
    AccountRepository& repo;
};

#endif // AUTHMANAGER_H
