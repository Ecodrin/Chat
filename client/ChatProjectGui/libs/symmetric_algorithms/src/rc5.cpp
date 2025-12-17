#include "../include/rc5.hpp"

namespace symmetric_algorithms {

    RC5GenerationKeys::RC5GenerationKeys(size_t w_len, size_t b_len): w_len{w_len}, b_len{b_len} {

    }

    boost::multiprecision::cpp_int RC5GenerationKeys::cycling_rotate_left(const boost::multiprecision::cpp_int & number, size_t shift, size_t width) {
        return ((number << shift) | (number >> (width - shift))) & ((boost::multiprecision::cpp_int{1} << width) - 1);
    }

    boost::multiprecision::cpp_int RC5GenerationKeys::cycling_rotate_right(const boost::multiprecision::cpp_int & number, size_t shift, size_t width) {
        return ((number >> shift) | (number << (width - shift))) & ((boost::multiprecision::cpp_int{1} << width) - 1);
    }

    
    std::vector<std::vector<std::byte>> RC5GenerationKeys::generation_round_keys(const std::vector<std::byte> & original_key, size_t number_round_keys) {
        boost::multiprecision::cpp_dec_float_50 Q_float((golden_ratio() - 1) * boost::multiprecision::cpp_dec_float_50(boost::multiprecision::cpp_int{2} << w_len) );
        boost::multiprecision::cpp_dec_float_50 P_float((exp(1) - 2) * boost::multiprecision::cpp_dec_float_50(boost::multiprecision::cpp_int{2} << w_len));\
        
        boost::multiprecision::cpp_int Q = round_odd(Q_float);
        boost::multiprecision::cpp_int P = round_odd(P_float);
        size_t u = w_len / 8;
        size_t c = b_len / u;
        if(b_len % u != 0) {
            c += 1;
        }
        std::vector<boost::multiprecision::cpp_int> L(c);
        std::vector<boost::multiprecision::cpp_int> key_b;
        std::vector<boost::multiprecision::cpp_int> S(2 * (number_round_keys + 1));
        for(size_t i = 0; i < original_key.size(); ++i) {
            key_b.push_back(boost::multiprecision::cpp_int{} | original_key[i]);
        }
        
        for(int i = (int)b_len - 1;i >= 0; --i) {
            L[i/u] = cycling_rotate_left(L[i / u], 8, 8) + key_b[i];
        }
        

        S[0] = P;
        for(size_t i = 1; i < S.size(); ++i) {
            S[i] = S[i-1] + Q;
        }
        
        boost::multiprecision::cpp_int A = 0;
        boost::multiprecision::cpp_int B = 0;
        size_t i = 0;
        size_t j = 0;
        size_t k = 0;
        size_t t = 2 * (number_round_keys + 1); 
        for(; k < 3 * std::max(t, c); k++, i = (i + 1) % t, j = (j + 1) % c) {
            A = S[i] = cycling_rotate_left((A + B + S[i]), 3, 8);
            B = L[j] = cycling_rotate_left((A + B + L[j]), (A+B).convert_to<size_t>(), 8);
            
        }
        std::vector<std::vector<std::byte>> res(S.size());
        for (size_t i = 0; i < res.size(); ++i) {
            res[i] = convert_to_bytes_vector(S[i]);
        }
        return res;
    }

    size_t RC5::get_block_size() {
        return real_block_size;
    }

    boost::multiprecision::cpp_dec_float_50 RC5GenerationKeys::golden_ratio() const {
        return (boost::multiprecision::sqrt(boost::multiprecision::cpp_dec_float_50(5)) + 1) / 2;
    }

    boost::multiprecision::cpp_int RC5GenerationKeys::round_odd(boost::multiprecision::cpp_dec_float_50 number) {
        auto s = boost::multiprecision::round(number);
        auto n = static_cast<boost::multiprecision::cpp_int>(s);
        if(n & 1 == 0) {
            if (number > s) {
                n += 1;
            } else {
                n -= 1;
            }
        }
        return n;
    }

    boost::multiprecision::cpp_int RC5GenerationKeys::convert_to_cpp_int(const std::vector<std::byte> & block) {
        boost::multiprecision::cpp_int res;
        for(size_t i = 0; i < block.size(); ++i) {
            res |= block[i];
            if(i != block.size() - 1) {
                res <<= 8;
            }
        }
        return res;
    }
    
    std::vector<std::byte> RC5GenerationKeys::convert_to_bytes_vector(const boost::multiprecision::cpp_int & block) {
        std::vector<std::byte> res;
        boost::multiprecision::cpp_int copy_block(block);
        while(copy_block > 0) {
            res.push_back(std::byte{(copy_block & 0xFF).convert_to<std::byte>()});
            copy_block >>= 8;
        }
        std::reverse(res.begin(), res.end());
        return res;
    }

