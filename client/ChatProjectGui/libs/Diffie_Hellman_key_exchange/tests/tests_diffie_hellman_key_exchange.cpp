#include "diffie_hellman_key_exchange.hpp"

#include <iostream>

using namespace diffie_hellman_key_exchange;

int main() {
    auto [a, g, p] = get_random_string_keys(256);
    std::cout << "a: " << a << " g: " << g << " p: " << p << std::endl;
    auto A = get_A_or_B_string({a, g, p});
    std::cout << "A: " << A << std::endl;
    auto b = generate_string_b({g, p});
    std::cout << "b: " << b << std::endl;
    auto B = get_A_or_B_string({b, g, p});
    std::cout << "B: " << B << std::endl;
    auto key1 = get_key_string({A, b, g, p});
    std::cout << "key1 A/b: " << key1 << std::endl;
    auto key2 = get_key_string({B, a, g, p});
    std::cout << "key2 B/a: " << key2 << std::endl;
}