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
#include "db.h"


class WorkWithData {
public:
    WorkWithData(const std::string & files_path, const std::string & login);

    ChatInfo add_chat(int alg, int enc_mode, int padd_mode, const std::string & interlocutor, const std::string & chat_id = "-1");
    std::pair<bool, ChatInfo> update_chat_status(const ChatInfo & info);
    std::vector<ChatInfo> get_chats();
    bool add_msg(const MsgData & msg_data);
    std::pair<bool, MsgData> send_msg(const MsgData & msg_data);
    std::vector<MsgData> get_msgs(const std::string & chat_id);
    bool add_file(const FileData & msg_data);
    std::string send_file(const FileData & msg_data);

    std::string get_recipient(const std::string & chat_id);
    std::string delete_chat(const std::string & chat_id);
private:
    std::string files_path;
    const std::string & login;
    std::mutex mutex;
    DB db;

};

#endif // WORKWITHDATA_H
