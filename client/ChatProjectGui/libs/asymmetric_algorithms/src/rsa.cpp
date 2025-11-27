#include "../include/rsa.hpp"

namespace asymmetric_algorithms {
    
    RSAGenerationKeys::RSAGenerationKeys(PrimalityTestEnum test_enum, double min_probability, size_t bit_length) : bit_length{bit_length}, min_probability{min_probability} {
        if(min_probability < 0.5 || min_probability >= 1) {
            throw std::invalid_argument("incorrect min probability");
        }
        change_primary_test(test_enum);
    }

    void RSAGenerationKeys::change_primary_test(PrimalityTestEnum test_enum) {
        switch (test_enum) {
        case PrimalityTestEnum::FermatPrimalityTest:
            primality_test = std::make_shared<FermatPrimalityTest>();
            break;
        case PrimalityTestEnum::SolovayStrassenPrimalityTest:
            primality_test = std::make_shared<SolovayStrassenPrimalityTest>();
            break;
        case PrimalityTestEnum::MillerRabinPrimalityTest:
            primality_test = std::make_shared<MillerRabinPrimalityTest>();
            break;
        default:
            throw std::invalid_argument("incorrect primality test");            
        }
    }

    
    std::pair<std::pair<boost::multiprecision::cpp_int, boost::multiprecision::cpp_int>, 
        std::pair<boost::multiprecision::cpp_int, boost::multiprecision::cpp_int>> 
        RSAGenerationKeys::generate_keys() {
        
        boost::multiprecision::cpp_int p; 
        boost::multiprecision::cpp_int q;
        boost::random::random_device rd;
        boost::multiprecision::cpp_int N, phi_N;
        boost::random::uniform_int_distribution<boost::multiprecision::cpp_int> dist(0, (boost::multiprecision::cpp_int{1} << bit_length));
        boost::multiprecision::cpp_int e = 65537;
        while(true) {
            while (true) {
                p = dist(rd);
                if (primality_test->check_primality(p, min_probability) >= min_probability) {
                    break;;
                }
            }
            while (true) {
                q = dist(rd);
                q |= (boost::multiprecision::cpp_int{1} << 512);
                if(primality_test->check_primality(q, min_probability) >= min_probability) {
                    break;
                }
            }
            N = p * q;
            phi_N = (p - 1) * (q - 1);

            auto [_, d, __] = SymbolService::extended_gcd(e, phi_N);
            while(d < 0) {
                d += phi_N;
            }
            d %= phi_N;
        
            if(boost::multiprecision::abs(p - q) > (boost::multiprecision::cpp_int{1} << 512) && SymbolService::gcd(phi_N, e) == 1 && check_vulnerability_d(d, N)) {
                return {{e, N}, {d, N}};
            }
        }
        /*
        e * d + phi_N * y = gcd = 1
        ed = 1 mod phi_N

        */
    }

    std::pair<std::pair<boost::multiprecision::cpp_int, boost::multiprecision::cpp_int>, 
            std::pair<boost::multiprecision::cpp_int, boost::multiprecision::cpp_int>> RSAGenerationKeys::generate_bad_keys() {
        boost::multiprecision::cpp_int p; 
        boost::multiprecision::cpp_int q;
        boost::random::random_device rd;
        boost::multiprecision::cpp_int N, phi_N;
        boost::random::uniform_int_distribution<boost::multiprecision::cpp_int> dist(0, (boost::multiprecision::cpp_int{1} << bit_length));
        boost::multiprecision::cpp_int e{"1073780833"};
        while(true) {
            while (true) {
                p = dist(rd);
                if (primality_test->check_primality(p, min_probability) >= min_probability) {
                    break;;
                }
            }
            while (true) {
                q = dist(rd);
                if(primality_test->check_primality(q, min_probability) >= min_probability) {
                    break;
                }
            }
            N = p * q;
            phi_N = (p - 1) * (q - 1);

            auto [_, d, __] = SymbolService::extended_gcd(e, phi_N);
            while(d < 0) {
                d += phi_N;
            }
            d %= phi_N;
        
            if(SymbolService::gcd(phi_N, e) == 1) {
                return {{e, N}, {d, N}};
            }
        }
    }

    bool RSAGenerationKeys::check_vulnerability_d(const boost::multiprecision::cpp_int & d, const boost::multiprecision::cpp_int & N) {
        return SymbolService::mod_pow(d, 4) > N / 81;
    }

    RSA::RSA(PrimalityTestEnum test_enum, double min_probability, size_t bit_length) : keys_generator{test_enum, min_probability, bit_length} {
        change_key();
    }

