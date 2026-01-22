#include "des.hpp"

#include <cassert>
#include <vector>
#include <string>
#include <iostream>
#include <future>
#include <thread>
#include <cstring>
#include <filesystem>
#include <cassert>
#include <random>
#include <cstdio> 
#include <chrono>
#include <algorithm>
#include <filesystem>

#include "bytes_utility.hpp"
#include "symmetric_context.hpp"

using namespace symmetric_interface_library;


std::vector<std::byte> random_bytes(size_t n) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<unsigned char> dis(0, 255);

    std::vector<std::byte> data(n);
    for (size_t i = 0; i < n; ++i) {
        data[i] = std::byte(dis(gen));
    }
    return data;
}

bool is_zeros_padding(PaddingModeEnum p) {
    return p == PaddingModeEnum::Zeros;
}

std::vector<size_t> get_diff(const std::vector<std::byte>& plaintext, const std::vector<std::byte>& decrypted) {
    std::vector<size_t> ind;
    for (size_t i = 0; i < plaintext.size(); ++i) {
        if (plaintext[i] != decrypted[i]) {
            ind.push_back(i);
        }
    }
    return ind;
}

bool plaintext_matches_decrypted(
    const std::vector<std::byte>& plaintext,
    const std::vector<std::byte>& decrypted,
    PaddingModeEnum padding
) {
    if (!is_zeros_padding(padding)) {
        auto f = plaintext == decrypted;
        return f;
    }


    if (decrypted.size() < plaintext.size()) return false;
    if (!std::equal(plaintext.begin(), plaintext.end(), decrypted.begin())) {

        return false;
    }
    for (size_t i = plaintext.size(); i < decrypted.size(); ++i) {
        if (decrypted[i] != std::byte{0}) {
            return false;
        }
    }
    return true;
}

bool test_in_memory(
    const std::vector<std::byte>& plaintext,
    EncryptionModeEnum mode,
    PaddingModeEnum padding
) {
    const size_t block_size = 8;
    std::vector<std::byte> key = random_bytes(7);
    std::vector<std::byte> iv = (mode != EncryptionModeEnum::ECB)
        ? random_bytes(block_size)
        : std::vector<std::byte>{};

    auto cipher = SymmetricContext(
        std::make_shared<symmetric_algorithms::DES>(key, std::make_shared<symmetric_algorithms::DESFeistelFunction>(), std::make_shared<symmetric_algorithms::DESGenerationRoundKeys>()),
        key,
        mode,
        padding,
        iv
    );

    std::vector<std::byte> encrypted, decrypted;
    auto f1 = cipher.encryption(plaintext, encrypted);
    f1.get();
    // std::cout << "input:\n";
    // bytes_utility::print_bytes_vector(std::cout, plaintext, " ");
    // std::cout << "\n-----------" << std::endl;
    auto f2 = cipher.decryption(encrypted, decrypted);
    f2.get();
    // std::cout << "output:\n";
    // bytes_utility::print_bytes_vector(std::cout, decrypted, " ");
    // std::cout << "\n-----------" << std::endl;
    return plaintext_matches_decrypted(plaintext, decrypted, padding);
}

bool test_file(
    const std::vector<std::byte>& plaintext,
    EncryptionModeEnum mode,
    PaddingModeEnum padding
) {
    const std::string in_file = "test_in.bin";
    const std::string enc_file = "test_enc.bin";
    const std::string dec_file = "test_dec.bin";


    {
        std::ofstream f(in_file, std::ios::binary);
        if (!f) {
            std::cerr << "Cannot create input file\n";
            return false;
        }
        if (!plaintext.empty()) {
            f.write(reinterpret_cast<const char*>(plaintext.data()), plaintext.size());
        }
        f.close();
        if (!f) {
            std::cerr << "Write failed\n";
            return false;
        }
    }

    const size_t block_size = 8;
    std::vector<std::byte> key = random_bytes(7);
    std::vector<std::byte> iv = (mode != EncryptionModeEnum::ECB)
        ? random_bytes(block_size)
        : std::vector<std::byte>{};

    std::vector<std::any> additional_params;
    if (mode == EncryptionModeEnum::RandomDelta) {
        additional_params.push_back(0); 
    }
    auto cipher = SymmetricContext(
        std::make_shared<symmetric_algorithms::DES>(
		key, 
		std::make_shared<symmetric_algorithms::DESFeistelFunction>(), 
		std::make_shared<symmetric_algorithms::DESGenerationRoundKeys>()),
        key,
        mode,
        padding,
        iv
    );
    try {
        auto f1 = cipher.encryption(in_file, enc_file);
        f1.get();
        auto f2 = cipher.decryption(enc_file, dec_file);
        f2.get();

    } catch (const std::exception& e) {
        std::cerr << "File test failed: " << e.what() 
                  << " (mode=" << static_cast<int>(mode) 
                  << ", padding=" << static_cast<int>(padding) << ")\n";
        std::remove(in_file.c_str());
        std::remove(enc_file.c_str());
        std::remove(dec_file.c_str());
        return false;
    }


    std::vector<std::byte> recovered;
    {
        std::ifstream f(dec_file, std::ios::binary);
        if (!f) {
            std::cerr << "Cannot open decrypted file\n";
            std::remove(in_file.c_str());
            std::remove(enc_file.c_str());
            std::remove(dec_file.c_str());
            return false;
        }
        f.seekg(0, std::ios::end);
        size_t len = f.tellg();
        f.seekg(0);
        if (len > 0) {
            recovered.resize(len);
            f.read(reinterpret_cast<char*>(recovered.data()), len);
        }
    }

    std::remove(in_file.c_str());
    std::remove(enc_file.c_str());
    std::remove(dec_file.c_str());

    return plaintext_matches_decrypted(plaintext, recovered, padding);
}


