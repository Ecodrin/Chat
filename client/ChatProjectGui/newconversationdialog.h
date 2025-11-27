#ifndef NEWCONVERSATIONDIALOG_H
#define NEWCONVERSATIONDIALOG_H

#include <QDialog>

#include "grpc_client.hpp"
#include "streamgrpcworker.h"


namespace Ui {
class NewConversationDialog;
}

class NewConversationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewConversationDialog(GreeterClient *client, std::shared_ptr<ChatStreamgRPCWorker> writer, QWidget *parent = nullptr);
    ~NewConversationDialog();

private slots:
    void on_CreateChatButton_clicked();

private:
    Ui::NewConversationDialog *ui;
    GreeterClient *client;
    std::shared_ptr<ChatStreamgRPCWorker> writer;
};

#endif // NEWCONVERSATIONDIALOG_H
