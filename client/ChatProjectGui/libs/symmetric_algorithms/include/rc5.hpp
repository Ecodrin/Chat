#pragma once

#include <boost/multiprecision/cpp_int.hpp>
#include <boost/multiprecision/cpp_dec_float.hpp>
#include <cmath>


#include "symmetric_encryption_interfaces.hpp"
#include "bytes_utility.hpp"

namespace symmetric_algorithms {

    class RC5GenerationKeys: public symmetric_interface_library::InterfaceGenerationRoundKeys {
    private:
        size_t w_len, b_len;

        boost::multiprecision::cpp_dec_float_50 golden_ratio() const;
        static boost::multiprecision::cpp_int round_odd(boost::multiprecision::cpp_dec_float_50 number);
    public:
        static boost::multiprecision::cpp_int cycling_rotate_left(const boost::multiprecision::cpp_int & number, size_t shift, size_t width);
        static boost::multiprecision::cpp_int cycling_rotate_right(const boost::multiprecision::cpp_int & number, size_t shift, size_t width);
        static boost::multiprecision::cpp_int convert_to_cpp_int(const std::vector<std::byte> & block);
        static std::vector<std::byte> convert_to_bytes_vector(const boost::multiprecision::cpp_int & block);
        
        RC5GenerationKeys(size_t w_len, size_t b_len);
        std::vector<std::vector<std::byte>> generation_round_keys(const std::vector<std::byte> & original_key, size_t number_round_keys = 16) override;
    };

    class RC5 : public symmetric_interface_library::InterfaceSymmetricAlgorithm {
    private:
        RC5GenerationKeys keys_expander;
        size_t real_block_size;
        size_t number_rounds;

        std::vector<std::byte> key;
        std::vector<std::vector<std::byte>> S;
        std::vector<boost::multiprecision::cpp_int> S_b;
        
    public:
        RC5(const std::vector<std::byte> & key, size_t block_size=8, size_t number_rounds=20);
        size_t get_block_size() override;
        std::vector<std::byte> encryption(const std::vector<std::byte> & encrypted_block) override;
        std::vector<std::byte> decryption(const std::vector<std::byte> & encrypted_block) override;
        void set_key(const std::vector<std::byte> & key) override;
        inline static size_t standart_block_size = 8;
        inline static size_t standart_key_size = 32;
    };
}