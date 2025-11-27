#pragma once

#include <iostream>
#include <vector>
#include <functional>
#include <memory>

#include "symmetric_encryption_interfaces.hpp"

namespace symmetric_algorithms {
    class SPNetwork {
        std::shared_ptr<symmetric_interface_library::InterfaceGenerationRoundKeys> keys_expander;
        size_t number_rounds = 16;

        std::vector<std::vector<std::byte>> round_keys;
        std::function<std::vector<std::byte>(const std::vector<std::byte> &, size_t round)> sbox_layer;
        std::function<std::vector<std::byte>(const std::vector<std::byte> &)> p_layer;
        std::function<std::vector<std::byte>(const std::vector<std::byte> &, size_t round)> inverse_sbox_layer;
        std::function<std::vector<std::byte>(const std::vector<std::byte> &)> inverse_p_layer;

    public:
        SPNetwork(
            std::shared_ptr<symmetric_interface_library::InterfaceGenerationRoundKeys> keys_expander,
            const std::vector<std::byte> & key, 
            std::function<std::vector<std::byte>(const std::vector<std::byte> &, size_t round)> sbox_layer,
            std::function<std::vector<std::byte>(const std::vector<std::byte> &)> p_layer,
            std::function<std::vector<std::byte>(const std::vector<std::byte> &, size_t round)> inverse_sbox_layer,
            std::function<std::vector<std::byte>(const std::vector<std::byte> &)> inverse_p_layer,
            size_t number_rounds = 16);

        void set_key(const std::vector<std::byte> & key, size_t number_keys);

        std::vector<std::byte> encryption(const std::vector<std::byte> & block);
        std::vector<std::byte> decryption(const std::vector<std::byte> & block);

        std::vector<std::vector<std::byte>> get_round_keys() const;
    };
}