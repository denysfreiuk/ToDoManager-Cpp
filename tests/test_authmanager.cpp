
#include <gtest/gtest.h>
#include <optional>
#include "../databaseManager/databaseManager.h"
#include "../databaseManager/accountRepository.h"
#include "../accounts/authManager.h"

struct AuthFx : public ::testing::Test {
    DatabaseManager db = DatabaseManager(":memory:");
    std::unique_ptr<AccountRepository> repo;
    std::unique_ptr<AuthManager> auth;

    void SetUp() override {
        ASSERT_TRUE(db.open());
        repo = std::make_unique<AccountRepository>(db);
        repo->initTable();
        auth = std::make_unique<AuthManager>(*repo);
    }
};

TEST_F(AuthFx, DefaultAdminCreated) {
    auto all = repo->getAllAccounts();
    bool hasAdmin = false;
    for (auto &a : all) if (a.getUsername() == "admin") hasAdmin = true;
    EXPECT_TRUE(hasAdmin);
}

TEST_F(AuthFx, AddAndAuthenticate) {
    EXPECT_TRUE(repo->addAccount("denys", std::hash<std::string>{}("qwerty")));
    auto acc = repo->getAccount("denys");
    ASSERT_TRUE(acc.has_value());
    EXPECT_EQ(acc->getUsername(), "denys");
}
