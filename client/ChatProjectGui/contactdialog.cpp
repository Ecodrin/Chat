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
        QPushButton * new_chat_button = new QPushButton("создать чат");
        ui->gridLayout->addWidget(new_chat_button );

    } else {
        std::string com = contact.substr(find_i+1, contact.size() - find_i);
        if (com == "(вы должны одобрить заявку)") {
            QPushButton * accept_contact_button_button = new QPushButton("Одобрить заявку");
            ui->gridLayout->addWidget(accept_contact_button_button );
            QObject::connect(accept_contact_button_button, &QPushButton::clicked, this, &ContactDialog::on_AcceptContactButton_clicked);
            // TODO добавить отклонение заявки в друзья
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
