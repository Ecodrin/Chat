#ifndef NEWCONVERSATIONDIALOG_H
#define NEWCONVERSATIONDIALOG_H

#include <QDialog>
#include <QMessageBox>

#include "grpc_client.hpp"
#include "streamgrpcworker.h"
#include "workwithdata.h"

namespace Ui {
class NewConversationDialog;
}

class NewConversationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewConversationDialog(GreeterClient *client, std::shared_ptr<ChatStreamgRPCWorker> writer,
                                    std::shared_ptr<WorkWithData> database,
                                    const std::string & contact,
                                    QWidget *parent = nullptr);
    ~NewConversationDialog();

private slots:
    void on_CreateChatButton_clicked();

private:
    std::string contact;
    Ui::NewConversationDialog *ui;
    GreeterClient *client;
    std::shared_ptr<ChatStreamgRPCWorker> writer;
    std::shared_ptr<WorkWithData> database;
};

#endif // NEWCONVERSATIONDIALOG_H
