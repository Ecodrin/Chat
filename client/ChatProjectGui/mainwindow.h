#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <QDialog>
#include <QWidget>
#include <QListWidget>
#include <QListWidgetItem>
#include <QPushButton>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QMainWindow>
#include <QScreen>
#include <QLineEdit>
#include <QRect>
#include <QStringListModel>

#include "grpc_client.hpp"
#include "contactdialog.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(GreeterClient * client, QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_DisconnectButton_clicked();
    void on_UpdateContactsButton_clicked();
    void on_AddContactButton_clicked();

signals:
    void requestBack();

private:

    GreeterClient * client;
    QWidget * parent = nullptr;
    Ui::MainWindow *ui;
    std::unique_ptr<QStringListModel> contacts_model;
};


#endif // MAINWINDOW_H
