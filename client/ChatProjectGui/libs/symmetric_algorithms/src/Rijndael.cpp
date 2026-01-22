#include "../include/Rijndael.hpp"


namespace symmetric_algorithms {
    void Rijndael::generate_sbox() {
        sbox.resize(256);
        for (size_t i = 0; i < 256; ++i) {
            std::byte b = std::byte{static_cast<unsigned char>(i)};
            if (b == std::byte{0}) {
                b = std::byte{0x63};
            } else {
                b = GaloisFieldService::find_inverse(b, irreducible_polynomials[irreducible_polynomial]);
                b = b ^
                    bytes_utility::cycling_rotate_left({b}, 1, 8)[0] ^
                    bytes_utility::cycling_rotate_left({b}, 2, 8)[0] ^
                    bytes_utility::cycling_rotate_left({b}, 3, 8)[0] ^
                    bytes_utility::cycling_rotate_left({b}, 4, 8)[0] ^
                    std::byte{0x63};
            }
            sbox[i] = b;
        }
    }       

    
    void Rijndael::generate_inv_sbox() {
        inv_sbox.reserve(256);
        for(size_t i = 0; i < 256; ++i) {
            std::byte byte = static_cast<std::byte>(i);
            std::byte b = GaloisFieldService::add(
                    bytes_utility::cycling_rotate_left({byte}, 1, 8)[0],
                    bytes_utility::cycling_rotate_left({byte}, 3, 8)[0]);
            b = GaloisFieldService::add(b, bytes_utility::cycling_rotate_left({byte}, 6, 8)[0]);
            b = GaloisFieldService::add(b, std::byte{0x05});
            inv_sbox[i] = GaloisFieldService::find_inverse(b, irreducible_polynomials[irreducible_polynomial]);
        } 
    }

    void RijndaelGenerationKeys::generate_sbox() {
        sbox.resize(256);
        for (size_t i = 0; i < 256; ++i) {
            std::byte b = std::byte{static_cast<unsigned char>(i)};
            if (b == std::byte{0}) {
                b = std::byte{0x63};
            } else {
                b = GaloisFieldService::find_inverse(b, polynomial);
                b = b ^
                    bytes_utility::cycling_rotate_left({b}, 1, 8)[0] ^
                    bytes_utility::cycling_rotate_left({b}, 2, 8)[0] ^
                    bytes_utility::cycling_rotate_left({b}, 3, 8)[0] ^
                    bytes_utility::cycling_rotate_left({b}, 4, 8)[0] ^
                    std::byte{0x63};
            }
            sbox[i] = b;
        }
    }



    Rijndael::Rijndael(const std::vector<std::byte> & key, size_t block_size, size_t irreducible_polynomial): 
        irreducible_polynomial{irreducible_polynomial}, block_size{block_size}, keys_expander{block_size, irreducible_polynomial}{
        generate_sbox();
        generate_inv_sbox();
        number_rounds = calculate_number_rounds(key.size());
        round_keys = keys_expander.generation_round_keys(key, number_rounds + 1);
    }



    void Rijndael::print_sbox() {
        for(size_t i = 0; i < 256; ++i) {
            if(i != 0 && i % 16 == 0) {
                std::cout << std::endl;
            }
            std::cout << std::hex << static_cast<unsigned int>(sbox[i]) << " ";
        }
    }


    void RijndaelGenerationKeys::print_sbox() {
        for(size_t i = 0; i < 256; ++i) {
            if(i != 0 && i % 16 == 0) {
                std::cout << std::endl;
            }
            std::cout << std::hex << static_cast<unsigned int>(sbox[i]) << " ";
        }
    }

    
    void Rijndael::print_inv_sbox() {
        for(size_t i = 0; i < 256; ++i) {
            if(i != 0 && i % 16 == 0) {
                std::cout << std::endl;
            }
            std::cout << std::hex << static_cast<unsigned int>(inv_sbox[i]) << " ";
        }
    }

    size_t Rijndael::calculate_number_rounds(size_t key_size) {
        size_t Nk = key_size / 4;
        size_t Nb = block_size / 4;
        return std::max(Nk, Nb) + 6;
    }

    
    void Rijndael::sub_bytes(std::vector<std::vector<std::byte>> & state) {
        for(size_t i = 0; i < block_size / 4; ++i) {
            for(size_t j = 0;j < 4; ++j) {
                state[j][i] = sbox[static_cast<int>(state[j][i])];
            }
        }
    }

    
    void Rijndael::inv_sub_bytes(std::vector<std::vector<std::byte>> & state) {
        for(size_t i = 0; i < block_size / 4; ++i) {
            for(size_t j = 0;j < 4; ++j) {
                state[j][i] = inv_sbox[static_cast<int>(state[j][i])];
            }
        }
    }

