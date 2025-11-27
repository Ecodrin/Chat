#include <iostream>

#include <boost/multiprecision/cpp_int.hpp>

#include "symbol_service.hpp"

namespace mp = boost::multiprecision;

void test_mod_pow() {
    std::cout << "Testing mod_pow...\n";
    assert(asymmetric_algorithms::SymbolService::mod_pow(2, 3, 5) == 3); // 2^3 mod 5 = 8 mod 5 = 3
    assert(asymmetric_algorithms::SymbolService::mod_pow(5, 0, 7) == 1); // a^0 mod m = 1
    assert(asymmetric_algorithms::SymbolService::mod_pow(17, 3, 5) == 3); // (17 mod 5)^3 mod 5 = 2^3 mod 5 = 3
    assert(asymmetric_algorithms::SymbolService::mod_pow(100, 1, 1) == 0); // a^1 mod 1 = 0
    auto large_result = asymmetric_algorithms::SymbolService::mod_pow(mp::cpp_int("123"), mp::cpp_int("456"), mp::cpp_int("1009"));
    assert(large_result >= 0);
    assert(large_result < mp::cpp_int("1009"));
    std::cout << "mod_pow tests passed.\n\n";
}

void test_is_quadratic_residue() {
    std::cout << "Testing is_quadratic_residue...\n";
    assert(asymmetric_algorithms::SymbolService::is_quadratic_residue(4, 7)); // 2^2 = 4 mod 7
    assert(!asymmetric_algorithms::SymbolService::is_quadratic_residue(3, 7)); // 3 не кв. вычет mod 7
    assert(asymmetric_algorithms::SymbolService::is_quadratic_residue(0, 11)); // 0 всегда вычет
    assert(asymmetric_algorithms::SymbolService::is_quadratic_residue(1, 13)); // 1 всегда вычет
    assert(!asymmetric_algorithms::SymbolService::is_quadratic_residue(10, 11)); // (p-1) mod p, p=11, (11-1)/2=5 (нечет), (-1)^5=-1
    std::cout << "is_quadratic_residue tests passed.\n\n";
}

void test_gcd() {
    std::cout << "Testing gcd...\n";
    assert(asymmetric_algorithms::SymbolService::gcd(48, 18) == 6);
    assert(asymmetric_algorithms::SymbolService::gcd(17, 13) == 1); // взаимно простые
    assert(asymmetric_algorithms::SymbolService::gcd(0, 15) == 15);
    assert(asymmetric_algorithms::SymbolService::gcd(20, 0) == 20);
    assert(asymmetric_algorithms::SymbolService::gcd(0, 0) == 0);
    assert(asymmetric_algorithms::SymbolService::gcd(1, 1000000) == 1);
    assert(asymmetric_algorithms::SymbolService::gcd(97, 97) == 97);
    std::cout << "gcd tests passed.\n\n";
}

void test_extended_gcd() {
    std::cout << "Testing extended_gcd...\n";
    auto [g, x, y] = asymmetric_algorithms::SymbolService::extended_gcd(30, 18);
    assert(g == 6); // gcd(30, 18)
    assert(30 * x + 18 * y == g); // проверка уравнения

    std::tie(g, x, y) = asymmetric_algorithms::SymbolService::extended_gcd(7, 5);
    assert(g == 1); // gcd(7, 5)
    assert(7 * x + 5 * y == g);

    std::tie(g, x, y) = asymmetric_algorithms::SymbolService::extended_gcd(0, 15);
    assert(g == 15);
    assert(0 * x + 15 * y == g);

    std::tie(g, x, y) = asymmetric_algorithms::SymbolService::extended_gcd(20, 0);
    assert(g == 20);
    assert(20 * x + 0 * y == g);

    std::tie(g, x, y) = asymmetric_algorithms::SymbolService::extended_gcd(13, 13);
    assert(g == 13);
    assert(13 * x + 13 * y == g);
    std::cout << "extended_gcd tests passed.\n\n";
}

void test_symbol_service() {
    std::cout << "Testing SymbolService...\n";

    // Legendre symbol
    assert(asymmetric_algorithms::SymbolService::calculate_legendre_symbol(4, 7) == 1); // 4 - вычет
    assert(asymmetric_algorithms::SymbolService::calculate_legendre_symbol(3, 7) == -1); // 3 - не вычет
    assert(asymmetric_algorithms::SymbolService::calculate_legendre_symbol(0, 11) == 0); // (0|p) = 0
    assert(asymmetric_algorithms::SymbolService::calculate_legendre_symbol(1, 13) == 1); // (1|p) = 1
    assert(asymmetric_algorithms::SymbolService::calculate_legendre_symbol(10, 11) == -1); // (-1|11) = -1

    // Jacobi symbol
    assert(asymmetric_algorithms::SymbolService::calculate_jakobi_symbol(2, 15) == 1); // (2|3)*(2|5) = (-1)*(-1) = 1
    assert(asymmetric_algorithms::SymbolService::calculate_jakobi_symbol(6, 15) == 0); // gcd > 1
    assert(asymmetric_algorithms::SymbolService::calculate_jakobi_symbol(-1, 7) == -1); // (-1)^((7-1)/2) = (-1)^3 = -1
    assert(asymmetric_algorithms::SymbolService::calculate_jakobi_symbol(-9, 15) == 0); // gcd > 1

    // Jacobi должен совпадать с Legendre для простого модуля
    assert(asymmetric_algorithms::SymbolService::calculate_jakobi_symbol(5, 7) == asymmetric_algorithms::SymbolService::calculate_legendre_symbol(5, 7));
    assert(asymmetric_algorithms::SymbolService::calculate_jakobi_symbol(2, 11) == asymmetric_algorithms::SymbolService::calculate_legendre_symbol(2, 11));

    std::cout << "SymbolService tests passed.\n\n";
}

void test_contiened_fractions() {
    auto find = asymmetric_algorithms::SymbolService::finding_continued_simple_fractions(415, 9394);
    for(const auto & el : find) {
        std::cout << el << std::endl;
    }
}

int main() {
    // test_mod_pow();
    // test_is_quadratic_residue();
    // test_gcd();
    // test_extended_gcd();
    // test_symbol_service();
    test_contiened_fractions();

    std::cout << "All tests passed successfully!\n";
    return 0;
}
