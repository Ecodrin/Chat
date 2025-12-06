#include "../include/bytes_utility.hpp"
namespace bytes_utility {
    
    std::vector<std::byte> permutations(const std::vector<std::byte> & input, const std::vector<size_t> & permutations_rule, PermutationsOrderRule permutations_order_rule) {
        if(input.size() == 0) {
            return {};
        }

        std::vector<std::byte> output;
        std::byte tmp;
        for(size_t i = 0; i < permutations_rule.size(); ++i) {
            if (i % 8 == 0 and i > 0) {
                output.push_back(tmp);
                tmp = std::byte{0};
            }
            tmp <<= 1;
            tmp |= get_bit(input, permutations_rule[i], permutations_order_rule);
        }
        if (permutations_rule.size() % 8 != 0) {
            tmp <<= (8 - permutations_rule.size() % 8);
        }
        output.push_back(tmp);
        return output;
    } 

    std::byte get_bit(const std::vector<std::byte> & input, size_t index, PermutationsOrderRule permutations_order_rule) {
        char first_index = (permutations_order_rule == PermutationsOrderRule::ForwardOrderFirstIndex) || (permutations_order_rule == PermutationsOrderRule::ReverseOrderFirstIndex);
        char bit_indexing_rule = (permutations_order_rule == PermutationsOrderRule::ReverseOrderZeroIndex) || (permutations_order_rule == PermutationsOrderRule::ReverseOrderFirstIndex);
        if (index - first_index >= input.size() * 8 || (index == 0 && first_index)) {
            throw std::runtime_error("incorrect index");
        }
        size_t desired_index;
        if (bit_indexing_rule == 0) {
            desired_index = index - first_index;
        } else {
            desired_index = input.size() * 8 - index - 1 + first_index;
        }
        return (input[desired_index / 8] >> (7 - desired_index % 8)) & std::byte{1};
    }

    void print_byte(std::ostream & stream, const std::byte & byte) {
        stream << std::bitset<8>(std::to_integer<int>(byte));
    }

    void print_bytes_vector(std::ostream & stream, const std::vector<std::byte> & a, const std::string & sep, const std::string & end) {
        for (const auto & el: a) {
            print_byte(stream, el);
            stream << sep;
        }
        stream << end;
    }

    std::vector<std::byte> random_bytes_vector(size_t size_vector) {
        std::vector<std::byte> res;
        std::random_device device;
        std::mt19937 gen(device());
        std::uniform_int_distribution<unsigned char> dist(0, 255);
        for(size_t i = 0; i < size_vector; ++i) {
            res.push_back(std::byte{dist(gen)});
        }
        return res;
    }

    
    std::vector<std::byte> adding_bytes_vectors(const std::vector<std::byte> & a, const std::vector<std::byte> & b) {
        std::vector<std::byte> res;
        u_int16_t carry = 0;        
        size_t i = a.size();
        size_t j = b.size();
        while (i > 0 || j > 0 || carry) {
            uint16_t sum = carry;
            if(i > 0) {
                i--;
                sum += static_cast<uint8_t>(a[i]);
            }
            if(j > 0) {
                j--;
                sum += static_cast<uint8_t>(b[j]);
            }

            res.push_back(std::byte{static_cast<uint8_t>(sum)});
            carry = sum >> 8;
        }
        
        std::reverse(res.begin(), res.end());
        return res;
    }

    std::vector<std::byte> xor_vector(const std::vector<std::byte> & a, const std::vector<std::byte> & b) {
        std::vector<std::byte> res;
        for (size_t i = 0; i < std::max(a.size(), b.size()); ++i) {
            if (i < b.size() && i < a.size()) {
                res.push_back(a[i] ^ b[i]);
            } else if (i < a.size()) {
                res.push_back(a[i] ^ std::byte{0});
            } else {
                res.push_back(b[i] ^ std::byte{0});
            }
        }
        return res;
    }

    size_t counting_number_units(const std::byte & byte) {
        size_t number = 0;
        for(size_t i = 0; i < 8; ++i) {
            if(((byte >> (7 - i)) & std::byte{1}) == std::byte{1}) {
                number += 1;
            }
        }
        return number;
    }

