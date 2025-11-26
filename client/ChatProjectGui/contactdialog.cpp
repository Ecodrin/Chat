#include "contactdialog.h"
#include "ui_contactdialog.h"



ContactDialog::ContactDialog(const std::string & contact, GreeterClient * client, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ContactDialog), client{client}
{
    ui->setupUi(this);
    size_t find_i = contact.find(" ");
    login = contact.substr(0, find_i);
    if (find_i == -1) {
        QPushButton * delete_button = new QPushButton("удалить контакт");
        ui->gridLayout->addWidget(delete_button );
        QObject::connect(delete_button, &QPushButton::clicked, this, &ContactDialog::on_DeleteContactButton_clicked);
        QPushButton * new_chat_button = new QPushButton("создать чат");
        ui->gridLayout->addWidget(new_chat_button);
        QObject::connect(new_chat_button, &QPushButton::clicked, this, &ContactDialog::on_NewChatButton_clicked);

    } else {
        std::string com = contact.substr(find_i+1, contact.size() - find_i);
        if (com == "(вы должны одобрить заявку)") {
            QPushButton * accept_contact_button_button = new QPushButton("Одобрить заявку");
            ui->gridLayout->addWidget(accept_contact_button_button );
            QObject::connect(accept_contact_button_button, &QPushButton::clicked, this, &ContactDialog::on_AcceptContactButton_clicked);

            QPushButton * decline_contact_button_button = new QPushButton("Отклонить заявку");
            ui->gridLayout->addWidget(decline_contact_button_button );
            QObject::connect(decline_contact_button_button, &QPushButton::clicked, this, &ContactDialog::on_DeclineContactButton_clicked);

        }
    }
}

ContactDialog::~ContactDialog()
{
    delete ui;
}

void ContactDialog::on_AcceptContactButton_clicked() {
    auto t = client->accept_contact(login);
    if (!t.first) {
        QMessageBox::information(this, "error", QString::fromStdString(t.second));
    }
    this->close();
}

void ContactDialog::on_DeclineContactButton_clicked() {
    auto t = client->decline_contact(login);
    if (!t.first) {
        QMessageBox::information(this, "error", QString::fromStdString(t.second));
    }
    this->close();
}


void ContactDialog::on_DeleteContactButton_clicked() {
    auto t = client->delete_contact(login);
    if (!t.first) {
        QMessageBox::information(this, "error", QString::fromStdString(t.second));
    }
    this->close();
}


void ContactDialog::on_NewChatButton_clicked() {
    NewConversationDialog * new_chat_dialog = new NewConversationDialog(client, this);
    new_chat_dialog->setAttribute(Qt::WA_DeleteOnClose);
    new_chat_dialog->exec();
    this->close();
}
