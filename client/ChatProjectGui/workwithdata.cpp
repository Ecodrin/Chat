#include "workwithdata.h"

WorkWithData::WorkWithData(const std::string & files_path) : files_path{files_path} {

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
        .alg_index=chat_data.alg_index,
        .enc_mode_index=chat_data.enc_mode_index,
        .padd_mode_index=chat_data.padd_mode_index
    };
    data[chat_data.chat_id] = {chat_data, {}};
    return info;
}

std::pair<bool, ChatInfo> WorkWithData::update_chat_status(const ChatInfo & info) {
    std::unique_lock<std::mutex> locker(mutex);
    if (data.find(info.chat_id) == data.end()) {
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
    }

    auto &chat_info = data[info.chat_id].first;
    if(chat_info.status != 0 and chat_info.status != 1) {
        std::cout << "!!!!!!!!!!!!!2" << std::endl;
        return {false, {}};
    }

    size_t key_size = symmetric_algorithms::get_alg_key_size(static_cast<symmetric_algorithms::SymmetricAlgorithmsEnum>(info.alg_index));
    size_t block_size = symmetric_algorithms::get_alg_block_size(static_cast<symmetric_algorithms::SymmetricAlgorithmsEnum>(info.alg_index));
    std::string string_key = Diffie_Hellman_key_exchange::get_key_string({
        info.ab_key, chat_info.key_info.ab,
        chat_info.key_info.g, chat_info.key_info.p
    });
    auto t = bytes_utility::get_bytes_from_string_numbers(string_key);
    t.resize(key_size);
    chat_info.key_info.key = t;

    std::string string_iv = Diffie_Hellman_key_exchange::get_key_string({
        info.ab_iv, chat_info.iv_info.ab,
        chat_info.iv_info.g, chat_info.iv_info.p
    });

    t = bytes_utility::get_bytes_from_string_numbers(string_iv);
    t.resize(block_size);
    chat_info.iv_info.key = t;
    chat_info.symmetric_context = std::make_shared<symmetric_interface_library::SymmetricContext>(
        symmetric_algorithms::get_alg(static_cast<symmetric_algorithms::SymmetricAlgorithmsEnum>(info.alg_index), chat_info.key_info.key),
        chat_info.key_info.key,
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
        .alg_index=chat_info.alg_index,
        .enc_mode_index=chat_info.enc_mode_index,
        .padd_mode_index=chat_info.padd_mode_index
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
    auto & chat = data[msg_data.chat_id].first;
    std::vector<std::byte> decr;
    chat.symmetric_context->decryption(bytes_utility::get_bytes_from_string(msg_data.data), decr).get();
    MsgData new_msg_data{
        .chat_id=msg_data.chat_id,
        .is_file=msg_data.is_file,
        .sender=msg_data.sender,
        .recipient=msg_data.recipient,
        .data = bytes_utility::get_string_from_bytes(decr),
        .timestamp=msg_data.timestamp,
    };
    data[msg_data.chat_id].second.push_back(new_msg_data);
    return true;
}


std::pair<bool, MsgData> WorkWithData::send_msg(const MsgData & msg_data) {
    std::lock_guard<std::mutex> locker(mutex);
    if(data.find(msg_data.chat_id) == data.end()) {
        return {false, {}};
    }
    auto & chat = data[msg_data.chat_id].first;
    std::vector<std::byte> encr;
    chat.symmetric_context->encryption(bytes_utility::get_bytes_from_string(msg_data.data), encr).get();
    MsgData new_msg_data{
        .chat_id=msg_data.chat_id,
        .is_file=msg_data.is_file,
        .sender=msg_data.sender,
        .recipient=chat.interlocutor,
        .data = bytes_utility::get_string_from_bytes(encr),
        .timestamp=msg_data.timestamp,
    };
    data[msg_data.chat_id].second.push_back(msg_data);
    return {true, new_msg_data};
}


std::vector<MsgData> WorkWithData::get_msgs(const std::string & chat_id) {
    std::lock_guard<std::mutex> locker(mutex);
    if(data.find(chat_id) == data.end()) {
        return {};
    }
    std::vector<MsgData> res(data[chat_id].second);

    return res;
}

bool WorkWithData::add_file(const FileData & msg_data) {
    std::ofstream out(files_path + "/" + msg_data.file_name, std::ios::app);
    if(!out.is_open()) {
        return false;
    }
    out << msg_data.data;
    out.close();

    if (msg_data.index_file_chunk == msg_data.total_file_chunk) {
        std::unique_lock<std::mutex> locker(mutex);
        auto & chat = data[msg_data.chat_id].first;
        locker.unlock();

        std::string output= msg_data.file_name;
        output = output.replace(0, 4, "");

        std::filesystem::path output_file_name = files_path + "/" + output;
        chat.symmetric_context->decryption(files_path + "/" + msg_data.file_name, output_file_name.string()).get();

        locker.lock();
        data[msg_data.chat_id].second.emplace_back(MsgData{
            .chat_id = msg_data.chat_id,
            .is_file = true,
            .sender = msg_data.sender,
            .recipient = msg_data.recipient,
            .data = output_file_name.string(),
            .timestamp = msg_data.timestamp,
        });
        std::filesystem::remove(files_path + "/" + msg_data.file_name);
    }
    return true;
}


std::string WorkWithData::send_file(const FileData & msg_data) {
    std::unique_lock<std::mutex> locker(mutex);
    auto & chat = data[msg_data.chat_id].first;
    locker.unlock();
    std::filesystem::path output = std::filesystem::path{msg_data.file_name}.filename();

    std::filesystem::path output_file_name = files_path + "/enc_" + output.string();

    std::cout << msg_data.file_name << " " << output_file_name.string() << std::endl;
    chat.symmetric_context->encryption(msg_data.file_name, output_file_name.string()).get();

    locker.lock();
    data[msg_data.chat_id].second.emplace_back(MsgData{
        .chat_id = msg_data.chat_id,
        .is_file = true,
        .sender = msg_data.sender,
        .recipient = chat.interlocutor,
        .data = msg_data.file_name,
        .timestamp = msg_data.timestamp,
    });
    return output_file_name.string();
}

std::string WorkWithData::get_recipient(const std::string & chat_id) {
    std::lock_guard<std::mutex> locker(mutex);
    auto & chat = data[chat_id].first;
    return chat.interlocutor;
}




