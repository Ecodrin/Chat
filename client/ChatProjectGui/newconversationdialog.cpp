#include "newconversationdialog.h"
#include "ui_newconversationdialog.h"

NewConversationDialog::NewConversationDialog(GreeterClient *client, std::shared_ptr<ChatStreamgRPCWorker> writer, QWidget *parent)
    : QDialog(parent), writer{writer}
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


}
