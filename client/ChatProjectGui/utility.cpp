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