    std::vector<std::byte> cycling_rotate_left(const std::vector<std::byte> & a, size_t number, size_t a_size_bits) {
        number = number % a_size_bits;
        std::vector<size_t> p;
        for(size_t i = 0; i < a_size_bits; ++i) {
            p.push_back((i + number) % a_size_bits);
        }

        return permutations(a, p, PermutationsOrderRule::ForwardOrderZeroIndex);
    }

    std::vector<std::byte> cycling_rotate_right(const std::vector<std::byte> & a, size_t number, size_t a_size_bits) {
        number = number % a_size_bits;
        std::vector<size_t> p;
        for(int i = 0; i < a_size_bits; ++i) {
            p.push_back(((i - (int)number) + a_size_bits) % a_size_bits);
        }

        return permutations(a, p, PermutationsOrderRule::ForwardOrderZeroIndex);
    }

    
    std::vector<std::byte> rotate_left(const std::vector<std::byte> & a, size_t number, size_t a_size_bits) {
        number = number % a_size_bits;
        std::vector<size_t> p;
        for(size_t i = 0; i < a_size_bits; ++i) {
            if (i + number >= a_size_bits) {
                break;
            } 
            p.push_back((i + number) % a_size_bits);
        }
        auto res = permutations(a, p, PermutationsOrderRule::ForwardOrderZeroIndex);
        res.resize(a.size());
        return res;
    }

    
    std::vector<std::byte> rotate_right(const std::vector<std::byte> & a, size_t number, size_t a_size_bits) {
        number = number % a_size_bits;
        std::vector<size_t> p;
        std::vector<std::byte> copy_a(a);
        copy_a.push_back(std::byte{0});
        for(int i = 0; i < a_size_bits; ++i) {
            if (i - (int)number < 0) {
                p.push_back(copy_a.size() * 8 - 2);
            } else {
                p.push_back(((i - number) + a_size_bits) % a_size_bits);
            }
        }

        auto res = permutations(copy_a, p, PermutationsOrderRule::ForwardOrderZeroIndex);
        res.resize(a.size());
        return res;
    }

    std::vector<std::byte> connect_arrays(const std::vector<std::byte> & a, size_t size_a_bits, const std::vector<std::byte> & b, size_t size_b_bits) {
        if(size_a_bits > a.size() * 8 || size_b_bits > b.size() * 8) {
            throw std::runtime_error("incorrect number bits");
        }
        std::vector<std::byte> concat{a};
        concat.insert(concat.end(), b.begin(), b.end());
        std::vector<size_t> p;
        for (size_t i = 0; i < size_a_bits; ++i) {
            p.push_back(i);
        }
        for (size_t i = 0; i < size_b_bits; ++i) {
            p.push_back(i + a.size() * 8);
        }
        return permutations(concat, p, PermutationsOrderRule::ForwardOrderZeroIndex);
    }

    void trim_trailing_zeros(std::vector<std::byte>& vec) {
        while (!vec.empty() && vec.back() == std::byte{0}) {
            vec.pop_back();
        }
    }

    std::byte adding_mod2(const std::vector<std::byte> & data, std::vector<size_t> indices) {
        std::byte t{0};
        for(const size_t & index : indices) {
            t ^= get_bit(data, index, PermutationsOrderRule::ForwardOrderZeroIndex);
        }
        return t;
    }

    std::vector<std::byte> get_bytes_from_string_numbers(const std::string & s) {
        std::vector<std::byte> res;
        boost::multiprecision::cpp_int block(s);
        while(block > 0) {
            res.push_back(std::byte{(block & 0xFF).convert_to<std::byte>()});
            block >>= 8;
        }
        std::reverse(res.begin(), res.end());
        return res;
    }

    std::vector<std::byte> get_bytes_from_string(const std::string & s) {
        std::vector<std::byte> res;
        for(const auto & b : s) {
            res.emplace_back(std::byte{ b});
        }
        return res;
    }

    std::string get_string_from_bytes(const std::vector<std::byte> & bytes) {
        std::string str;
        str.reserve(bytes.size());
        for (auto b : bytes) {
            str.push_back(static_cast<char>(b));
        }
        return str;
    }
}
