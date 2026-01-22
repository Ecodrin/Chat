#include "../include/encryption_modes.hpp"

namespace symmetric_interface_library {

    ECB::ECB(std::shared_ptr<InterfaceSymmetricAlgorithm> _alg) {
        this->alg = _alg;
        
    }

    std::vector<std::byte> ECB::encryption(const std::vector<std::byte> & encrypted_block) {
        size_t block_size = alg->get_block_size();
        if (encrypted_block.size() % block_size != 0) {
            throw std::runtime_error("input block size incorrect");
        }
        size_t count_blocks = encrypted_block.size() / block_size;
        size_t count_threads = std::thread::hardware_concurrency() - 1;
        size_t count_blocks_per_thread = (count_blocks / count_threads + 1);
        std::vector<std::thread> threads;
        std::vector<std::byte> res(encrypted_block.size());
        for(size_t i = 0; i < count_blocks; i += count_blocks_per_thread) {
            threads.push_back(std::thread([this, i, &encrypted_block, block_size, count_blocks_per_thread, count_blocks, &res]() {
                for (size_t j = 0; j < count_blocks_per_thread && j + i < count_blocks; ++j) {
                    std::vector<std::byte> block(block_size);
                    std::copy(encrypted_block.begin() + (j + i) * block_size, encrypted_block.begin() + (j + 1 + i) * block_size, block.begin());
                    std::vector<std::byte> new_block = alg->encryption(block);
                    std::copy(new_block.begin(), new_block.end(), res.begin() + (i + j) * block_size);
                }
                }));
        }

        for (auto& t : threads) {
            if (t.joinable()) {
                t.join();
            }
        }
        return res;
    }

    
    std::vector<std::byte> ECB::decryption(const std::vector<std::byte> & encrypted_block) {
        size_t block_size = alg->get_block_size();
        if (encrypted_block.size() % block_size != 0) {
            throw std::runtime_error("input block size incorrect");
        }
        size_t count_blocks = encrypted_block.size() / block_size;
        size_t count_threads = std::thread::hardware_concurrency() - 1;
        size_t count_blocks_per_thread = (count_blocks / count_threads + 1);
        std::vector<std::thread> threads;
        std::vector<std::byte> result(count_blocks * block_size);
        for(size_t i = 0; i < count_blocks; i += count_blocks_per_thread) {
            threads.push_back(std::thread([this, i, &encrypted_block, block_size, count_blocks_per_thread, count_blocks, &result]() {
                for (size_t j = 0; j < count_blocks_per_thread && j + i < count_blocks; ++j) {
                    std::vector<std::byte> block(block_size);
                    std::copy(encrypted_block.begin() + (j + i) * block_size, encrypted_block.begin() + (j + 1 + i) * block_size, block.begin());
                    std::vector<std::byte> new_block = alg->decryption(block);
                    std::copy(new_block.begin(), new_block.end(), result.begin() + (i + j) * block_size);
                    }
                }));
        }

        for (auto& t : threads) {
            if (t.joinable()) {
                t.join();
            }
        }
        return result;
    }

    CBC::CBC(std::shared_ptr<InterfaceSymmetricAlgorithm> _alg, const std::vector<std::byte> & iv): iv{iv} {
        this->alg = _alg;
        
    }

    std::vector<std::byte> CBC::encryption(const std::vector<std::byte> & encrypted_block) {
        size_t block_size = alg->get_block_size();
        if (encrypted_block.size() % block_size != 0) {
            throw std::runtime_error("input block size incorrect");
        }
        std::vector<std::byte> prev_c{iv};
        size_t count_blocks = encrypted_block.size() / block_size;

        std::vector<std::byte> result(count_blocks * block_size);
        for(size_t i = 0; i < count_blocks; ++i) {
            std::vector<std::byte> block(block_size);
            std::copy(encrypted_block.begin() + i * block_size, encrypted_block.begin() + (i + 1) * block_size, block.begin());
            std::vector<std::byte> cur_c = alg->encryption(bytes_utility::xor_vector(block, prev_c));
            std::copy(cur_c.begin(), cur_c.end(), result.begin() + i * block_size);
            prev_c = cur_c;    
        }
        return result;
    }
    
