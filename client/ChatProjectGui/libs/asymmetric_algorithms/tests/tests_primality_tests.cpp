#include "primality_tests.hpp"

int main() {

    asymmetric_algorithms::FermatPrimalityTest test1;
    asymmetric_algorithms::SolovayStrassenPrimalityTest test2;
    asymmetric_algorithms::MillerRabinPrimalityTest test3;
    boost::multiprecision::cpp_int a("115792089237316195423570985008687907852837564279074904382605163141518161494337");
    // boost::multiprecision::cpp_int a("317");
    
    std::cout << test1.check_primality(a, 0.99) << std::endl;
    std::cout << test2.check_primality(a, 0.99) << std::endl;
    std::cout << test3.check_primality(a, 0.99) << std::endl;
}