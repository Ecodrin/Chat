#include "../include/des.hpp"

namespace symmetric_algorithms {
    size_t DES::get_block_size() {
        return block_size;
    }

    

    void DES::set_key(const std::vector<std::byte> & key) {
        feistel_network.set_key(key);
    }

    std::vector<std::byte> DES::encryption(const std::vector<std::byte> & encrypted_block) {
        if (encrypted_block.size() != block_size) {
            throw std::runtime_error("incorrect block size");
        }
        std::vector<std::byte> permutatedBlock = bytes_utility::permutations(encrypted_block, IP_BLOCK, bytes_utility::PermutationsOrderRule::ForwardOrderFirstIndex);

        std::vector<std::byte> encryption_result = feistel_network.encryption(permutatedBlock);
        return bytes_utility::permutations(encryption_result, IP_INV_BLOCK, bytes_utility::PermutationsOrderRule::ForwardOrderFirstIndex);
    }
    
    std::vector<std::byte> DES::decryption(const std::vector<std::byte> & encrypted_block) {
        if (encrypted_block.size() != block_size) {
            throw std::runtime_error("incorrect block size");
        }
        std::vector<std::byte> permutatedBlock = bytes_utility::permutations(encrypted_block, IP_BLOCK, bytes_utility::PermutationsOrderRule::ForwardOrderFirstIndex);
        std::vector<std::byte> decryption_result = feistel_network.decryption(permutatedBlock);
        return bytes_utility::permutations(decryption_result, IP_INV_BLOCK, bytes_utility::PermutationsOrderRule::ForwardOrderFirstIndex);
    }

    DES::DES(const std::vector<std::byte> & key, std::shared_ptr<symmetric_interface_library::InterfaceEncryption> feistel_function, std::shared_ptr<symmetric_interface_library::InterfaceGenerationRoundKeys> des_generation_round_keys) : feistel_network{des_generation_round_keys, feistel_function, key} {

    }

    DES::DES(const std::vector<std::byte> & key) : feistel_network{std::make_shared<DESGenerationRoundKeys>(), std::make_shared<DESFeistelFunction>(), key} {
    }

    DES::DES() : feistel_network{std::make_shared<DESGenerationRoundKeys>(), std::make_shared<DESFeistelFunction>(), {
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, 
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}}} {

    }

    std::vector<std::byte> DESFeistelFunction::encryption(const std::vector<std::byte> & block, const std::vector<std::byte> & round_key) {
        std::vector<std::byte> permutated_block = bytes_utility::permutations(block, E_BLOCK, bytes_utility::PermutationsOrderRule::ForwardOrderFirstIndex);
        
        std::vector<std::byte> b_blocks = bytes_utility::xor_vector(permutated_block, round_key);
        
        std::vector<std::byte> s_blocks;
        size_t k = 0;
        std::byte tmp{0};
        for(size_t i = 1; i <= 8 * b_blocks.size(); ++i) {
            tmp |= bytes_utility::get_bit(b_blocks, i, bytes_utility::PermutationsOrderRule::ForwardOrderFirstIndex);
            if(i % 6 == 0) {
                s_blocks.push_back(apply_sbox(tmp, i / 6 - 1));
                tmp = std::byte{0};
            }
            tmp <<= 1;
        }

        return bytes_utility::permutations(s_blocks, P_BLOCK, bytes_utility::PermutationsOrderRule::ForwardOrderFirstIndex);
    }

    std::byte DESFeistelFunction::apply_sbox(std::byte input, size_t s_box_num) {
        if (s_box_num >= S_BLOCKS.size()) {
            throw std::invalid_argument("Invalid S-box number or input");
        }
        const std::vector<std::vector<size_t>>& sbox = S_BLOCKS[s_box_num];
        // 1 и 6 бит
        size_t row = static_cast<size_t>((input & std::byte{0x01}) | ((input & std::byte{0x20}) >> 4));
        // 2, 3, 4, 5
        size_t col = static_cast<size_t>((input & std::byte{0x1E}) >> 1); 
        
        return static_cast<std::byte>(sbox[row][col]);
    }

    std::vector<std::byte> DESGenerationRoundKeys::additional_original_key(const std::vector<std::byte> & original_key) {
        size_t number_units = 0;
        std::vector<std::byte> augmented_key;
        std::vector<size_t> p_block;
        for(size_t i = 1; i <= original_key.size() * 8; ++i) {
            if(p_block.size() == 7) {
                std::vector<std::byte> t = bytes_utility::permutations(original_key, p_block, bytes_utility::PermutationsOrderRule::ForwardOrderFirstIndex);
                std::byte new_b = t[0];
                size_t count = bytes_utility::counting_number_units(new_b);
                new_b <<= 1;
                if (count % 2 == 0) {
                    new_b |= std::byte{1};
                }
                augmented_key.push_back(new_b);
                p_block.clear();
            }
            p_block.push_back(i);
        }

        {
            // последний бит дополняем 
            std::vector<std::byte> t = bytes_utility::permutations(original_key, p_block, bytes_utility::PermutationsOrderRule::ForwardOrderFirstIndex);
            std::byte new_b = t[0];
            size_t count = bytes_utility::counting_number_units(new_b);
            new_b <<= 1;
            if (count % 2 == 0) {
                new_b |= std::byte{1};
            }
            augmented_key.push_back(new_b);
            p_block.clear();
        }
        return augmented_key;
    }


    std::vector<std::vector<std::byte>> DESGenerationRoundKeys::generation_round_keys(const std::vector<std::byte> & original_key, size_t number_round_keys) {
        if((original_key.size() != key_size && original_key.size() != key_size + 1)  || number_round_keys != 16) {
            throw std::runtime_error("incorrect size original_key");
        }
        std::vector<std::byte> augmented_key;
        if(original_key.size() == key_size + 1) {
            augmented_key = original_key;
        } else {
            augmented_key = additional_original_key(original_key);
        }
        std::vector<std::byte> C;
        std::vector<std::byte> D;
        C = bytes_utility::permutations(augmented_key, C0, bytes_utility::PermutationsOrderRule::ForwardOrderFirstIndex);
        D = bytes_utility::permutations(augmented_key, D0, bytes_utility::PermutationsOrderRule::ForwardOrderFirstIndex);
        std::vector<std::vector<std::byte>> keys;
        for(size_t i = 0; i < number_round_keys; ++i) {
            C = bytes_utility::cycling_rotate_left(C, KEY_SHIFTS[i], 28);
            D = bytes_utility::cycling_rotate_left(D, KEY_SHIFTS[i], 28);
            keys.push_back(generate_round_key(C, D, i));
        }
        return keys;
    }

    std::vector<std::byte> DESGenerationRoundKeys::generate_round_key(const std::vector<std::byte> &C, const std::vector<std::byte> &D, size_t i) {
        std::vector<std::byte> t = bytes_utility::connect_arrays(C, 28, D, 28);

        return bytes_utility::permutations(t, PS2, bytes_utility::PermutationsOrderRule::ForwardOrderFirstIndex);
    }
}