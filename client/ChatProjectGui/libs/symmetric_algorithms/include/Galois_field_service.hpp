#pragma once

#include <iostream>
#include <cstddef>
#include <string>

#include "bytes_utility.hpp"

namespace symmetric_algorithms {
    class GaloisFieldService {
    private:

    public:
        static std::byte add(const std::byte &a, const std::byte & b);
        static std::byte multiply(const std::byte &a, const std::byte & b, const std::byte & mod);
        static std::byte multiply(const std::vector<std::byte> &a, const std::byte & b, const std::byte & mod);
        static std::pair<std::vector<std::byte>, std::byte> divide(const std::byte &a, const std::byte & b);
        static std::pair<std::vector<std::byte>, std::byte> divide(const std::vector<std::byte> &a, const std::byte & b);
        static std::pair<std::vector<std::byte>, std::byte> divide(const std::vector<std::byte> &a, const std::vector<std::byte> & b);
        static std::vector<std::byte> get_polynomials(size_t pow);
        static std::vector<std::byte> get_irreducible_polynomials();
        static bool check_irreducible(const std::byte & b);
        static std::byte find_inverse(const std::byte & b, const std::byte & mod);


        static std::byte mod_exp(const std::byte &a, const std::byte&exp, const std::byte & mod);

        static void print(const std::vector<std::byte> & b);
        static void print(const std::byte & b);
    };
}