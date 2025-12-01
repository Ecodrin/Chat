#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(GreeterClient * client, QWidget *parent)
    : QWidget(parent), client{client}, parent{parent}
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ChatSessionCallResult res = client->chat_session(&clientcontext_chat_session);
    if (!res.error.empty()) {
        QMessageBox::information(this, "error", QString::fromStdString(res.error));
        return;
    }


    database = std::make_shared<WorkWithData>();

    writer = std::make_shared<ChatStreamgRPCWorker>(std::move(res.writer), [&](const chat::ChatMsg & msg){
        if(msg.has_new_chat_msg()) {
            auto new_chat_msg = msg.new_chat_msg();
            ChatInfo info{
                .interlocutor = new_chat_msg.sender(),
                .chat_id = new_chat_msg.chat_id(),
                .ab_key = new_chat_msg.ab_for_key(),
                .g_key = new_chat_msg.g_for_key(),
                .p_key = new_chat_msg.p_for_key(),
                .ab_iv = new_chat_msg.ab_for_iv(),
                .g_iv = new_chat_msg.g_for_iv(),
                .p_iv = new_chat_msg.p_for_iv(),
                .alg_index = int(new_chat_msg.alg()),
                .enc_mode_index = int(new_chat_msg.enc_mode()),
                .padd_mode_index = int(new_chat_msg.padd_mode()),
            };
            auto new_chat_info = database->update_chat_status(info);
            if(new_chat_info.first) {
                std::mutex mutex;
                std::lock_guard<std::mutex> locker(mutex);
                client->add_chat(writer, new_chat_info.second);
            }
        }
    });


    contacts_model = std::make_unique<QStringListModel>();
    on_UpdateContactsButton_clicked();
    QObject::connect(ui->ContactsListView, &QListView::doubleClicked, [=](const QModelIndex & index){
        QString text = contacts_model->data(index, Qt::DisplayRole).toString();
        ContactDialog *contact_dialog = new ContactDialog(text.toStdString(), client, writer, database, this);
        contact_dialog->setAttribute(Qt::WA_DeleteOnClose);
        contact_dialog->exec();

        on_UpdateContactsButton_clicked();
    });
    on_UpdateContactsButton_clicked();
}

void MainWindow::Disconnect() {
    on_DisconnectButton_clicked();
}

MainWindow::~MainWindow()
{
    Disconnect();
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

