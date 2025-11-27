#include "../include/wieners_attack.hpp"

namespace asymmetric_algorithms {
    boost::multiprecision::cpp_int wiener_attack(const std::pair<boost::multiprecision::cpp_int, boost::multiprecision::cpp_int> & public_key) {
        std::vector<boost::multiprecision::cpp_int> contiened_fractions = SymbolService::finding_continued_simple_fractions(public_key.first, public_key.second);
        boost::multiprecision::cpp_int e = public_key.first;
        boost::multiprecision::cpp_int N = public_key.second;
        
        for(const auto & factor : SymbolService::finding_convergent_series_from_continuous_simple_fraction(contiened_fractions)) {
            auto [k, d] = factor;
            if (d == 0) {
                break;
            }
            if(k == 0) {
                continue;
            }
            boost::multiprecision::cpp_int phi_N = (e * d - 1) / k;
            // std::cout << "K: " << k << " D: "<< d << " N: " << N << " phiN: " << phi_N << std::endl;
            auto [p, q] = SymbolService::solving_quadratic_equation(1, -(N - phi_N + 1), N);
            // std::cout << "K: " << k << " D: "<< d << " N: " << N << " phiN: " << phi_N << " P*Q: " << p * q << " P: " << p << " Q: " << q << std::endl;
            if (p * q == N) {
                return d;
            }
        }
        return 0;
    }
}