#include "db.h"

DB::DB(const std::string & db_name): db_name{db_name} {
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(QString::fromStdString(db_name));
    if(!db.isOpen()) {
        return;
    }

    QSqlQuery query(db);
    if (!query.exec("CREATE TABLE IF NOT EXIST chats_info("
                "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                "interlocutor TEXT,"
                "chat_id TEXT,"
                "alg_index INTEGER,"
                "enc_mode_index INTEGER,"
                "padd_mode_index INTEGER,"
                "status INTEGER,"
                "key_ab TEXT,"
                "key_g TEXT,"
                "key_p TEXT,"
                "key_key TEXT,"
                "iv_ab TEXT,"
                "iv_g TEXT,"
                "iv_p TEXT,"
                "iv_key TEXT)"
                    )) {
        qDebug("error in create chats_info table sql");
        throw std::runtime_error("error in create chats_info table sql");
    }

    if(!query.exec("CREATE TABLE IF NOT EXIST chats_data("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                    "chat_id TEXT,"
                    "is_file INTEGER"
                    "sender TEXT,"
                    "recipient TEXT,"
                    "data TEXT,"
                    "timestamp INTEGER)"
                    )) {
        qDebug("error in create chats_data table sql");
        throw std::runtime_error("error in create chats_data table sql");
    }
}


bool DB::add_chat(ChatData chat_data) {
    QSqlQuery query(db);
    query.prepare("INSERT INTO chats_info (interlocutor, chat_id, alg_index, "
                  "enc_mode_index, padd_mode_index, status, key_ab, key_g, key_p, key_key, iv_ab, iv_g, iv_p, iv_key), "
                  "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?");
    query.addBindValue(QString::fromStdString(chat_data.interlocutor));
    query.addBindValue(QString::fromStdString(chat_data.chat_id));
    query.addBindValue(chat_data.alg_index);
    query.addBindValue(chat_data.enc_mode_index);
    query.addBindValue(chat_data.padd_mode_index);
    query.addBindValue(chat_data.status);
    query.addBindValue(QString::fromStdString(chat_data.key_info.ab));
    query.addBindValue(QString::fromStdString(chat_data.key_info.g));
    query.addBindValue(QString::fromStdString(chat_data.key_info.p));
    query.addBindValue(QString::fromStdString(bytes_utility::get_string_from_bytes(chat_data.key_info.key)));
    query.addBindValue(QString::fromStdString(chat_data.iv_info.ab));
    query.addBindValue(QString::fromStdString(chat_data.iv_info.g));
    query.addBindValue(QString::fromStdString(chat_data.iv_info.p));
    query.addBindValue(QString::fromStdString(bytes_utility::get_string_from_bytes(chat_data.iv_info.key)));

    if(!query.exec()) {
        qDebug("error in add chat");
        return false;
    }
    return true;
}


ChatData get_chat(const std::string & chat_id) {

}


bool DB::update_chat(ChatData chat_data) {
    QSqlQuery query(db);
    query.prepare("UPDATE chats_info SET "
                  "interlocutor = ?, chat_id = ?, alg_index = ?, "
                  "enc_mode_index = ?, padd_mode_index = ?, status = ?,"
                  "key_ab = ?, key_g = ?, key_p = ?, key_key = ?, "
                  "iv_ab = ?, iv_g = ?, iv_p = ?, iv_key = ? WHERE chat_id = ?");
    query.addBindValue(QString::fromStdString(chat_data.interlocutor));
    query.addBindValue(QString::fromStdString(chat_data.chat_id));
    query.addBindValue(chat_data.alg_index);
    query.addBindValue(chat_data.enc_mode_index);
    query.addBindValue(chat_data.padd_mode_index);
    query.addBindValue(chat_data.status);
    query.addBindValue(QString::fromStdString(chat_data.key_info.ab));
    query.addBindValue(QString::fromStdString(chat_data.key_info.g));
    query.addBindValue(QString::fromStdString(chat_data.key_info.p));
    query.addBindValue(QString::fromStdString(bytes_utility::get_string_from_bytes(chat_data.key_info.key)));
    query.addBindValue(QString::fromStdString(chat_data.iv_info.ab));
    query.addBindValue(QString::fromStdString(chat_data.iv_info.g));
    query.addBindValue(QString::fromStdString(chat_data.iv_info.p));
    query.addBindValue(QString::fromStdString(bytes_utility::get_string_from_bytes(chat_data.iv_info.key)));
    query.addBindValue(QString::fromStdString(chat_data.chat_id));

    if(!query.exec()) {
        qDebug("error in add chat");
        return false;
    }
    return true;
}