    std::vector<std::byte> CBC::decryption(const std::vector<std::byte> & encrypted_block) {
        size_t block_size = alg->get_block_size();
        if (encrypted_block.size() % block_size != 0) {
            throw std::runtime_error("input block size incorrect");
        }
        size_t count_blocks = encrypted_block.size() / block_size;
        size_t count_threads = std::thread::hardware_concurrency() - 1;
        size_t count_blocks_per_thread = (count_blocks / count_threads + 1);
        std::vector<std::thread> threads;
        std::vector<std::byte> result(count_blocks * block_size);
        std::mutex mutex; 
        for(size_t i = 0; i < count_blocks; i += count_blocks_per_thread) {
            threads.emplace_back([this, block_size, &result, i, &mutex, &encrypted_block, count_blocks_per_thread, count_blocks] {
                std::vector<std::byte> prev_c{iv};
                for(size_t j = 0; j < count_blocks_per_thread && j + i < count_blocks; ++j) {
                    if(i != 0 || j != 0) {
                        std::lock_guard<std::mutex> m(mutex);
                        std::copy(encrypted_block.begin() + (i + j - 1) * block_size, encrypted_block.begin() + (i + j) * block_size, prev_c.begin());
                    }
                    std::vector<std::byte> block(block_size);
                    std::copy(encrypted_block.begin() + (i + j) * block_size, encrypted_block.begin() + (i + 1 + j) * block_size, block.begin());
                    std::vector<std::byte> p = bytes_utility::xor_vector(prev_c, alg->decryption(block));
                    std::copy(p.begin(), p.end(), result.begin() + (i + j) * block_size);  
                }
            });
        }
        for(auto & t : threads) {
            if (t.joinable()) {
                t.join();
            }
        }
        return result;
    }


    PCBC::PCBC(std::shared_ptr<InterfaceSymmetricAlgorithm> _alg, const std::vector<std::byte> & iv): iv{iv} {
        this->alg = _alg;
        
    }

    std::vector<std::byte> PCBC::encryption(const std::vector<std::byte> & encrypted_block) {
        size_t block_size = alg->get_block_size();
        if (encrypted_block.size() % block_size != 0) {
            throw std::runtime_error("input block size incorrect");
        }
        std::vector<std::byte> prev_c{iv};
        size_t count_blocks = encrypted_block.size() / block_size;
        std::vector<std::byte> result(count_blocks * block_size);

        std::vector<std::byte> mc{iv};
        for(size_t i = 0; i < count_blocks; ++i) {
            std::vector<std::byte> block(block_size);
            std::copy(encrypted_block.begin() + i * block_size, encrypted_block.begin() + (i + 1) * block_size, block.begin());
            std::vector<std::byte> c = alg->encryption(bytes_utility::xor_vector(block, mc));
            std::copy(c.begin(), c.end(), result.begin() + i * block_size);
            mc = bytes_utility::xor_vector(block, c);
        }
        return result;
    }

    std::vector<std::byte> PCBC::decryption(const std::vector<std::byte> & encrypted_block) {
        size_t block_size = alg->get_block_size();
        if (encrypted_block.size() % block_size != 0) {
            throw std::runtime_error("input block size incorrect");
        }
        std::vector<std::byte> prev_c{iv};
        size_t count_blocks = encrypted_block.size() / block_size;
        std::vector<std::byte> result(count_blocks * block_size);

        std::vector<std::byte> mc{iv};
        for(size_t i = 0; i < count_blocks; ++i) {
            std::vector<std::byte> block(block_size);
            std::copy(encrypted_block.begin() + i * block_size, encrypted_block.begin() + (i + 1) * block_size, block.begin());
            std::vector<std::byte> m = bytes_utility::xor_vector(alg->decryption(block), mc);
            std::copy(m.begin(), m.end(), result.begin() + i * block_size);
            mc = bytes_utility::xor_vector(block, m);
        }
        return result;
    }

    CFB::CFB(std::shared_ptr<InterfaceSymmetricAlgorithm> _alg, const std::vector<std::byte> & iv): iv{iv} {
        this->alg = _alg;
        
    }

