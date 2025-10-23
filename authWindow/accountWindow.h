#ifndef ACCOUNTWINDOW_H
#define ACCOUNTWINDOW_H

#pragma once
#include <QDialog>
#include <optional>
#include <string>

using namespace std;

QT_BEGIN_NAMESPACE
namespace Ui {
    class AccountWindow;
}
QT_END_NAMESPACE

class AccountWindow : public QDialog {
    Q_OBJECT

public:
    explicit AccountWindow(QWidget *parent = nullptr);
    ~AccountWindow();

    void setStatus(const QString &text, bool isError = false);

    signals:
        void loginRequested(const string &username, const string &password);
    void registerRequested(const string &username, const string &password);

    private slots:
        void on_btnLogin_clicked();
    void on_btnRegister_clicked();
    void on_textChanged();

private:
    Ui::AccountWindow *ui;
};


#endif //ACCOUNTWINDOW_H
