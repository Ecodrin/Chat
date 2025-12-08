#ifndef DELETECHATDIALOG_H
#define DELETECHATDIALOG_H

#include <QDialog>
#include <QMessageBox>

#include "workwithdata.h"
#include "grpc_client.hpp"

namespace Ui {
class DeleteChatDialog;
}

class DeleteChatDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DeleteChatDialog(
        std::string &chat_id,
        GreeterClient *client,
        std::shared_ptr<WorkWithData> database,
        std::shared_ptr<ChatStreamgRPCWorker> writer,
        QWidget *parent = nullptr);
    ~DeleteChatDialog();

private slots:
    void on_DeleteChatButton_clicked();

private:
    std::string &chat_id;
    GreeterClient *client;
    std::shared_ptr<WorkWithData> database;
    std::shared_ptr<ChatStreamgRPCWorker> writer;
    Ui::DeleteChatDialog *ui;
};

#endif // DELETECHATDIALOG_H
