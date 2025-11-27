#pragma once

#include <iostream>
#include <set>
#include <boost/multiprecision/cpp_int.hpp>

#include "symbol_service.hpp"


namespace asymmetric_algorithms {
    boost::multiprecision::cpp_int wiener_attack(const std::pair<boost::multiprecision::cpp_int, boost::multiprecision::cpp_int> & public_key);
}