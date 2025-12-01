#include "newconversationdialog.h"
#include "ui_newconversationdialog.h"

NewConversationDialog::NewConversationDialog(GreeterClient *client,
                                             std::shared_ptr<ChatStreamgRPCWorker> writer,
                                             std::shared_ptr<WorkWithData> database,
                                             const std::string & contact,
                                             QWidget *parent)
    : QDialog(parent), writer{writer}, database{database}, contact{contact}
    , ui(new Ui::NewConversationDialog), client{client}
{
    ui->setupUi(this);
}

NewConversationDialog::~NewConversationDialog()
{
    ui->comboBoxAlg->clear();
    ui->comboBoxEncMode->clear();
    ui->comboBoxPadding->clear();
    delete ui;
}

void NewConversationDialog::on_CreateChatButton_clicked() {
    int alg_index = ui->comboBoxAlg->currentIndex();
    int enc_mode_index = ui->comboBoxEncMode->currentIndex();
    int enc_padding_index = ui->comboBoxPadding->currentIndex();
    if(!writer) {
        QMessageBox::information(this, "error", "connection failed");
        this->close();
        return;
    }
    std::cout << "create chat button clicked\n";
    ChatInfo chat_info = database->add_chat(alg_index, enc_mode_index, enc_padding_index, contact);
    auto [error, error_string] = client->add_chat(writer, chat_info);
    std::cout << "after " << error << error_string << std::endl;
    if (!error) {
        QMessageBox::information(this, "error", QString::fromStdString(error_string));
        this->close();
        return;
    }
    this->close();

}