    std::vector<std::byte> RC5::encryption(const std::vector<std::byte> & encrypted_block) {
        if (encrypted_block.size() != real_block_size) {
            throw std::invalid_argument("incorrect block size");
        }
        std::vector<std::byte> left_part(
            encrypted_block.begin(),
            encrypted_block.begin() + encrypted_block.size() / 2
        );
        std::vector<std::byte> right_part(
            encrypted_block.begin() +  encrypted_block.size() / 2,
            encrypted_block.end()
        );
        boost::multiprecision::cpp_int A = RC5GenerationKeys::convert_to_cpp_int(left_part);
        boost::multiprecision::cpp_int B = RC5GenerationKeys::convert_to_cpp_int(right_part);
        
        A = A + S_b[0];
        B = B + S_b[1];
        for(size_t i = 1; i <= number_rounds; ++i) {
            A = (RC5GenerationKeys::cycling_rotate_left(A ^ B, (B % (real_block_size * 8 / 2)).convert_to<size_t>(), real_block_size / 2 * 8)) + S_b[2 * i];
            B = (RC5GenerationKeys::cycling_rotate_left(B ^ A, (A % (real_block_size * 8 / 2)).convert_to<size_t>(), real_block_size / 2 * 8)) + S_b[2 * i + 1];
        }

        std::vector<std::byte> left = RC5GenerationKeys::convert_to_bytes_vector(A);
        std::vector<std::byte> right = RC5GenerationKeys::convert_to_bytes_vector(B);
        // std::cout << left.size() << " " << right.size() << std::endl;
        // bytes_utility::print_bytes_vector(std::cout, left);
        // bytes_utility::print_bytes_vector(std::cout, right);
        while (left.size() < real_block_size / 2) {
            left.insert(left.begin(), std::byte{0});
        }
        while (right.size() < real_block_size / 2) {
            right.insert(right.begin(), std::byte{0});
        }
        
        return bytes_utility::connect_arrays(left, left.size() * 8, right, right.size() * 8);
    }

    std::vector<std::byte> RC5::decryption(const std::vector<std::byte> & encrypted_block) {
        if (encrypted_block.size() != real_block_size) {
            throw std::invalid_argument("incorrect block size");
        }
        std::vector<std::byte> left_part(
            encrypted_block.begin(),
            encrypted_block.begin() + encrypted_block.size() / 2
        );
        std::vector<std::byte> right_part(
            encrypted_block.begin() +  encrypted_block.size() / 2,
            encrypted_block.end()
        );
        boost::multiprecision::cpp_int A = RC5GenerationKeys::convert_to_cpp_int(left_part);
        boost::multiprecision::cpp_int B = RC5GenerationKeys::convert_to_cpp_int(right_part);

        for(int i = number_rounds; i >= 1; --i) {
            B = (RC5GenerationKeys::cycling_rotate_right(B - S_b[2 *i + 1], (A % (real_block_size * 8 / 2)).convert_to<size_t>(), real_block_size / 2 * 8)) ^ A;
            A = (RC5GenerationKeys::cycling_rotate_right(A - S_b[2 * i], (B % (real_block_size * 8 / 2)).convert_to<size_t>(), real_block_size / 2 * 8)) ^ B;
        }
        B = B - S_b[1];
        A = A - S_b[0];
        std::vector<std::byte> left = RC5GenerationKeys::convert_to_bytes_vector(A);
        std::vector<std::byte> right = RC5GenerationKeys::convert_to_bytes_vector(B);
        while (left.size() < real_block_size / 2) {
            left.insert(left.begin(), std::byte{0});
        }
        while (right.size() < real_block_size / 2) {
            right.insert(right.begin(), std::byte{0});
        }
        return bytes_utility::connect_arrays(left, left.size() * 8, right, right.size() * 8);
    }

    RC5::RC5(const std::vector<std::byte> & key, size_t block_size, size_t number_rounds): 
            keys_expander{block_size * 8 / 2, key.size()}, key{key}, real_block_size{block_size},
            number_rounds{number_rounds} {
        set_key(key);
    }

    void RC5::set_key(const std::vector<std::byte> & key) {
        this->key = key;
        S = keys_expander.generation_round_keys(key, number_rounds);
        S_b.clear();
        for(size_t i = 0; i < S.size(); ++i) {
            S_b.push_back(RC5GenerationKeys::convert_to_cpp_int(S[i]));
        }
    }
}