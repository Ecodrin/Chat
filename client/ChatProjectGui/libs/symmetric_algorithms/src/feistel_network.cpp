#include "../include/feistel_network.hpp"



namespace symmetric_algorithms {
    FeistelNetwork::FeistelNetwork(
            std::shared_ptr<symmetric_interface_library::InterfaceGenerationRoundKeys> keys_expander, 
            std::shared_ptr<symmetric_interface_library::InterfaceEncryption> round_function, 
            const std::vector<std::byte> & key, 
            size_t number_rounds) :
        keys_expander{keys_expander}, round_function{round_function}, number_rounds{number_rounds} {
            round_keys = keys_expander->generation_round_keys(key, number_rounds);
    }   

    void FeistelNetwork::set_key(const std::vector<std::byte> & key) {
        round_keys = keys_expander->generation_round_keys(key, number_rounds);
    }

    std::vector<std::byte> FeistelNetwork::encryption(const std::vector<std::byte> & block) {
        std::vector<std::byte> result{block.size()};
        std::vector<std::byte> L{block.size() / 2};
        std::vector<std::byte> R{block.size() / 2};
        std::copy(block.begin(), block.begin() + block.size() / 2, L.begin());
        std::copy(block.begin() + block.size() / 2, block.end(), R.begin());
        for(size_t i = 0; i < number_rounds; ++i) {
            std::vector<std::byte> x = round_function->encryption(R, round_keys[i]);
            x = bytes_utility::xor_vector(x, L);
            L = R;
            R = x;

        }
        std::copy(L.begin(), L.end(), result.begin());
        std::copy(R.begin(), R.end(), result.begin() + result.size() / 2);
        return result;
    }

    std::vector<std::byte> FeistelNetwork::decryption(const std::vector<std::byte> & block) {
        std::vector<std::byte> result{block.size()};
        std::vector<std::byte> L{block.size() / 2};
        std::vector<std::byte> R{block.size() / 2};
        std::copy(block.begin(), block.begin() + block.size() / 2, L.begin());
        std::copy(block.begin() + block.size() / 2, block.end(), R.begin());
        for(size_t i = 0; i < number_rounds; ++i) {
            std::vector<std::byte> x = round_function->encryption(L, round_keys[round_keys.size() - i - 1]);
            x = bytes_utility::xor_vector(x, R);
            R = L;
            L = x;

        }
        std::copy(L.begin(), L.end(), result.begin());
        std::copy(R.begin(), R.end(), result.begin() + result.size() / 2);
        return result;
    }
}
