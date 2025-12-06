#ifndef UTILITY_H
#define UTILITY_H

#include <random>
#include <sstream>
#include <iostream>


std::string generate_random_string(size_t length=100);
bool has_suffix(const std::string & s, const std::string & suffix);

#endif // UTILITY_H
