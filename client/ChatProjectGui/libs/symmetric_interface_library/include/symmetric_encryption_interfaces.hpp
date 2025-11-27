#pragma once

#include <cstddef>
#include <vector>
#include <iostream>
#include <future>
#include <thread>
#include <fstream>
#include <mutex>
#include <any>
#include <random>

#include "bytes_utility.hpp"

namespace symmetric_interface_library{
    class InterfaceGenerationRoundKeys {
    public: 
        virtual std::vector<std::vector<std::byte>> generation_round_keys(const std::vector<std::byte> & original_key, size_t number_round_keys = 16) = 0;
    };

    class InterfaceEncryption {
    public:
        virtual std::vector<std::byte> encryption(const std::vector<std::byte> & block, const std::vector<std::byte> & round_key) = 0;
    };

    class InterfaceSymmetricAlgorithm {
    public:
        virtual size_t get_block_size() = 0;
        virtual void set_key(const std::vector<std::byte> & key) = 0;
        virtual std::vector<std::byte> encryption(const std::vector<std::byte> & encrypted_block) = 0;
        virtual std::vector<std::byte> decryption(const std::vector<std::byte> & encrypted_block) = 0;

    };

}