void tests() {
    std::vector<EncryptionModeEnum> modes = {
        EncryptionModeEnum::ECB,
        EncryptionModeEnum::CBC,
        EncryptionModeEnum::PCBC,
        EncryptionModeEnum::CFB,
        EncryptionModeEnum::OFB,
        EncryptionModeEnum::CTR,
        EncryptionModeEnum::RandomDelta,
    };

    std::vector<PaddingModeEnum> paddings = {
        PaddingModeEnum::Zeros,
        PaddingModeEnum::ANSIX923,
        PaddingModeEnum::PKCS7,
        PaddingModeEnum::ISO10126,
    };

    std::vector<size_t> lengths = {0, 1, 15, 16, 17, 31, 32, 33, 100, 257};

    int total = 0, passed = 0;

    for (auto mode : modes) {
        for (auto pad : paddings) {
            for (size_t len : lengths) {
                auto plaintext = random_bytes(len);

                total++;
                if (test_in_memory(plaintext, mode, pad)){ 
                    passed++;
                } else {
                    std::cout << "memory " << "mode: " << (int) mode << " padding: " << (int)pad << std::endl;
                }

                total++;
                if (test_file(plaintext, mode, pad)) {
                    passed++;
                } else {
                    std::cout << "file   " << "mode: " << (int) mode << " padding: " << (int)pad << std::endl;
                }
            }
        }
    }
    
    std::cout << "Passed: " << passed << " / " << total << "\n";
    if (passed == total) {
        std::cout << "All tests passed!\n";
        return;
    } else {
        std::cerr << "Some tests failed!\n";
        return;
    }


}


void check_file() {
    const size_t block_size = 8;
    std::vector<std::byte> key = random_bytes(7);
    std::vector<std::byte> iv = random_bytes(block_size);
    auto cipher = SymmetricContext(
        std::make_shared<symmetric_algorithms::DES>(key),
        key,
        EncryptionModeEnum::CTR,
        PaddingModeEnum::PKCS7,
        iv
    );


    std::string s = ".png";
    std::string input_file_name = "/mnt/c/Users/Матвей/Documents/Unik/labs_C/sem5/CO/libs/symmetric_algorithms/tests/test";
    std::filesystem::path output_file_name = input_file_name + "_bin" + s;
    std::filesystem::path result_file_name = input_file_name + "_result" + s;
    input_file_name += s;
    std::filesystem::path input_file_name_path = input_file_name;
    cipher.encryption(input_file_name_path, output_file_name).get();
    std::cout << "start size: " << std::filesystem::file_size(input_file_name_path) << std::endl;
    std::cout << "encrypt size: " << std::filesystem::file_size(output_file_name) << std::endl;
    cipher.decryption(output_file_name, result_file_name).get();
    std::cout << "decrypt size: " << std::filesystem::file_size(result_file_name) << std::endl;

}


void check_time_tests(std::function<void(void)> f) {
    auto start = std::chrono::high_resolution_clock::now();
    f();
    auto end = std::chrono::high_resolution_clock::now();

    auto duration_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    std::cout << "Время выполнения: " << duration_ns.count() << " нс\n";

    auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Время выполнения: " << duration_ms.count() << " мс\n";
    using seconds_f = std::chrono::duration<double>;
    auto duration_s = std::chrono::duration_cast<seconds_f>(end - start);
    std::cout << "Время выполнения: " << duration_s.count() << " с\n";

}

int main() {
    
    // tests();
    check_time_tests(check_file);

    // std::vector<std::byte> key = {
    //                                 std::byte{0xAE}, std::byte{0xCB}, std::byte{0x81}, std::byte{0x71}, 
    //                                 std::byte{0x15}, std::byte{0x83}, std::byte{0x3C}, std::byte{0xCF}
    //                             };
    // symmetric_algorithms::DESGenerationRoundKeys d;
    // std::cout << "Key:\n";
    // bytes_utility::print_bytes_vector(std::cout, key);
    // auto res = d.generation_round_keys(key);
    // std::cout << "Key:\n";
    // bytes_utility::print_bytes_vector(std::cout, key);
    // std::cout << std::endl;
    // std::cout << "Round keys:\n";
    // for(const auto & rk : res) {
    //     bytes_utility::print_bytes_vector(std::cout, rk, true);
    //     std::cout << std::endl;
    // }
}