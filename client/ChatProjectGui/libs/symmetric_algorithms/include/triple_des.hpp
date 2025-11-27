#pragma once

#include <iostream>

#include "symmetric_encryption_interfaces.hpp"
#include "des.hpp"

namespace symmetric_algorithms {
    class TripleDES : public symmetric_interface_library::InterfaceSymmetricAlgorithm {
    private:
        size_t block_size = 8;
        DES alg1;
        DES alg2;
        DES alg3;
    public:
        enum class TripleDESType {
            EEE,
            EDE
        };

        TripleDES(const std::vector<std::byte> & key, TripleDESType triple_des_type = TripleDESType::EDE);
        size_t get_block_size() override;
        void set_key(const std::vector<std::byte> & key) override;
        std::vector<std::byte> encryption(const std::vector<std::byte> & encrypted_block) override;
        std::vector<std::byte> decryption(const std::vector<std::byte> & encrypted_block) override;
    
    private:
        TripleDESType triple_des_type;
    };
}