    boost::multiprecision::cpp_int RSA::calculate(const boost::multiprecision::cpp_int & block, const std::pair<boost::multiprecision::cpp_int, boost::multiprecision::cpp_int> & key) {
        return SymbolService::mod_pow(block, key.first, key.second);
    }

    boost::multiprecision::cpp_int RSA::convert_to_cpp_int(const std::vector<std::byte> & block) {
        boost::multiprecision::cpp_int res;
        for(size_t i = 0; i < block.size(); ++i) {
            res |= block[i];
            if(i != block.size() - 1) {
                res <<= 8;
            }
        }
        return res;
    }
    
    std::vector<std::byte> RSA::convert_to_bytes_vector(const boost::multiprecision::cpp_int & block) {
        std::vector<std::byte> res;
        boost::multiprecision::cpp_int copy_block(block);
        while(copy_block > 0) {
            res.push_back(std::byte{(copy_block & 0xFF).convert_to<std::byte>()});
            copy_block >>= 8;
        }
        std::reverse(res.begin(), res.end());
        return res;
    }

    std::vector<std::byte> RSA::proccess(const std::vector<std::byte> block, const std::pair<boost::multiprecision::cpp_int, boost::multiprecision::cpp_int> & key) {
        boost::multiprecision::cpp_int converted_block = convert_to_cpp_int(block);
        auto res = calculate(converted_block, key);
        size_t output_size = (boost::multiprecision::msb(key.second) + 8) / 8; 
        auto result_bytes = convert_to_bytes_vector(res);
        if (result_bytes.size() < output_size) {
            result_bytes.insert(result_bytes.begin(), output_size - result_bytes.size(), std::byte{0});
        }
        
        return result_bytes;
    }

    
    std::pair<boost::multiprecision::cpp_int, boost::multiprecision::cpp_int> RSA::get_public_key() const {
        return public_key;
    }

    void RSA::change_key() {
        auto [puk, prk] = keys_generator.generate_keys();
        public_key = puk;
        private_key = prk;
    }

    std::future<void> RSA::decryption(const std::vector<std::byte>& block, std::vector<std::byte> & res) {
        return std::async(std::launch::async, [this, block, &res]() {
            boost::multiprecision::cpp_int N = public_key.second;
            size_t require_block_size = (boost::multiprecision::msb(N) + 8) / 8;
            for(size_t i = 0; i < block.size(); i += require_block_size) {
                std::vector<std::byte> b{
                    block.begin() + i,
                    block.begin() + i + require_block_size
                };
                auto r = proccess(b, private_key);
                unpadding(r);
                res.insert(res.end(), r.begin(), r.end());
            }
        });
    }

    std::future<void> RSA::encryption(const std::vector<std::byte>& block, std::vector<std::byte> & res) {
        return std::async(std::launch::async, [this, block, &res]() {
            boost::multiprecision::cpp_int N = public_key.second;
            size_t require_block_size = (boost::multiprecision::msb(N) + 8) / 8;
            size_t data_block_size = require_block_size - 11;
            for(size_t i = 0; i < block.size(); i += data_block_size) {
                std::vector<std::byte> b{
                    block.begin() + i,
                    block.begin() + std::min(i + data_block_size, block.size())
                };
                padding(b, require_block_size);
                auto r = proccess(b, public_key);
                res.insert(res.end(), r.begin(), r.end());
            }
        });
    }

    void RSA::save_keys(const std::string & filename) const {
        std::ofstream f1(filename + ".pub");
        std::ofstream f2(filename + "");
        if(!f1.is_open() || !f2.is_open()) {
            throw std::invalid_argument("incorrect filename");
        }
        f1 << public_key.first << " " << public_key.second;
        f2 << private_key.first << " " << private_key.second;
        f1.close();
        f2.close();
    }

    
    RSA::RSA(const std::string & public_filename, const std::string & private_filename) {
        std::ifstream f1(public_filename);
        std::ifstream f2(private_filename);
        if(!f1.is_open() || !f2.is_open()) {
            throw std::invalid_argument("incorrect filename");
        }
        f1 >> public_key.first >> public_key.second;
        f2 >> private_key.first >> private_key.second;
        f1.close();
        f2.close();
    }

    RSA::RSA(const std::string & public_filename) {
        std::ifstream f1(public_filename);
        if(!f1.is_open()) {
            throw std::invalid_argument("incorrect filename");
        }
        f1 >> public_key.first >> public_key.second;
        f1.close();
    }

