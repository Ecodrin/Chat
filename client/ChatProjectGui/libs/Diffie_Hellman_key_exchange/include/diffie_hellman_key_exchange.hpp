#pragma once

#include <tuple>
#include <iostream>
#include <vector>
#include <cstddef>

#include <boost/multiprecision/cpp_int.hpp>

#include "primality_tests.hpp"
#include "symbol_service.hpp"

namespace Diffie_Hellman_key_exchange {

    // a g p
    std::tuple<boost::multiprecision::cpp_int, boost::multiprecision::cpp_int, boost::multiprecision::cpp_int> get_random_keys(size_t number_p_bits=3072);
    std::tuple<std::string, std::string, std::string> get_random_string_keys(size_t number_p_bits=3072);

    // _ g p -> b g p
    boost::multiprecision::cpp_int generate_b(std::pair<boost::multiprecision::cpp_int, boost::multiprecision::cpp_int> keys);
    std::string generate_string_b(std::pair<std::string, std::string> keys);

    // g ^ x mod p = A/B
    boost::multiprecision::cpp_int get_A_or_B(std::tuple<boost::multiprecision::cpp_int, boost::multiprecision::cpp_int, boost::multiprecision::cpp_int> keys);
    std::string get_A_or_B_string(std::tuple<std::string, std::string, std::string> keys);

    // A/B^b/a mod p = key
    // A/B b/a g p
    boost::multiprecision::cpp_int get_key(std::tuple<boost::multiprecision::cpp_int, boost::multiprecision::cpp_int, boost::multiprecision::cpp_int, boost::multiprecision::cpp_int> keys);
    std::string get_key_string(std::tuple<std::string, std::string, std::string, std::string> keys);

}