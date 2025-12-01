#pragma once

#include <iostream>
#include <tuple>
#include <set>
#include <boost/multiprecision/cpp_int.hpp>

namespace asymmetric_algorithms {
    class SymbolService {
    public:
        static boost::multiprecision::cpp_int calculate_Legendre_symbol(const boost::multiprecision::cpp_int & a, const boost::multiprecision::cpp_int & p); 
        static boost::multiprecision::cpp_int calculate_Jakobi_symbol(const boost::multiprecision::cpp_int & a, const boost::multiprecision::cpp_int & p);
        
        static boost::multiprecision::cpp_int 
                    mod_pow(const boost::multiprecision::cpp_int & a, const boost::multiprecision::cpp_int & exp, boost::multiprecision::cpp_int mod);

        static boost::multiprecision::cpp_int 
                    mod_pow(const boost::multiprecision::cpp_int & a, const boost::multiprecision::cpp_int & exp);
        static bool is_quadratic_residue(const boost::multiprecision::cpp_int & a,const boost::multiprecision::cpp_int & p);
        static boost::multiprecision::cpp_int gcd(const boost::multiprecision::cpp_int & a, const boost::multiprecision::cpp_int & b);
        static std::tuple<boost::multiprecision::cpp_int, boost::multiprecision::cpp_int, boost::multiprecision::cpp_int> 
                    extended_gcd(const boost::multiprecision::cpp_int & a, const boost::multiprecision::cpp_int & b);

        
        static std::vector<boost::multiprecision::cpp_int> finding_continued_simple_fractions(const boost::multiprecision::cpp_int & u, const boost::multiprecision::cpp_int & v);
        static std::vector<std::pair<boost::multiprecision::cpp_int, boost::multiprecision::cpp_int>> 
                                            finding_convergent_series_from_continuous_simple_fraction(const std::vector<boost::multiprecision::cpp_int> & factors);
        

        static std::pair<boost::multiprecision::cpp_int, boost::multiprecision::cpp_int> solving_quadratic_equation(const boost::multiprecision::cpp_int & a, 
                                                                                                                    const boost::multiprecision::cpp_int & b,
                                                                                                                    const boost::multiprecision::cpp_int & c);
    };
}