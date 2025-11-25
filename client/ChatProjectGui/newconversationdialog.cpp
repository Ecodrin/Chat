#include "newconversationdialog.h"
#include "ui_newconversationdialog.h"

NewConversationDialog::NewConversationDialog(GreeterClient *client, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::NewConversationDialog), client{client}
{
    ui->setupUi(this);
}

NewConversationDialog::~NewConversationDialog()
{
    delete ui;
}
