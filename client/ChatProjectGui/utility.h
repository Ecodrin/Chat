#ifndef UTILITY_H
#define UTILITY_H

#include <random>
#include <sstream>
#include <iostream>

#include <QByteArray>

std::string generate_random_string(size_t length=100);
bool has_suffix(const std::string & s, const std::string & suffix);
std::vector<std::byte> qbyte_array_to_bytes_vector(const QByteArray & array);
QByteArray bytes_vector_to_qbate_array(const std::vector<std::byte> & vector);

#endif // UTILITY_H
