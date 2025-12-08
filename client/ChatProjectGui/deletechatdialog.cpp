#include "deletechatdialog.h"
#include "ui_deletechatdialog.h"

DeleteChatDialog::DeleteChatDialog(
    std::string &chat_id,
    GreeterClient *client,
    std::shared_ptr<WorkWithData> database,
    std::shared_ptr<ChatStreamgRPCWorker> writer,
    QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DeleteChatDialog),
        writer{writer},
        client{client},
        database{database},
        chat_id{chat_id}
{
    ui->setupUi(this);
}

DeleteChatDialog::~DeleteChatDialog()
{
    delete ui;
}

void DeleteChatDialog::on_DeleteChatButton_clicked() {
    std::string interlocutor = database->delete_chat(chat_id);
    auto err = client->delete_chat(writer, chat_id, interlocutor);
    this->close();
    if(!err.first) {
        QMessageBox::information(this, "error", QString::fromStdString(err.second));
    }
}
