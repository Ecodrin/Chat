#include "../include/Diffie_Hellman_key_exchange.hpp"


namespace Diffie_Hellman_key_exchange {
    std::tuple<boost::multiprecision::cpp_int, boost::multiprecision::cpp_int, boost::multiprecision::cpp_int> get_random_keys(size_t number_p_bits) {
        // a g p
        boost::multiprecision::cpp_int g = 2;

        boost::random::random_device rand_device;
        boost::random::uniform_int_distribution<boost::multiprecision::cpp_int> dist(boost::multiprecision::cpp_int{1} << (number_p_bits - 2), boost::multiprecision::cpp_int{1} << (number_p_bits - 1));
        asymmetric_algorithms::MillerRabinPrimalityTest primality_test;
        boost::multiprecision::cpp_int q = dist(rand_device);
        boost::multiprecision::cpp_int p;
        while (!primality_test.check_primality(q, 0.99)) {
            p = 2 * q + 1;
            if(!primality_test.check_primality(p, 0.99)) {
                break;
            }
        }
        boost::random::uniform_int_distribution<boost::multiprecision::cpp_int> dist_a(2, q);
        boost::multiprecision::cpp_int a = asymmetric_algorithms::SymbolService::mod_pow(g, dist_a(rand_device), p);
        return {a, g, p};
    }

    boost::multiprecision::cpp_int generate_b(std::pair<boost::multiprecision::cpp_int, boost::multiprecision::cpp_int> keys) {
        auto [g, p] = keys;
        boost::random::random_device rand_device;
        boost::random::uniform_int_distribution<boost::multiprecision::cpp_int> dist_b(2, (p - 1) / 2);
        boost::multiprecision::cpp_int b = asymmetric_algorithms::SymbolService::mod_pow(g, dist_b(rand_device), p);
        return b;
    }

    std::string generate_string_b(std::pair<std::string, std::string> keys) {
        return boost::multiprecision::to_string(generate_b({boost::multiprecision::cpp_int{keys.first}, boost::multiprecision::cpp_int{keys.second}}));
    }

    std::tuple<std::string, std::string, std::string> get_random_string_keys(size_t number_p_bits) {
        auto [a, g, p] = get_random_keys(number_p_bits);
        return {boost::multiprecision::to_string(a), boost::multiprecision::to_string(g), boost::multiprecision::to_string(p)};
    }

    boost::multiprecision::cpp_int get_A_or_B(std::tuple<boost::multiprecision::cpp_int, boost::multiprecision::cpp_int, boost::multiprecision::cpp_int> keys) {
        auto [a_or_b, g, p] = keys;
        return asymmetric_algorithms::SymbolService::mod_pow(g, a_or_b, p);
    }

    std::string get_A_or_B_string(std::tuple<std::string, std::string, std::string> keys) {
        auto [a_or_b, g, p] = keys;
        return boost::multiprecision::to_string(asymmetric_algorithms::SymbolService::mod_pow(boost::multiprecision::cpp_int{g}, boost::multiprecision::cpp_int{a_or_b}, boost::multiprecision::cpp_int{p}));
    }

    boost::multiprecision::cpp_int get_key(std::tuple<boost::multiprecision::cpp_int, boost::multiprecision::cpp_int, boost::multiprecision::cpp_int, boost::multiprecision::cpp_int> keys) {
        auto [A_or_B, b_or_a, g, p] = keys;
        return asymmetric_algorithms::SymbolService::mod_pow(A_or_B, b_or_a, p);
    }

    std::string get_key_string(std::tuple<std::string, std::string, std::string, std::string> keys) {
        auto [A_or_B, b_or_a, g, p] = keys;
        return boost::multiprecision::to_string(get_key({
            boost::multiprecision::cpp_int{A_or_B},
            boost::multiprecision::cpp_int{b_or_a},
            boost::multiprecision::cpp_int{g},
            boost::multiprecision::cpp_int{p}
        }));
    }
}