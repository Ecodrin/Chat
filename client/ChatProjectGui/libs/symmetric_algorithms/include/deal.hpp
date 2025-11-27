#pragma once 

#include <iostream>
#include <vector>
#include <cstddef>

#include "symmetric_encryption_interfaces.hpp"
#include "des.hpp"
#include "bytes_utility.hpp"
#include "feistel_network.hpp"

namespace symmetric_algorithms {
    class DEALGenerationRoundKeys : public symmetric_interface_library::InterfaceGenerationRoundKeys {
    private:
        std::shared_ptr<symmetric_interface_library::InterfaceSymmetricAlgorithm> cipher;
        void check_correct_number_round_keys(size_t keys_size, size_t number_round_keys);
        std::vector<std::vector<std::byte>> generate_start_keys(const std::vector<std::byte> & original_key);
    public:
        DEALGenerationRoundKeys();
        std::vector<std::vector<std::byte>> generation_round_keys(const std::vector<std::byte> & original_key, size_t number_round_keys=6) override;
    };

    class DEALFeistelFunction : public symmetric_interface_library::InterfaceEncryption {
    private:
        // std::shared_ptr<symmetric_interface_library::InterfaceSymmetricAlgorithm> cipher;
    public:
        DEALFeistelFunction();
        std::vector<std::byte> encryption(const std::vector<std::byte> & block, const std::vector<std::byte> & round_key) override;
    };

    class DEAL : public symmetric_interface_library::InterfaceSymmetricAlgorithm {
    private:
        size_t block_size = 16;
        FeistelNetwork feistel_network;
        static size_t get_number_rounds(size_t keys_size_t);

    public:
        DEAL(const std::vector<std::byte> & key, std::shared_ptr<symmetric_interface_library::InterfaceEncryption> deal_feistel_function, std::shared_ptr<symmetric_interface_library::InterfaceGenerationRoundKeys> deal_generation_round_keys);
        DEAL(const std::vector<std::byte> & key);
        size_t get_block_size() override;
        void set_key(const std::vector<std::byte> & key) override;
        std::vector<std::byte> encryption(const std::vector<std::byte> & encrypted_block) override;
        std::vector<std::byte> decryption(const std::vector<std::byte> & encrypted_block) override;
    };
}