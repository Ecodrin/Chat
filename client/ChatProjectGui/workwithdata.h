#ifndef WORKWITHDATA_H
#define WORKWITHDATA_H

#include <string>
#include <mutex>
#include <vector>
#include <cstddef>
#include <unordered_map>
#include <filesystem>

#include "encryption_modes.hpp"
#include "encryption_padding.hpp"
#include "symmetric_context.hpp"
#include "Diffie_Hellman_key_exchange.hpp"
#include "utility.h"
#include "enc_utility.hpp"
#include "bytes_utility.hpp"


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

class WorkWithData {
public:
    WorkWithData(const std::string & files_path);

    ChatInfo add_chat(int alg, int enc_mode, int padd_mode, const std::string & interlocutor, const std::string & chat_id = "-1");
    std::pair<bool, ChatInfo> update_chat_status(const ChatInfo & info);
    std::vector<ChatInfo> get_chats();
    bool add_msg(const MsgData & msg_data);
    std::pair<bool, MsgData> send_msg(const MsgData & msg_data);
    std::vector<MsgData> get_msgs(const std::string & chat_id);
    bool add_file(const FileData & msg_data);
    std::string send_file(const FileData & msg_data);

    std::string get_recipient(const std::string & chat_id);
private:
    std::string files_path;;
    // потом переписать на бд
    std::mutex mutex;
    std::unordered_map<std::string, std::pair<ChatData, std::vector<MsgData>>> data;

};

#endif // WORKWITHDATA_H
