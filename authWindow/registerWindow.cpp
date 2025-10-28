#include "registerWindow.h"
#include "ui_registerWindow.h"
using namespace std;

RegisterWindow::RegisterWindow(QWidget *parent)
    : QDialog(parent),
      ui(new Ui::RegisterWindow)
{
    ui->setupUi(this);
    setWindowTitle("Registration");

    ui->labelStatus->clear();
    connect(ui->lineEditUsername, &QLineEdit::textChanged, this, &RegisterWindow::on_textChanged);
    connect(ui->lineEditPassword, &QLineEdit::textChanged, this, &RegisterWindow::on_textChanged);
}

RegisterWindow::~RegisterWindow() {
    delete ui;
}

void RegisterWindow::on_btnRegister_clicked() {
    string user = ui->lineEditUsername->text().toStdString();
    string pass1 = ui->lineEditPassword->text().toStdString();
    string pass2 = ui->lineEditConfirmPassword->text().toStdString();

    if (user.empty() || pass1.empty() || pass2.empty()) {
        setStatus("All fields must be filled in!", true);
        return;
    }
    if (pass1 != pass2) {
        setStatus("Passwords do not match!", true);
        return;
    }
    emit registerRequested(user, pass1);
}

void RegisterWindow::setStatus(const QString &text, bool isError) {
    ui->labelStatus->setText(text);
    ui->labelStatus->setStyleSheet(isError ? "color: red;" : "color: green;");
}

void RegisterWindow::on_textChanged(){
    ui->labelStatus->clear();
}