    void RijndaelGenerationKeys::sub_word(std::vector<std::byte> & state) {
        for(size_t i = 0; i < state.size(); ++i) {
            std::byte input = state[i];
            std::byte output = sbox[static_cast<int>(input)];

            state[i] = output;
        }
    }


    void Rijndael::calculate_shift_rows(std::vector<std::vector<std::byte>> & state) {
        state[1] = bytes_utility::cycling_rotate_left(state[1], 1 * 8, state[1].size() * 8);
        if (block_size == 16) {
            state[2] = bytes_utility::cycling_rotate_left(state[2], 2 * 8, state[2].size() * 8);
            state[3] = bytes_utility::cycling_rotate_left(state[3], 3 * 8, state[3].size() * 8);
        } else if (block_size == 24) {
            state[2] = bytes_utility::cycling_rotate_left(state[2], 2 * 8, state[2].size() * 8);
            state[3] = bytes_utility::cycling_rotate_left(state[3], 3 * 8, state[3].size() * 8);
        } else if (block_size == 32) {
            state[2] = bytes_utility::cycling_rotate_left(state[2], 3 * 8, state[2].size() * 8);
            state[3] = bytes_utility::cycling_rotate_left(state[3], 4 * 8, state[3].size() * 8);
        }
    }
    
    void Rijndael::calculate_inv_shift_rows(std::vector<std::vector<std::byte>> & state) {
        state[1] = bytes_utility::cycling_rotate_right(state[1], 1 * 8, state[1].size() * 8);
        if (block_size == 16) {
            state[2] = bytes_utility::cycling_rotate_right(state[2], 2 * 8, state[2].size() * 8);
            state[3] = bytes_utility::cycling_rotate_right(state[3], 3 * 8, state[3].size() * 8);
        } else if (block_size == 24) {
            state[2] = bytes_utility::cycling_rotate_right(state[2], 2 * 8, state[2].size() * 8);
            state[3] = bytes_utility::cycling_rotate_right(state[3], 3 * 8, state[3].size() * 8);
        } else if (block_size == 32) {
            state[2] = bytes_utility::cycling_rotate_right(state[2], 3 * 8, state[2].size() * 8);
            state[3] = bytes_utility::cycling_rotate_right(state[3], 4 * 8, state[3].size() * 8);
        }
    }

    void Rijndael::calculate_add_round_key(std::vector<std::vector<std::byte>> & state, size_t index) {
        size_t k = 0;
        for(size_t i = 0; i < block_size / 4; ++i) {
            for(size_t j = 0;j < 4; ++j) {
                state[j][i] = state[j][i] ^ round_keys[index][k++];
            }
        }
    }

    
    void Rijndael::mix_columns(std::vector<std::vector<std::byte>> & state) {
        for (size_t i = 0; i < block_size / 4; ++i) {

            std::vector<std::byte> column(4);
            for(size_t k = 0; k < 4; ++k) {
                column[k] = state[k][i];
            }
            for(size_t k = 0; k < 4; ++k) {
                std::byte res{0x00};
                for(size_t j = 0; j < 4; ++j) {
                    res = GaloisFieldService::add(
                        GaloisFieldService::multiply(
                            column[j], mix_colums_tab[k][j], 
                            irreducible_polynomials[irreducible_polynomial]), 
                        res);
                }
                state[k][i] = res;
            }
        }
    }

    void Rijndael::inv_mix_columns(std::vector<std::vector<std::byte>> & state) {
        for (size_t i = 0; i < block_size / 4; ++i) {

            std::vector<std::byte> column(4);
            for(size_t k = 0; k < 4; ++k) {
                column[k] = state[k][i];
            }
            for(size_t k = 0; k < 4; ++k) {
                std::byte res{0x00};
                for(size_t j = 0; j < 4; ++j) {
                    res = GaloisFieldService::add(
                        GaloisFieldService::multiply(
                            column[j], inv_mix_colums_tab[k][j], 
                            irreducible_polynomials[irreducible_polynomial]), 
                        res);
                }
                state[k][i] = res;
            }
        }
    }

