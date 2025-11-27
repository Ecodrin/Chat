#include "wieners_attack.hpp"
#include "rsa.hpp"

using namespace asymmetric_algorithms;

int main() {
    auto d = wiener_attack({1073780833, 1220275921});
    std::cout << d << std::endl;
    d = wiener_attack({1779399043, 2796304957});
    std::cout << d << std::endl;


    boost::multiprecision::cpp_int e{"1073780833"};
        std::cout << boost::multiprecision::msb(e) << std::endl;
    RSAGenerationKeys generator(PrimalityTestEnum::FermatPrimalityTest, 0.9, 127);
    
    // auto keys = generator.generate_bad_keys();
    // std::cout << keys.second.first << " " << boost::multiprecision::msb(keys.second.second)<< std::endl;
    // while (RSAGenerationKeys::check_vulnerability_d(keys.second.first, keys.second.second)) {
    // std::cout << keys.second.first << " " << boost::multiprecision::msb(keys.second.second)<< std::endl;
    //     keys = generator.generate_bad_keys();
    // }

    // std::cout << keys.first.first << " " << keys.first.second << std::endl;
    // d = wiener_attack(keys.first);
    // std::cout << "Get d: "<< d << " D: " << keys.second.first << std::endl;
}