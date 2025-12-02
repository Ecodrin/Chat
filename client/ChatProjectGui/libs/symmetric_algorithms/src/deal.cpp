#include "../include/deal.hpp"

namespace symmetric_algorithms {
    DEALFeistelFunction::DEALFeistelFunction() {

    }

    std::vector<std::byte> DEALFeistelFunction::encryption(const std::vector<std::byte> & block, const std::vector<std::byte> & round_key) {
        DES cipher{round_key};
        return cipher.encryption(block);
    }

    DEALGenerationRoundKeys::DEALGenerationRoundKeys(){
        cipher = std::make_shared<DES>(std::vector<std::byte>{
                                        std::byte{0x12},
                                        std::byte{0x34}, 
                                        std::byte{0x56},
                                        std::byte{0x78}, 
                                        std::byte{0x90}, 
                                        std::byte{0xAB}, 
                                        std::byte{0xCD}, 
                                        std::byte{0xEF}}, 
                                        std::make_shared<DESFeistelFunction>(), std::make_shared<DESGenerationRoundKeys>());
    }

    void DEALGenerationRoundKeys::check_correct_number_round_keys(size_t keys_size_t, size_t number_round_keys) {
        switch (keys_size_t)
        {
        case 16:
        case 24:
            if(number_round_keys != 6) {
                throw std::runtime_error("unknown number_round_keys");
            }
            break;
        case 32:
            if(number_round_keys != 8) {
                throw std::runtime_error("unknown number_round_keys");
            }
            break;    
        default:
            throw std::runtime_error("unknown block size");
        }
    }

    std::vector<std::vector<std::byte>> DEALGenerationRoundKeys::generation_round_keys(const std::vector<std::byte> & original_key, size_t number_round_keys) {
        check_correct_number_round_keys(original_key.size(), number_round_keys);
        std::vector<std::vector<std::byte>> round_keys;
        std::vector<std::vector<std::byte>> special_keys = generate_start_keys(original_key);
        std::vector<std::byte> special_64_bit_number(8);
        special_64_bit_number[0] = std::byte{1}; 
        for(size_t i = 0; i < number_round_keys / special_keys.size(); ++i) {
            for(size_t j = 0; j < special_keys.size(); ++j) {
                std::vector<std::byte> tmp_xor;
                if(j == 0 && i == 0) {
                    round_keys.push_back(cipher->encryption(special_keys[0]));
                } else if(i == 0) {     
                    round_keys.push_back(cipher->encryption(bytes_utility::xor_vector(special_keys[j], round_keys.back())));
                } else {
                    round_keys.push_back(cipher->encryption(bytes_utility::xor_vector(special_64_bit_number, bytes_utility::xor_vector(special_keys[j], round_keys.back()))));
                    special_64_bit_number = bytes_utility::cycling_rotate_left(special_64_bit_number, 1, 64);
                }
            }
        }
        return round_keys;
    }

    std::vector<std::vector<std::byte>> DEALGenerationRoundKeys::generate_start_keys(const std::vector<std::byte> & original_key) {
        std::vector<std::vector<std::byte>> special_keys(original_key.size() / 8);
        for(size_t i = 0; i < special_keys.size(); ++i) {
            special_keys[i].resize(8);
            std::copy(original_key.begin() + i * 8, original_key.begin() + (i + 1) * 8, special_keys[i].begin());
        }
        return special_keys;
    }

    size_t DEAL::get_block_size() {
        return block_size;
    }

    void DEAL::set_key(const std::vector<std::byte> & key) {
        feistel_network.set_key(key);
    }

    DEAL::DEAL(const std::vector<std::byte> & key, std::shared_ptr<symmetric_interface_library::InterfaceEncryption> deal_feistel_function, std::shared_ptr<symmetric_interface_library::InterfaceGenerationRoundKeys> deal_generation_round_keys) : 
        feistel_network{deal_generation_round_keys, deal_feistel_function, key, get_number_rounds(key.size())} {

    }
    
    DEAL::DEAL(const std::vector<std::byte> & key) : feistel_network{std::make_shared<DEALGenerationRoundKeys>(), std::make_shared<DEALFeistelFunction>(), key, get_number_rounds(key.size())} {

    }

    size_t DEAL::get_number_rounds(size_t key_size) {
        size_t number_rounds;
        switch (key_size)
        {
        case 16:
        case 24:
            number_rounds = 6;
            break;
        case 32:
            number_rounds = 8;
            break;
        default:
            throw std::runtime_error("unknown key size");
        }
        return number_rounds;
    }

    std::vector<std::byte> DEAL::encryption(const std::vector<std::byte> & encrypted_block) {
        if(encrypted_block.size() != block_size) {
            throw std::invalid_argument("incorrect block size");
        }
        return feistel_network.encryption(encrypted_block);
    }
    
    std::vector<std::byte> DEAL::decryption(const std::vector<std::byte> & encrypted_block) {
        if(encrypted_block.size() != block_size) {
            throw std::invalid_argument("incorrect block size");
        }
        return feistel_network.decryption(encrypted_block);
    }

}