    std::vector<std::vector<std::byte>> Rijndael::convert_to_state(const std::vector<std::byte> & vec) {
        if(vec.size() != block_size) {
            throw std::invalid_argument("incorrect block size");
        }
        size_t k = 0;
        std::vector<std::vector<std::byte>> state(4);
        for(size_t i = 0; i < block_size/ 4; ++i) {
            for(size_t j = 0; j < 4; ++j) {
                state[j].push_back(vec[k++]);
            }
        }
        return state;
    }

    
    std::vector<std::byte> Rijndael::convert_to_vec(const  std::vector<std::vector<std::byte>> & state) {
        std::vector<std::byte> res;
        for(size_t i = 0; i < block_size / 4; ++i) {
            for(size_t j = 0; j < 4; ++j) {
                res.push_back(state[j][i]);
            }
        }
        return res;
    }

    std::vector<std::byte> Rijndael::encryption(const std::vector<std::byte> & encrypted_block) {
        auto state = convert_to_state(encrypted_block);
        calculate_add_round_key(state, 0);       
        
        for(size_t i = 1;i < number_rounds; ++i) {
            sub_bytes(state);
            calculate_shift_rows(state);

            mix_columns(state);

            calculate_add_round_key(state,i);

        }
        sub_bytes(state);
        calculate_shift_rows(state);
        calculate_add_round_key(state,number_rounds);
        return convert_to_vec(state);
    }

    std::vector<std::byte> Rijndael::decryption(const std::vector<std::byte> & encrypted_block) {
        auto state = convert_to_state(encrypted_block);

        calculate_add_round_key(state, number_rounds);

        calculate_inv_shift_rows(state);

        inv_sub_bytes(state);

        for(int i = static_cast<int>(number_rounds - 1); i > 0; --i) {

            calculate_add_round_key(state, i);

            inv_mix_columns(state);

            calculate_inv_shift_rows(state);

            inv_sub_bytes(state);
        }
        calculate_add_round_key(state, 0);
        return convert_to_vec(state);
    }

    size_t Rijndael::get_block_size() {
        return block_size;
    }

    void Rijndael::set_key(const std::vector<std::byte> & key) {
        round_keys = keys_expander.generation_round_keys(key, number_rounds + 1);
    }

    RijndaelGenerationKeys::RijndaelGenerationKeys(size_t block_size, size_t i_polynomial): block_size{block_size} {
        auto polynomials = GaloisFieldService::get_irreducible_polynomials();
        polynomial = polynomials[i_polynomial];
        rcon.reserve(16);
        rcon.push_back({std::byte{0x01}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}});
        for(size_t i = 1; i < block_size; ++i) {
            rcon.push_back({std::byte{GaloisFieldService::multiply(rcon[i-1][0], std::byte{0x02}, polynomial)}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}});
        }
        generate_sbox();
    }

    std::vector<std::vector<std::byte>> RijndaelGenerationKeys::generation_round_keys(const std::vector<std::byte> & original_key, size_t number_round_keys) {
        size_t Nk = original_key.size() / 4;
        size_t Nb = block_size / 4;
        size_t Nr = number_round_keys - 1;
        size_t total_worlds = Nb * (Nr + 1);
        std::vector<std::vector<std::byte>> keys(total_worlds, std::vector<std::byte>(4));

        size_t i;
        for(i = 0; i < Nk; ++i) {
            for(size_t j = 0; j < 4; ++j) {
                keys[i][j] = original_key[4 * i + j];
            }
        }

        for(; i < total_worlds; ++i) {
            auto temp = keys[i - 1];
            if(i % Nk == 0) {
                temp = bytes_utility::cycling_rotate_left(temp, 8, temp.size() * 8);
                sub_word(temp);
                temp = bytes_utility::xor_vector(temp, rcon[i / Nk - 1]);
            } else if (Nk == 8 && i % Nk == 4) {
                sub_word(temp);
            }
            keys[i] = bytes_utility::xor_vector(keys[i-Nk], temp);
        }

        std::vector<std::vector<std::byte>> round_keys(number_round_keys);
        for(size_t i = 0; i < number_round_keys; ++i) {
            std::vector<std::byte> round_key(Nb * 4);
            for (size_t w = 0; w < Nb; ++w) {
                for (size_t byte = 0; byte < 4; ++byte) {
                    round_key[w * 4 + byte] = keys[i * Nb + w][byte];
                }
            }
            round_keys[i] = round_key;
        }
        return round_keys;
    }

    void Rijndael::print_state(const std::vector<std::vector<std::byte>> & state) const {
        for(size_t j = 0; j < 4; ++j) {
            for(size_t i = 0 ; i < block_size / 4; ++i) {
                std::cout << std::hex << static_cast<int>(state[j][i]) << " ";
            }
            std::cout << std::endl;
        }
        std::cout << "=======================" << std::endl;
    }
}