#include "workwithdata.h"

WorkWithData::WorkWithData() {

}

ChatInfo WorkWithData::add_chat(int alg, int enc_mode, int padd_mode, const std::string & interlocutor, const std::string & chat_id) {
    std::lock_guard<std::mutex> locker(mutex);
    std::string id = chat_id;
    if (chat_id == "-1" or data.find(chat_id) == data.end()) {
        id = generate_random_string();
    }
    ChatData chat_data{
        .interlocutor=interlocutor,
        .chat_id=id,
        .alg_index=alg,
        .enc_mode_index=enc_mode,
        .padd_mode_index=padd_mode,
        .symmetric_context=std::shared_ptr<symmetric_interface_library::SymmetricContext>(nullptr),
        .status=1,
    };
    auto [a_key, g_key, p_key] = Diffie_Hellman_key_exchange::get_random_string_keys();
    auto [a_iv, g_iv, p_iv] = Diffie_Hellman_key_exchange::get_random_string_keys();
    chat_data.key_info = KeyInfo{
        .ab=a_key,
        .g=g_key,
        .p=p_key,
    };

    chat_data.iv_info = KeyInfo{
        .ab=a_iv,
        .g=g_iv,
        .p=p_iv,
    };

    ChatInfo info {
        .interlocutor=interlocutor,
        .chat_id=chat_data.chat_id,
        .ab_key=Diffie_Hellman_key_exchange::get_A_or_B_string({chat_data.key_info.ab, chat_data.key_info.g, chat_data.key_info.p}),
        .g_key=chat_data.key_info.g,
        .p_key=chat_data.key_info.p,
        .ab_iv=Diffie_Hellman_key_exchange::get_A_or_B_string({chat_data.iv_info.ab, chat_data.iv_info.g, chat_data.iv_info.p}),
        .g_iv=chat_data.iv_info.g,
        .p_iv=chat_data.iv_info.p,
    };
    data[chat_data.chat_id] = {chat_data, {}};
    return info;
}

std::pair<bool, ChatInfo> WorkWithData::update_chat_status(const ChatInfo & info) {
    std::unique_lock<std::mutex> locker(mutex);
    if (data.find(info.chat_id) == data.end()) {
        locker.unlock();
        ChatData chat_data{
            .interlocutor=info.interlocutor,
            .chat_id=info.chat_id,
            .alg_index=info.alg_index,
            .enc_mode_index=info.enc_mode_index,
            .padd_mode_index=info.padd_mode_index,
            .symmetric_context=std::shared_ptr<symmetric_interface_library::SymmetricContext>(nullptr),
            .status=0,
        };
        auto b_key = Diffie_Hellman_key_exchange::generate_string_b({info.g_key, info.p_key});
        auto b_iv = Diffie_Hellman_key_exchange::generate_string_b({info.g_iv, info.p_iv});
        chat_data.key_info = KeyInfo{
            .ab=b_key,
            .g=info.g_key,
            .p=info.p_key,
        };

        chat_data.iv_info = KeyInfo{
            .ab=b_iv,
            .g=info.g_iv,
            .p=info.p_iv,
        };
        data[chat_data.chat_id] = {chat_data, {}};
        locker.lock();
    }

    auto &chat_info = data[info.chat_id].first;
    if(chat_info.status != 0 and chat_info.status != 1) {
        std::cout << "!!!!!!!!!!!!!2" << std::endl;
        return {false, {}};
    }

    size_t block_size = symmetric_algorithms::get_alg_block_size(static_cast<symmetric_algorithms::SymmetricAlgorithmsEnum>(info.alg_index));
    std::string string_key = Diffie_Hellman_key_exchange::get_key_string({
        info.ab_key, chat_info.key_info.ab,
        chat_info.key_info.g, chat_info.key_info.p
    });
    chat_info.key_info.key = bytes_utility::get_bytes_from_string(string_key, block_size);

    std::string string_iv = Diffie_Hellman_key_exchange::get_key_string({
        info.ab_iv, chat_info.iv_info.ab,
        chat_info.iv_info.g, chat_info.iv_info.p
    });
    chat_info.iv_info.key = bytes_utility::get_bytes_from_string(string_key, block_size);
    // std::cout << "Key: " << string_key << std::endl;
    // std::cout << "IV: " << string_iv << std::endl;
    chat_info.symmetric_context = std::make_shared<symmetric_interface_library::SymmetricContext>(
        symmetric_algorithms::get_alg(static_cast<symmetric_algorithms::SymmetricAlgorithmsEnum>(info.alg_index), chat_info.iv_info.key),
        chat_info.iv_info.key,
        static_cast<symmetric_interface_library::EncryptionModeEnum>(chat_info.enc_mode_index),
        static_cast<symmetric_interface_library::PaddingModeEnum>(chat_info.padd_mode_index),
        chat_info.iv_info.key
    );
    if(chat_info.status == 1) {
        chat_info.status = 2;
        return {false, {}};
    }
    chat_info.status = 2;

    ChatInfo return_info {
        .interlocutor=info.interlocutor,
        .chat_id=chat_info.chat_id,
        .ab_key=Diffie_Hellman_key_exchange::get_A_or_B_string({chat_info.key_info.ab, chat_info.key_info.g, chat_info.key_info.p}),
        .g_key=chat_info.key_info.g,
        .p_key=chat_info.key_info.p,
        .ab_iv=Diffie_Hellman_key_exchange::get_A_or_B_string({chat_info.iv_info.ab, chat_info.iv_info.g, chat_info.iv_info.p}),
        .g_iv=chat_info.iv_info.g,
        .p_iv=chat_info.iv_info.p,
    };
    return {true, return_info};
}

std::vector<ChatInfo> WorkWithData::get_chats() {
    std::lock_guard<std::mutex> locker(mutex);
    std::vector<ChatInfo> chats;
    for(const auto & chat : data) {
        auto chat_info = chat.second.first;
        ChatInfo info {
            .interlocutor=chat_info.interlocutor,
            .chat_id=chat_info.chat_id,
            // остальная информация там не нужна
        };
        chats.push_back(info);
    }
    return chats;
}

bool WorkWithData::add_msg(const MsgData & msg_data) {
    std::lock_guard<std::mutex> locker(mutex);
    if(data.find(msg_data.chat_id) == data.end()) {
        return false;
    }
    data[msg_data.chat_id].second.push_back(msg_data);
    return true;
}


std::vector<MsgData> WorkWithData::get_msgs(const std::string & chat_id) {
    std::lock_guard<std::mutex> locker(mutex);
    if(data.find(chat_id) == data.end()) {
        return {};
    }
    std::vector<MsgData> res(data[chat_id].second);
    return res;
}



