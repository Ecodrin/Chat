#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(GreeterClient * client, QWidget *parent)
    : QWidget(parent), client{client}, parent{parent}
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->ChatWidgets->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);


    std::filesystem::create_directory(files_path);
    if(!std::filesystem::create_directory(files_path + "/" + client->get_login())) {
        qDebug("error in create files path");
    }


    ChatSessionCallResult res = client->chat_session(&clientcontext_chat_session);
    if (!res.error.empty()) {
        QMessageBox::information(this, "error", QString::fromStdString(res.error));
        return;
    }


    database = std::make_shared<WorkWithData>(files_path + "/" + client->get_login(), client->get_login());
    chats_model = std::make_unique<QStandardItemModel>();

    connect(this, &MainWindow::updateChatSignal, this, &MainWindow::show_chat, Qt::QueuedConnection);
    connect(this, &MainWindow::updateChatsSignal, this, &MainWindow::update_chats, Qt::QueuedConnection);
    connect(this, &MainWindow::updateContactsSignal, this, &MainWindow::update_contacts, Qt::QueuedConnection);


    writer = std::make_shared<ChatStreamgRPCWorker>(std::move(res.writer), [=](const chat::ChatMsg & msg){
        if(msg.has_new_chat_msg()) {
            auto future = QtConcurrent::run([this, msg, client](){
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
                emit updateChatsSignal();
            });
            futures.push_back(future);
        } else if(msg.has_default_msg()) {
            auto future = QtConcurrent::run([this, msg](){
                auto default_msg = msg.default_msg();
                MsgData msg_data{
                    .chat_id=default_msg.chat_id(),
                    .is_file=false,
                    .sender=default_msg.sender(),
                    .recipient=default_msg.recipient(),
                    .data=default_msg.data(),
                    .timestamp=int(default_msg.timestamp())
                };
                if(database->add_msg(msg_data)) {
                    if(msg_data.chat_id == current_chat_id) {
                        emit updateChatSignal();
                    }
                }
            });

            futures.push_back(future);
        } else if(msg.has_file_msg()) {
            auto future = QtConcurrent::run([this, msg](){
                auto file_msg = msg.file_msg();

                FileData data{
                    .chat_id=file_msg.chat_id(),
                    .is_file=true,
                    .sender=file_msg.sender(),
                    .recipient=file_msg.recipient(),
                    .data=file_msg.data(),
                    .timestamp=int(file_msg.timestamp()),
                    .file_name=file_msg.file_name(),
                    .index_file_chunk=size_t(file_msg.index_file_chunk()),
                    .total_file_chunk=size_t(file_msg.total_file_chunk()),
                };

                database->add_file(data);
                emit updateChatSignal();
            });

            futures.push_back(future);
        } else if(msg.has_delete_chat_msg()) {

            auto future = QtConcurrent::run([this, msg](){
                auto delete_msg = msg.delete_chat_msg();
                database->delete_chat(delete_msg.chat_id());
                emit updateChatSignal();
                emit updateChatsSignal();
            });

            futures.push_back(future);
        } else if(msg.has_contact_notification()) {
            auto future = QtConcurrent::run([this](){
                emit updateContactsSignal();
            });
            futures.push_back(future);
        }
    });


    contacts_model = std::make_unique<QStringListModel>();
    emit updateContactsSignal();
    QObject::connect(ui->ContactsListView, &QListView::doubleClicked, [=](const QModelIndex & index){
        QString text = contacts_model->data(index, Qt::DisplayRole).toString();
        ContactDialog *contact_dialog = new ContactDialog(text.toStdString(), client, writer, database, this);
        contact_dialog->setAttribute(Qt::WA_DeleteOnClose);
        contact_dialog->exec();

        emit updateContactsSignal();
    });
    emit updateContactsSignal();
    update_chats();


    QObject::connect(ui->ChatsListView, &QListView::doubleClicked, [=](const QModelIndex & index){
        QStandardItem *item = chats_model->itemFromIndex(index);
        if(item) {
            std::string interlocutor = item->text().toStdString();
            std::string chat_id = item->data(Qt::UserRole).toString().toStdString();
            ui->ChatWidgets->clear();
            current_chat_id = chat_id;
            emit updateChatSignal();
        }
    });

    QObject::connect(ui->ChatsListView, &QListView::pressed, [=](const QModelIndex & index){
        if(QApplication::mouseButtons() & Qt::RightButton) {
            QStandardItem *item = chats_model->itemFromIndex(index);
            std::string chat_id = item->data(Qt::UserRole).toString().toStdString();
            DeleteChatDialog * dialog = new DeleteChatDialog(chat_id, client, database, writer, this);
            dialog->setAttribute(Qt::WA_DeleteOnClose);
            dialog->exec();
            emit updateChatsSignal();
            emit updateChatSignal();
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
    if (!std::filesystem::remove_all(files_path + "/" + client->get_login())) {
        qDebug("error in remove dir Disconnect");
    }
    std::filesystem::remove_all(files_path + "/" + client->get_login());
    clientcontext_chat_session.TryCancel();
    auto res = client->disconnect();
    if (!res.first) {
        QMessageBox::information(this, "error", QString::fromStdString(res.second));
    }

    ui->LineContactEdit->clear();
    ui->LineContactEdit->clear();
    ui->ChatWidgets->clear();

    chats_model->clear();

    for (auto &future: futures) {
        future.waitForFinished();
    }
    requestBack();
}

void requestBack() {
    emit requestBack();
}


void MainWindow::update_contacts() {
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
    emit updateContactsSignal();
    ui->LineContactEdit->clear();
}

void MainWindow::update_chats() {
    chats_model->clear();
    auto chats = database->get_chats();
    QStringList list;
    for(auto & chat : chats) {
        QString chat_str = QString::fromStdString(chat.interlocutor);
        QString chat_id_str = QString::fromStdString(chat.chat_id);
        QStandardItem * item = new QStandardItem(chat_str);
        item->setData(chat_id_str, Qt::UserRole);
        chats_model->appendRow(item);
    }
    ui->ChatsListView->setModel(chats_model.get());
}


void MainWindow::show_chat() {
    if(current_chat_id.empty()) {
        return;
    }
    ui->ChatWidgets->clear();
    auto msgs = database->get_msgs(current_chat_id);

    for(const auto & msg : msgs) {

        QListWidgetItem *item = new QListWidgetItem(ui->ChatWidgets);
        item->setData(Qt::UserRole, msg.timestamp);
        item->setData(Qt::UserRole + 1, QString::fromStdString(msg.sender));
        item->setData(Qt::UserRole + 2, QString::fromStdString(msg.data));

        ChatWidget * chat_widget = new ChatWidget(msg, ui->ChatWidgets);
        chat_widget->adjustSize();
        item->setSizeHint(chat_widget->sizeHint());

        ui->ChatWidgets->addItem(item);
        ui->ChatWidgets->setItemWidget(item, chat_widget);
    }
    ui->ChatWidgets->scrollToBottom();
}

void MainWindow::on_SendMsgButton_clicked() {
    QtConcurrent::run([this](){
        QString qtext = ui->LineText->text();
        std::string text = qtext.toStdString();
        if(text.size() == 0) {
            return;
        }
        MsgData msg{
            .chat_id=current_chat_id,
            .is_file=false,
            .sender=client->get_login(),
            .recipient="",
            .data=text,
            .timestamp=int(std::time(nullptr)),
        };
        auto new_msg = database->send_msg(msg);
        if(!new_msg.first) {
            return;
        }
        auto t = client->send_msg(writer, new_msg.second);
        if(!t.first) {
            return;
        }
        emit updateChatSignal();
        ui->LineText->clear();
    });
}

void MainWindow::on_FileButton_clicked() {
    QString filename = QFileDialog::getOpenFileName(
        this,
        tr("отправить файл"),
        QDir::homePath(),
        tr("Все файлы (*)")
    );
    if(filename == "") {
        return;
    }
    if(current_chat_id == "") {
        return;
    }
    QtConcurrent::run([this, filename](){
        FileData file_data{
            .chat_id=current_chat_id,
            .is_file=true,
            .sender=client->get_login(),
            .recipient="",
            .timestamp=int(std::time(nullptr)),
            .file_name=filename.toStdString(),
            .index_file_chunk=0,
            .total_file_chunk=0,
        };
        std::string enc_filename = database->send_file(file_data);
        std::ifstream file(enc_filename);
        if (!file.is_open()) {
            return;
        }
        size_t chunk_size = symmetric_interface_library::SymmetricContext::get_read_file_scale_block();
        size_t i = 1;
        size_t total_i = std::filesystem::file_size(enc_filename) / chunk_size;
        if (std::filesystem::file_size(enc_filename) && chunk_size != 0) {
            total_i += 1;
        }
        std::string buffer;

        while(true) {
            buffer.resize(chunk_size);
            file.read(reinterpret_cast<char*>(buffer.data()), chunk_size);
            if(file.gcount() == 0) {
                break;
            }
            buffer.resize(file.gcount());
            FileData file_data{
                               .chat_id=current_chat_id,
                               .is_file=true,
                               .sender=client->get_login(),
                               .recipient=database->get_recipient(current_chat_id),
                               .data=buffer,
                               .timestamp=int(std::time(nullptr)),
                               .file_name=std::filesystem::path{enc_filename}.filename(),
                               .index_file_chunk=i,
                               .total_file_chunk=total_i,
                               };
            i++;
            auto s = client->send_file(writer, file_data);
            if(!s.first) {
                std::cout << s.second << std::endl;
                return;
            }
        }
        file.close();
        std::filesystem::remove(enc_filename);

        emit updateChatSignal();
    });

}
