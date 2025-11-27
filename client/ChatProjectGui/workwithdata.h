#ifndef WORKWITHDATA_H
#define WORKWITHDATA_H

#include <string>
#include <mutex>
#include <vector>
#include <cstddef>
#include <unordered_map>

#include "encryption_modes.hpp"
#include "encryption_padding.hpp"
#include "symmetric_context.hpp"

struct ChatData {
    std::string chat_id;
    std::string ab;
    std::string g;
    std::string p;
    std::vector<std::byte> key;
    symmetric_interface_library::EncryptionModeEnum enc_mode;
    symmetric_interface_library::PaddingModeEnum padd_mode;
    symmetric_interface_library::SymmetricContext symmetric_context;
    int status;
};

class WorkWithData {
public:
    WorkWithData();


private:
    // потом переписать на бд
    std::mutex mutex;
    std::unordered_map<std::string, ChatData> data;

};

#endif // WORKWITHDATA_H
