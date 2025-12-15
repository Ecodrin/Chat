#ifndef DB_H
#define DB_H

#include <memory>
#include <string>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <filesystem>
#include <fstream>
#include <QDir>

#include "bytes_utility.hpp"
#include "symmetric_context.hpp"
#include "utility.h"

struct KeyInfo{
    std::string ab;
    std::string g;
    std::string p;
    std::vector<std::byte> key;
};



struct  ChatData {
    std::string interlocutor;
    std::string chat_id;
    KeyInfo key_info;
    KeyInfo iv_info;
    int alg_index;
    int enc_mode_index;
    int padd_mode_index;
    std::shared_ptr<symmetric_interface_library::SymmetricContext> symmetric_context;
    int status;
};

struct MsgData {
    std::string chat_id;
    bool is_file;
    std::string sender;
    std::string recipient;
    std::string data;
    int timestamp;
};

struct ChatInfo {
    std::string interlocutor;
    std::string chat_id;
    std::string ab_key;
    std::string g_key;
    std::string p_key;
    std::string ab_iv;
    std::string g_iv;
    std::string p_iv;
    int alg_index;
    int enc_mode_index;
    int padd_mode_index;
};

struct FileData {
    std::string chat_id;
    bool is_file;
    std::string sender;
    std::string recipient;
    std::string data;
    int timestamp;
    std::string file_name;
    size_t index_file_chunk;
    size_t total_file_chunk;
};

class DB {
private:
    std::string db_name;
    QSqlDatabase db;
public:
    DB(const std::string & db_name);
    bool add_chat(ChatData chat_data);
    bool check_exist_chat(const std::string & chat_id);
    ChatData get_chat(const std::string & chat_id);
    bool update_chat(ChatData chat_data);
    std::pair<bool, std::vector<ChatData>> get_chats();
    bool add_msg_or_file(const MsgData & msg_data);
    std::vector<MsgData> get_msgs(const std::string & chat_id);
    bool delete_chat(const std::string & chat_id);
    ~DB();
};

#endif // DB_H