    void RSA::padding(std::vector<std::byte> &block, size_t require_size) {
        
        size_t data_size = block.size();
        size_t ps_len = require_size - data_size - 3; 
        
        if (ps_len < 8) {
            throw std::runtime_error("Padding too small - data block too large");
        }

        std::vector<std::byte> padded_block;
        padded_block.reserve(require_size);

        padded_block.push_back(std::byte{0x00});
        padded_block.push_back(std::byte{0x02});
        
        std::random_device rd;
        std::uniform_int_distribution<uint8_t> dist(1, 255);
        for (size_t i = 0; i < ps_len; ++i) {
            padded_block.push_back(static_cast<std::byte>(dist(rd)));
        }
        
        padded_block.push_back(std::byte{0x00});
        padded_block.insert(padded_block.end(), block.begin(), block.end());
        
        block = std::move(padded_block);

        
        // bytes_utility::print_bytes_vector(std::cout, block, " ");
        // std::cout << std::endl;
    }

    void RSA::unpadding(std::vector<std::byte> &block) {
        if (block.size() < 11) {
            throw std::runtime_error("Block too small for unpadding");
        }
        
        if (block[0] != std::byte{0x00} || block[1] != std::byte{0x02}) {
            // throw std::runtime_error("incorrect padding");
            return;
        }
        
        size_t i = 2;
        while (i < block.size() && block[i] != std::byte{0x00}) {
            ++i;
        }
        
        if (i >= block.size() - 1) {
            throw std::runtime_error("Padding separator not found");
        }
        
        std::vector<std::byte> message(
            block.begin() + i + 1,
            block.end()
        );
        
        block = std::move(message);
    }

    std::future<void> RSA::encryption(const std::string& input_file_path, const std::string& output_file_path) {
        return std::async(std::launch::async, [this, input_file_path, output_file_path]() {
            std::ifstream input(input_file_path, std::ios::binary);
            std::ofstream output(output_file_path, std::ios::binary);
            
            if (!input.is_open()) {
                throw std::invalid_argument("Cannot open input file");
            }
            if (!output.is_open()) {
                throw std::invalid_argument("Cannot open output file");
            }
            
            boost::multiprecision::cpp_int N = public_key.second;
            size_t require_block_size = (boost::multiprecision::msb(N) + 8) / 8;
            size_t data_block_size = require_block_size - 11;
            
            std::vector<std::byte> buffer(data_block_size);
            while (input.read(reinterpret_cast<char*>(buffer.data()), data_block_size)) {
                size_t bytes_read = input.gcount();
                std::vector<std::byte> current_block(buffer.data(), buffer.data() + bytes_read);
                
                
                padding(current_block, require_block_size);
                // bytes_utility::print_bytes_vector(std::cout, current_block);
                auto processed_block = proccess(current_block, public_key);
                output.write(reinterpret_cast<const char*>(processed_block.data()), processed_block.size());
                
                std::fill(buffer.begin(), buffer.end(), std::byte{0});
            }

            size_t bytes_read = input.gcount();
            if (bytes_read > 0) {
                std::vector<std::byte> last_block(buffer.data(), buffer.data() + bytes_read);
                
                padding(last_block, require_block_size);
                auto processed_block = proccess(last_block, public_key);
                output.write(reinterpret_cast<const char*>(processed_block.data()), processed_block.size());
            }
            output.flush();
            input.close();
            output.close();
        });
    }

    std::future<void> RSA::decryption(const std::string& input_file_path, const std::string& output_file_path) {
        return std::async(std::launch::async, [this, input_file_path, output_file_path]() {
            std::ifstream input(input_file_path, std::ios::binary);
            std::ofstream output(output_file_path, std::ios::binary);
            
            if (!input.is_open()) {
                throw std::invalid_argument("Cannot open input file");
            }
            if (!output.is_open()) {
                throw std::runtime_error("Cannot open output file");
            }
            
            boost::multiprecision::cpp_int N = public_key.second;
            size_t encrypted_block_size = (boost::multiprecision::msb(N) + 8) / 8; 
            
            std::vector<std::byte> buffer(encrypted_block_size);
            size_t block_count = 0;

            while (input.read(reinterpret_cast<char*>(buffer.data()), encrypted_block_size)) {
                size_t bytes_read = input.gcount();
                
                std::vector<std::byte> encrypted_block(buffer.data(), buffer.data() + bytes_read);
                
                auto processed_block = proccess(encrypted_block, private_key);
                
                unpadding(processed_block);
                
                output.write(reinterpret_cast<const char*>(processed_block.data()), processed_block.size());
                
                std::fill(buffer.begin(), buffer.end(), std::byte{0});
            }

            output.flush();
            input.close();
            output.close();
            
        });
    }
}