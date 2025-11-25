#ifndef REGISTRATIONWINDOW_H
#define REGISTRATIONWINDOW_H

#include <QDialog>
#include <QWidget>
#include <QPushButton>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QMainWindow>
#include <QScreen>
#include <QRect>

#include "grpc_client.hpp"
#include "mainwindow.h"

namespace Ui {
class RegistrationWindow;
}

class RegistrationWindow : public QDialog
{
    Q_OBJECT

public:
    explicit RegistrationWindow(GreeterClient * client, QWidget *parent = nullptr);
    ~RegistrationWindow();
private slots:
    void on_registerButton_clicked();
    void on_AuthButton_clicked();
private:
    MainWindow *main_window = nullptr;
    GreeterClient * client;
    Ui::RegistrationWindow *ui;
};

#endif // REGISTRATIONWINDOW_H