ChatData DB::get_chat(const std::string & chat_id) {
    QSqlQuery query(db);
    query.prepare("SELECT interlocutor, chat_id, alg_index, "
                    "enc_mode_index, padd_mode_index, status, key_ab, key_g, key_p, "
                  "key_key, iv_ab, iv_g, iv_p, iv_key from chats_info WHERE chat_id = ?");
    query.addBindValue(QString::fromStdString(chat_id));
    if(!query.exec()){
        qDebug("error in get_chat");
        return {{}};
    }
    query.next();
    ChatData data {
        .interlocutor = query.value(0).toString().toStdString(),
        .chat_id = query.value(1).toString().toStdString(),
        .key_info{
            .ab = query.value(6).toString().toStdString(),
            .g  = query.value(7).toString().toStdString(),
            .p = query.value(8).toString().toStdString(),
            .key = bytes_utility::get_bytes_from_string(query.value(9).toString().toStdString()),
        },
        .iv_info{
            .ab = query.value(10).toString().toStdString(),
            .g  = query.value(11).toString().toStdString(),
            .p = query.value(12).toString().toStdString(),
            .key = bytes_utility::get_bytes_from_string(query.value(13).toString().toStdString()),
        },
        .alg_index = query.value(2).toInt(),
        .enc_mode_index = query.value(3).toInt(),
        .padd_mode_index = query.value(4).toInt(),
        .status = query.value(5).toInt(),
    };
    return data;
}

std::pair<bool, std::vector<ChatData>> DB::get_chats() {
    std::vector<ChatData> result;
    QSqlQuery query(db);
    if (!query.exec("SELECT interlocutor, chat_id, alg_index, "
                    "enc_mode_index, padd_mode_index, status, key_ab, key_g, key_p, key_key, iv_ab, iv_g, iv_p, iv_key from chats_info")) {
        qDebug("error in get_chats");
        return {false, {}};
    }
    while(query.next()) {
        ChatData data {
            .interlocutor = query.value(0).toString().toStdString(),
            .chat_id = query.value(1).toString().toStdString(),
            .key_info{
                .ab = query.value(6).toString().toStdString(),
                .g  = query.value(7).toString().toStdString(),
                .p = query.value(8).toString().toStdString(),
                .key = bytes_utility::get_bytes_from_string(query.value(9).toString().toStdString()),
            },
            .iv_info{
                .ab = query.value(10).toString().toStdString(),
                .g  = query.value(11).toString().toStdString(),
                .p = query.value(12).toString().toStdString(),
                .key = bytes_utility::get_bytes_from_string(query.value(13).toString().toStdString()),
            },
            .alg_index = query.value(2).toInt(),
            .enc_mode_index = query.value(3).toInt(),
            .padd_mode_index = query.value(4).toInt(),
            .status = query.value(5).toInt(),
        };
        result.push_back(data);
    }

    return {true, result};
}

bool DB::add_msg_or_file(const MsgData & msg_data) {
    QSqlQuery query(db);
    query.prepare("INSERT INTO chats_data (chat_id, is_file, sender, recipient, data, timestamp) VALUES(?, ?, ?, ?, ?, ?)");
    query.addBindValue(QString::fromStdString(msg_data.chat_id));
    query.addBindValue(int(msg_data.is_file));
    query.addBindValue(QString::fromStdString(msg_data.sender));
    query.addBindValue(QString::fromStdString(msg_data.recipient));
    query.addBindValue(QString::fromStdString(msg_data.data));
    query.addBindValue(msg_data.timestamp);
    if(!query.exec()) {
        qDebug("error in add_msg");
        return false;
    }
    return true;
}


std::vector<MsgData> DB::get_msgs(const std::string & chat_id) {
    QSqlQuery query(db);
    query.prepare("SELECT chat_id, is_file, sender, recipient, data, timestamp FROM chats_data WHERE chat_id = ? ORDER BY timestaps ASC");
    query.addBindValue(QString::fromStdString(chat_id));
    if (!query.exec()) {
        qDebug("error in get_msgs");
        return {};
    }
    std::vector<MsgData> result;
    while(query.next()) {
        MsgData data{
            .chat_id = query.value(0).toString().toStdString(),
            .is_file = bool(query.value(1).toInt()),
            .sender = query.value(2).toString().toStdString(),
            .recipient = query.value(3).toString().toStdString(),
            .data = query.value(4).toString().toStdString(),
            .timestamp = query.value(5).toInt(),
        };
        result.push_back(data);
    }
    return result;
}


bool DB::delete_chat(const std::string & chat_id) {
    QSqlQuery query(db);
    query.prepare("DELETE FROM chats_data WHERE chat_id = ?");
    query.addBindValue(QString::fromStdString(chat_id));
    return query.exec();
}


DB::~DB() {
    db.close();
}
