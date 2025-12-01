#include "../include/triple_des.hpp"


namespace symmetric_algorithms {
    size_t TripleDES::get_block_size() {
        return block_size;
    }

    std::vector<std::byte> TripleDES::encryption(const std::vector<std::byte> & encrypted_block) {
        if(triple_des_type == TripleDESType::EEE) {
            return alg3.encryption(alg2.encryption(alg1.encryption(encrypted_block)));
        } else if (triple_des_type == TripleDESType::EDE) {
            return alg3.encryption(alg2.decryption(alg1.encryption(encrypted_block)));
        }else {
            throw std::invalid_argument("incorrect triple_des_type");
        }
        return {};
    }

    std::vector<std::byte> TripleDES::decryption(const std::vector<std::byte> & encrypted_block) {
        if(triple_des_type == TripleDESType::EEE) {
            return alg1.decryption(alg2.decryption(alg3.decryption(encrypted_block)));
        } else if (triple_des_type == TripleDESType::EDE) {
            return alg1.decryption(alg2.encryption(alg3.decryption(encrypted_block)));
        } else {
            throw std::invalid_argument("incorrect triple_des_type");
        }
        return {};
    }

    TripleDES::TripleDES(const std::vector<std::byte> & key, TripleDESType triple_des_type) : triple_des_type{triple_des_type} {
        set_key(key);
    }

    size_t TripleDES::get_standart_block_size() {
        return block_size;
    }
    
    void TripleDES::set_key(const std::vector<std::byte> & key) {
        if (key.size() != (block_size - 1) * 3 && key.size() != block_size * 3 ) {
            throw std::invalid_argument("incorrect size key");
        }
        size_t n = block_size - 1;
        if (key.size() == block_size * 3) {
            n = block_size;
        }
        std::vector<std::byte> key1(n), key2(n), key3(n);
        std::copy(key.begin(), key.begin() + n, key1.begin());
        std::copy(key.begin() + n, key.begin() + 2 * n, key2.begin());
        std::copy(key.begin() + 2 * n, key.end(), key3.begin());
        alg1.set_key(key1);
        alg2.set_key(key2);
        alg3.set_key(key3);
    }
}