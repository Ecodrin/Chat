#include "Galois_field_service.hpp"



using namespace symmetric_algorithms;

int main() {
    auto t = GaloisFieldService::get_irreducible_polynomials();
    std::cout << t.size() << std::endl;
    for(int i = 0; i < t.size(); ++i) {
        auto p = t[i];
        std::cout << i << " ";
        auto tttt = {std::byte{0x01}, p};
        GaloisFieldService::print(tttt);
    }

    // auto a = std::byte{0x84};
    // auto b = std::byte{0x33};
    // auto mod = std::byte{0x17};
    // // GaloisFieldService::print(a);
    // // GaloisFieldService::print(b);
    // // GaloisFieldService::print(mod);
    // auto t = GaloisFieldService::multiply(a, b, mod);
    // GaloisFieldService::print(t);

    // auto t = std::byte{0x02};
    // auto mod = std::byte{0xF3};
    // auto i = GaloisFieldService::find_inverse(t, mod);
    // GaloisFieldService::print(i);

    // auto tt = GaloisFieldService::multiply(t, i, mod);
    // GaloisFieldService::print(tt);

    // auto ms = GaloisFieldService::get_irreducible_polynomials();
    // for(const auto & m : ms) {
    //     GaloisFieldService::print(m);
    // }

    // bytes_utility::print_bytes_vector(std::cout, bytes_utility::random_bytes_vector(16), true);
}