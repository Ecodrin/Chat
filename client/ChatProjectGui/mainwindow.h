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

#include <memory>
#include <thread>

#include "grpc_client.hpp"
#include "contactdialog.h"

#include "workwithdata.h"
#include "streamgrpcworker.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(GreeterClient * client, QWidget *parent = nullptr);
    ~MainWindow();
    void Disconnect();

    void update_chats();

private slots:
    void on_DisconnectButton_clicked();
    void on_UpdateContactsButton_clicked();
    void on_AddContactButton_clicked();

signals:
    void requestBack();

private:
    grpc::ClientContext clientcontext_chat_session;

    GreeterClient * client;
    QWidget * parent = nullptr;
    Ui::MainWindow *ui;
    std::unique_ptr<QStringListModel> contacts_model;
    std::unique_ptr<QStringListModel> chats_model;

    std::shared_ptr<ChatStreamgRPCWorker> writer;
    std::shared_ptr<WorkWithData> database;
};


#endif // MAINWINDOW_H
