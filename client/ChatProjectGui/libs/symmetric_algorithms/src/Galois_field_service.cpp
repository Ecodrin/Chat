#include "../include/Galois_field_service.hpp"

namespace symmetric_algorithms{
    
    std::byte GaloisFieldService::add(const std::byte &a, const std::byte & b) {
        return a ^ b;
    }

    std::byte GaloisFieldService::multiply(const std::byte &a, const std::byte & b, const std::byte & mod) {
        // if(!check_irreducible(mod)) {
        //     throw std::invalid_argument("not irreducible");
        // }
        auto m = bytes_utility::multiply_by_xor(std::vector<std::byte>{std::byte{0x00}, a}, std::vector<std::byte>{std::byte{0x00}, b});
        return divide(m, {std::byte{0x01}, mod}).second;
    }

    std::byte GaloisFieldService::multiply(const std::vector<std::byte> &a, const std::byte & b, const std::byte & mod) {
        // if(!check_irreducible(mod)) {
        //     throw std::invalid_argument("not irreducible");
        // }
        auto m = bytes_utility::multiply_by_xor(a, std::vector<std::byte>{std::byte{0x00}, b});
        return divide(m, {std::byte{0x01}, mod}).second;
    }

    std::pair<std::vector<std::byte>, std::byte> GaloisFieldService::divide(const std::byte &a, const std::byte & b) {

        std::vector<std::byte> vector_b = {std::byte{0x00}, b};
        auto t = bytes_utility::divide_by_xor({std::byte{0x00}, a}, vector_b);
        return {t.first, t.second.back()};
    }

    std::pair<std::vector<std::byte>, std::byte> GaloisFieldService::divide(const std::vector<std::byte> &a, const std::byte & b) {

        std::vector<std::byte> vector_b = {std::byte{0x00}, b};
        auto t = bytes_utility::divide_by_xor(a, vector_b);
        return {t.first, t.second.back()};
    }

    std::pair<std::vector<std::byte>, std::byte> GaloisFieldService::divide(const std::vector<std::byte> &a, const std::vector<std::byte> & b) {
        auto t = bytes_utility::divide_by_xor(a, b);
        return {t.first, t.second.back()};
    }

    void GaloisFieldService::print(const std::vector<std::byte> & b) {
        std::string res;
        for(int i = b.size() * 8 - 1; i >= 0; --i) {
            if(bytes_utility::get_bit(b, i, bytes_utility::PermutationsOrderRule::ReverseOrderZeroIndex) > std::byte{0}) {
                if(i == 1) {
                    res += "x";
                } else if(i == 0) {
                    res += "1";
                } else {
                    res += "x^" + std::to_string(i);
                }
                if (res.size() != 0) {
                    res += " + ";
                }
            }
        }
        if(res.size() > 0 && res[res.size() - 1] == ' ') {
            res.erase(res.end()-3, res.end());
        }

        std::cout << res << std::endl;
    }

    void GaloisFieldService::print(const std::byte & b) {
        std::string res;
        for(int i =  7; i >= 0; --i) {
            if(bytes_utility::get_bit({b}, i, bytes_utility::PermutationsOrderRule::ReverseOrderZeroIndex) > std::byte{0}) {
                if(i == 1) {
                    res += "x";
                } else if(i == 0) {
                    res += "1";
                } else {
                    res += "x^" + std::to_string(i);
                }
                if (res.size() != 0) {
                    res += " + ";
                }
            }
        }
        if(res.size() > 0 && res[res.size() - 1] == ' ') {
            res.erase(res.end()-3, res.end());
        }

        std::cout << res << std::endl;
    }


    std::vector<std::byte> GaloisFieldService::get_polynomials(size_t pow) {
        std::vector<std::byte> res(1);
        for(int i = pow; i >= 0; --i) {
            std::vector<std::byte> tmp;
            for(const auto & p:res) {
                tmp.push_back(p);
                std::byte t{p};
                std::vector<std::byte> tt{t};
                bytes_utility::add_one(tt, i, bytes_utility::PermutationsOrderRule::ReverseOrderZeroIndex);
                tmp.push_back(tt.back());
            }
            res = tmp;
        }
        return res;
    }

    
    bool GaloisFieldService::check_irreducible(const std::byte & b) {
        if((b & std::byte{1}) == std::byte{0}) {
            return false;
        }
        std::vector<std::byte> polynomias = get_polynomials(4);
        for(int i = 1; i < polynomias.size(); ++i) {
            if((polynomias[i] & std::byte{1}) == polynomias[i]) {
                continue;
            }
            auto [q, r] = divide({std::byte{0x01}, b}, polynomias[i]);
            if (r == std::byte{0x00}) {
                return false;
            }
        }
        return true;
    }
    
    std::vector<std::byte> GaloisFieldService::get_irreducible_polynomials() {
        std::vector<std::byte> polynomias = get_polynomials(7);
        std::vector<std::byte> res;
        for(int i = 1; i < polynomias.size(); ++i) {
            if (check_irreducible(polynomias[i])) {
                res.push_back(polynomias[i]);
            } 
        }
        return res;
    }

    std::byte GaloisFieldService::mod_exp(const std::byte &a, const std::byte&exp, const std::byte & mod) {
        std::byte res{1};
        std::byte t = a;
        std::byte exp_m = exp;
        while(exp_m > std::byte{0}) {
            if((exp_m & std::byte{1} )> std::byte{0}) {
                res = multiply(res, t, mod);
            }
            t = multiply(t, t, mod);
            exp_m >>= 1;
        }
        return res;
    }
    
    std::byte GaloisFieldService::find_inverse(const std::byte & b, const std::byte & mod) {
        auto i = mod_exp(b, std::byte{254}, mod);
        return i;
    }
}
