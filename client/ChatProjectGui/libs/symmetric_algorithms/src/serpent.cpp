#include "../include/serpent.hpp"

namespace symmetric_algorithms {
    std::vector<std::vector<std::byte>> SerpentGenerationKeys::generation_round_keys(const std::vector<std::byte> & original_key, size_t number_round_keys) {
        if (original_key.size() != 32) {
            throw std::invalid_argument("key size must be 256 bit");
        }
        std::vector<std::vector<std::byte>> keys; 
        std::vector<std::vector<std::byte>> w(8);
        for(size_t i = 0; i < 8; ++i) {
            w[i].resize(4);
            std::copy(original_key.begin() + i * 4, original_key.begin() + (i + 1) * 4, w[i].begin());
        }
        int k = 3;
        for(size_t i = 0; i < number_round_keys; ++i) {
            for(size_t j = 0; j < 4; ++j) {
                std::vector<std::byte> new_w = bytes_utility::xor_vector(w[0], w[3]);
                new_w = bytes_utility::xor_vector(new_w, w[5]);
                new_w = bytes_utility::xor_vector(new_w, w[7]);
                new_w = bytes_utility::xor_vector(new_w, phi);
                new_w = bytes_utility::xor_vector(new_w, {std::byte{0}, std::byte{0}, std::byte{0}, std::byte{(unsigned char)(4 * i + j)}});
                new_w = bytes_utility::cycling_rotate_left(new_w, 11, new_w.size() * 8);
                w.erase(w.begin());
                w.push_back(new_w);
            }
            std::vector<std::byte> round_key;
            std::byte b;
            uint8_t s;
            std::byte out;
            for(size_t j = 0; j < 16; ++j) {
                b = {w[4 + (j / 4)][j % 4]};
                std::byte t1  = b >> 4;
                std::byte res_t1 = std::byte(sboxes[k][static_cast<size_t>(t1)]);
                std::byte t2 = b & std::byte{0x0F};
                std::byte res_t2 = std::byte(sboxes[k][static_cast<size_t>(t2)]);
                auto tmp = (res_t1 << 4) | res_t2;
                round_key.push_back(tmp);
            }
            keys.emplace_back(round_key);          
            k = ((k - 1) + 8) % 8;
        }
        return keys;
    }

    size_t Serpent::get_block_size() {
        return block_size;
    }

    void Serpent::set_key(const std::vector<std::byte> & key) {
        std::vector<std::byte> copy_key(key);
        if (copy_key.size() != 32) {
            copy_key.push_back(std::byte{1} << 7);
            for(int i = 0; i < 31 - key.size(); ++i) {
                copy_key.push_back(std::byte{0});
            }
        }
        sp_network.set_key(copy_key, 33);
    }

    
    Serpent::Serpent(const std::vector<std::byte> & key): 
        sp_network{ 
            std::make_shared<SerpentGenerationKeys>(),
            key,
            Serpent::sbox_layer,
            Serpent::p_layer,
            Serpent::inverse_sbox_layer,
            Serpent::inverse_p_layer,
            32
        } {
    }

    std::vector<std::byte> Serpent::encryption(const std::vector<std::byte> & encrypted_block) {
        if (encrypted_block.size() != block_size) {
            throw std::invalid_argument("incorrect block size");
        }
        std::vector<std::byte> first_permutations = bytes_utility::permutations(encrypted_block, IP, bytes_utility::PermutationsOrderRule::ForwardOrderZeroIndex);

        std::vector<std::byte> res = sp_network.encryption(first_permutations);

        std::vector<std::byte> second_permutations = bytes_utility::permutations(res, IIP, bytes_utility::PermutationsOrderRule::ForwardOrderZeroIndex);
        return second_permutations;
    }

    std::vector<std::byte> Serpent::decryption(const std::vector<std::byte> & encrypted_block) {
        if (encrypted_block.size() != block_size) {
            throw std::invalid_argument("incorrect block size");
        }
        std::vector<std::byte> first_permutations = bytes_utility::permutations(encrypted_block, IP, bytes_utility::PermutationsOrderRule::ForwardOrderZeroIndex);

        std::vector<std::byte> res = sp_network.decryption(first_permutations);

        std::vector<std::byte> second_permutations = bytes_utility::permutations(res, IIP, bytes_utility::PermutationsOrderRule::ForwardOrderZeroIndex);
        return second_permutations;
    }

    std::vector<std::byte> Serpent::sbox_layer(const std::vector<std::byte> & block, size_t round) {

        std::vector<size_t> sbox = Serpent::sboxes[round % 8];
        std::vector<std::byte> res;
        std::byte tmp;
        for(size_t i = 0; i < block.size(); ++i) {

            std::byte t = block[i];
            std::byte t1  = t >> 4;
            std::byte res_t1 = std::byte(sbox[static_cast<size_t>(t1)]);
            std::byte t2 = t & std::byte{0x0F};
            std::byte res_t2 = std::byte(sbox[static_cast<size_t>(t2)]);
            tmp = (res_t1 << 4) | res_t2;
            res.push_back(tmp);
        }
        return res;
    }

    std::vector<std::byte> Serpent::inverse_sbox_layer(const std::vector<std::byte> & block, size_t round) {
        std::vector<size_t> sbox = Serpent::inv_sboxes[round % 8];
        std::vector<std::byte> res;
        std::byte tmp;
        for(size_t i = 0; i < block.size(); ++i) {

            std::byte t = block[i];
            std::byte t1  = t >> 4;
            std::byte res_t1 = std::byte(sbox[static_cast<size_t>(t1)]);
            std::byte t2 = t & std::byte{0x0F};
            std::byte res_t2 = std::byte(sbox[static_cast<size_t>(t2)]);
            tmp = (res_t1 << 4) | res_t2;
            res.push_back(tmp);
        }
        return res;
    }
    

    std::vector<std::byte> Serpent::p_layer(const std::vector<std::byte> & block) {
        std::vector<std::byte> res;
        for (size_t i = 0; i < 128 / 8; ++i) {
            std::byte tmp;
            for(size_t j = 0; j < 8; ++j) {
                tmp <<= 1;
                tmp |= bytes_utility::adding_mod2(block, lt[i * 8 + j]);
            }
            res.push_back(tmp);
            tmp = std::byte{0};
        }
        return res;
    }

    std::vector<std::byte> Serpent::inverse_p_layer(const std::vector<std::byte> & block) {
        std::vector<std::byte> res;
        for (size_t i = 0; i < 128 / 8; ++i) {
            std::byte tmp;
            for(size_t j = 0; j < 8; ++j) {
                tmp <<= 1;
                tmp |= bytes_utility::adding_mod2(block, inverse_lt[i * 8 + j]);
            }
            res.push_back(tmp);
            tmp = std::byte{0};
        }
        return res;
    }
}