#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(GreeterClient * client, QWidget *parent)
    : QWidget(parent), client{client}, parent{parent}
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    contacts_model = std::make_unique<QStringListModel>();
    on_UpdateContactsButton_clicked();

    QObject::connect(ui->ContactsListView, &QListView::doubleClicked, [=](const QModelIndex & index){
        QString text = contacts_model->data(index, Qt::DisplayRole).toString();
        ContactDialog *contact_dialog = new ContactDialog(text.toStdString(), client, this);
        contact_dialog->setAttribute(Qt::WA_DeleteOnClose);
        contact_dialog->exec();

        on_UpdateContactsButton_clicked();
    });
    on_UpdateContactsButton_clicked();


    ChatSessionCallResult res = client->chat_session(&clientcontext_chat_session);
    if (!res.error.empty()) {
        QMessageBox::information(this, "error", QString::fromStdString(res.error));
        return;
    }
    msgs_writer = std::move(res.writer);

    getting_msgs_thread = std::thread([this]() {
        chat::OutputMsg msg;
        while(msgs_writer && msgs_writer->Read(&msg)) {
            // Сделать сохранение в бд
            // добавить сообщение в открытый чат
        }
    });
}

void MainWindow::Disconnect() {
    on_DisconnectButton_clicked();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_DisconnectButton_clicked() {
    clientcontext_chat_session.TryCancel();
    auto res = client->disconnect();
    if (!res.first) {
        QMessageBox::information(this, "error", QString::fromStdString(res.second));
    }
    requestBack();
    ui->LineContactEdit->clear();
    ui->LineContactEdit->clear();

    if(getting_msgs_thread.joinable()) {
        getting_msgs_thread.join();
    }
}

void requestBack() {
    emit requestBack();
}


void MainWindow::on_UpdateContactsButton_clicked() {
    auto [contacts, error_msg] = client->get_all_contacts();
    if (error_msg != "") {
        QMessageBox::information(this, "error", QString::fromStdString(error_msg));
        return;
    }
    QStringList list;
    for(const auto & contact : contacts) {
        QString contact_str = QString::fromStdString(contact.login);
        if (contact.status == 1) {
            contact_str += " (заявка отправлена)";
        } else if(contact.status == 2) {
            contact_str += " (вы должны одобрить заявку)";
        }
        list << contact_str;
    }
    contacts_model->setStringList(list);
    ui->ContactsListView->setModel(contacts_model.get());
}

void MainWindow::on_AddContactButton_clicked() {
    QString qcontact = ui->LineContactEdit->text();
    auto t = client->add_contact(qcontact.toStdString());
    if (!t.first) {
        QMessageBox::information(this, "error", QString::fromStdString(t.second));
        return;
    }
    on_UpdateContactsButton_clicked();
    ui->LineContactEdit->clear();
}