    std::vector<std::byte> CFB::encryption(const std::vector<std::byte> & encrypted_block) {
        size_t block_size = alg->get_block_size();
        if (encrypted_block.size() % block_size != 0) {
            throw std::runtime_error("input block size incorrect");
        }
        size_t count_blocks = encrypted_block.size() / block_size;
        
        std::vector<std::byte> result(count_blocks * block_size);
        std::vector<std::byte> c_prev{iv};
        for(size_t i = 0; i < count_blocks; ++i) {
            std::vector<std::byte> block(block_size);
            std::copy(encrypted_block.begin() + i * block_size, encrypted_block.begin() + (i + 1) * block_size, block.begin());
            std::vector<std::byte> new_block = bytes_utility::xor_vector(alg->encryption(c_prev), block);
            std::copy(new_block.begin(), new_block.end(), result.begin() + i * block_size);
            c_prev = new_block;
        }
        return result;
    }
    std::vector<std::byte> CFB::decryption(const std::vector<std::byte> & encrypted_block) {
        size_t block_size = alg->get_block_size();
        if (encrypted_block.size() % block_size != 0) {
            throw std::runtime_error("input block size incorrect");
        }
        size_t count_blocks = encrypted_block.size() / block_size;
        
        std::vector<std::byte> result(count_blocks * block_size);
        std::vector<std::byte> c_prev{iv};
        std::vector<std::thread> threads;
        size_t count_threads = std::thread::hardware_concurrency() - 1;
        size_t count_blocks_per_thread = (count_blocks / count_threads + 1);
        std::mutex mutex;
        for(size_t i = 0; i < count_blocks; i += count_blocks_per_thread) {
            threads.emplace_back([this, &encrypted_block, i, block_size, &mutex, count_blocks_per_thread, count_blocks, &result](){
                std::vector<std::byte> prev_c{iv};
                for(size_t j = 0; j < count_blocks_per_thread && i + j < count_blocks; ++j) {
                    if(i != 0 || j != 0) {
                        std::lock_guard<std::mutex> m(mutex);
                        std::copy(encrypted_block.begin() + (i + j - 1) * block_size, encrypted_block.begin() + (i + j) * block_size, prev_c.begin());
                    }
                    std::vector<std::byte> block(block_size);
                    std::copy(encrypted_block.begin() + (i + j) * block_size, encrypted_block.begin() + (i + 1 + j) * block_size, block.begin());
                    std::vector<std::byte> new_block = bytes_utility::xor_vector(alg->encryption(prev_c), block);
                    std::copy(new_block.begin(), new_block.end(), result.begin() + (i + j) * block_size); 
                }                   
            });
            
        }
        for(auto & t : threads) {
            if (t.joinable()) {
                t.join();
            }
        }
        return result;
    }




    OFB::OFB(std::shared_ptr<InterfaceSymmetricAlgorithm> _alg, const std::vector<std::byte> & iv): iv{iv} {
        this->alg = _alg;
        
    }

    std::vector<std::byte> OFB::encryption(const std::vector<std::byte> & encrypted_block) {
        size_t block_size = alg->get_block_size();
        std::vector<std::byte> result{encrypted_block};
        size_t total_size = result.size();
        std::vector<std::byte> key_stream{iv};

        for(size_t i = 0; i < total_size; i += block_size) {
            key_stream = alg->encryption(key_stream);
            
            size_t bytes_to_process = std::min(block_size, total_size - i);
            for(size_t j = 0; j < bytes_to_process; ++j) {
                result[i + j] ^= key_stream[j];
            }
        }
        return result;
    }   

    std::vector<std::byte> OFB::decryption(const std::vector<std::byte> & encrypted_block) {
        return encryption(encrypted_block);
    }  
    
    CTR::CTR(std::shared_ptr<InterfaceSymmetricAlgorithm> _alg, const std::vector<std::byte> & iv): iv{iv} {
        this->alg = _alg;
        
    }

    std::vector<std::byte> CTR::encryption(const std::vector<std::byte> & encrypted_block) {
        size_t block_size = alg->get_block_size();
        if(iv.size() != block_size) {
            iv.resize(block_size);
        }
        size_t count_blocks = (encrypted_block.size() + block_size - 1) / block_size;
        
        std::vector<std::byte> res(encrypted_block.size());
        std::vector<std::thread> threads;
        size_t count_threads = std::thread::hardware_concurrency() - 1;
        size_t count_blocks_per_thread = (count_blocks / count_threads + 1);
        
        for (size_t i = 0; i < count_blocks; i += count_blocks_per_thread) {
            threads.push_back(std::thread([this, i, block_size, &encrypted_block, &res, count_blocks_per_thread, count_blocks]() {
                for(size_t k = 0; k < count_blocks_per_thread && i + k < count_blocks; ++k) {
                    size_t offset = (i + k) * block_size;
                    size_t length = std::min(block_size, encrypted_block.size() - offset);
                
                    std::vector<std::byte> counter_block = iv;
                    counter_block.resize(block_size);
                
                    uint64_t counter = i + k;
                    for (size_t j = 0; j < sizeof(counter); j++) {
                        size_t pos = block_size - 1 - j;
                        if (pos < counter_block.size()) {
                            counter_block[pos] = static_cast<std::byte>((counter >> (8 * j)) & 0xFF);
                        }
                    }
                    
                    std::vector<std::byte> keystream = this->alg->encryption(counter_block);
                    
                    for (size_t j = 0; j < length; j++) {
                        res[offset + j] = encrypted_block[offset + j] ^ keystream[j];
                    }
                }
            }));
        }
        for (auto& t : threads) {
            t.join();
        }

        return res;
    }
    std::vector<std::byte> CTR::decryption(const std::vector<std::byte> & encrypted_block) {
        return encryption(encrypted_block);
    }

