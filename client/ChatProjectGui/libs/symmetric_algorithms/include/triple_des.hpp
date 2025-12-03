#pragma once

#include <iostream>

#include "symmetric_encryption_interfaces.hpp"
#include "des.hpp"

namespace symmetric_algorithms {
    class TripleDES : public symmetric_interface_library::InterfaceSymmetricAlgorithm {
    private:
        DES alg1;
        DES alg2;
        DES alg3;
    public:
        enum class TripleDESType {
            EEE,
            EDE
        };

        TripleDES(const std::vector<std::byte> & key, TripleDESType triple_des_type = TripleDESType::EEE);
        TripleDES() = default;
        size_t get_block_size() override;
        void set_key(const std::vector<std::byte> & key) override;
        std::vector<std::byte> encryption(const std::vector<std::byte> & encrypted_block) override;
        std::vector<std::byte> decryption(const std::vector<std::byte> & encrypted_block) override;

        static inline size_t standart_key_size = 21;
        static inline size_t block_size = 8;
    
    private:
        TripleDESType triple_des_type;
    };
}