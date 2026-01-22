#pragma once

#include <iostream>
#include <cstddef>

#include "Galois_field_service.hpp"
#include "symmetric_encryption_interfaces.hpp"

namespace symmetric_algorithms {

    class RijndaelGenerationKeys: public symmetric_interface_library::InterfaceGenerationRoundKeys {
    private:
        size_t block_size;
        std::byte polynomial;
        std::vector<std::byte> sbox;
        void generate_sbox();
        void sub_word(std::vector<std::byte> & state);

        std::vector<std::vector<std::byte>> rcon;
    public:
        void print_sbox();
        RijndaelGenerationKeys(size_t block_size, size_t i_polynomial);
        std::vector<std::vector<std::byte>> generation_round_keys(const std::vector<std::byte> & original_key, size_t number_round_keys=10) override;
    };


    class Rijndael : public symmetric_interface_library::InterfaceSymmetricAlgorithm {
    private:
        RijndaelGenerationKeys keys_expander;
        std::vector<std::byte> sbox;
        std::vector<std::byte> inv_sbox;
        size_t block_size;
        size_t irreducible_polynomial;
        size_t number_rounds;
        std::vector<std::vector<std::byte>> round_keys;
        std::vector<std::byte> irreducible_polynomials = GaloisFieldService::get_irreducible_polynomials();

        void calculate_add_round_key(std::vector<std::vector<std::byte>> & state, size_t index);
        void calculate_shift_rows(std::vector<std::vector<std::byte>> & state);
        void calculate_inv_shift_rows(std::vector<std::vector<std::byte>> & state);
        void generate_sbox();
        void generate_inv_sbox();
        void inv_sub_bytes(std::vector<std::vector<std::byte>> & state);
        void sub_bytes(std::vector<std::vector<std::byte>> & state);
        void mix_columns(std::vector<std::vector<std::byte>> & state);
        void inv_mix_columns(std::vector<std::vector<std::byte>> & state);

        size_t calculate_number_rounds(size_t key_size);

        std::vector<std::vector<std::byte>> convert_to_state(const std::vector<std::byte> & vec);
        std::vector<std::byte> convert_to_vec(const  std::vector<std::vector<std::byte>> & state);

        std::vector<std::vector<std::byte>> mix_colums_tab = {
                {std::byte{0x02}, std::byte{0x03}, std::byte{0x01}, std::byte{0x01}},
                {std::byte{0x01}, std::byte{0x02}, std::byte{0x03}, std::byte{0x01}},
                {std::byte{0x01}, std::byte{0x01}, std::byte{0x02}, std::byte{0x03}},
                {std::byte{0x03}, std::byte{0x01}, std::byte{0x01}, std::byte{0x02}}
        };

        std::vector<std::vector<std::byte>> inv_mix_colums_tab = {
                {std::byte{0x0E}, std::byte{0x0B}, std::byte{0x0D}, std::byte{0x09}},
                {std::byte{0x09}, std::byte{0x0E}, std::byte{0x0B}, std::byte{0x0D}},
                {std::byte{0x0D}, std::byte{0x09}, std::byte{0x0E}, std::byte{0x0B}},
                {std::byte{0x0B}, std::byte{0x0D}, std::byte{0x09}, std::byte{0x0E}}
        };

        void print_state(const std::vector<std::vector<std::byte>> & state) const;
    public:
        void print_sbox();
        void print_inv_sbox();
        Rijndael(const std::vector<std::byte> & key, size_t block_size=16, size_t irreducible_polynomial=0);


        std::vector<std::byte> encryption(const std::vector<std::byte> & encrypted_block) override;
        std::vector<std::byte> decryption(const std::vector<std::byte> & encrypted_block) override;
        
        size_t get_block_size() override;
        void set_key(const std::vector<std::byte> & key) override;
    };
}