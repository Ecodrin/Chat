#pragma once
#include <iostream>
#include <bitset>
#include <cstddef>
#include <vector>
#include <random>
#include <algorithm>

namespace bytes_utility {
    enum class PermutationsOrderRule {
        ForwardOrderZeroIndex,
        ForwardOrderFirstIndex,
        ReverseOrderZeroIndex,
        ReverseOrderFirstIndex,
    };
    std::vector<std::byte> permutations(const std::vector<std::byte> & input, const std::vector<size_t> & permutations_rule, PermutationsOrderRule permutations_order_rule);
    void print_byte(std::ostream & stream, const std::byte & byte); 
    void print_bytes_vector(std::ostream & stream, const std::vector<std::byte> & a, const std::string & sep=" ", const std::string & end="\n");
    std::vector<std::byte> random_bytes_vector(size_t size_vector);
    std::vector<std::byte> adding_bytes_vectors(const std::vector<std::byte> & v1, const std::vector<std::byte> & v2);
    std::vector<std::byte> xor_vector(const std::vector<std::byte> & a, const std::vector<std::byte> & b);
    std::byte get_bit(const std::vector<std::byte> & input, size_t index, PermutationsOrderRule permutations_order_rule);

    size_t counting_number_units(const std::byte & byte);

    std::vector<std::byte> cycling_rotate_left(const std::vector<std::byte> & a, size_t number, size_t a_size_bits);
    std::vector<std::byte> cycling_rotate_right(const std::vector<std::byte> & a, size_t number, size_t a_size_bits);
    std::vector<std::byte> rotate_left(const std::vector<std::byte> & a, size_t number, size_t a_size_bits);
    std::vector<std::byte> rotate_right(const std::vector<std::byte> & a, size_t number, size_t a_size_bits);

    std::vector<std::byte> connect_arrays(const std::vector<std::byte> & a, size_t size_a_bits, const std::vector<std::byte> & b, size_t size_b_bits);

    void trim_trailing_zeros(std::vector<std::byte>& vec);

    std::byte adding_mod2(const std::vector<std::byte> & data, std::vector<size_t> indices);
}