    RandomDelta::RandomDelta(std::shared_ptr<InterfaceSymmetricAlgorithm> _alg, const std::vector<std::byte> & iv): iv{iv} {
        this->alg = _alg;
        
    }

    std::vector<std::byte> RandomDelta::encryption(const std::vector<std::byte> & encrypted_block) {
        size_t block_size = alg->get_block_size();
        if (encrypted_block.size() % block_size != 0) {
            throw std::runtime_error("input block size incorrect");
        }
        size_t n = encrypted_block.size() / block_size;
        
        size_t count_blocks = encrypted_block.size() / block_size;
        
        std::vector<std::byte> res(encrypted_block.size());
        std::vector<std::thread> threads;
        size_t count_threads = std::thread::hardware_concurrency() - 1;
        size_t count_blocks_per_thread = (count_blocks / count_threads + 1);

        std::vector<std::byte> random_delta{iv.begin() + iv.size() / 2 + 1, iv.end()};
        for (size_t i = 0; i < count_blocks; i += count_blocks_per_thread) {
            threads.emplace_back([this, i, block_size, &encrypted_block, &res, count_blocks_per_thread, count_blocks, random_delta]() {
                std::vector<std::byte> counter_block = iv;
                counter_block.resize(block_size);
                for(size_t k = 1; k < i - 1 && i != 0; ++k) {
                    counter_block = bytes_utility::adding_bytes_vectors(counter_block, random_delta);
                }
                for(size_t k = 0; k < count_blocks_per_thread && i + k < count_blocks; ++k) {
                    counter_block = bytes_utility::adding_bytes_vectors(counter_block, random_delta);
                    std::vector<std::byte> block(block_size);
                    std::copy(encrypted_block.begin() + (i + k) * block_size, encrypted_block.begin() + (i + k + 1) * block_size, block.begin());
                    std::vector<std::byte> new_block = alg->encryption(bytes_utility::xor_vector(block, counter_block));
                    
                    std::copy(new_block.begin(), new_block.end(), res.begin() + (i + k) * block_size);
                }
            });
        }
        for (auto& t : threads) {
            t.join();
        }
        return res;
    }

    std::vector<std::byte> RandomDelta::decryption(const std::vector<std::byte> & encrypted_block) {
        size_t block_size = alg->get_block_size();
        if (encrypted_block.size() % block_size != 0) {
            throw std::runtime_error("input block size incorrect");
        }
        size_t n = encrypted_block.size() / block_size;
        
        size_t count_blocks = encrypted_block.size() / block_size;
        
        std::vector<std::byte> res(encrypted_block.size());
        std::vector<std::thread> threads;
        size_t count_threads = std::thread::hardware_concurrency() - 1;
        size_t count_blocks_per_thread = (count_blocks / count_threads + 1);

        std::vector<std::byte> random_delta{iv.begin() + iv.size() / 2 + 1, iv.end()};
        for (size_t i = 0; i < count_blocks; i += count_blocks_per_thread) {
            threads.emplace_back([this, i, block_size, &encrypted_block, &res, count_blocks_per_thread, count_blocks, random_delta]() {
                std::vector<std::byte> counter_block = iv;
                counter_block.resize(block_size);
                for(size_t k = 1; k < i - 1 && i != 0; ++k) {
                    counter_block = bytes_utility::adding_bytes_vectors(counter_block, random_delta);
                }
                for(size_t k = 0; k < count_blocks_per_thread && i + k < count_blocks; ++k) {
                    counter_block = bytes_utility::adding_bytes_vectors(counter_block, random_delta);
                    std::vector<std::byte> block(block_size);
                    std::copy(encrypted_block.begin() + (i + k) * block_size, encrypted_block.begin() + (i + k + 1) * block_size, block.begin());
                    std::vector<std::byte> new_block = bytes_utility::xor_vector(alg->decryption(block), counter_block);
                    
                    
                    std::copy(new_block.begin(), new_block.end(), res.begin() + (i + k) * block_size);
                }
            });
        }
        for (auto& t : threads) {
            t.join();
        }
        return res;
    }

}