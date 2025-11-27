#pragma once

#include <iostream>
#include <vector>
#include "symmetric_encryption_interfaces.hpp"

namespace symmetric_algorithms {

    class FeistelNetwork {
    protected:
        std::shared_ptr<symmetric_interface_library::InterfaceGenerationRoundKeys> keys_expander;
        std::shared_ptr<symmetric_interface_library::InterfaceEncryption> round_function;
        size_t number_rounds = 16;

        std::vector<std::vector<std::byte>> round_keys;

    public:
        FeistelNetwork(
            std::shared_ptr<symmetric_interface_library::InterfaceGenerationRoundKeys> keys_expander, 
            std::shared_ptr<symmetric_interface_library::InterfaceEncryption> round_function, 
            const std::vector<std::byte> & key, 
            size_t number_rounds = 16);

        void set_key(const std::vector<std::byte> & key);

        std::vector<std::byte> encryption(const std::vector<std::byte> & block);
        std::vector<std::byte> decryption(const std::vector<std::byte> & block);
    };

}