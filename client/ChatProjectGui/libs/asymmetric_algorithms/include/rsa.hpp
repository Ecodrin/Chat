#pragma once

#include <iostream>
#include <memory>
#include <utility>
#include <future>
#include <fstream>
#include <cstddef>
#include <boost/multiprecision/cpp_int.hpp>

#include "primality_tests.hpp"
#include "bytes_utility.hpp"

namespace asymmetric_algorithms {
    class RSAGenerationKeys {
    private:
        size_t bit_length;
        double min_probability;
        std::shared_ptr<PrimalityTest> primality_test;
        void change_primary_test(PrimalityTestEnum test_enum);
    public:

        RSAGenerationKeys() = default;
        RSAGenerationKeys(PrimalityTestEnum test_enum, double min_probability, size_t bit_length);
        std::pair<std::pair<boost::multiprecision::cpp_int, boost::multiprecision::cpp_int>, 
            std::pair<boost::multiprecision::cpp_int, boost::multiprecision::cpp_int>> generate_keys();

        std::pair<std::pair<boost::multiprecision::cpp_int, boost::multiprecision::cpp_int>, 
            std::pair<boost::multiprecision::cpp_int, boost::multiprecision::cpp_int>> generate_bad_keys();

            
        static bool check_vulnerability_d(const boost::multiprecision::cpp_int & d, const boost::multiprecision::cpp_int & N);
    };
    
    class RSA {
    private:
        RSAGenerationKeys keys_generator;
        std::pair<boost::multiprecision::cpp_int, boost::multiprecision::cpp_int> public_key;
        std::pair<boost::multiprecision::cpp_int, boost::multiprecision::cpp_int> private_key;


        std::vector<std::byte> proccess(const std::vector<std::byte> block, const std::pair<boost::multiprecision::cpp_int, boost::multiprecision::cpp_int> & key);
        static boost::multiprecision::cpp_int calculate(const boost::multiprecision::cpp_int & block, const std::pair<boost::multiprecision::cpp_int, boost::multiprecision::cpp_int> & key);
        void padding(std::vector<std::byte> &block, size_t require_size);
        void unpadding(std::vector<std::byte> &block);
    public:
        RSA(PrimalityTestEnum test_enum, double min_probability, size_t bit_length);
        RSA(const std::string & public_filename);
        RSA(const std::string & public_filename, const std::string & private_filename);
        void save_keys(const std::string & filename) const;
        void change_key();
        std::pair<boost::multiprecision::cpp_int, boost::multiprecision::cpp_int> get_public_key() const;
        static boost::multiprecision::cpp_int convert_to_cpp_int(const std::vector<std::byte> & block);
        static std::vector<std::byte> convert_to_bytes_vector(const boost::multiprecision::cpp_int & block);

        std::future<void> encryption(const std::vector<std::byte>& block, std::vector<std::byte> & res);
        std::future<void> decryption(const std::vector<std::byte>& block, std::vector<std::byte> & res);
        std::future<void> encryption(const std::string& input_file, const std::string& output_file);
        std::future<void> decryption(const std::string& input_file, const std::string& output_file);
    };

};