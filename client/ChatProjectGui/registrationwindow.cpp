#include "registrationwindow.h"
#include "ui_registrationwindow.h"

RegistrationWindow::RegistrationWindow(GreeterClient * client, QWidget *parent)
    : client{client},
        QDialog(parent),
        ui(new Ui::RegistrationWindow)
{
    ui->setupUi(this);
}

RegistrationWindow::~RegistrationWindow()
{
    delete ui;
}

void RegistrationWindow::on_registerButton_clicked() {
    auto login = ui->LoginLineEdit->text().toStdString();
    auto password = ui->PasswordLineEdit->text().toStdString();
    auto t = client->registration(login, password);
    if (!t.first) {
        QMessageBox::information(this, "error", QString::fromStdString(t.second));
        return;
    }
    ui->LoginLineEdit->clear();
    ui->PasswordLineEdit->clear();
    if (!main_window) {
        main_window = new MainWindow(client);
        connect(main_window, &MainWindow::requestBack, this, [this]() {
            this->show();
            main_window->deleteLater();
            main_window = nullptr;
        });
    }

    main_window->show();
    this->hide();
}

void RegistrationWindow::on_AuthButton_clicked() {
    auto login = ui->LoginLineEdit->text().toStdString();
    auto password = ui->PasswordLineEdit->text().toStdString();
    auto t = client->auth(login, password);
    if (!t.first) {
        QMessageBox::information(this, "error", QString::fromStdString(t.second));
        return;
    }
    ui->LoginLineEdit->clear();
    ui->PasswordLineEdit->clear();
    if (!main_window) {
        main_window = new MainWindow(client);
        connect(main_window, &MainWindow::requestBack, this, [this]() {
            this->show();
            main_window->deleteLater();
            main_window = nullptr;
        });
    }

    main_window->show();
    this->hide();
}
