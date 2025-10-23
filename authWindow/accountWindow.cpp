#include "accountWindow.h"
#include "ui_accountWindow.h"
#include <QString>

AccountWindow::AccountWindow(QWidget *parent)
    : QDialog(parent), ui(new Ui::AccountWindow) {
    ui->setupUi(this);
    setWindowTitle("Authorization");
    ui->labelStatus->clear();

    connect(ui->lineEditUsername, &QLineEdit::textChanged, this, &AccountWindow::on_textChanged);
    connect(ui->lineEditPassword, &QLineEdit::textChanged, this, &AccountWindow::on_textChanged);
}

AccountWindow::~AccountWindow() {
    delete ui;
}

void AccountWindow::on_btnLogin_clicked() {
    string user = ui->lineEditUsername->text().toStdString();
    string pass = ui->lineEditPassword->text().toStdString();
    emit loginRequested(user, pass);
}

void AccountWindow::on_btnRegister_clicked() {
    string user = ui->lineEditUsername->text().toStdString();
    string pass = ui->lineEditPassword->text().toStdString();
    emit registerRequested(user, pass);
}

void AccountWindow::setStatus(const QString &text, bool isError) {
    ui->labelStatus->setText(text);
    if (isError)
        ui->labelStatus->setStyleSheet("color: red; font-weight: bold;");
    else
        ui->labelStatus->setStyleSheet("color: green; font-weight: bold;");
}

void AccountWindow::on_textChanged() {
    ui->labelStatus->clear();
}