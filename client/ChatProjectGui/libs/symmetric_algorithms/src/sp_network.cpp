#include "../include/sp_network.hpp"


namespace symmetric_algorithms {
    SPNetwork::SPNetwork(
            std::shared_ptr<symmetric_interface_library::InterfaceGenerationRoundKeys> keys_expander,
            const std::vector<std::byte> & key, 
            std::function<std::vector<std::byte>(const std::vector<std::byte> &, size_t round)> sbox_layer,
            std::function<std::vector<std::byte>(const std::vector<std::byte> &)> p_layer,
            std::function<std::vector<std::byte>(const std::vector<std::byte> &, size_t round)> inverse_sbox_layer,
            std::function<std::vector<std::byte>(const std::vector<std::byte> &)> inverse_p_layer,
            size_t number_rounds) : 
                keys_expander{keys_expander}, sbox_layer{sbox_layer}, p_layer{p_layer}, number_rounds{number_rounds},
                inverse_sbox_layer{inverse_sbox_layer}, inverse_p_layer{inverse_p_layer} {
        round_keys = keys_expander->generation_round_keys(key);
    }

    void SPNetwork::set_key(const std::vector<std::byte> & key, size_t number_keys) {
        round_keys = keys_expander->generation_round_keys(key, number_keys);
    }

    std::vector<std::byte> SPNetwork::encryption(const std::vector<std::byte> & block) {
        if (round_keys.size() != number_rounds + 1) {
            throw std::invalid_argument("round_keys.size() != number_rounds + 1");
        }
        std::vector<std::byte> new_block{block};
        for(size_t i = 0; i < number_rounds; ++i) {
            new_block = bytes_utility::xor_vector(round_keys[i], new_block);
            new_block = sbox_layer(new_block, i);
            if (i != number_rounds - 1) {
                new_block = p_layer(new_block);
            }
        }
        new_block = bytes_utility::xor_vector(new_block, round_keys[round_keys.size() - 1]);
        return new_block;
    }

    std::vector<std::byte> SPNetwork::decryption(const std::vector<std::byte> & block) {
        if (round_keys.size() != number_rounds + 1) {
            throw std::invalid_argument("round_keys.size() != number_rounds + 1");
        }
        std::vector<std::byte> new_block(block);
        new_block = bytes_utility::xor_vector(new_block, round_keys[round_keys.size() - 1]);
        for(int i = (int)number_rounds - 1; i >= 0; --i) {
            if (i != number_rounds - 1) {
                new_block = inverse_p_layer(new_block);
            }
            new_block = inverse_sbox_layer(new_block, i);
            new_block = bytes_utility::xor_vector(new_block, round_keys[i]);
        }
        return new_block;
    }

    std::vector<std::vector<std::byte>> SPNetwork::get_round_keys() const {
        return round_keys;
    }
}