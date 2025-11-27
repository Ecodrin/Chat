#include "symbol_service.hpp"


namespace asymmetric_algorithms {

    boost::multiprecision::cpp_int SymbolService::calculate_legendre_symbol(const boost::multiprecision::cpp_int & a, const boost::multiprecision::cpp_int & p) {
        if (p < 3) {
            throw std::invalid_argument("p must be != 2");
        }
        if (a % p == 0) {
            return 0;
        }
        if (SymbolService::is_quadratic_residue(a, p)) {
            return 1;
        }
        return -1;
    }


    boost::multiprecision::cpp_int SymbolService::mod_pow(const boost::multiprecision::cpp_int & a, const boost::multiprecision::cpp_int & exp,  boost::multiprecision::cpp_int mod) {
        boost::multiprecision::cpp_int tmp_exp{exp};
        boost::multiprecision::cpp_int base{a};
        boost::multiprecision::cpp_int res = 1;
        if (exp < 0) {
            throw std::invalid_argument("exp must be >= 0 in mod_pow");
        }
        base %= mod;
        while (tmp_exp > 0){
            if(tmp_exp % 2 == 1) {
                res = (res * base) % mod;
            } 
            base = (base * base) % mod;
            tmp_exp /= 2;
        }
        return res % mod;
    }

    boost::multiprecision::cpp_int SymbolService::mod_pow(const boost::multiprecision::cpp_int & a, const boost::multiprecision::cpp_int & exp) {
        boost::multiprecision::cpp_int tmp_exp{exp};
        boost::multiprecision::cpp_int base{a};
        boost::multiprecision::cpp_int res = 1;
        if (exp < 0) {
            throw std::invalid_argument("exp must be >= 0 in mod_pow");
        }
        while (tmp_exp > 0){
            if(tmp_exp % 2 == 1) {
                res = (res * base);
            } 
            base = (base * base);
            tmp_exp /= 2;
        }
        return res;
    }

    bool SymbolService::is_quadratic_residue(const boost::multiprecision::cpp_int & a, const boost::multiprecision::cpp_int & p) {
        if(a == 0) {
            return true;
        }
        boost::multiprecision::cpp_int new_a{a};
        if (a < 0) {
            new_a = (a % p);
            new_a += p;
        }
        if (mod_pow(new_a, (p - 1) / 2, p) == 1) {
            return true;
        } 
        if (mod_pow(new_a, (p - 1) / 2, p) - p == -1) {
            return false;
        } 
        throw std::invalid_argument("incorrect a, b in is_quadratic_residue");
    }

    
    boost::multiprecision::cpp_int SymbolService::calculate_jakobi_symbol(const boost::multiprecision::cpp_int & a, const boost::multiprecision::cpp_int & p) {
        if(p % 2 == 0) {
            throw std::invalid_argument("p must be odd");
        }
        boost::multiprecision::cpp_int r = 1;
        boost::multiprecision::cpp_int tmp_a{a};
        
        boost::multiprecision::cpp_int tmp_p{p};
        if (a < 0) {
            tmp_a *= -1;
            if (tmp_p % 4 == 3) {
                r *= -1;
            }
        }
        while (tmp_a != 0) {
            boost::multiprecision::cpp_int t = 0;
            while (tmp_a % 2 == 0) {
                ++t;
                tmp_a /= 2;
            }
            if (t % 2 != 0) {
                if(tmp_p % 8 == 3 || tmp_p % 8 == 5) {
                    r *= -1;
                }
            }
            
            if(tmp_a % 4 == tmp_p % 4 && tmp_p % 4 == 3) {
                r *= -1;
            }
            auto c{tmp_a};
            tmp_a = tmp_p % c;
            tmp_p = c;
        }
        if(tmp_p == 1) {
            return r;
        } else {
            return 0;
        }
    }

    boost::multiprecision::cpp_int SymbolService::gcd(const boost::multiprecision::cpp_int & a, const boost::multiprecision::cpp_int & b) {
        boost::multiprecision::cpp_int tmp_a{a};
        boost::multiprecision::cpp_int tmp_b{b};

        while (tmp_b > 0) {
            tmp_a = tmp_a % tmp_b;
            std::swap(tmp_a, tmp_b);
        }
        return tmp_a;
    }


    std::tuple<boost::multiprecision::cpp_int, boost::multiprecision::cpp_int, boost::multiprecision::cpp_int> SymbolService::extended_gcd(const boost::multiprecision::cpp_int& a, const boost::multiprecision::cpp_int& b) {
        boost::multiprecision::cpp_int old_r = a, r = b;
        boost::multiprecision::cpp_int old_s = 1, s = 0;
        boost::multiprecision::cpp_int old_t = 0, t = 1;

        while (r != 0) {
            boost::multiprecision::cpp_int q = old_r / r;

            boost::multiprecision::cpp_int tmp_r = r;
            r = old_r - q * r;
            old_r = tmp_r;

            boost::multiprecision::cpp_int tmp_s = s;
            s = old_s - q * s;
            old_s = tmp_s;

            boost::multiprecision::cpp_int tmp_t = t;
            t = old_t - q * t;
            old_t = tmp_t;
        }

        if (old_r < 0) {
            old_r = -old_r;
            old_s = -old_s;
            old_t = -old_t;
        }

        return std::make_tuple(old_r, old_s, old_t);
    }



    std::vector<boost::multiprecision::cpp_int> SymbolService::finding_continued_simple_fractions(const boost::multiprecision::cpp_int & u, const boost::multiprecision::cpp_int & v) {
        boost::multiprecision::cpp_int x = u;
        boost::multiprecision::cpp_int y = v;
        std::vector<boost::multiprecision::cpp_int> res;
        while (y > 0) {
            boost::multiprecision::cpp_int tmp = y;
            res.push_back(x / y);
            y = x % y;
            x = tmp;
        }
        return res;
    }

    std::pair<boost::multiprecision::cpp_int, boost::multiprecision::cpp_int> SymbolService::solving_quadratic_equation(const boost::multiprecision::cpp_int & a, 
                                                                                                                    const boost::multiprecision::cpp_int & b,
                                                                                                                    const boost::multiprecision::cpp_int & c) {
        if(a == 0) {
            return {-c / b, 0};
        }
        auto d = b * b - 4 * a * c;
        if(d < 0) {
            return {0, 0};
        }
        auto sqrt_d = boost::multiprecision::sqrt(d);
        return {(-b - sqrt_d) / (2 * a), (-b + sqrt_d) / (2 * a)};
    }


    std::vector<std::pair<boost::multiprecision::cpp_int, boost::multiprecision::cpp_int>> 
                            SymbolService::finding_convergent_series_from_continuous_simple_fraction(const std::vector<boost::multiprecision::cpp_int> & factors) {
        boost::multiprecision::cpp_int prev_h = 1, prev_prev_h = 0;
        boost::multiprecision::cpp_int prev_k = 0, prev_prev_k = 1;

        std::vector<std::pair<boost::multiprecision::cpp_int, boost::multiprecision::cpp_int>> res;
        for(const auto & a : factors) {
            boost::multiprecision::cpp_int h = a * prev_h + prev_prev_h;
            boost::multiprecision::cpp_int k = a * prev_k + prev_prev_k;
            prev_prev_h = prev_h;
            prev_prev_k = prev_k;
            prev_h = h;
            prev_k = k;
            res.push_back({h, k});
        }
        return res;
    }
        
}