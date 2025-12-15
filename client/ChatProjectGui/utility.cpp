#include "utility.h"


std::string generate_random_string(size_t length) {
    std::string m = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    std::random_device dev;
    std::uniform_int_distribution<size_t> dist(0, m.size() - 1);
    std::ostringstream stream;
    for(size_t i = 0; i < length; ++i) {
        stream << dist(dev);
    }
    return stream.str();
}

bool has_suffix(const std::string & s, const std::string & suffix) {
    size_t i = s.find(suffix);
    if(i == s.size()) {
        return false;
    }
    return  i == s.size() - suffix.size();
}


std::vector<std::byte> qbyte_array_to_bytes_vector(const QByteArray & array) {
    std::vector<std::byte> res;
    res.reserve(array.size());
    for(char c: array) {
        res.emplace_back(std::byte{static_cast<unsigned char>(c)});
    }
    return res;
}


QByteArray bytes_vector_to_qbate_array(const std::vector<std::byte> & vector) {
    QByteArray res;
    res.reserve(vector.size());
    for(auto c : vector) {
        res.push_back(static_cast<unsigned char>(c));
    }
    return res;
}
